// Fill out your copyright notice in the Description page of Project Settings.

#include "MyUserWidget.h"
#include "SaveGameDemo.h"
#include "WidgetAnimation.h"
#include "EngineUtils.h"
#include "MyPawn.h"

// Sets default values
void UMyUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Find our pawn
	for (TActorIterator<AMyPawn> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		class AMyPawn* FoundPawn = *ActorItr;

		if (FoundPawn != nullptr)
			if (PawnInWorld != FoundPawn)
				PawnInWorld = FoundPawn;
	}
	
	// Default variables
	canPlayAnimation = false;
	SaveAnim = nullptr;
	hasRetrievedStoredAnimation = false;
	canDisplaySaveDateAndLevel = false;
}

// Called every frame
void UMyUserWidget::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);
	AnimationLogic();
}

#pragma region Text
// Pawn Health
FText UMyUserWidget::GetHealthText() const
{
	if (PawnInWorld == nullptr)
		return FText::FromString(FString(TEXT("Health: --")));
	else 
	{
		FString health = FString(TEXT("Health:  "));
		health.AppendInt(PawnInWorld->CharacterStats.health);

		return FText::FromString(health);
	}
}

// Pawn Score
FText UMyUserWidget::GetScoreText() const
{
	if (PawnInWorld == nullptr)
		return FText::FromString(FString(TEXT("Score: --")));
	else
	{
		FString score = FString(TEXT("Score:  "));
		score.AppendInt(PawnInWorld->CharacterStats.score);

		return FText::FromString(score);
	}
}

// Save Slot Date and Level Name (USaveGame Save System)
FText UMyUserWidget::GetSaveSlotText() const
{
	bool DoOnce = canDisplaySaveDateAndLevel;
	
	const FString SaveSlotName = FString(TEXT("PlayerSaveSlot"));

	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0))
	{
		if (!DoOnce) 
		{
			// Load the date from save game object
			class UMySaveGame* LoadInstance = Cast<UMySaveGame>(UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass()));

			if (LoadInstance->IsValidLowLevel())
			{
				LoadInstance = Cast<UMySaveGame>(UGameplayStatics::LoadGameFromSlot(LoadInstance->PlayerSaveSlotName, LoadInstance->UserIndex));
				FDateTime SystemDate = LoadInstance->PlayerSaveSlotDate;

				// Convert Date to FString
				FString DateName = SystemDate.ToString();

				// Get our current level name
				FString CurrentLevel = UGameplayStatics::GetCurrentLevelName(GetWorld(), true);

				DateName.Append(FString(TEXT("\n")) + CurrentLevel);
				return FText::FromString(DateName);
				DoOnce = true;
			}
			else
			{
				// If save game object not found, create a new one
				class UMySaveGame* LoadInstance_2 = Cast<UMySaveGame>(UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass()));

				if (!LoadInstance_2)
					return FText::FromString(FString(TEXT("INVALID LOAD")));
				else
				{
					LoadInstance_2 = Cast<UMySaveGame>(UGameplayStatics::LoadGameFromSlot(LoadInstance_2->PlayerSaveSlotName, LoadInstance_2->UserIndex));
					FDateTime SystemDate = LoadInstance_2->PlayerSaveSlotDate;

					// Convert Date to FString
					FString DateName = SystemDate.ToString();

					// Get our current level name
					FString CurrentLevel = UGameplayStatics::GetCurrentLevelName(GetWorld(), true);

					DateName.Append(FString(TEXT("\n")) + CurrentLevel);
					return FText::FromString(DateName);
					DoOnce = true;
				}
			}
		}
	}

	return FText::FromString(FString(TEXT("{DISPLAYS SAVE GAME}")));
}

// Save Slot Date and Level Name (Custom Save System)
FText UMyUserWidget::GetCustomSaveSlotText() const
{
	bool DoOnce = canDisplaySaveDateAndLevel;

	if (PawnInWorld != nullptr)
	{
		if (PawnInWorld->hasCompletedCustomSaveGame)
		{
			if (!DoOnce) 
			{
				FDateTime SystemDate = PawnInWorld->SaveGameTime;

				// Convert Date to FString
				FString DateName = SystemDate.ToString();

				// Get our current level name
				FString CurrentLevel = UGameplayStatics::GetCurrentLevelName(GetWorld(), true);

				DateName.Append(FString(TEXT("\n")) + CurrentLevel);
				return FText::FromString(DateName);
				DoOnce = true;
			}
		}
		else
			return FText::FromString(FString(TEXT("")));
	}

	return FText::FromString(FString(TEXT("")));
}
#pragma endregion

#pragma region Animation
void UMyUserWidget::AnimationLogic()
{
	if (PawnInWorld != nullptr)
	{
		// Play animation only when we are preparing our save game
		if (PawnInWorld->canSaveGame)
		{
			// Find our stored Widget Animation pointer
			if (!hasRetrievedStoredAnimation) 
			{
				class TMap<FString, UWidgetAnimation*> AnimMap;
				GetSaveAnimation(AnimMap);

				// TMap Iterator
				for (auto& Element : AnimMap)
				{
					SaveAnim = Element.Value;
					hasRetrievedStoredAnimation = true;
				}
			}
			else
			{
				// Now play the animation
				if (SaveAnim != nullptr)
				{
					if (!canPlayAnimation)
					{
						PlayAnimation(SaveAnim, 0.f, 1, EUMGSequencePlayMode::Forward, 1.f);
						canPlayAnimation = true;
					}
				}
			}
		}
	}
}

void UMyUserWidget::GetSaveAnimation(class TMap<FString, UWidgetAnimation*>& OutResults)
{
	OutResults.Empty();

	class UProperty* Prop = GetClass()->PropertyLink;

	// Run through all properties of this class to find any widget animations
	while (Prop != nullptr)
	{
		// Only interested in object properties
		if (Prop->GetClass() == UObjectProperty::StaticClass())
		{
			class UObjectProperty* ObjectProp = Cast<UObjectProperty>(Prop);

			// Only want the properties that are widget animations
			if (ObjectProp->PropertyClass == UWidgetAnimation::StaticClass())
			{
				class UObject* Object = ObjectProp->GetObjectPropertyValue_InContainer(this);

				class UWidgetAnimation* WidgetAnimToStore = Cast<UWidgetAnimation>(Object);

				if (WidgetAnimToStore != nullptr)
				{
					FString NameToStore = WidgetAnimToStore->GetFName().ToString();
					OutResults.Add(NameToStore, WidgetAnimToStore);
				}
			}
		}

		Prop = Prop->PropertyLinkNext;
	}
}
#pragma endregion


