// Copyright Gargowle. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SPlayerState.generated.h"

// Forward declare ASPlayerstate for use in declare delegate macros
class ASPlayerState;

// Event handler for credits
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnCreditChanged, AActor*, InstigatorActor, ASPlayerState*, PlayerState, int, NewCreditScore, int, Delta);
// Event handler for personal record time
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnRecordTimeChanged, ASPlayerState*, PlayerState, float, NewTime, float, OldRecord);

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

	UPROPERTY(BlueprintReadOnly)
	float PersonalRecordTime;

public:

	ASPlayerState();

	UFUNCTION(BlueprintPure)
	int GetCredits() const;	

	UFUNCTION(Client, Reliable)
	void ClientCreditChanged(AActor* InstigatorActor, ASPlayerState* PlayerState, int NewCreditScore, int Delta);

	UFUNCTION(BlueprintCallable)
	void SetCredits(AActor* InstigatorActor, int CreditsToSet);

	UFUNCTION(BlueprintCallable)
	void AddCredits(AActor* InstigatorActor, int CreditsToAdd);

	UPROPERTY(BlueprintAssignable)
	FOnCreditChanged OnCreditChanged;

	UPROPERTY(BlueprintAssignable)
	FOnRecordTimeChanged OnRecordTimeChanged;

	UFUNCTION(BlueprintNativeEvent)
	void SavePlayerState(USSaveGame* SaveObject);

	UFUNCTION(BlueprintNativeEvent)
	void LoadPlayerState(USSaveGame* SaveObject);

	/* Checks current record and only sets if better time was passed in. */
	UFUNCTION(BlueprintCallable)
	bool UpdatePersonalRecord(float NewTime);
};
