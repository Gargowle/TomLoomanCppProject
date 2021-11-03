// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTTask_RangedAttack.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"


EBTNodeResult::Type USBTTask_RangedAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* MyController = OwnerComp.GetAIOwner();
	if(ensure(MyController))
	{
		ACharacter* MyCharacter = Cast<ACharacter>(MyController->GetPawn());
		if (MyCharacter == nullptr)
		{
			return EBTNodeResult::Failed;
		}

		FVector MuzzleLocation = MyCharacter->GetMesh()->GetSocketLocation(TEXT("Muzzle_01"));

		AActor* TargetActor = Cast<AActor>(MyController->GetBlackboardComponent()->GetValueAsObject(TEXT("TargetActor")));
		if (TargetActor == nullptr)
		{
			return EBTNodeResult::Failed;
		}

		FVector Direction = TargetActor->GetActorLocation() - MuzzleLocation;

		FRotator MuzzleRotation = Direction.Rotation();

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Instigator = MyCharacter;

		AActor* NewProj = GetWorld()->SpawnActor<AActor>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);

		return NewProj ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;

	}
	return EBTNodeResult::Failed;
}
