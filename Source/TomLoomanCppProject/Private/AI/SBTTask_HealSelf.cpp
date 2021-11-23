// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTTask_HealSelf.h"

#include "AIController.h"
#include "SAttributeComponent.h"

EBTNodeResult::Type USBTTask_HealSelf::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* MyController = OwnerComp.GetAIOwner();
	ensure(MyController);

	APawn* MyPawn = MyController->GetPawn();
	ensure(MyPawn);

	USAttributeComponent* AttributeCompo = USAttributeComponent::GetAttributes(MyPawn);

	AttributeCompo->ApplyFullHealth(MyPawn);

	return EBTNodeResult::Succeeded;
}
