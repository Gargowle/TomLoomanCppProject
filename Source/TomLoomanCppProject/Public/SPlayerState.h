// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnCreditChanged, AActor*, InstigatorActor, ASPlayerState*, PlayerState, int, NewCreditScore, int, Delta);

/**
 * 
 */
UCLASS()
class TOMLOOMANCPPPROJECT_API ASPlayerState : public APlayerState
{
	GENERATED_BODY()

private:
	
	int Credits;

public:

	UFUNCTION(BlueprintPure)
	int GetCredits() const;	

	UFUNCTION(BlueprintCallable)
	void AddCredits(AActor* InstigatorActor, int CreditsToAdd);

	UPROPERTY(BlueprintAssignable)
	FOnCreditChanged OnCreditChanged;
};
