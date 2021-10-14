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

	GetWorldTimerManager().SetTimer(TimerHandle_Teleport, this, &ASTeleportProjectile::StartTeleportation, ProjectileFlightTime);
}

void ASTeleportProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	SphereComp->IgnoreActorWhenMoving(GetInstigator(), true);
	SphereComp->OnComponentHit.AddDynamic(this, &ASTeleportProjectile::OnActorHit);
}

void ASTeleportProjectile::OnActorHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!MovementComp->IsVelocityUnderSimulationThreshold())
	{
		MovementComp->StopMovementImmediately();
		GetWorldTimerManager().ClearTimer(TimerHandle_Teleport);
		StartTeleportation();
		
	}
}

void ASTeleportProjectile::TeleportInstigator()
{
	const FVector TeleportLocation = GetActorLocation();
	GetInstigator()->GetRootComponent()->SetWorldLocation(TeleportLocation);
	K2_DestroyActor();
}

void ASTeleportProjectile::StartTeleportation()
{
	if(ensure(PortalEmitter))
	{
		EffectComp->SetTemplate(PortalEmitter);
	}
	MovementComp->StopMovementImmediately();
	GetWorldTimerManager().SetTimer(TimerHandle_Teleport, this, &ASTeleportProjectile::TeleportInstigator, PortalOpenDelay);
}
