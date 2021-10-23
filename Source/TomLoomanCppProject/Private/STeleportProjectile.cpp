// Fill out your copyright notice in the Description page of Project Settings.


#include "STeleportProjectile.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

// Called when the game starts or when spawned
void ASTeleportProjectile::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(TimerHandle_Teleport, this, &ASTeleportProjectile::Explode, ProjectileFlightTime);
}

void ASTeleportProjectile::Explode_Implementation()
{
	// make sure that this method is not triggered twice by accident
	GetWorldTimerManager().ClearTimer(TimerHandle_Teleport);

	// some necessary duplicated lines again since we will not call the Super
	UGameplayStatics::SpawnEmitterAtLocation(this, ImpactVFX, GetActorLocation(), GetActorRotation());

	EffectComp->DeactivateSystem();

	MovementComp->StopMovementImmediately();
	SetActorEnableCollision(false);

	StartTeleportation();
}

void ASTeleportProjectile::TeleportInstigator()
{
	const FVector TeleportLocation = GetActorLocation();
	GetInstigator()->GetRootComponent()->SetWorldLocation(TeleportLocation);
	K2_DestroyActor();
}

void ASTeleportProjectile::StartTeleportation()
{
	GetWorldTimerManager().SetTimer(TimerHandle_Teleport, this, &ASTeleportProjectile::TeleportInstigator, PortalOpenDelay);
}
