// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WidgetTree.h"
#include "TextWidgetTypes.h"
#include "TextBlock.h"
#include "MyUserWidget.generated.h"

UCLASS(BlueprintType, Blueprintable)
class SAVEGAMEDEMO_API UMyUserWidget : public UUserWidget
{
	GENERATED_BODY()

private:

	// Save Animation Reference & Variable
	class UWidgetAnimation* SaveAnim;
	bool canPlayAnimation;

	// Animation Method
	void AnimationLogic();

	// Stored Animation Variable
	bool hasRetrievedStoredAnimation;

	// Save Slot Display Variable
	bool canDisplaySaveDateAndLevel;

public:

	// Sets default values for this object's properties
	virtual void NativeConstruct() override;
	
public:

	// Called every frame
	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;

	// Find save animation and store a pointer for it
	void GetSaveAnimation(class TMap<FString, UWidgetAnimation*> &OutResults);

	// Functions we use to update our pawn's stats. Bind inside the Widget Blueprint.
	UFUNCTION(BlueprintPure, Category = "Text")
	FText GetHealthText() const;

	UFUNCTION(BlueprintPure, Category = "Text")
	FText GetScoreText() const;

	// Function that shows the date and level name of save slot (USaveGame System)
	UFUNCTION(BlueprintPure, Category = "Text")
	FText GetSaveSlotText() const;

	// Function that shows the date and level name of save game (Custom Save System)
	UFUNCTION(BlueprintPure, Category = "Text")
	FText GetCustomSaveSlotText() const;

	// Pawn Actor Reference
	UPROPERTY()
	class AMyPawn* PawnInWorld;

};
