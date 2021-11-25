// Fill out your copyright notice in the Description page of Project Settings.


#include "SMagicProjectile.h"

#include "SGameplayFunctionLibrary.h"
#include "GameFramework/ProjectileMovementComponent.h"

ASMagicProjectile::ASMagicProjectile()
{
	DamageAmount = 50.0f;
}

void ASMagicProjectile::OnActorHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if(OtherActor && OtherActor != GetInstigator())
	{
		USGameplayFunctionLibrary::ApplyDirectionalDamage(GetInstigator(), OtherActor, DamageAmount, Hit);
		Explode();
	}
}

