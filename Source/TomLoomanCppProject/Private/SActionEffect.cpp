// Fill out your copyright notice in the Description page of Project Settings.


#include "SActionEffect.h"

#include "SActionComponent.h"

void USActionEffect::ExecutePeriodicEffect_Implementation(AActor* Instigator)
{
}

USActionEffect::USActionEffect()
{
	bAutoStart = true;
}

void USActionEffect::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	if (Duration > 0.0f)
	{
		FTimerDelegate DurationDelegate;
		DurationDelegate.BindUFunction(this, TEXT("StopAction"), Instigator);

		GetWorld()->GetTimerManager().SetTimer(DurationHandle, DurationDelegate, Duration, false);
	}

	if(Period > 0.0f)
	{
		FTimerDelegate PeriodDelegate;
		PeriodDelegate.BindUFunction(this, TEXT("ExecutePeriodicEffect"), Instigator);

		GetWorld()->GetTimerManager().SetTimer(PeriodHandle, PeriodDelegate, Period, true);
	}
}

void USActionEffect::StopAction_Implementation(AActor* Instigator)
{
	if(GetWorld()->GetTimerManager().GetTimerRemaining(PeriodHandle) < KINDA_SMALL_NUMBER)
	{
		ExecutePeriodicEffect(Instigator);
	}

	Super::StopAction_Implementation(Instigator);

	GetWorld()->GetTimerManager().ClearTimer(PeriodHandle);
	GetWorld()->GetTimerManager().ClearTimer(DurationHandle);
	
	ActionComp->RemoveAction(this);
}
