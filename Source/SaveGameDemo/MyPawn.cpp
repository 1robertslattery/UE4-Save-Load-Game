// Fill out your copyright notice in the Description page of Project Settings.

#include "MyPawn.h"
#include "SaveGameDemo.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Runtime/Engine/Classes/GameFramework/FloatingPawnMovement.h"
#include "MyGameInstance.h"
#include "EngineUtils.h"
#include "Runtime/Engine/Classes/Engine/Engine.h" // For Debug Messages
#include "UObject/ConstructorHelpers.h"
#include "Donut.h"

// Headers For Custom Save System
#include "Archive.h"
#include "BufferArchive.h"
#include "FileManager.h"
#include "FileHelper.h"
#include "MemoryReader.h"

const FName AMyPawn::PlayerTagName(TEXT("Player"));

// Sets default values
AMyPawn::AMyPawn(const FObjectInitializer &ObjectInitializer) : Super(ObjectInitializer)
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Construct Sphere Component
	CollisionComp = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("Collision Comp"));
	CollisionComp->bGenerateOverlapEvents = true;
	CollisionComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	CollisionComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	CollisionComp->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
	CollisionComp->SetSimulatePhysics(true);
	CollisionComp->SetAngularDamping(0.1f);
	CollisionComp->SetLinearDamping(0.1f);
	CollisionComp->BodyInstance.MassScale = 3.5f;
	CollisionComp->SetNotifyRigidBodyCollision(true);
	RootComponent = CollisionComp;

	// Construct Static Mesh Component
	PawnMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("Mesh"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ObjMesh(TEXT("/Game/StarterContent/Shapes/Shape_Sphere"));
	
	if (ObjMesh.Succeeded()) 
	{
		PawnMesh->SetStaticMesh(ObjMesh.Object);
		PawnMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		PawnMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		PawnMesh->SetupAttachment(RootComponent);
	}

	// Construct Floating Pawn Movement
	PawnMovement = ObjectInitializer.CreateDefaultSubobject<UFloatingPawnMovement>(this, TEXT("FloatingPawnMovement"));
	PawnMovement->UpdatedComponent = CollisionComp;
	PawnMovement->MaxSpeed = 300.f;
	PawnMovement->Acceleration = 800.f;
	PawnMovement->Deceleration = 2000.f;

	// Default Variables
	CharacterStats.health = 100.f;
	CharacterStats.score = 0;
	isJumping = false;
	loadingSequenceTimer = 0.f;
	savingSequenceTimer = 0.f;
	hasCompletedLoadGame = false;
	isAssemblingSaveGameData = false;
	canSaveGame = false;
	DonutOneInWorld = NULL;
	DonutTwoInWorld = NULL;
	hasCompletedCustomSaveGame = false;

	// Actor Tag
	this->Tags.Add(PlayerTagName);

}

#pragma region Setup Logic
// Called when the game starts or when spawned
void AMyPawn::BeginPlay()
{
	Super::BeginPlay();
	FindDonuts();
}

void AMyPawn::FindDonuts()
{
	class UWorld* const world = GetWorld();

	if (world)
	{
		for (TActorIterator<ADonut> ActorItr(world); ActorItr; ++ActorItr)
		{
			class ADonut* FoundDonut = *ActorItr;

			if (FoundDonut != nullptr)
			{
				if (FoundDonut->ActorHasTag(FName(TEXT("DonutOne"))))
				{
					if (DonutOneInWorld != FoundDonut)
					{
						DonutOneInWorld = FoundDonut;
					}
				}
			}
		}

		for (TActorIterator<ADonut> ActorItr_2(world); ActorItr_2; ++ActorItr_2)
		{
			class ADonut* FoundDonut_2 = *ActorItr_2;

			if (FoundDonut_2 != nullptr)
			{
				if (FoundDonut_2->ActorHasTag(FName(TEXT("DonutTwo"))))
				{
					if (DonutTwoInWorld != FoundDonut_2)
					{
						DonutTwoInWorld = FoundDonut_2;
					}
				}
			}
		}
	}
}
#pragma endregion

// Called every frame
void AMyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SaveGameLogic(DeltaTime);
	LoadGameLogic(DeltaTime);
}

#pragma region Movement Logic
// Called to bind functionality to input
void AMyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &AMyPawn::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMyPawn::MoveRight);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMyPawn::Jump);
	PlayerInputComponent->BindAction("Reset", IE_Pressed, this, &AMyPawn::ResetLevel);

}

void AMyPawn::MoveForward(float v)
{
	if (Controller != NULL && v != NULL)
		AddMovementInput(GetActorForwardVector(), v);
}

void AMyPawn::MoveRight(float h)
{
	if (Controller != NULL && h != NULL)
		AddMovementInput(GetActorRightVector(), h);
}

void AMyPawn::Jump()
{
	if (!isJumping)
	{
		const float JumpImpulse = 120000.f;
		const FVector LaunchVelocity = FVector(0.f, 0.f, JumpImpulse);
		PawnMovement->UpdatedPrimitive->AddImpulse(LaunchVelocity);

		// Add score when jumping
		CharacterStats.score = (CharacterStats.score) + 100;
		isJumping = true;
	}
}

void AMyPawn::NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	if (isJumping)
		isJumping = false;
}
#pragma endregion

#pragma region Save/Load Logic [Main]
void AMyPawn::SaveGameLogic(float dTime)
{
	if (canSaveGame)
	{
		savingSequenceTimer += 1 * dTime;

		if (savingSequenceTimer > 3.f)
		{
			// USaveGame Object System Only
			InitSaveGame();

			// Uncomment if you'd like to use the Custom Save System
			//SaveGameToFile(FString(TEXT("C:\\Users\\{YOUR_USERNAME}\\Documents\\MyCustomSaveFile.save")));

			savingSequenceTimer = 0.f;
			canSaveGame = false;
		}
		else
		{
			if (!isAssemblingSaveGameData)
			{
				// As long as our actors have been found and we still have references to them, we will be able to save our game
				if (this->IsValidLowLevel() && DonutOneInWorld->IsValidLowLevel() && DonutTwoInWorld->IsValidLowLevel())
				{
					GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, FString(TEXT("Getting ready to save...")), true);
					isAssemblingSaveGameData = true;
				}
				else
				{
					// Otherwise, cancel save game
					savingSequenceTimer = 0.f;
					canSaveGame = false;
					isAssemblingSaveGameData = false;
				}
			}
		}
	}
}

// Load save game only when a Game Instance variable tells us to do so
void AMyPawn::LoadGameLogic(float dTime)
{
	if (!hasCompletedLoadGame)
	{
		loadingSequenceTimer += 1 * dTime;

		if (loadingSequenceTimer > 0.20f)
		{
			// USaveGame Object System Only
			InitLoadGame();

			// Uncomment if you'd like to use the Custom Save System
			//LoadGameFromFile(FString(TEXT("C:\\Users\\{YOUR_USERNAME}\\Documents\\MyCustomSaveFile.save")), CharacterStats.health, CharacterStats.score, DonutRots, SaveGameTime);

			loadingSequenceTimer = 0.f;
			hasCompletedLoadGame = true;
			isAssemblingSaveGameData = false;
		}
	}
}
#pragma endregion

#pragma region Save/Load Logic [USaveGame]
void AMyPawn::InitSaveGame()
{
	// Create save game object, make sure it exists, then save player variables
	class UMySaveGame* SaveGameInstance = Cast<UMySaveGame>(UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass()));

	if (SaveGameInstance->IsValidLowLevel())
	{
		// Pawn and Donut Stats
		SaveGameInstance->PawnHealthInstance = CharacterStats.health;
		SaveGameInstance->PawnScoreInstance = CharacterStats.score;
		SaveGameInstance->DonutRotations.EmplaceAt(0, DonutOneInWorld->GetActorRotation());
		SaveGameInstance->DonutRotations.EmplaceAt(1, DonutTwoInWorld->GetActorRotation());
		
		// Save computer local date, time, month, etc.
		SaveGameInstance->PlayerSaveSlotDate = FDateTime::Now();

		// Then save to slot
		UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveGameInstance->PlayerSaveSlotName, SaveGameInstance->UserIndex);
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString(TEXT("Game saved!")), true);

		// Windows .Sav File Paths:
		// If in editor:			\Unreal Projects\{UE4_PROJECT_NAME}\Saved\SaveGames\PlayerSaveSlot.sav
		// If in packaged game:		C:\Users\{YOUR_USERNAME}\AppData\Local\{UE4_PROJECT_NAME}\Saved\SaveGames\PlayerSaveSlot.sav
	}
	else 
	{
		// If the save game object is not found, create a new one
		class UMySaveGame* SaveGameInstance_2 = Cast<UMySaveGame>(UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass()));
		
		if (!SaveGameInstance_2)
			return;
		else
		{
			// Pawn and Donut Stats
			SaveGameInstance_2->PawnHealthInstance = CharacterStats.health;
			SaveGameInstance_2->PawnScoreInstance = CharacterStats.score;
			SaveGameInstance_2->DonutRotations.EmplaceAt(0, DonutOneInWorld->GetActorRotation());
			SaveGameInstance_2->DonutRotations.EmplaceAt(1, DonutTwoInWorld->GetActorRotation());

			// Save computer local date, time, month, etc.
			SaveGameInstance_2->PlayerSaveSlotDate = FDateTime::Now();

			// Then save to slot
			UGameplayStatics::SaveGameToSlot(SaveGameInstance_2, SaveGameInstance_2->PlayerSaveSlotName, SaveGameInstance_2->UserIndex);
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString(TEXT("Game saved!")), true);

			// Windows .Sav File Paths:
			// If in editor:			\Unreal Projects\{UE4_PROJECT_NAME}\Saved\SaveGames\PlayerSaveSlot.sav
			// If in packaged game:		C:\Users\{YOUR_USERNAME}\AppData\Local\{UE4_PROJECT_NAME}\Saved\SaveGames\PlayerSaveSlot.sav
		}
	}
}

// Load the pawn and donut stats we saved
void AMyPawn::InitLoadGame()
{
	// As long as our actors have been found...
	if (this->IsValidLowLevel() && DonutOneInWorld->IsValidLowLevel() && DonutTwoInWorld->IsValidLowLevel())
	{
		// Determine whether to load save data with Game Instance hasReachedCheckpoint variable
		class UMyGameInstance* gInstance = Cast<UMyGameInstance>(GetGameInstance());

		if (gInstance)
		{
			if (gInstance->hasReachedCheckpoint)
			{
				GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, FString(TEXT("Map opened from CHECKPOINT!")), true);

				// Only load game stats if the load .sav file exists
				const FString SaveSlotName = FString(TEXT("PlayerSaveSlot"));

				if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0))
				{
					class UMySaveGame* LoadInstance = Cast<UMySaveGame>(UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass()));

					if (LoadInstance->IsValidLowLevel())
					{
						LoadInstance = Cast<UMySaveGame>(UGameplayStatics::LoadGameFromSlot(LoadInstance->PlayerSaveSlotName, LoadInstance->UserIndex));
						CharacterStats.health = LoadInstance->PawnHealthInstance;
						CharacterStats.score = LoadInstance->PawnScoreInstance;

						if (LoadInstance->DonutRotations.Num() > 1)
						{
							DonutOneInWorld->SetActorRotation(LoadInstance->DonutRotations[0]);
							DonutTwoInWorld->SetActorRotation(LoadInstance->DonutRotations[1]);
						}

						GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Purple, FString(TEXT("Game loaded from save!")), true);
					}
					else 
					{
						// If save game object not found, create a new one
						class UMySaveGame* LoadInstance_2 = Cast<UMySaveGame>(UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass()));

						if (!LoadInstance_2)
							return;
						else
						{
							LoadInstance_2 = Cast<UMySaveGame>(UGameplayStatics::LoadGameFromSlot(LoadInstance_2->PlayerSaveSlotName, LoadInstance_2->UserIndex));
							CharacterStats.health = LoadInstance_2->PawnHealthInstance;
							CharacterStats.score = LoadInstance_2->PawnScoreInstance;

							if (LoadInstance->DonutRotations.Num() > 1)
							{
								DonutOneInWorld->SetActorRotation(LoadInstance_2->DonutRotations[0]);
								DonutTwoInWorld->SetActorRotation(LoadInstance_2->DonutRotations[1]);
							}

							GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Purple, FString(TEXT("Game loaded from save!")), true);
						}
					}
				}
				else
				{
					GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString(TEXT("No save game found.")), true);
				}
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, FString(TEXT("Map opened from START!")), true);
			}
		}
	}
}
#pragma endregion

#pragma region Save/Load Logic [Custom]
// You must write the entire Filepath if writing it in C++. 
// Also use "\\" to separate folder directories.
// For Example: FString SavePath = FString(TEXT("C:\\MyProject\\MySaveDir\\mysavefile.save"));
bool AMyPawn::SaveGameToFile(const FString& FullFilePath)
{
	bool retVal = false;
	
	// Get Donut Rotations
	DonutRots.EmplaceAt(0, DonutOneInWorld->GetActorRotation());
	DonutRots.EmplaceAt(1, DonutTwoInWorld->GetActorRotation());

	// Current Save Date and Time
	SaveGameTime = FDateTime::Now();

	// Variable Data -> Binary
	FBufferArchive ToBinary;
	CustomSaveLoadData(ToBinary, CharacterStats.health, CharacterStats.score, DonutRots, SaveGameTime);

	// If no data
	if (ToBinary.Num() <= 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString(TEXT("File could not be saved")), true);
		retVal = false;
	}
	
	// Binary -> Hard Disk
	if (FFileHelper::SaveArrayToFile(ToBinary, *FullFilePath))
	{
		// Free Binary Array 	
		ToBinary.FlushCache();
		ToBinary.Empty();

		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString(TEXT("Custom Save File Success!")), true);
		retVal = true;
		hasCompletedCustomSaveGame = retVal;
	}

	// Free Binary Array 	
	ToBinary.FlushCache();
	ToBinary.Empty();

	return retVal;
}

bool AMyPawn::LoadGameFromFile(const FString& FullFilePath, float& HealthSaveData, int32& ScoreSaveData, TArray<struct FRotator>& DonutRotatorDataArray, FDateTime &SaveTime)
{
	bool retVal = true;
	
	// Determine whether to load save data with Game Instance hasReachedCheckpoint variable
	class UMyGameInstance* gInstance = Cast<UMyGameInstance>(GetGameInstance());

	if (gInstance)
	{
		if (gInstance->hasReachedCheckpoint)
		{
			// Binary Array Data
			TArray<uint8> BinaryArray;

			// File Load Error
			if (BinaryArray.Num() <= 0)
				retVal = false;

			// Debug Messages
			if (!FFileHelper::LoadFileToArray(BinaryArray, *FullFilePath))
			{
				GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString(TEXT("INVALID FILE!")), true);
				retVal = false;
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Purple, FString(TEXT("Game loaded from CUSTOM SAVE!")), true);
				GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Blue, FString::FromInt(BinaryArray.Num()), true);
				GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Blue, FString(TEXT("Loaded File Size:")), true);
				GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, FString(TEXT("Map opened from CHECKPOINT!")), true);
			}

			// Read the Data Retrieved by GFileManager
			FMemoryReader FromBinary = FMemoryReader(BinaryArray, true); //true, free data after done
			FromBinary.Seek(0);
			CustomSaveLoadData(FromBinary, CharacterStats.health, CharacterStats.score, DonutRots, SaveGameTime);
			
			if (!hasCompletedCustomSaveGame)
				hasCompletedCustomSaveGame = true;

			// Set donut rotations if the array is filled
			if (DonutRots.Num() > 1)
			{
				DonutOneInWorld->SetActorRotation(DonutRots[0]);
				DonutTwoInWorld->SetActorRotation(DonutRots[1]);
			}

			// Clean up, empty, and close buffer 
			FromBinary.FlushCache();
			BinaryArray.Empty();
			FromBinary.Close();
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, FString(TEXT("Map opened from START!")), true);
		}
	}

	return retVal;
}

void AMyPawn::CustomSaveLoadData(FArchive& Ar, float& HealthSaveData, int32& ScoreSaveData, TArray<struct FRotator>& DonutRotatorDataArray, FDateTime& SaveTime)
{
	// "<<" will place your data into an archived binary format. It can also mean getting data out of the archive and assiging it to a variable.
	Ar << HealthSaveData; // float = 4 bytes
	Ar << ScoreSaveData; // int32 = 4 bytes
	Ar << DonutRotatorDataArray; // FRotator = Up to 3 x float bytes (We are using 2 in our array) 
	Ar << SaveTime; // Tested only on Windows PC
}
#pragma endregion

#pragma region Resets
// Reload Current Level
void AMyPawn::ResetLevel()
{
	if (!canSaveGame)
		UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), true);
}
#pragma endregion

