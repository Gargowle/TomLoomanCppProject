// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "SBTService_CheckPlaceHidden.generated.h"

/**
 * 
 */
UCLASS()
class TOMLOOMANCPPPROJECT_API USBTService_CheckPlaceHidden : public UBTService
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere, Category = "AI")
	FBlackboardKeySelector PlaceToCheckKey;

	UPROPERTY(EditAnywhere, Category = "AI")
	FBlackboardKeySelector ActorToHideFromKey;

	UPROPERTY(EditAnywhere, Category = "AI")
	FBlackboardKeySelector IsPlaceHiddenKey;

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
