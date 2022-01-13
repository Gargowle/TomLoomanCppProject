// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SPowerUp.h"
#include "SHealthPotionPowerUp.generated.h"

/**
 * 
 */
UCLASS()
class TOMLOOMANCPPPROJECT_API ASHealthPotionPowerUp : public ASPowerUp
{
	GENERATED_BODY()

	ASHealthPotionPowerUp();

protected:

	UPROPERTY(EditAnywhere)
	float HealingAmount;

	UPROPERTY(EditAnywhere)
	int CreditCosts;
	
	virtual void ApplyEffect(APawn* InstigatorPawn) override;

	virtual bool IsInstigatorEligible(APawn * InstigatorPawn) override;

	virtual FText GetInteractText_Implementation(APawn* InstigatorPawn) override;
	
};
