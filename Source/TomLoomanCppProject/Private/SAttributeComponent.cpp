// Fill out your copyright notice in the Description page of Project Settings.


#include "SAttributeComponent.h"

#include "SGameModeBase.h"

// Sets default values for this component's properties
USAttributeComponent::USAttributeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	HealthMax = 100.0f;
	Health = HealthMax;
}

bool USAttributeComponent::IsFullHealth()
{
	return Health >= HealthMax;
}

USAttributeComponent* USAttributeComponent::GetAttributes(AActor* FromActor)
{
	// check if nullptr, because as this is a public static function we have no idea who is going to call this
	if(FromActor)
	{
		// TODO: future potential to make this more efficient in how the component is found.
		// However, only one place in the code needs to be edited (this place) in order to update it everywhere this is used.
		return Cast<USAttributeComponent>(FromActor->GetComponentByClass(USAttributeComponent::StaticClass()));		
	}

	return nullptr;
}

bool USAttributeComponent::IsActorAlive(AActor* FromActor)
{
	USAttributeComponent* AttributeComp = GetAttributes(FromActor);
	if(AttributeComp) // no ensure because not clear how caller might intend to use this method
	{
		return AttributeComp->IsAlive();
	}

	return false;
}

bool USAttributeComponent::ApplyHealthChange(AActor* InstigatorActor, float Delta)
{
	if(!GetOwner()->CanBeDamaged())
	if(!GetOwner()->CanBeDamaged() && Delta < 0.0f)
	{
		return false;
	}
	const float OldHealth = Health;

	Health = FMath::Clamp((Health+Delta), 0.0f, HealthMax);

	const float ActualDelta = Health - OldHealth;	
	OnHealthChanged.Broadcast(InstigatorActor, this, Health, ActualDelta);

	// Death
	if(ActualDelta < 0.0f && Health <= 0.0f)
	{
		ASGameModeBase* GM = GetWorld()->GetAuthGameMode<ASGameModeBase>();
		if (GM)
		{
			GM->OnActorKilled(GetOwner(), InstigatorActor);
		}
	}

	return ActualDelta != 0.0f;
}

bool USAttributeComponent::IsAlive() const
{
	return Health > 0.0f;
}

bool USAttributeComponent::Kill(AActor* InstigatorActor)
{
	return ApplyHealthChange(InstigatorActor, -HealthMax);
}

float USAttributeComponent::GetHealth()
{
	return Health;
}

bool USAttributeComponent::ApplyFullHealth(AActor* InstigatorActor)
{
	return ApplyHealthChange(InstigatorActor, HealthMax);
}
