// Copyright Gargowle. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SPowerUp.h"
#include "SCreditPowerUp.generated.h"

/**
 * 
 */
UCLASS()
class TOMLOOMANCPPPROJECT_API ASCreditPowerUp : public ASPowerUp
{
	GENERATED_BODY()

	ASCreditPowerUp();

protected:
	
	UPROPERTY(EditAnywhere)
	int CreditsToAdd;
	
	virtual void ApplyEffect(APawn* InstigatorPawn) override;

	virtual bool IsInstigatorEligible(APawn* InstigatorPawn) override;
};
