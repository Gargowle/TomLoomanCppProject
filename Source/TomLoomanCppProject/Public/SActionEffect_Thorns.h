// Copyright Gargowle. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SActionEffect.h"
#include "SActionEffect_Thorns.generated.h"

class USAttributeComponent;

/**
 * 
 */
UCLASS()
class TOMLOOMANCPPPROJECT_API USActionEffect_Thorns : public USActionEffect
{
	GENERATED_BODY()

public:

	USActionEffect_Thorns();

	virtual void StartAction_Implementation(AActor* Instigator) override;

	virtual void StopAction_Implementation(AActor* Instigator) override;

protected:

	UFUNCTION() // declared UFUNCTION in order to bind to USAttributeComponent's OnHealthChanged
	void ApplyThornDamage(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewValue, float Delta);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
	float ThornsFactor;
};
