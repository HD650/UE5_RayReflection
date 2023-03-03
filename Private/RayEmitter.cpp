// Fill out your copyright notice in the Description page of Project Settings.


#include "RayEmitter.h"
#include "Math/UnrealMathUtility.h"

#define DEBUG 0

// Sets default values
ARayEmitter::ARayEmitter()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	fRoot = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(fRoot);
	fSplineComponent = CreateDefaultSubobject<USplineComponent>("Spline");
	fSplineComponent->AttachToComponent(fRoot, FAttachmentTransformRules::KeepRelativeTransform);

	// default value of param
	fLength = 10000;
	fReflectCount = 10;
}

void ARayEmitter::AllocateSplineMesh()
{
	for (int32 i = 0; i < fReflectCount; i++)
	{
		FString name = "StaticMeshComponent" + i;
		USplineMeshComponent* oneSplineMesh = NewObject<USplineMeshComponent>(this, USplineMeshComponent::StaticClass(), FName(name));
		oneSplineMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		oneSplineMesh->SetMobility(EComponentMobility::Movable);
		oneSplineMesh->SetForwardAxis(ESplineMeshAxis::Z, true);
		oneSplineMesh->RegisterComponentWithWorld(GetWorld());
		oneSplineMesh->AttachToComponent(fSplineComponent, FAttachmentTransformRules::KeepRelativeTransform);
		oneSplineMesh->SetCastShadow(false);
		oneSplineMesh->SetStaticMesh(fMesh);
		oneSplineMesh->SetMaterial(0, fMaterial);
		fMeshCache.Push(oneSplineMesh);
	}
}

// Called when the game starts or when spawned
void ARayEmitter::BeginPlay()
{
	Super::BeginPlay();
	APlayerController* contorller = this->GetWorld()->GetFirstPlayerController();
	contorller->bShowMouseCursor = true;

	if (DEBUG)
	{
		fSplineComponent->SetDrawDebug(true);
		GEngine->Exec(GetWorld(), TEXT("show Splines"));
	}

	AllocateSplineMesh();
}

void ARayEmitter::FaceMouse()
{
	APlayerController* contorller = this->GetWorld()->GetFirstPlayerController();
	FVector mouse_location, mouse_rotation, mouseEnd;
	contorller->DeprojectMousePositionToWorld(mouse_location, mouse_rotation);
	mouseEnd = mouse_location + fLength * mouse_rotation;
	FHitResult OutHit;
	bool isHit = GetWorld()->LineTraceSingleByChannel(OutHit, mouse_location, mouseEnd, ECollisionChannel::ECC_Visibility);
	if (isHit)
	{
		FVector cur_location = this->GetActorLocation();
		FVector hitLocation = OutHit.Location;
		FVector forward = hitLocation - cur_location;
		forward.Normalize();
		this->SetActorRotation(forward.Rotation());
	}
}

void ARayEmitter::ComputeReflection(TArray<FVector>& aListOfReflectionLocation, TArray<FVector>& alistOfReflectionRotation)
{
	// do reflection
	FVector start = GetActorLocation();
	FVector forward = GetActorForwardVector();
	FVector end = start + forward * fLength;

	FHitResult OutHit;
	FCollisionQueryParams CollisionParams;
	int32 currentReflect = 0;
	bool isHit = false;

	// do ray casting and collect points
	do
	{
		aListOfReflectionLocation.Push(start);
		alistOfReflectionRotation.Push(forward);

		isHit = GetWorld()->LineTraceSingleByChannel(OutHit, start, end, ECollisionChannel::ECC_Visibility);
		if (isHit)
		{
			end = OutHit.Location;
			forward = FMath::GetReflectionVector(forward, OutHit.Normal);
			forward.Normalize();
			currentReflect += 1;
		}
		if (DEBUG)
			DrawDebugLine(GetWorld(), start, end, FColor::Red, false, 1, 0, 5);

		start = end;
		end = start + forward * fLength;
	} while (isHit && currentReflect < fReflectCount);
	// add the last point
	aListOfReflectionLocation.Push(start);
	alistOfReflectionRotation.Push(forward);
}

void ARayEmitter::UpdateSpline(TArray<FVector>& aListOfReflectionLocation, TArray<FVector>& aListOfReflectionRotation)
{
	// clean spine line and spline mesh first
	fSplineComponent->ClearSplinePoints(false);
	// update spline line points and clamp them
	fSplineComponent->SetSplinePoints(aListOfReflectionLocation, ESplineCoordinateSpace::World, true);
	int32 SplinePoints = fSplineComponent->GetNumberOfSplinePoints();

	for (int32 SplineCount = 0; SplineCount < SplinePoints; SplineCount++)
	{
		fSplineComponent->SetSplinePointType(SplineCount, ESplinePointType::CurveClamped, true);
	}

	// update spline mesh
	for (int32 meshCount = 0; meshCount < fMeshCache.Num(); meshCount++)
	{

		if (meshCount < SplinePoints)
		{
			// define the positions of the points and tangents
			const FVector StartPoint = fSplineComponent->GetLocationAtSplinePoint(meshCount, ESplineCoordinateSpace::Type::Local);
			const FVector StartTangent = fSplineComponent->GetTangentAtSplinePoint(meshCount, ESplineCoordinateSpace::Type::Local);
			const FVector EndPoint = fSplineComponent->GetLocationAtSplinePoint(meshCount + 1, ESplineCoordinateSpace::Type::Local);
			const FVector EndTangent = fSplineComponent->GetTangentAtSplinePoint(meshCount + 1, ESplineCoordinateSpace::Type::Local);
			fMeshCache[meshCount]->SetStartAndEnd(StartPoint, StartTangent, EndPoint, EndTangent, true);
			fMeshCache[meshCount]->SetVisibility(true);
		}
		else
		{
			fMeshCache[meshCount]->SetVisibility(false);
		}
	}
}

// Called every frame
void ARayEmitter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// rotating
	FaceMouse();

	TArray<FVector> listOfReflectionLocation;
	TArray<FVector> listOfReflectionRotation;

	ComputeReflection(listOfReflectionLocation, listOfReflectionRotation);

	UpdateSpline(listOfReflectionLocation, listOfReflectionRotation);
}