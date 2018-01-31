// Fill out your copyright notice in the Description page of Project Settings.

#include "SaveGameDemoGameModeBase.h"
#include "SaveGameDemo.h"
#include "MyHUD.h"
#include "MyGameInstance.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"
#include "MyPawn.h"

// Sets default values
ASaveGameDemoGameModeBase::ASaveGameDemoGameModeBase()
{
	DefaultPawnClass = AMyPawn::StaticClass();
	HUDClass = AMyHUD::StaticClass();
}

// Sets APlayerStart of our pawn on BeginPlay, using Game Instance variables
class AActor* ASaveGameDemoGameModeBase::ChoosePlayerStart_Implementation(AController* Player)
{
	class AActor* retVal = nullptr;

	if (Player)
	{
		class UWorld* const world = GetWorld();

		if (world != nullptr) 
		{
			class UMyGameInstance* gInstance = Cast<UMyGameInstance>(GetGameInstance());

			if (gInstance)
			{
				TArray<class AActor*> PlayerStarts;
				UGameplayStatics::GetAllActorsOfClass(world, APlayerStart::StaticClass(), PlayerStarts);
				TArray<class AActor*> PreferredStarts;

				for (TActorIterator<APlayerStart> Itr(world); Itr; ++Itr)
				{
					if (!gInstance->hasReachedCheckpoint)
					{
						if (gInstance->FirstPlayerStartName == Itr->PlayerStartTag)
							PreferredStarts.Add(*Itr);
					}
					else
					{
						if (gInstance->CheckpointPlayerStartName == Itr->PlayerStartTag)
							PreferredStarts.Add(*Itr);
					}
				}

				return PreferredStarts[FMath::RandRange(0, PreferredStarts.Num() - 1)];
			}
		}
	}

	return retVal;
}



