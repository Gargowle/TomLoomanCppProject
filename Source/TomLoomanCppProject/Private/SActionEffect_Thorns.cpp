// Copyright Gargowle. All Rights Reserved.


#include "SActionEffect_Thorns.h"
#include "SActionComponent.h"
#include "SAttributeComponent.h"
#include "SGameplayFunctionLibrary.h"

USActionEffect_Thorns::USActionEffect_Thorns()
{
	Duration = 0.0f; // no timer set to call StopAction -> Infinite Duration
	Period = 0.0f; // no timer set to call (non-existing) ExecutePeriodicEffect method

	ThornsFactor = 0.2f;
}

void USActionEffect_Thorns::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);
	
	USAttributeComponent* AttrComp = USAttributeComponent::GetAttributes(GetOwningComponent()->GetOwner());
	if(ensure(AttrComp))
	{
		AttrComp->OnHealthChanged.AddDynamic(this, &USActionEffect_Thorns::ApplyThornDamage);
	}
}

void USActionEffect_Thorns::StopAction_Implementation(AActor* Instigator)
{
	Super::StopAction_Implementation(Instigator);

	USAttributeComponent* AttrComp = USAttributeComponent::GetAttributes(GetOwningComponent()->GetOwner());
	if (ensure(AttrComp))
	{
		AttrComp->OnHealthChanged.RemoveDynamic(this, &USActionEffect_Thorns::ApplyThornDamage);
	}
}

void USActionEffect_Thorns::ApplyThornDamage(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewValue, float Delta)
{
	// only apply thorns damage if actually damage was dealt
	if (Delta >= 0)
	{
		return;
	}

	AActor* Owner = GetOwningComponent()->GetOwner();

	// do not apply thorns damage in case the instigator is the Owner
	if (InstigatorActor == Owner)
	{
		return;
	}

	int32 ThornsDamage = FMath::RoundToInt(-ThornsFactor * Delta);

	if(ThornsDamage != 0)
	{
		USGameplayFunctionLibrary::ApplyDamage(Owner, InstigatorActor, ThornsDamage);		
	}
}
