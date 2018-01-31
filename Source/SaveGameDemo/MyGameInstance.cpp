// Fill out your copyright notice in the Description page of Project Settings.

#include "MyGameInstance.h"
#include "SaveGameDemo.h"

// Sets default values
UMyGameInstance::UMyGameInstance()
{
	hasReachedCheckpoint = false;
	FirstPlayerStartName = FName(TEXT("NewStart"));
	CheckpointPlayerStartName = FName(TEXT("Checkpoint"));
}



