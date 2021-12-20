// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerState.h"

int ASPlayerState::GetCredits() const
{
	return Credits;
}

void ASPlayerState::AddCredits(AActor* InstigatorActor, int CreditsToAdd)
{
	if (CreditsToAdd != 0)
	{
		Credits += CreditsToAdd;

		OnCreditChanged.Broadcast(InstigatorActor, this, Credits, CreditsToAdd);
	}
}
