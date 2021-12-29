// Fill out your copyright notice in the Description page of Project Settings.


#include "SActionEffect_Thorns.h"
#include "SActionComponent.h"
#include "SAttributeComponent.h"

USActionEffect_Thorns::USActionEffect_Thorns()
{
	Duration = 0.0f; // no timer set to call StopAction -> Infinite Duration
	Period = 0.0f; // no timer set to call (non-existing) ExecutePeriodicEffect method

	ThornsFactor = 0.2f;
}

void USActionEffect_Thorns::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	USActionComponent* ActionComp = Cast<USActionComponent>(GetOuter());
	if(ensure(ActionComp))
	{
		AActor* Owner = ActionComp->GetOwner();
		USAttributeComponent* AttrComp = Cast<USAttributeComponent>(Owner->GetComponentByClass(USAttributeComponent::StaticClass()));
		if(ensure(AttrComp))
		{
			AttrComp->OnHealthChanged.AddDynamic(this, &USActionEffect_Thorns::ApplyThornDamage);
		}
	}
}

void USActionEffect_Thorns::StopAction_Implementation(AActor* Instigator)
{
	Super::StopAction_Implementation(Instigator);

	USActionComponent* ActionComp = Cast<USActionComponent>(GetOuter());
	if (ensure(ActionComp))
	{
		AActor* Owner = ActionComp->GetOwner();
		USAttributeComponent* AttrComp = Cast<USAttributeComponent>(Owner->GetComponentByClass(USAttributeComponent::StaticClass()));
		if (ensure(AttrComp))
		{
			AttrComp->OnHealthChanged.RemoveDynamic(this, &USActionEffect_Thorns::ApplyThornDamage);
		}
	}
}

void USActionEffect_Thorns::ApplyThornDamage(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta)
{
	// only apply thorns damage if actually damage was dealt
	if (Delta >= 0)
	{
		return;
	}

	USActionComponent* ActionComp = Cast<USActionComponent>(GetOuter());
	if (ensure(ActionComp))
	{
		AActor* Owner = ActionComp->GetOwner();

		// do not apply thorns damage in case the instigator is the Owner
		if (InstigatorActor == Owner)
		{
			return;
		}

		USAttributeComponent* AttrComp = Cast<USAttributeComponent>(InstigatorActor->GetComponentByClass(USAttributeComponent::StaticClass()));
		if (ensure(AttrComp))
		{
			AttrComp->ApplyHealthChange(Owner, FMath::TruncToFloat(ThornsFactor * Delta));
		}
	}
}
