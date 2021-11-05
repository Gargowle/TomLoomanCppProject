// Fill out your copyright notice in the Description page of Project Settings.


#include "SHealthPotionPowerUp.h"
#include "SAttributeComponent.h"
#include "SCharacter.h"

ASHealthPotionPowerUp::ASHealthPotionPowerUp()
{
	HealingAmount = 50.0f;
}

void ASHealthPotionPowerUp::ApplyEffect(APawn* InstigatorPawn)
{
	USAttributeComponent* AttributeComp = Cast<USAttributeComponent>(InstigatorPawn->GetComponentByClass(USAttributeComponent::StaticClass()));
	AttributeComp->ApplyHealthChange(HealingAmount);
}

bool ASHealthPotionPowerUp::IsInstigatorEligible(APawn* InstigatorPawn)
{
	// instigator pawn must have the attribute component
	USAttributeComponent* AttributeComp = Cast<USAttributeComponent>(InstigatorPawn->GetComponentByClass(USAttributeComponent::StaticClass()));
	if (AttributeComp)
	{
		// if instigator has full health, the instigator is not eligible for the health potion
		return AttributeComp->IsFullHealth() ? false : true;
	}
	return false;

}
