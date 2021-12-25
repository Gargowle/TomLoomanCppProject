// Fill out your copyright notice in the Description page of Project Settings.


#include "SMagicProjectile.h"

#include "GameplayTagContainer.h"
#include "SActionComponent.h"
#include "SGameplayFunctionLibrary.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "SActionEffect.h"

ASMagicProjectile::ASMagicProjectile()
{
	DamageAmount = 50.0f;
	MovementComp->InitialSpeed = 4000.0f;

}

void ASMagicProjectile::OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor && OtherActor != GetInstigator())
	{

		USActionComponent* ActionComp = Cast<USActionComponent>(OtherActor->GetComponentByClass(USActionComponent::StaticClass()));
		if(ActionComp && ActionComp->ActiveGameplayTags.HasTag(ParryTag))
		{
			// Deflect projectile
			MovementComp->Velocity *= -1.0;

			SetInstigator(Cast<APawn>(OtherActor));
			return;
		}

		if(USGameplayFunctionLibrary::ApplyDirectionalDamage(GetInstigator(), OtherActor, DamageAmount, SweepResult))
		{
			Explode();

			if(ActionComp)
			{
				ActionComp->AddAction(GetInstigator(), BurningActionClass);
			}
		}
	}
}

