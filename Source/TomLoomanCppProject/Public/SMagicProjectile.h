// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SProjectileBase.h"
#include "SMagicProjectile.generated.h"

UCLASS()
class TOMLOOMANCPPPROJECT_API ASMagicProjectile : public ASProjectileBase
{
	GENERATED_BODY()

	ASMagicProjectile();

protected:
		
	virtual void OnActorHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

	UPROPERTY(EditAnywhere, Category = "Damage")
	float DamageAmount;
};
