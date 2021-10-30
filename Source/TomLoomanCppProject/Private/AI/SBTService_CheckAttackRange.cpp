// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTService_CheckAttackRange.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

void USBTService_CheckAttackRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	bool bWithinRange = false;
	bool bHasLineOfSight = false;

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	ensure(BlackboardComp); // Check if Behavior tree is setup properly

	AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetActorKey.SelectedKeyName));

	// does not need to be ensured because maybe the behavior at the moment does not require a target actor
	if(TargetActor)
	{
		AAIController* MyController = OwnerComp.GetAIOwner();
		ensure(MyController); // at this point there should always be a Controller

		APawn* AIPawn = MyController->GetPawn();
		ensure(AIPawn); // at this point there should always be a Pawn

		float DistanceTo = FVector::Distance(AIPawn->GetActorLocation(), TargetActor->GetActorLocation());
		
		bWithinRange = (DistanceTo < AttackRange);
		if (bWithinRange)
		{
			bHasLineOfSight = MyController->LineOfSightTo(TargetActor);
		}
	}

	BlackboardComp->SetValueAsBool(IsInAttackRangeKey.SelectedKeyName, (bWithinRange && bHasLineOfSight));
}
