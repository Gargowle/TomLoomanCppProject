// Fill out your copyright notice in the Description page of Project Settings.


#include "SHealthPotionPowerUp.h"
#include "SAttributeComponent.h"
#include "SCharacter.h"
#include "SPlayerState.h"

#define  LOCTEXT_NAMESPACE "InteractableActors"

ASHealthPotionPowerUp::ASHealthPotionPowerUp()
{
	HealingAmount = 50.0f;
	CreditCosts = 20;
}

void ASHealthPotionPowerUp::ApplyEffect(APawn* InstigatorPawn)
{
	USAttributeComponent* AttributeComp = Cast<USAttributeComponent>(InstigatorPawn->GetComponentByClass(USAttributeComponent::StaticClass()));
	AttributeComp->ApplyHealthChange(this, HealingAmount);
}

bool ASHealthPotionPowerUp::IsInstigatorEligible(APawn* InstigatorPawn)
{
	// instigator pawn must have the attribute component and the used player state must be ASPlayerstate
	USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(InstigatorPawn);
	ASPlayerState* PlayerState = InstigatorPawn->GetPlayerState<ASPlayerState>();

	if (AttributeComp && ensureMsgf(PlayerState, TEXT("This framework needs the ASPlayerState class to be used. However, some other class has been chosen.")))
	{
		// if instigator has full health, the instigator is not eligible for the health potion
		// instigator must have enough credits
		if(!AttributeComp->IsFullHealth() && PlayerState->GetCredits() >= CreditCosts)
		{
			// if all conditions fullfilled, pay price and return true
			PlayerState->AddCredits(this, -CreditCosts);
			return true;
		}
	}
	return false;

}

FText ASHealthPotionPowerUp::GetInteractText_Implementation(APawn* InstigatorPawn)
{
	USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(InstigatorPawn);
	if(AttributeComp && AttributeComp->IsFullHealth())
	{
		return LOCTEXT("HealthPotion_FullHealthWarning", "Already at full health.");
	}
	return FText::Format(LOCTEXT("HealthPotion_InteractMessage", "Cost {0} Credits. Restores health to maximum."), CreditCosts);
}

#undef LOCTEXT_NAMESPACE
