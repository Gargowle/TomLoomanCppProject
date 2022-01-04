// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerState.h"

#include "SSaveGame.h"

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

void ASPlayerState::SavePlayerState_Implementation(USSaveGame* SaveObject)
{
	if(SaveObject)
	{
		SaveObject->Credits = Credits;
	}
}

void ASPlayerState::LoadPlayerState_Implementation(USSaveGame* SaveObject)
{
	if(SaveObject)
	{
		Credits = SaveObject->Credits;
	}
}
