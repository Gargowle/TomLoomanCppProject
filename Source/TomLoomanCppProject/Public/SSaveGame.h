// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SSaveGame.generated.h"


USTRUCT()
struct FActorSaveData
{
	GENERATED_BODY()

public:

	// Identifier which Actor this belongs to
	UPROPERTY()
	FString ActorName;

	// For movable Actors, keep location, rotation, scale.
	UPROPERTY()
	FTransform Transform;

	// For serialized data (empty array of bytes)
	UPROPERTY()
	TArray<uint8> ByteData;
};

/**
 * 
 */
UCLASS()
class TOMLOOMANCPPPROJECT_API USSaveGame : public USaveGame
{
	GENERATED_BODY()

public:

	UPROPERTY() // such that it can be found by the saving system
	int32 Credits;

	UPROPERTY()
	TArray<FActorSaveData> SavedActors;

};
