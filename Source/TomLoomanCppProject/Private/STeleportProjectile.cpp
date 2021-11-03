// Fill out your copyright notice in the Description page of Project Settings.


#include "STeleportProjectile.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"

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

	UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());

	StartTeleportation();
}

void ASTeleportProjectile::TeleportInstigator()
{
	AActor* ActorToTeleport = GetInstigator();
	if (ensure(ActorToTeleport))
	{
		ActorToTeleport->TeleportTo(GetActorLocation(), ActorToTeleport->GetActorRotation());
	}
	Destroy();
}

void ASTeleportProjectile::StartTeleportation()
{
	GetWorldTimerManager().SetTimer(TimerHandle_Teleport, this, &ASTeleportProjectile::TeleportInstigator, PortalOpenDelay);
}
