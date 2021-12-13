// Fill out your copyright notice in the Description page of Project Settings.


#include "SAction.h"
#include "SActionComponent.h"

void USAction::StartAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp, Log, TEXT("Running: %s"), *GetNameSafe(this));

	// use outer instead of instigator because instigator could be anything while the tags should be managed for the executor of the action
	USActionComponent* OwningComp = GetOwningComponent();
	OwningComp->ActiveGameplayTags.AppendTags(GrantsTags);
}

void USAction::StopAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp, Log, TEXT("Stopping: %s"), *GetNameSafe(this));

	USActionComponent* OwningComp = GetOwningComponent();
	OwningComp->ActiveGameplayTags.RemoveTags(GrantsTags);
}

UWorld* USAction::GetWorld() const
{
	// Outer is set when creating Action via NewObject<T>
	UActorComponent* Comp = Cast<UActorComponent>(GetOuter());
	if (Comp)
	{
		return Comp->GetWorld();
	}

	return nullptr;
}

USActionComponent* USAction::GetOwningComponent() const
{
	// cast must succeed because Outer is set to USActionComponent explicitly in the AddAction method of the USActionComponent
	return Cast<USActionComponent>(GetOuter());
}
