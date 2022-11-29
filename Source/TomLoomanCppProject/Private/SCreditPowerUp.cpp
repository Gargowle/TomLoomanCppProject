// Copyright Gargowle. All Rights Reserved.


#include "SCreditPowerUp.h"
#include "SPlayerState.h"

ASCreditPowerUp::ASCreditPowerUp()
{
	CreditsToAdd = 10;
}

void ASCreditPowerUp::ApplyEffect(APawn* InstigatorPawn)
{
	ASPlayerState * InstigatorPlayerState = InstigatorPawn->GetPlayerState<ASPlayerState>();

	InstigatorPlayerState->AddCredits(InstigatorPawn, CreditsToAdd);
}

bool ASCreditPowerUp::IsInstigatorEligible(APawn* InstigatorPawn)
{
	return true;
}
