// Fill out your copyright notice in the Description page of Project Settings.

#include "Donut.h"
#include "SaveGameDemo.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
ADonut::ADonut(const FObjectInitializer &ObjectInitializer) : Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Construct Scene Component
	DummyRoot = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("Root Component"));
	RootComponent = DummyRoot;

	// Construct Static Mesh Component
	DonutMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("Mesh"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ObjMesh(TEXT("/Game/StarterContent/Shapes/Shape_Torus"));
	DonutMesh->SetStaticMesh(ObjMesh.Object);
	DonutMesh->SetupAttachment(RootComponent);
	
	// Default variables
	rotationSequenceTimer = 0.f;
	canRotate = false;
}

// Called every frame
void ADonut::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// When canRotate is true, set new rotations for each donut
	if (canRotate) 
	{
		if (this->ActorHasTag(FName(TEXT("DonutOne"))))
			RotateDonut(FRotator(0.f, 0.f, 90.f), DeltaTime);
		else if (this->ActorHasTag(FName(TEXT("DonutTwo"))))
			RotateDonut(FRotator(90.f, 0.f, 0.f), DeltaTime);
	}
}

// Rotate the donut in a specified rotation over 3 seconds
void ADonut::RotateDonut(const FRotator rotation, float dTime)
{
	rotationSequenceTimer += 1 * dTime;

	if (rotationSequenceTimer < 3.f)
	{
		const FRotator CurrentRot = GetActorRotation();
		const FRotator TargetRot = rotation;
		const FRotator FinalRot = FMath::RInterpTo(CurrentRot, TargetRot, dTime, 1.f);
		SetActorRotation(FinalRot);
	}
	else
	{
		canRotate = false;
		rotationSequenceTimer = 0.f;
	}
}