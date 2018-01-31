// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MyGameInstance.generated.h"

UCLASS()
class SAVEGAMEDEMO_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:

	// Sets default values for this pawn's properties
	UMyGameInstance();
	
public:

	// Checkpoint Variable
	bool hasReachedCheckpoint;

	// Player Start (1) Object Name
	FName FirstPlayerStartName;

	// Player Start (2) Object Name
	FName CheckpointPlayerStartName;
		
};
