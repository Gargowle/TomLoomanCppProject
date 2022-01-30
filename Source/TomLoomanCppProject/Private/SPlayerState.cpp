// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerState.h"

#include "SAttributeComponent.h"
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

void ASPlayerState::SetCredits(AActor* InstigatorActor, int CreditsToSet)
{
	if (!GetWorld()->IsNetMode(NM_Client))
	{
		int32 CreditsDiff = CreditsToSet - Credits;
		Credits = CreditsToSet;
		ClientCreditChanged(InstigatorActor, this, Credits, CreditsDiff);

		OnCreditChanged.Broadcast(InstigatorActor, this, Credits, CreditsDiff);
	}
}

void ASPlayerState::AddCredits(AActor* InstigatorActor, int CreditsToAdd)
{
	if (CreditsToAdd != 0)
	{
		SetCredits(InstigatorActor, Credits+CreditsToAdd);
	}
}

void ASPlayerState::ClientCreditChanged_Implementation(AActor* InstigatorActor, ASPlayerState* PlayerState, int NewCreditScore, int Delta)
{
	OnCreditChanged.Broadcast(InstigatorActor, PlayerState, NewCreditScore, Delta);
}


bool ASPlayerState::UpdatePersonalRecord(float NewTime)
{
	if (NewTime > PersonalRecordTime)
	{
		float OldRecord = PersonalRecordTime;

		PersonalRecordTime = NewTime;

		OnRecordTimeChanged.Broadcast(this, NewTime, OldRecord);

		return true;
	}

	return false;
}

bool ASPlayerState::OverrideSpawnTransform(USSaveGame* SaveObject)
{
	if (APawn* MyPawn = GetPawn())
	{
		FPlayerSaveData* FoundData = SaveObject->GetPlayerData(this);
		if(FoundData && FoundData->bResumeAtTransform)
		{
			MyPawn->SetActorLocation(FoundData->Location);
			MyPawn->SetActorRotation(FoundData->Rotation);

			// PlayerState owner is a Player Controller
			AController* PC = Cast<AController>(GetOwner());
			// Set control rotation to change camera direction, setting Pawn rotation is not enough
			PC->SetControlRotation(FoundData->Rotation);

			return true;
		}
	}

	return false;
}

void ASPlayerState::SavePlayerState_Implementation(USSaveGame* SaveObject)
{
	if(SaveObject)
	{
		// Gather all relevant data for player
		FPlayerSaveData SaveData;
		SaveData.Credits = Credits;
		SaveData.PersonalRecordTime = PersonalRecordTime;
		// Stored as FString for simplicity (origina Steam ID is uint64)
		SaveData.PlayerId = GetUniqueId().ToString();

		// May not be alive while we save
		APawn* MyPawn = GetPawn();
		if (MyPawn)
		{
			USAttributeComponent* MyAttributes = USAttributeComponent::GetAttributes(MyPawn);
			if(MyAttributes && MyAttributes->IsAlive())
			{
				SaveData.Location = MyPawn->GetActorLocation();
				SaveData.Rotation = MyPawn->GetActorRotation();
				SaveData.bResumeAtTransform = true;
			}			
		}

		SaveObject->SavedPlayers.Add(SaveData);
	}
}

void ASPlayerState::LoadPlayerState_Implementation(USSaveGame* SaveObject)
{
	if(SaveObject)
	{
		FPlayerSaveData* FoundData = SaveObject->GetPlayerData(this);
		if(FoundData)
		{
			// Makes sure we trigger Credits changed event
			SetCredits(this, FoundData->Credits);

			PersonalRecordTime = FoundData->PersonalRecordTime;
			OnRecordTimeChanged.Broadcast(this, PersonalRecordTime, PersonalRecordTime);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Could not find SaveGame data for player id '%i'."), GetPlayerId());
		}
	}
}

void ASPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASPlayerState, Credits);
}
