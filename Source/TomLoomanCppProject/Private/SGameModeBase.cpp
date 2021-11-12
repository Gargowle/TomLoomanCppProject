// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameModeBase.h"

#include "DrawDebugHelpers.h"
#include "EngineUtils.h"
#include "SAttributeComponent.h"
#include "AI/SAICharacter.h"
#include "EnvironmentQuery/EnvQueryManager.h"

ASGameModeBase::ASGameModeBase()
{
	SpawnTimerInterval = 2.0f;
}

void ASGameModeBase::StartPlay()
{
	Super::StartPlay();

	GetWorldTimerManager().SetTimer(TimerHandle_SpawnBots, this, &ASGameModeBase::SpawnBotTimerElapsed, SpawnTimerInterval, true);
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
		QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &ASGameModeBase::OnQueryCompleted);
	}	
		
}

void ASGameModeBase::OnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	
	if(QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawn bot EQS Query has failed!"));
		return;
	}

	TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();

	if(Locations.IsValidIndex(0))
	{
		GetWorld()->SpawnActor<AActor>(MinionClass, Locations[0], FRotator::ZeroRotator);

		// Track all the used spawn locations
		DrawDebugSphere(GetWorld(), Locations[0], 50.0f, 16, FColor::Blue, false,  60.0f);
	}
}
