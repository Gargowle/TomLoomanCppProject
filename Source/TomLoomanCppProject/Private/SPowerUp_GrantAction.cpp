// Copyright Gargowle. All Rights Reserved.


#include "SPowerUp_GrantAction.h"

#include "SActionComponent.h"

void ASPowerUp_GrantAction::ApplyEffect(APawn* InstigatorPawn)
{
	USActionComponent* ActionComp = Cast<USActionComponent>(InstigatorPawn->GetComponentByClass(USActionComponent::StaticClass()));
	if(ensure(ActionComp))
	{
		ActionComp->AddAction(InstigatorPawn, ActionClassToGrant);
	}
}

bool ASPowerUp_GrantAction::IsInstigatorEligible(APawn* InstigatorPawn)
{
	USActionComponent* ActionComp = Cast<USActionComponent>(InstigatorPawn->GetComponentByClass(USActionComponent::StaticClass()));
	if(ensure(ActionComp) && !ActionComp->HasAction(ActionClassToGrant))
	{
		return true;
	}
	return false;
}
