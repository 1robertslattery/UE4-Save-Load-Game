// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SaveGameDemoGameModeBase.generated.h"

UCLASS()
class SAVEGAMEDEMO_API ASaveGameDemoGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:

	// Sets default values for this pawn's properties
	ASaveGameDemoGameModeBase();
	
public:

	// Player Start Override
	virtual class AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	bool ShouldSpawnAtStartSpot(AController* Player) override { return false; };

};
