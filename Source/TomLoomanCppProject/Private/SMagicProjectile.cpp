// Fill out your copyright notice in the Description page of Project Settings.


#include "SMagicProjectile.h"

#include "SAttributeComponent.h"

ASMagicProjectile::ASMagicProjectile()
{
	DamageAmount = 50.0f;
}

void ASMagicProjectile::OnActorHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if(OtherActor && OtherActor != GetInstigator())
	{
		USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(OtherActor);
		if(AttributeComp)
		{
			AttributeComp->ApplyHealthChange(GetInstigator(), -DamageAmount);
		}

		Explode();
	}
}

