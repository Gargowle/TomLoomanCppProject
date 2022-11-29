// Copyright Gargowle. All Rights Reserved.


#include "SGameModeBase.h"

#include "DrawDebugHelpers.h"
#include "EngineUtils.h"
#include "SActionComponent.h"
#include "SAttributeComponent.h"
#include "SCharacter.h"
#include "SMonsterData.h"
#include "SPlayerState.h"
#include "AI/SAICharacter.h"
#include "Engine/AssetManager.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "TomLoomanCppProject/TomLoomanCppProject.h"
#include "SSaveGameSubsystem.h"


static TAutoConsoleVariable<bool> CVarSpawnBots(TEXT("su.SpawnBots"), true, TEXT("Enable spawning of bots via timer."), ECVF_Cheat);
static TAutoConsoleVariable<bool> CVarDrawDebugSphereOnBotSpawn(TEXT("su.DrawDebugSphereOnBotSpawn"), false, TEXT("Enable debug spheres at spawn location."), ECVF_Cheat);

ASGameModeBase::ASGameModeBase()
{
	SpawnTimerInterval = 2.0f;
	CreditsPerKill = 5;
	CooldownTimeBetweenFailures = 8.0f;
	NrOfCoinsToSpawnAtGameStart = 4;

	PlayerStateClass = ASPlayerState::StaticClass();
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

	if(CooldownBotSpawnUntil > GetWorld()->TimeSeconds)
	{
		// Still cooling down
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

	const float MaxBotCount = 40.0f;

	if (NrOfAliveBots >= MaxBotCount)
	{
		UE_LOG(LogTemp, Log, TEXT("At maximum bot capacity. Skipping bot spawn."));
		return;
	}

	// Give points to spend
	if(SpawnCreditCurve)
	{
		AvailableSpawnCredit += SpawnCreditCurve->GetFloatValue(GetWorld()->TimeSeconds);
	}

	if(MonsterTable)
	{
		TArray<FMonsterInfoRow*> Rows;
		MonsterTable->GetAllRows("", Rows);

		// Get total weight
		float TotalWeight = 0;
		for(FMonsterInfoRow* Entry : Rows)
		{
			TotalWeight += Entry->Weight;
		}

		// Reset
		TotalWeight = 0;
		SelectedMonsterRow = nullptr;

		// get random enemy
		const float RandomWeight = FMath::RandRange(0.0f, TotalWeight);
		for(FMonsterInfoRow* Entry : Rows)
		{
			TotalWeight += Entry->Weight;

			if(RandomWeight <= TotalWeight)
			{
				SelectedMonsterRow = Entry;
				break;
			}
		}

		if(!SelectedMonsterRow || SelectedMonsterRow->SpawnCost >= AvailableSpawnCredit)
		{
			// Too expensive to spawn, try again soon
			CooldownBotSpawnUntil = GetWorld()->TimeSeconds + CooldownTimeBetweenFailures;

			LogOnScreen(this, FString::Printf(TEXT("Spawn cooling down until: %f"), CooldownBotSpawnUntil), FColor::Red);
			return;
		}
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
		UAssetManager* Manager = UAssetManager::GetIfValid();
		if(Manager && SelectedMonsterRow)
		{
			//Apply spawn cost
			AvailableSpawnCredit -= SelectedMonsterRow->SpawnCost;
			
			LogOnScreen(this, TEXT("Loading monster ..."), FColor::Green);

			TArray<FName> Bundles; // emtpy TArray because no bundles are defined
			FStreamableDelegate Delegate = FStreamableDelegate::CreateUObject(this, &ASGameModeBase::OnMonsterLoaded, SelectedMonsterRow->MonsterId, Locations[0]);
			Manager->LoadPrimaryAsset(SelectedMonsterRow->MonsterId, Bundles, Delegate);
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
		// Store time if it was better than previous record
		ASPlayerState* PS = VictimPlayer->GetPlayerState<ASPlayerState>();
		if(ensure(PS))
		{
			PS->UpdatePersonalRecord(GetWorld()->TimeSeconds);
		}

		// Immediately auto save on death
		USSaveGameSubsystem* SG = GetGameInstance()->GetSubsystem<USSaveGameSubsystem>();
		SG->WriteSaveGame();

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

void ASGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	// Save/Load logic is in SaveGameSubsystem
	USSaveGameSubsystem* SG = GetGameInstance()->GetSubsystem<USSaveGameSubsystem>();

	// Optional slot name (falls back to slot specified in USSaveGameSettings class / project settings / ini
	FString SelectedSaveSlot = UGameplayStatics::ParseOption(Options, "SaveGame");
	SG->LoadSaveGame(SelectedSaveSlot);
}

void ASGameModeBase::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	USSaveGameSubsystem* SG = GetGameInstance()->GetSubsystem<USSaveGameSubsystem>();
	SG->HandleStartingNewPlayer(NewPlayer);

	Super::HandleStartingNewPlayer_Implementation(NewPlayer);

	// Now we are ready to override spawn location.
	// Alternatively, we could override core spawn location to use store locations immediately (skipping the whole 'find player start' logic)
	SG->OverrideSpawnTransform(NewPlayer);
}
