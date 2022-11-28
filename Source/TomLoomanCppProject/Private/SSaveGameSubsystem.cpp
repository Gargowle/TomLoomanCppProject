// Fill out your copyright notice in the Description page of Project Settings.


#include "SSaveGameSubsystem.h"

#include "EngineUtils.h"
#include "SGameplayInterface.h"
#include "SPlayerState.h"
#include "SSaveGame.h"
#include "SSaveGameSettings.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"


void USSaveGameSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// get default slot name for slot name from project settings
	const USSaveGameSettings* SGSettings = GetDefault<USSaveGameSettings>();
	CurrentSlotName = SGSettings->SaveSlotName;
}

void USSaveGameSubsystem::SetSlotName(FString NewSlotName)
{
	// Ignore empty name
	if (NewSlotName.Len() == 0) return;

	CurrentSlotName = NewSlotName;
}


void USSaveGameSubsystem::WriteSaveGame()
{
	// Clear arrays, as they may contain data from previously loaded SaveGame
	CurrentSaveGame->SavedPlayers.Empty();
	CurrentSaveGame->SavedActors.Empty();

	AGameStateBase* GS = GetWorld()->GetGameState();
	if (GS == nullptr)
	{
		// TODO: warn about failure to save
		return;
	}

	// just iterate all player states as we don't have proper IDs to match yet (Steam accounts, ...)
	for (APlayerState* APS : GS->PlayerArray)
	{
		ASPlayerState* PS = Cast<ASPlayerState>(APS);
		if (ensure(PS))
		{
			PS->SavePlayerState(CurrentSaveGame);
			break; // single player only at this point
		}
	}

	// Iterate over the entire world of actors
	for (FActorIterator It(GetWorld()); It; ++It)
	{
		AActor* Actor = *It;

		// only interested in our "gameplay actors"
		if (!Actor->Implements<USGameplayInterface>())
		{
			continue;
		}

		FActorSaveData ActorData;
		ActorData.ActorName = Actor->GetFName();
		ActorData.Transform = Actor->GetActorTransform();

		// Pass the array to fill with data from actor
		FMemoryWriter MemWriter(ActorData.ByteData);

		FObjectAndNameAsStringProxyArchive Ar(MemWriter, true);

		// Find only variables with UPROPERTY(SaveGame)
		Ar.ArIsSaveGame = true;

		// Converts Actor's SaveGame UPROPERTIES into binary array
		Actor->Serialize(Ar);

		CurrentSaveGame->SavedActors.Add(ActorData);
	}

	UGameplayStatics::SaveGameToSlot(CurrentSaveGame, CurrentSlotName, 0);

	OnSaveGameWritten.Broadcast(CurrentSaveGame);
}

void USSaveGameSubsystem::LoadSaveGame(FString InSlotName)
{
	SetSlotName(InSlotName);

	// current Slot Name: name of the save file
	// UserIndex: relevant for PS /XBox with several users connected (game pads)
	if (UGameplayStatics::DoesSaveGameExist(CurrentSlotName, 0))
	{
		CurrentSaveGame = Cast<USSaveGame>(UGameplayStatics::LoadGameFromSlot(CurrentSlotName, 0));
		if (CurrentSaveGame == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to load SaveGame data."));
			return;
		}

		UE_LOG(LogTemp, Log, TEXT("Loaded SaveGame Data."));

		// Iterate over the entire world of actors
		for (FActorIterator It(GetWorld()); It; ++It)
		{
			AActor* Actor = *It;

			// only interested in our "gameplay actors", skip actors that are being destroyed
			if (!IsValid(Actor) || !Actor->Implements<USGameplayInterface>())
			{
				continue;
			}

			for (FActorSaveData ActorData : CurrentSaveGame->SavedActors)
			{
				if (ActorData.ActorName == Actor->GetFName())
				{
					Actor->SetActorTransform(ActorData.Transform);

					FMemoryReader MemReader(ActorData.ByteData);
					FObjectAndNameAsStringProxyArchive Ar(MemReader, true);

					// Consider only variables with UPROPERTY(SaveGame)
					Ar.ArIsSaveGame = true;

					// Convert binary array back into actor's variables
					Actor->Serialize(Ar);

					ISGameplayInterface::Execute_OnActorLoaded(Actor);

					break;
				}
			}
		}

		OnSaveGameLoaded.Broadcast(CurrentSaveGame);
	}
	else
	{
		// currently it makes sense to put this here since this method is triggered at game start either way.
		// So the first time the game is started, a save game will be created.
		CurrentSaveGame = Cast<USSaveGame>(UGameplayStatics::CreateSaveGameObject(USSaveGame::StaticClass()));

		UE_LOG(LogTemp, Log, TEXT("Created New SaveGame data."));
	}
}

void USSaveGameSubsystem::HandleStartingNewPlayer(AController* NewPlayer)
{
	ASPlayerState* PS = NewPlayer->GetPlayerState<ASPlayerState>();
	if (ensure(PS))
	{
		PS->LoadPlayerState(CurrentSaveGame);
	}
}

bool USSaveGameSubsystem::OverrideSpawnTransform(AController* NewPlayer)
{
	if (NewPlayer == nullptr || !IsValid(NewPlayer))
	{
		return false;
	}

	APlayerState* PS = NewPlayer->GetPlayerState<APlayerState>();
	if (PS == nullptr)
	{
		return false;
	}

	if (APawn* MyPawn = PS->GetPawn())
	{
		FPlayerSaveData* FoundData = CurrentSaveGame->GetPlayerData(PS);
		if (FoundData && FoundData->bResumeAtTransform)
		{
			MyPawn->SetActorLocation(FoundData->Location);
			MyPawn->SetActorRotation(FoundData->Rotation);

			// PlayerState owner is a Player Controller
			AController* PC = Cast<AController>(PS->GetOwner());
			// Set control rotation to change camera direction, setting Pawn rotation is not enough
			PC->SetControlRotation(FoundData->Rotation);

			return true;
		}
	}

	return false;
}
