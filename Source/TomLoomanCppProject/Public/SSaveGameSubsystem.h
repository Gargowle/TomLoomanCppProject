// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SSaveGameSubsystem.generated.h"

class USSaveGame;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSaveGameSignature, class USSaveGame*, SaveObject);

/**
 * 
 */
UCLASS(meta=(DisplayName = "SaveGame System"))
class TOMLOOMANCPPPROJECT_API USSaveGameSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

protected:
	/* Name of slot to save/load to disk. Filled by SaveGameSettings but can be overridden via InitGame() options string */
	FString CurrentSlotName;

	UPROPERTY()
	USSaveGame* CurrentSaveGame;

public:

	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	void WriteSaveGame();

	/* Load from disk, optional slot name*/
	void LoadSaveGame(FString InSlotName=TEXT(""));

	/* Change slot name that next time will be used to load/save data */
	UFUNCTION(BlueprintCallable)
	void SetSlotName(FString NewSlotName);

	/* Restore serialized data from PlayerState into player */
	void HandleStartingNewPlayer(AController* NewPlayer);

	UFUNCTION(BlueprintCallable)
	bool OverrideSpawnTransform(AController* NewPlayer);

	UPROPERTY(BlueprintAssignable)
	FOnSaveGameSignature OnSaveGameLoaded;

	UPROPERTY(BlueprintAssignable)
	FOnSaveGameSignature OnSaveGameWritten;

	/* Initialize this subsystem; good moment to load in SaveGameSettings variables*/
	void Initialize(FSubsystemCollectionBase& Collection) override;
};
