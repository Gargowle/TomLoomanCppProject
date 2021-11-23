// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTTask_SetBlackboardBoolValue.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

USBTTask_SetBlackboardBoolValue::USBTTask_SetBlackboardBoolValue()
{
	bValueToSet = true;
}

EBTNodeResult::Type USBTTask_SetBlackboardBoolValue::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* MyController = OwnerComp.GetAIOwner();
	ensure(MyController);

	MyController->GetBlackboardComponent()->SetValueAsBool(EntryToSetKey.SelectedKeyName, bValueToSet);

	return EBTNodeResult::Succeeded;
}
