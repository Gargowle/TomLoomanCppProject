// Fill out your copyright notice in the Description page of Project Settings.


#include "SCreditPowerUp.h"
#include "SPlayerState.h"

ASCreditPowerUp::ASCreditPowerUp()
{
	CreditsToAdd = 10;
}

void ASCreditPowerUp::ApplyEffect(APawn* InstigatorPawn)
{
	ASPlayerState * InstigatorPlayerState = Cast<ASPlayerState>(InstigatorPawn->GetPlayerState());

	InstigatorPlayerState->AddCredits(InstigatorPawn, CreditsToAdd);
}

bool ASCreditPowerUp::IsInstigatorEligible(APawn* InstigatorPawn)
{
	return true;
}
