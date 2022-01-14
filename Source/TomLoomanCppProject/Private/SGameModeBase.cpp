// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameModeBase.h"

#include "DrawDebugHelpers.h"
#include "EngineUtils.h"
#include "SActionComponent.h"
#include "SAttributeComponent.h"
#include "SCharacter.h"
#include "SGameplayInterface.h"
#include "SMonsterData.h"
#include "SPlayerState.h"
#include "SSaveGame.h"
#include "AI/SAICharacter.h"
#include "Engine/AssetManager.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "TomLoomanCppProject/TomLoomanCppProject.h"


static TAutoConsoleVariable<bool> CVarSpawnBots(TEXT("su.SpawnBots"), true, TEXT("Enable spawning of bots via timer."), ECVF_Cheat);
static TAutoConsoleVariable<bool> CVarDrawDebugSphereOnBotSpawn(TEXT("su.DrawDebugSphereOnBotSpawn"), false, TEXT("Enable debug spheres at spawn location."), ECVF_Cheat);

ASGameModeBase::ASGameModeBase()
{
	SpawnTimerInterval = 2.0f;
	CreditsPerKill = 5;
	NrOfCoinsToSpawnAtGameStart = 4;

	PlayerStateClass = ASPlayerState::StaticClass();

	SlotName = "SaveGame01";
}

void ASGameModeBase::StartPlay()
{
	Super::StartPlay();

	GetWorldTimerManager().SetTimer(TimerHandle_SpawnBots, this, &ASGameModeBase::SpawnBotTimerElapsed, SpawnTimerInterval, true);

	// spawn coins in level
	UEnvQueryInstanceBlueprintWrapper* CoinSpawnQueryInstance = UEnvQueryManager::RunEQSQuery(this, CoinPlacementAtGameStartQuery, this, EEnvQueryRunMode::AllMatching, nullptr);
	if(ensure(CoinSpawnQueryInstance))
	{
		CoinSpawnQueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &ASGameModeBase::OnSpawnCoinsQueryCompleted);
	}
}

void ASGameModeBase::KillAll()
{
	for (TActorIterator<ASAICharacter> It(GetWorld()); It; ++It)
	{
		ASAICharacter* Bot = *It;
		USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(Bot);
		if (AttributeComp && AttributeComp->IsAlive())
		{
			AttributeComp->Kill(this); // @fixme: pass in player? for kill credit
		}
	}
}

void ASGameModeBase::SpawnBotTimerElapsed()
{
	if(!CVarSpawnBots.GetValueOnGameThread())
	{
		UE_LOG(LogTemp, Warning, TEXT("Bot spawning disabled via cvar 'CVarSpawnBots'."));
		return;
	}
	// check if full bot capacity is reached already. If reached, return before query is actually made
	int32 NrOfAliveBots = 0;
	for (TActorIterator<ASAICharacter> It(GetWorld()); It; ++It)
	{
		ASAICharacter* Bot = *It;
		USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(Bot);
		if (AttributeComp && AttributeComp->IsAlive())
		{
			++NrOfAliveBots;
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Found %i alive bots."), NrOfAliveBots);

	float MaxBotCount = 10.0f;
	if (DifficultyCurve)
	{
		MaxBotCount = DifficultyCurve->GetFloatValue(GetWorld()->GetTimeSeconds());
	}

	if (NrOfAliveBots >= MaxBotCount)
	{
		UE_LOG(LogTemp, Log, TEXT("At maximum bot capacity. Skipping bot spawn."));
		return;
	}

	//last entry does not need to be specified (only if we want to specify the return class); "Tom Looman never did this in his life"
	UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(this, SpawnBotQuery, this, EEnvQueryRunMode::RandomBest5Pct, nullptr);

	if(ensure(QueryInstance))
	{
		QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &ASGameModeBase::OnSpawnBotQueryCompleted);
	}	
		
}

void ASGameModeBase::OnSpawnBotQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	
	if(QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawn bot EQS Query has failed!"));
		return;
	}

	TArray<FVector> Locations;
	QueryInstance->GetQueryResultsAsLocations(Locations);

	if(Locations.IsValidIndex(0))
	{
		if(MonsterTable)
		{
			TArray<FMonsterInfoRow*> Rows;
			MonsterTable->GetAllRows("", Rows);
			
			// get random enemy
			const int32 RandomIndex = FMath::RandRange(0, Rows.Num() - 1);
			const FMonsterInfoRow* SelectedRow = Rows[RandomIndex];

			UAssetManager* Manager = UAssetManager::GetIfValid();
			if(Manager)
			{
				LogOnScreen(this, TEXT("Loading monster ..."), FColor::Green);

				TArray<FName> Bundles; // emtpy TArray because no bundles are defined
				FStreamableDelegate Delegate = FStreamableDelegate::CreateUObject(this, &ASGameModeBase::OnMonsterLoaded, SelectedRow->MonsterId, Locations[0]);
				Manager->LoadPrimaryAsset(SelectedRow->MonsterId, Bundles, Delegate);
			}

		}


		// Track all the used spawn locations
		if(CVarDrawDebugSphereOnBotSpawn.GetValueOnGameThread())
		{
			DrawDebugSphere(GetWorld(), Locations[0], 50.0f, 16, FColor::Blue, false, 60.0f);
		}
	}
}

void ASGameModeBase::OnMonsterLoaded(FPrimaryAssetId LoadedId, FVector SpawnLocation)
{
	LogOnScreen(this, TEXT("Finished loading."), FColor::Green);

	UAssetManager* Manager = UAssetManager::GetIfValid();
	if (Manager)
	{
		USMonsterData* MonsterData = Cast<USMonsterData>(Manager->GetPrimaryAssetObject(LoadedId));
		if(MonsterData)
		{
			AActor* NewBot = GetWorld()->SpawnActor<AActor>(MonsterData->MonsterClass, SpawnLocation, FRotator::ZeroRotator);
			if (NewBot)
			{
				LogOnScreen(this, FString::Printf(TEXT("Spawned enemy: %s (%s)"), *GetNameSafe(NewBot), *GetNameSafe(MonsterData)));

				// Grant special actions, buffs etc.
				USActionComponent* ActionComp = Cast<USActionComponent>(NewBot->GetComponentByClass(USActionComponent::StaticClass()));
				if (ActionComp)
				{
					for (TSubclassOf<USAction> ActionClass : MonsterData->Actions)
					{
						ActionComp->AddAction(NewBot, ActionClass);
					}
				}
			}
		}
	}
}

void ASGameModeBase::OnActorKilled(AActor* VictimActor, AActor* Killer)
{
	UE_LOG(LogTemp, Log, TEXT("OnActorKilled: Victim: %s, Killer: %s"), *GetNameSafe(VictimActor), *GetNameSafe(Killer));

	// if player is killed
	ASCharacter* VictimPlayer = Cast <ASCharacter>(VictimActor);
	if (VictimPlayer)
	{
		FTimerHandle TimerHandle_Respawn;

		FTimerDelegate Delegate_Respawn;
		Delegate_Respawn.BindUFunction(this, TEXT("RespawnPlayerElapsed"), VictimPlayer->GetController());

		float RespawnDelay = 2.0f;
		GetWorldTimerManager().SetTimer(TimerHandle_Respawn, Delegate_Respawn, RespawnDelay, false);

		return;
	}

	// if anyone killed by player
	ASCharacter* KillerPlayer = Cast <ASCharacter>(Killer);
	if(KillerPlayer)
	{
		ASPlayerState* KillerPlayerState = KillerPlayer->GetPlayerState<ASPlayerState>();
		if(ensureMsgf(KillerPlayerState, TEXT("This framework needs the ASPlayerState class to be used. However, some other class has been chosen.")))
		{
			KillerPlayerState->AddCredits(VictimActor, CreditsPerKill);
		}
	}

}

void ASGameModeBase::RespawnPlayerElapsed(AController* Controller)
{
	if(ensure(Controller))
	{
		Controller->UnPossess();

		RestartPlayer(Controller);
	}
}

void ASGameModeBase::OnSpawnCoinsQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	if(QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawn coin at game start EQS Query has failed!"));
		return;		
	}

	TArray<FVector> Locations;
	QueryInstance->GetQueryResultsAsLocations(Locations);

	FVector HeightOffset = FVector::ZeroVector;
	HeightOffset.Z = 50;

	for(int Index = 0; Index < NrOfCoinsToSpawnAtGameStart; ++Index)
	{
		if(Locations.IsValidIndex(Index))
		{

			GetWorld()->SpawnActor<AActor>(CoinClass, Locations[Index] + HeightOffset, FRotator::ZeroRotator);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Spawn coin EQS could only deliver %i of %i requested legitimate spawn points"), Index, NrOfCoinsToSpawnAtGameStart);
		}
	}
}

void ASGameModeBase::WriteSaveGame()
{
	// just iterate all player states as we don't have proper IDs to match yet (Steam accounts, ...)
	for(APlayerState* APS : GameState->PlayerArray)
	{
		ASPlayerState* PS = Cast<ASPlayerState>(APS);
		if(ensure(PS))
		{
			PS->SavePlayerState(CurrentSaveGame);
			break; // single player only at this point
		}

	}

	// Clear the list (from previous save game)
	CurrentSaveGame->SavedActors.Empty();

	// Iterate over the entire world of actors
	for(FActorIterator It(GetWorld()); It; ++It)
	{
		AActor* Actor = *It;

		// only interested in our "gameplay actors"
		if (!Actor->Implements<USGameplayInterface>())
		{
			continue;
		}

		FActorSaveData ActorData;
		ActorData.ActorName = Actor->GetName();
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

	UGameplayStatics::SaveGameToSlot(CurrentSaveGame, SlotName, 0);
}

void ASGameModeBase::LoadSaveGame()
{
	// Slot Name: name of the save file
	// UserIndex: relevant for PS /XBox with several users connected (game pads)
	if(UGameplayStatics::DoesSaveGameExist(SlotName, 0))
	{
		CurrentSaveGame = Cast<USSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));
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

			// only interested in our "gameplay actors"
			if (!Actor->Implements<USGameplayInterface>())
			{
				continue;
			}

			for (FActorSaveData ActorData : CurrentSaveGame->SavedActors)
			{
				if(ActorData.ActorName == Actor->GetName())
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
	}
	else
	{
		// currently it makes sense to put this here since this method is triggered at game start either way.
		// So the first time the game is started, a save game will be created.
		CurrentSaveGame = Cast<USSaveGame>(UGameplayStatics::CreateSaveGameObject(USSaveGame::StaticClass()));

		UE_LOG(LogTemp, Log, TEXT("Created New SaveGame data."));
	}

}

void ASGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	LoadSaveGame();
}

void ASGameModeBase::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	ASPlayerState* PS = NewPlayer->GetPlayerState<ASPlayerState>();
	if(PS)
	{
		PS->LoadPlayerState(CurrentSaveGame);
	}

	Super::HandleStartingNewPlayer_Implementation(NewPlayer);
}
