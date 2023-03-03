// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "RayEmitter.generated.h"


UCLASS()
class RAY_API ARayEmitter : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARayEmitter();
	UPROPERTY(EditAnywhere)
	USceneComponent* fRoot;
	UPROPERTY(EditAnywhere)
	USplineComponent* fSplineComponent;
	UPROPERTY(EditAnywhere)
	UStaticMesh* fMesh;
	UPROPERTY(EditAnywhere)
	UMaterial* fMaterial;
	UPROPERTY(EditAnywhere)
	double fLength;
	UPROPERTY(EditAnywhere)
	int32 fReflectCount;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void FaceMouse();
	void AllocateSplineMesh();
	void ComputeReflection(TArray<FVector>& aListOfReflectionLocation, TArray<FVector>& alistOfReflectionRotation);
	void UpdateSpline(TArray<FVector>& aListOfReflectionLocation, TArray<FVector>& aListOfReflectionRotation);
	TArray<USplineMeshComponent*> fMeshCache;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
