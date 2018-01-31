// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "MySaveGame.generated.h"

UCLASS()
class SAVEGAMEDEMO_API UMySaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:

	// Sets default values for this pawn's properties
	UMySaveGame();
	
public:

	// Pawn Stats Variables
	UPROPERTY()
	float PawnHealthInstance;

	UPROPERTY()
	int32 PawnScoreInstance;
	
	// Donut Rotations Array
	UPROPERTY()
	TArray<struct FRotator> DonutRotations;
		
	// Save Slot One Name
	UPROPERTY()
	FString PlayerSaveSlotName;

	// Save Slot User Index
	UPROPERTY()
	uint32 UserIndex;
	
	// Struct we can use to save our computer's local date, time, month, etc.
	UPROPERTY()
	FDateTime PlayerSaveSlotDate;
		

};
