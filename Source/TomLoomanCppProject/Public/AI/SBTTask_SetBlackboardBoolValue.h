// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "SBTTask_SetBlackboardBoolValue.generated.h"

/**
 * 
 */
UCLASS()
class TOMLOOMANCPPPROJECT_API USBTTask_SetBlackboardBoolValue : public UBTTaskNode
{
	GENERATED_BODY()

	USBTTask_SetBlackboardBoolValue();

protected:

	UPROPERTY(EditAnywhere, Category = "AI")
	FBlackboardKeySelector EntryToSetKey;

	UPROPERTY(EditAnywhere, Category = "AI")
	bool bValueToSet;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
