// Fill out your copyright notice in the Description page of Project Settings.


#include "SMagicProjectile.h"

#include "SAttributeComponent.h"

// Called when the game starts or when spawned
void ASMagicProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASMagicProjectile::OnActorHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if(OtherActor && OtherActor != GetInstigator())
	{
		USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(OtherActor);
		if(AttributeComp)
		{
			AttributeComp->ApplyHealthChange(-20.0f);
		}

		Explode();
	}
}

