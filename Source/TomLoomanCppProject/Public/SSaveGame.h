// Copyright Gargowle. All Rights Reserved.

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
	FName ActorName;

	// For movable Actors, keep location, rotation, scale.
	UPROPERTY()
	FTransform Transform;

	// For serialized data (empty array of bytes), contains all "SaveGame" marked variables of the Actor
	UPROPERTY()
	TArray<uint8> ByteData;
};

USTRUCT()
struct FPlayerSaveData
{
	GENERATED_BODY()

public:

	/* Player ID as defined by the online sub system converted to FString for simplicity */
	UPROPERTY()
	FString PlayerId;

	UPROPERTY()
	int32 Credits;

	/* Longest survival time */
	UPROPERTY()
	float PersonalRecordTime;

	/* Location if player was alive during save */
	UPROPERTY()
	FVector Location;

	/* Orientation if player was alive during save */
	UPROPERTY()
	FRotator Rotation;

	/* We do not always want to restore location, and may just resume player at specific respawn point in the world */
	UPROPERTY()
	bool bResumeAtTransform;
};

/**
 * 
 */
UCLASS()
class TOMLOOMANCPPPROJECT_API USSaveGame : public USaveGame
{
	GENERATED_BODY()

public:

	UPROPERTY()
	TArray<FPlayerSaveData> SavedPlayers;

	/* Actors stored from a level (currently does not support a specific level and just assumes the demo map) */
	UPROPERTY()
	TArray<FActorSaveData> SavedActors;

	FPlayerSaveData* GetPlayerData(APlayerState* PlayerState);
};
