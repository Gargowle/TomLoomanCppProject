// Copyright Gargowle. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SPowerUp.h"
#include "SPowerUp_GrantAction.generated.h"

class USAction;
/**
 * 
 */
UCLASS(Abstract)
class TOMLOOMANCPPPROJECT_API ASPowerUp_GrantAction : public ASPowerUp
{
	GENERATED_BODY()

protected:
	
	virtual void ApplyEffect(APawn* InstigatorPawn) override;

	virtual bool IsInstigatorEligible(APawn* InstigatorPawn) override;

	UPROPERTY(EditDefaultsOnly, Category = "Action")
	TSubclassOf<USAction> ActionClassToGrant;
};
