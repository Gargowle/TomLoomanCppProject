// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerState.h"

#include "SSaveGame.h"
#include "Net/UnrealNetwork.h"
#include "TomLoomanCppProject/TomLoomanCppProject.h"


ASPlayerState::ASPlayerState()
{
	bReplicates = true;
}

int ASPlayerState::GetCredits() const
{
	return Credits;
}

void ASPlayerState::AddCredits(AActor* InstigatorActor, int CreditsToAdd)
{
	if (CreditsToAdd != 0)
	{
		if(GetPawn()->HasAuthority())
		{
			Credits += CreditsToAdd;
			ClientCreditChanged(InstigatorActor, this, Credits, CreditsToAdd);

			OnCreditChanged.Broadcast(InstigatorActor, this, Credits, CreditsToAdd);
		}
	}
}

void ASPlayerState::ClientCreditChanged_Implementation(AActor* InstigatorActor, ASPlayerState* PlayerState, int NewCreditScore, int Delta)
{
	OnCreditChanged.Broadcast(InstigatorActor, PlayerState, NewCreditScore, Delta);
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
		OnCreditChanged.Broadcast(this, this, Credits, Credits);
	}
}

void ASPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASPlayerState, Credits);
}
