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

}

void ASPowerUp::Interact_Implementation(APawn* InstigatorPawn)
{
	//ISGameplayInterface::Interact_Implementation(InstigatorPawn);
	if (IsInstigatorEligible(InstigatorPawn))
	{
		ApplyEffect(InstigatorPawn);
		MeshComp->SetVisibility(false);
		MeshComp->SetCollisionProfileName(TEXT("NoCollision"));
		FTimerHandle TimerHandle_Respawn;
		GetWorldTimerManager().SetTimer(TimerHandle_Respawn, this, &ASPowerUp::Respawn, SecondsToRespawn);
	}
}

void ASPowerUp::Respawn()
{
	MeshComp->SetCollisionProfileName(TEXT("PowerUp"));
	MeshComp->SetVisibility(true);
}

// Called when the game starts or when spawned
void ASPowerUp::BeginPlay()
{
	Super::BeginPlay();
	
}

