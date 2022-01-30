// Fill out your copyright notice in the Description page of Project Settings.


#include "SSaveGame.h"
#include "GameFramework/PlayerState.h"

FPlayerSaveData* USSaveGame::GetPlayerData(APlayerState* PlayerState)
{
	if(PlayerState == nullptr)
	{
		return nullptr;
	}

	// Will not give unique ID while PIE so we skip that step while testing in editor.
	// UObjects do not have access to UWorld, so it must be grabbed via PlayerState instead
	if(PlayerState->GetWorld()->IsPlayInEditor())
	{
		UE_LOG(LogTemp, Log, TEXT("During PIE we cannot use PlayerID to retrieve Saved Player data. Using first entry in array if available."))

		if (SavedPlayers.IsValidIndex(0))
		{
			return &SavedPlayers[0];
		}

		// No saved player data available
		return nullptr;
	}

	// Easiest way to deal with the different IDs is with FString data type (original Steam ID is uint64)
	// Keep in mind that GetUniqueId() returns the online ID, where GetUniqueID is a function from UObject (confusing!)
	FString PlayerId = PlayerState->GetUniqueId().ToString();
	// Iterate the array and match by PlayerId (e.g. unique ID provided by Steam)
	return SavedPlayers.FindByPredicate([&](const FPlayerSaveData& Data) {return Data.PlayerId == PlayerId; });
}
