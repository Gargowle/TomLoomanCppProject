// Copyright Gargowle. All Rights Reserved.


#include "AI/SBTService_CheckPlaceHidden.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

void USBTService_CheckPlaceHidden::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	ensure(BlackboardComp);

	AAIController* MyController = OwnerComp.GetAIOwner();
	ensure(MyController);

	APawn* MyPawn = MyController->GetPawn();
	ensure(MyPawn);

	APawn* PawnToHideFrom = Cast<APawn>(BlackboardComp->GetValueAsObject(ActorToHideFromKey.SelectedKeyName));
	ensure(PawnToHideFrom);

	FVector PlaceToCheck = BlackboardComp->GetValueAsVector(PlaceToCheckKey.SelectedKeyName);

	// implementation parallel to AController::LineOfSightTo; However, this function could not be used directly because it only
	// accepts another actor as look at location, not an FVector
	FCollisionQueryParams CollisionParams(SCENE_QUERY_STAT(LineOfSight), true);
	// set MyPawn and PawnToHideFrom as ignored actors to make sure that the place is not just occluded by one of these two
	CollisionParams.AddIgnoredActor(PawnToHideFrom);
	CollisionParams.AddIgnoredActor(MyPawn);

	FVector ViewPoint;
	FRotator ViewRotation;
	PawnToHideFrom->GetActorEyesViewPoint(ViewPoint, ViewRotation);
		
	bool bHit = GetWorld()->LineTraceTestByChannel(ViewPoint, PlaceToCheck, ECC_Visibility, CollisionParams);

	// the place is hidden if there was a hit
	BlackboardComp->SetValueAsBool(IsPlaceHiddenKey.SelectedKeyName, bHit);
}
