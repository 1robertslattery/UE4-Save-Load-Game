// Fill out your copyright notice in the Description page of Project Settings.

#include "CheckpointTrigger.h"
#include "SaveGameDemo.h"
#include "Components/BoxComponent.h"
#include "MyGameInstance.h"
#include "EngineUtils.h"
#include "Runtime/Engine/Classes/Engine/Engine.h" // For debug messages
#include "MyPawn.h"
#include "Donut.h"

// Sets default values
ACheckpointTrigger::ACheckpointTrigger()
{
	GetCollisionComponent()->bGenerateOverlapEvents = true;
	GetCollisionComponent()->OnComponentBeginOverlap.AddDynamic(this, &ACheckpointTrigger::OnOverlapBegin);
	GetCollisionComponent()->OnComponentEndOverlap.AddDynamic(this, &ACheckpointTrigger::OnOverlapEnd);
	hasBeenOverlapped = false;
	DonutOneInWorld = NULL;
	DonutTwoInWorld = NULL;

}

#pragma region Setup Logic
// Called when the game starts or when spawned
void ACheckpointTrigger::BeginPlay()
{
	Super::BeginPlay();
	FindDonut(DonutOneInWorld, FName(TEXT("DountOne")));
	FindDonut(DonutTwoInWorld, FName(TEXT("DountTwo")));
	this->SetActorHiddenInGame(false);
	GetCollisionComponent()->SetHiddenInGame(false);
}

void ACheckpointTrigger::FindDonut(class ADonut* WorldDonut, const FName Name)
{
	class UWorld* const world = GetWorld();

	if (world)
	{
		for (TActorIterator<ADonut> ActorItr(world); ActorItr; ++ActorItr)
		{
			class ADonut* FoundDonut = *ActorItr;

			if (FoundDonut != nullptr)
			{
				if (FoundDonut->ActorHasTag(Name))
				{
					if (WorldDonut != FoundDonut)
					{
						WorldDonut = FoundDonut;
					}
				}
			}
		}
	}
}
#pragma endregion

#pragma region Set Actor Variables
void ACheckpointTrigger::SetNewCheckpoint()
{
	class UMyGameInstance* gInstance = Cast<UMyGameInstance>(GetGameInstance());

	if (gInstance)
	{
		if (!gInstance->hasReachedCheckpoint) 
		{
			gInstance->hasReachedCheckpoint = true;
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Magenta, FString(TEXT("Reached CHECKPOINT!")), true);
		}
	}
}

void ACheckpointTrigger::SetDonutVariables()
{
	if (DonutOneInWorld != NULL)
		if (!DonutOneInWorld->canRotate)
			DonutOneInWorld->canRotate = true;

	if (DonutTwoInWorld != NULL)
		if (!DonutTwoInWorld->canRotate)
			DonutTwoInWorld->canRotate = true;
}
#pragma endregion

#pragma region Overlap Events
void ACheckpointTrigger::OnOverlapBegin(UPrimitiveComponent* overlappedComp, AActor* Other, UPrimitiveComponent* Box, int32 otherBodyIndex, bool bFromSweep, const FHitResult &hitResult)
{
	class AMyPawn* pawn = Cast<AMyPawn>(Other);

	if (pawn && !hasBeenOverlapped)
	{
		SetNewCheckpoint();
		SetDonutVariables();

		// Now our pawn can prepare to save ALL actor stats
		if (!pawn->canSaveGame)
			pawn->canSaveGame = true;

		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString(TEXT("IN TRIGGER")), true);
		hasBeenOverlapped = true;
	}
}

void ACheckpointTrigger::OnOverlapEnd(UPrimitiveComponent* overlappedComp, AActor* Other, UPrimitiveComponent* Box, int32 OtherBodyIndex)
{
	class AMyPawn* pawn = Cast<AMyPawn>(Other);

	if (pawn)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString(TEXT("OUT OF TRIGGER")), true);
		pawn = nullptr;
	}
}
#pragma endregion

