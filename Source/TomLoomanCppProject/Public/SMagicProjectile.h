// Copyright Gargowle. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "SProjectileBase.h"
#include "SMagicProjectile.generated.h"

class USActionEffect;

UCLASS()
class TOMLOOMANCPPPROJECT_API ASMagicProjectile : public ASProjectileBase
{
	GENERATED_BODY()

	ASMagicProjectile();

protected:
		
	virtual void OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	UPROPERTY(EditAnywhere, Category = "Damage")
	float DamageAmount;

	UPROPERTY(EditAnywhere, Category = "Damage")
	FGameplayTag ParryTag;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<USActionEffect> BurningActionClass;

};
