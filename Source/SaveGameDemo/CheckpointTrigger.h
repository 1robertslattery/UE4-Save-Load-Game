// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "CheckpointTrigger.generated.h"

UCLASS()
class SAVEGAMEDEMO_API ACheckpointTrigger : public ATriggerBox
{
	GENERATED_BODY()
	
private:

	// Find actors we are using
	void FindDonut(class ADonut* WorldDonut, const FName Name);
	
	// Checkpoint Method
	void SetNewCheckpoint();

	// Donut Variables Method
	void SetDonutVariables();
	
	// Actor Overlap Events
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* overlappedComp, AActor* Other, UPrimitiveComponent* Box, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &hitResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* overlappedComp, AActor* Other, UPrimitiveComponent* Box, int32 OtherBodyIndex);

	// Actor Overlap Variable
	bool hasBeenOverlapped;

public:

	// Sets default values for this character's properties
	ACheckpointTrigger();

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:

	// Donut Actor References
	UPROPERTY()
	class ADonut* DonutOneInWorld;

	UPROPERTY()
	class ADonut* DonutTwoInWorld;
		
	
};
