// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerUp.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ASPowerUp::ASPowerUp()
{
	SecondsToRespawn = 10.0f;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;
	MeshComp->SetCollisionProfileName(TEXT("PowerUp"));

	bReplicates = true;
}

void ASPowerUp::Interact_Implementation(APawn* InstigatorPawn)
{
	//ISGameplayInterface::Interact_Implementation(InstigatorPawn);
	if (IsInstigatorEligible(InstigatorPawn))
	{
		ApplyEffect(InstigatorPawn);
		MulticastConsumePowerUp();
	}
}

void ASPowerUp::MulticastConsumePowerUp_Implementation()
{
	RootComponent->SetVisibility(false, true);
	SetActorEnableCollision(false);
	FTimerHandle TimerHandle_Respawn;
	GetWorldTimerManager().SetTimer(TimerHandle_Respawn, this, &ASPowerUp::Respawn, SecondsToRespawn);
}

void ASPowerUp::Respawn()
{
	SetActorEnableCollision(true);
	RootComponent->SetVisibility(true, true);
}

// Called when the game starts or when spawned
void ASPowerUp::BeginPlay()
{
	Super::BeginPlay();
	
}

