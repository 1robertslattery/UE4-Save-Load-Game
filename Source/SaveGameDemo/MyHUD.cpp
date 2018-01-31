// Fill out your copyright notice in the Description page of Project Settings.

#include "MyHUD.h"
#include "SaveGameDemo.h"
#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h"
#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
#include "Runtime/UMG/Public/IUMGModule.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "MyUserWidget.h"
#include "UObject/ConstructorHelpers.h"

AMyHUD::AMyHUD()
{
	static ConstructorHelpers::FClassFinder<UMyUserWidget> WidgetAsset(TEXT("/Game/Blueprints/SaveUI"));

	if (WidgetAsset.Succeeded())
	{
		SaveWidgetClass = WidgetAsset.Class;
	}
}

// Called when the game starts or when spawned
void AMyHUD::BeginPlay()
{
	Super::BeginPlay();
	ActivateWidget();
}

// Create widget and add to viewport
void AMyHUD::ActivateWidget()
{
	class UWorld* const world = GetWorld();

	if (world != nullptr)
	{
		if (SaveWidgetClass != NULL)
		{
			SaveUI = CreateWidget<UMyUserWidget>(world, SaveWidgetClass);

			if (SaveUI != NULL)
				SaveUI->AddToViewport();
		}
	}
}

