// Fill out your copyright notice in the Description page of Project Settings.


#include "SAction.h"
#include "SActionComponent.h"
#include "TomLoomanCppProject/TomLoomanCppProject.h"

void USAction::StartAction_Implementation(AActor* Instigator)
{
	//(UE_LOG(LogTemp, Log, TEXT("Running: %s"), *GetNameSafe(this));
	LogOnScreen(this, FString::Printf(TEXT("Started: %s"), *ActionName.ToString()), FColor::Green);

	// use outer instead of instigator because instigator could be anything while the tags should be managed for the executor of the action
	USActionComponent* OwningComp = GetOwningComponent();
	OwningComp->ActiveGameplayTags.AppendTags(GrantsTags);

	bIsRunning = true;
}

void USAction::StopAction_Implementation(AActor* Instigator)
{
	//UE_LOG(LogTemp, Log, TEXT("Stopping: %s"), *GetNameSafe(this));
	LogOnScreen(this, FString::Printf(TEXT("Stopped: %s"), *ActionName.ToString()), FColor::White);

	// if StopAction is called even though the action is not running, there is a serious problem that needs to be dealt with immediately
	ensureAlways(bIsRunning);

	USActionComponent* OwningComp = GetOwningComponent();
	OwningComp->ActiveGameplayTags.RemoveTags(GrantsTags);

	bIsRunning = false;
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

bool USAction::CanStart_Implementation(AActor* Instigator)
{
	if(IsRunning())
	{
		return false;
	}

	USActionComponent* OwningComp = GetOwningComponent();

	return !OwningComp->ActiveGameplayTags.HasAny(BlockedTags);
}

USActionComponent* USAction::GetOwningComponent() const
{
	// cast must succeed because Outer is set to USActionComponent explicitly in the AddAction method of the USActionComponent
	return Cast<USActionComponent>(GetOuter());
}

bool USAction::IsRunning() const
{
	return bIsRunning;
}
