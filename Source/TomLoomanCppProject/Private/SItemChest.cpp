// Fill out your copyright notice in the Description page of Project Settings.


#include "SItemChest.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"



// Sets default values
ASItemChest::ASItemChest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>("BaseMesh");
	RootComponent = BaseMesh;

	LidMesh = CreateDefaultSubobject<UStaticMeshComponent>("LidMesh");
	LidMesh->SetupAttachment(RootComponent);

	TargetPitch = 110.0;

	bReplicates = true;
	BaseMesh->SetSimulatePhysics(true);
	BaseMesh->SetIsReplicated(true);
	SetReplicateMovement(true);


	bLidOpened = false;
}

void ASItemChest::OnActorLoaded_Implementation()
{
	ISGameplayInterface::OnActorLoaded_Implementation();

	// make sure that the "visual state" of the treasure chest is actually loaded correctly
	OnRep_LidOpened();
}

void ASItemChest::Interact_Implementation(APawn* InstigatorPawn)
{
	bLidOpened = !bLidOpened;
	OnRep_LidOpened();
}

void ASItemChest::OnRep_LidOpened()
{
	float CurrPitch = bLidOpened ? TargetPitch : 0.0f;
	LidMesh->SetRelativeRotation(FRotator(CurrPitch, 0, 0));
}

void ASItemChest::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Whenever bLidOpened changed on the server, send it to all clients (no other logic involved)
	// (other options for example: only replicate to owner of a weapon, ...)
	DOREPLIFETIME(ASItemChest, bLidOpened);
}
