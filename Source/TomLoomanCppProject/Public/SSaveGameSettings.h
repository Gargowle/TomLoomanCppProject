// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "SSaveGameSettings.generated.h"

/**
 * 
 */
UCLASS(Config="Game", defaultconfig, meta = (DisplayName = "Save Game Settings")) // 'defaultconfig' = "Save object config only to Default INIs, not to local INIs."
class TOMLOOMANCPPPROJECT_API USSaveGameSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	/* Default slot name (changeable in the project settings) if not specified by the game's UI*/
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "General")
	FString SaveSlotName;

	USSaveGameSettings();
};
