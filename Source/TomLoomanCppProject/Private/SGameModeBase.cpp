// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameModeBase.h"

#include "DrawDebugHelpers.h"
#include "EngineUtils.h"
#include "SAttributeComponent.h"
#include "SCharacter.h"
#include "SPlayerState.h"
#include "AI/SAICharacter.h"
#include "EnvironmentQuery/EnvQueryManager.h"


static TAutoConsoleVariable<bool> CVarSpawnBots(TEXT("su.SpawnBots"), true, TEXT("Enable spawning of bots via timer."), ECVF_Cheat);
static TAutoConsoleVariable<bool> CVarDrawDebugSphereOnBotSpawn(TEXT("su.DrawDebugSphereOnBotSpawn"), false, TEXT("Enable debug spheres at spawn location."), ECVF_Cheat);

ASGameModeBase::ASGameModeBase()
{
	SpawnTimerInterval = 2.0f;
	CreditsPerKill = 5;
	NrOfCoinsToSpawnAtGameStart = 4;
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
		GetWorld()->SpawnActor<AActor>(MinionClass, Locations[0], FRotator::ZeroRotator);

		// Track all the used spawn locations
		if(CVarDrawDebugSphereOnBotSpawn.GetValueOnGameThread())
		{
			DrawDebugSphere(GetWorld(), Locations[0], 50.0f, 16, FColor::Blue, false, 60.0f);
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
		ASPlayerState* KillerPlayerState = Cast<ASPlayerState>(KillerPlayer->GetPlayerState());
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
