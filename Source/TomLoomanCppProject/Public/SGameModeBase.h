// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "GameFramework/GameModeBase.h"
#include "SGameModeBase.generated.h"

class UEnvQuery;
class UEnvQueryInstanceBlueprintWrapper;
class USSaveGame;


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

	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	void WriteSaveGame();

	void LoadSaveGame();

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

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

	FString SlotName; // to be deleted once there is proper UI to choose a slot

	UPROPERTY()
	USSaveGame* CurrentSaveGame;

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
