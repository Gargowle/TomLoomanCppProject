// Copyright Gargowle. All Rights Reserved.


#include "AI/SBTService_CheckLowHealth.h"

#include "AIController.h"
#include "SAttributeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

USBTService_CheckLowHealth::USBTService_CheckLowHealth()
{
	LowHealthThreshold = 50.0f;
}

void USBTService_CheckLowHealth::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	ensure(BlackboardComp);

	AAIController* MyController = OwnerComp.GetAIOwner();
	ensure(MyController);
	
	APawn* MyPawn = MyController->GetPawn();
	ensure(MyPawn);

	USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(MyPawn);
	ensure(AttributeComp);

	bool bIsLowHealth = AttributeComp->GetHealth() <= LowHealthThreshold;

	BlackboardComp->SetValueAsBool(IsLowHealthKey.SelectedKeyName, bIsLowHealth);
}
