// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnCreditChanged, AActor*, InstigatorActor, ASPlayerState*, PlayerState, int, NewCreditScore, int, Delta);

class USSaveGame;

/**
 * 
 */
UCLASS()
class TOMLOOMANCPPPROJECT_API ASPlayerState : public APlayerState
{
	GENERATED_BODY()

protected:

	UPROPERTY(Replicated)
	int32 Credits;

public:

	ASPlayerState();

	UFUNCTION(BlueprintPure)
	int GetCredits() const;	

	UFUNCTION(Client, Reliable)
	void ClientCreditChanged(AActor* InstigatorActor, ASPlayerState* PlayerState, int NewCreditScore, int Delta);

	UFUNCTION(BlueprintCallable)
	void AddCredits(AActor* InstigatorActor, int CreditsToAdd);

	UPROPERTY(BlueprintAssignable)
	FOnCreditChanged OnCreditChanged;

	UFUNCTION(BlueprintNativeEvent)
	void SavePlayerState(USSaveGame* SaveObject);

	UFUNCTION(BlueprintNativeEvent)
	void LoadPlayerState(USSaveGame* SaveObject);
};
