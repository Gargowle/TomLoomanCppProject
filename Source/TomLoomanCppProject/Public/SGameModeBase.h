// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "GameFramework/GameModeBase.h"
#include "SGameModeBase.generated.h"

class UEnvQuery;
class UEnvQueryInstanceBlueprintWrapper;
/**
 * 
 */
UCLASS()
class TOMLOOMANCPPPROJECT_API ASGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:

	ASGameModeBase();

	virtual void StartPlay() override;

	UFUNCTION(Exec)
	void KillAll();
	
	virtual void OnActorKilled(AActor* VictimActor, AActor* Killer);

protected:

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TSubclassOf<AActor> MinionClass;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UEnvQuery* SpawnBotQuery;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UCurveFloat* DifficultyCurve;

	FTimerHandle TimerHandle_SpawnBots;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float SpawnTimerInterval;

	UFUNCTION()
	void SpawnBotTimerElapsed();

	UFUNCTION()
	void OnSpawnBotQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);

	UFUNCTION()
	void RespawnPlayerElapsed(AController* Controller);

	UPROPERTY(EditDefaultsOnly, Category = "GameSettings")
	int CreditsPerKill;

	UPROPERTY(EditDefaultsOnly, Category = "GameSettings")
	UEnvQuery* CoinPlacementAtGameStartQuery;

	UPROPERTY(EditDefaultsOnly, Category = "GameSettings")
	int NrOfCoinsToSpawnAtGameStart;

	UPROPERTY(EditDefaultsOnly, Category = "GameSettings")
	TSubclassOf<AActor> CoinClass;

	UFUNCTION()
	void OnSpawnCoinsQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);
};
