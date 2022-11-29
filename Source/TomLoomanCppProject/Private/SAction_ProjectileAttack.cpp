// Copyright Gargowle. All Rights Reserved.


#include "SAction_ProjectileAttack.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

USAction_ProjectileAttack::USAction_ProjectileAttack()
{
	AttackAnimDelay = 0.17f;
	HandSocketName = TEXT("Muzzle_01");
}

void USAction_ProjectileAttack::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	ACharacter* InstigatorCharacter = Cast<ACharacter>(Instigator);
	if(ensure(InstigatorCharacter))
	{
		InstigatorCharacter->PlayAnimMontage(AttackAnim);
		UGameplayStatics::SpawnEmitterAttached(AttackVFX, InstigatorCharacter->GetMesh(), HandSocketName);

		if (InstigatorCharacter->HasAuthority())
		{

			FTimerDelegate TimerDel;
			TimerDel.BindUFunction(this, FName("AttackDelay_Elapsed"), InstigatorCharacter);

			FTimerHandle TimerHandle_AttackDelay;

			GetWorld()->GetTimerManager().SetTimer(TimerHandle_AttackDelay, TimerDel, AttackAnimDelay, /*looping*/ false);
		}
	}
}

void USAction_ProjectileAttack::AttackDelay_Elapsed(ACharacter* InstigatorCharacter)
{
	if (ensure(ProjectileClass))
	{
		// Line Trace for finding out the projectile launch rotation
		// This is done by line tracing the camera forward vector for finding the location that the crosshair is aiming at
		FCollisionObjectQueryParams ObjectQueryParams;
		ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
		ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
		ObjectQueryParams.AddObjectTypesToQuery(ECC_PhysicsBody);

		FCollisionQueryParams CollisionQueryParams;
		CollisionQueryParams.AddIgnoredActor(InstigatorCharacter);

		FHitResult AttackTargetHitResult;

		constexpr float LineTraceLength = 100000.0f; // 1000 meter

		FVector CameraPosition = InstigatorCharacter->GetPawnViewLocation();
		FVector LineTraceEnd = CameraPosition + InstigatorCharacter->GetControlRotation().Vector() * LineTraceLength;

		bool bHitSuccessful = GetWorld()->LineTraceSingleByObjectType(AttackTargetHitResult, CameraPosition, LineTraceEnd, ObjectQueryParams, CollisionQueryParams);

		FVector HandLocation = InstigatorCharacter->GetMesh()->GetSocketLocation(HandSocketName);

		FRotator RotationFromLineTrace = ((bHitSuccessful ? AttackTargetHitResult.Location : LineTraceEnd) - HandLocation).Rotation();

		//TM = Transform Matrix
		// Spawn at the location of the Muzzle_01 location into the yielded by the line trace
		FTransform SpawnTM = FTransform(RotationFromLineTrace, HandLocation);

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		// add the information on who shot this projectile
		SpawnParams.Instigator = InstigatorCharacter;

		GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnTM, SpawnParams);
	}

	StopAction(InstigatorCharacter);
}
