// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MyPawn.generated.h"

USTRUCT(BlueprintType)
struct FCharacterData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Score")
	int32 score;

};

UCLASS()
class SAVEGAMEDEMO_API AMyPawn : public APawn
{
	GENERATED_BODY()

private:

	// Find donuts we are using
	void FindDonut(class ADonut* WorldDonut, const FName Name);

	// Jump Variable
	bool isJumping;
	
	// Save & Load Variables
	float loadingSequenceTimer;
	float savingSequenceTimer;
	bool hasCompletedLoadGame;
	bool isAssemblingSaveGameData;

	// Reload Level Method
	void ResetLevel();

	// Actor Tag Name
	static const FName PlayerTagName;

public:

	// Sets default values for this pawn's properties
	AMyPawn(const FObjectInitializer &ObjectInitializer);

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Actor interface for resetting pawn jump
	virtual void NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;
	
public:	

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Movement Methods
	void MoveForward(float v);
	void MoveRight(float h);
	void Jump();

	// Pawn Actor Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
	class USphereComponent* CollisionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	class UStaticMeshComponent* PawnMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	class UFloatingPawnMovement* PawnMovement;

	// Struct Reference
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	FCharacterData CharacterStats;
			
	// Donut Actor References
	UPROPERTY()
	class ADonut* DonutOneInWorld;

	UPROPERTY()
	class ADonut* DonutTwoInWorld;
	
public:
	
	// Main Save/Load Methods & Variable
	void SaveGameLogic(float dTime);
	void LoadGameLogic(float dTime);
	bool canSaveGame;

public:

	// 
	// USaveGame Object System
	// 

	// USaveGame Save Method
	void InitSaveGame();

	// USaveGame Load Method
	void InitLoadGame();
				
public:

	// 
	// Custom Save System
	// 

	// Custom Save Game Variable
	bool SaveGameToFile(const FString& FullFilePath);
	bool hasCompletedCustomSaveGame;

	// Custom Load Game Variable
	bool LoadGameFromFile(const FString& FullFilePath, float& HealthSaveData, int32& ScoreSaveData, TArray<struct FRotator>& DonutRotatorDataArray, FDateTime &SaveTime);

	// Archived Binary Save/Load Method
	void CustomSaveLoadData(FArchive& Ar, float& HealthSaveData, int32& ScoreSaveData, TArray<struct FRotator>& DonutRotatorDataArray, FDateTime &SaveTime);
	
	// Donut Rotation Array
	UPROPERTY()
	TArray<struct FRotator> DonutRots;

	// Save Date and Time
	UPROPERTY()
	FDateTime SaveGameTime;

};
