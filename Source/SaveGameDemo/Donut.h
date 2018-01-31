// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Donut.generated.h"

UCLASS()
class SAVEGAMEDEMO_API ADonut : public AActor
{
	GENERATED_BODY()
	
private:

	// Timer for rotation sequence
	float rotationSequenceTimer;
		
public:	

	// Sets default values for this actor's properties
	ADonut(const FObjectInitializer &ObjectInitializer);
	
public:	

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Rotation Method & Variable
	void RotateDonut(const FRotator rotation, float dTime);
	bool canRotate;

	// Donut Actor Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Root Component")
	class USceneComponent* DummyRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	class UStaticMeshComponent* DonutMesh;
	
};
