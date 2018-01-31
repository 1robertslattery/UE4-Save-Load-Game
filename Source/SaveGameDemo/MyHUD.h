// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MyHUD.generated.h"

UCLASS()
class SAVEGAMEDEMO_API AMyHUD : public AHUD
{
	GENERATED_BODY()
	
public:

	// Sets default values for this pawn's properties
	AMyHUD();

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:

	// Widget Blueprint Class
	TSubclassOf<class UMyUserWidget> SaveWidgetClass;

	// Point to Widget Class
	class UMyUserWidget* SaveUI;

	// Create Widget Method
	void ActivateWidget();

};
