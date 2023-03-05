// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "RayEmitter.generated.h"

UENUM(BlueprintType)
enum class RenderType : uint8 {
	SplineMesh       UMETA(DisplayName = "SplineMesh"),
	NiagaraVFX        UMETA(DisplayName = "Niagara VFX"),
};

UCLASS()
class RAY_API ARayEmitter : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARayEmitter();
	UPROPERTY(EditAnywhere)
	RenderType fRenderType;
	UPROPERTY(EditAnywhere)
	USceneComponent* fRoot;
	UPROPERTY(EditAnywhere)
	USplineComponent* fSplineComponent;
	UPROPERTY(EditAnywhere)
	UStaticMesh* fMesh;
	UPROPERTY(EditAnywhere)
	UNiagaraSystem* fParticleSystem;
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
	void SetUpParticleSystem();
	void ComputeReflection(TArray<FVector>& aListOfReflectionLocation, TArray<FVector>& alistOfReflectionRotation);
	void UpdateSpline(TArray<FVector>& aListOfReflectionLocation, TArray<FVector>& aListOfReflectionRotation);
	void UpdateParticleSystem(TArray<FVector>& aListOfReflectionLocation, TArray<FVector>& aListOfReflectionRotation);
	TArray<USplineMeshComponent*> fMeshCache;
	TArray<UNiagaraComponent*> fParticleCache;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
