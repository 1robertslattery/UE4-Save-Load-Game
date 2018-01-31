// Fill out your copyright notice in the Description page of Project Settings.

#include "MySaveGame.h"
#include "SaveGameDemo.h"

// Sets default values
UMySaveGame::UMySaveGame()
{
	PawnHealthInstance = 100.f;
	PawnScoreInstance = 0.f;
	
	if (DonutRotations.Num() > 0)
		DonutRotations.Empty();

	PlayerSaveSlotName = FString(TEXT("PlayerSaveSlot"));
	UserIndex = 0;
}



