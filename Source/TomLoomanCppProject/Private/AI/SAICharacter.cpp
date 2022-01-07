// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SAICharacter.h"

#include "AIController.h"
#include "BrainComponent.h"
#include "DrawDebugHelpers.h"
#include "SActionComponent.h"
#include "SAttributeComponent.h"
#include "SWorldUserWidget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/CapsuleComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "Gameframework/CharacterMovementComponent.h"

static TAutoConsoleVariable<bool> CVarDebugDrawAIStrings(TEXT("su.DebugDrawAIStrings"), false, TEXT("Enable debug strings drawn when AI spots player "), ECVF_Cheat);

ASAICharacter::ASAICharacter()
{
	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>("PawnSensingComp");
	AttributeComp = CreateDefaultSubobject<USAttributeComponent>("AttributeComp");
	ActionComp = CreateDefaultSubobject<USActionComponent>("ActionComp");

	// by default this is just set to PlacedInWorld which leads to spawned AI characters not being possessed
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	TimeToHitParamName = TEXT("HitFlashTimeToHit");

	// for projectiles to be able to hit the mesh such that directional damage can be applied at the right point
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
	// for interacting with projectiles that work via overlap (not hit)
	GetMesh()->SetGenerateOverlapEvents(true);
}

void ASAICharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	PawnSensingComp->OnSeePawn.AddDynamic(this, &ASAICharacter::OnPawnSeen);
	AttributeComp->OnHealthChanged.AddDynamic(this, &ASAICharacter::OnHealthChanged);
}

void ASAICharacter::OnPawnSeen(APawn* Pawn)
{
	if(GetTargetActor() != Pawn)
	{
		SetTargetActor(Pawn);
		if (CVarDebugDrawAIStrings.GetValueOnGameThread())
		{
			DrawDebugString(GetWorld(), GetActorLocation(), TEXT("PLAYER SPOTTED"), nullptr, FColor::White, 4.0f, true);
		}

		MulticastSpawnPlayerSpottedWidget();
	}
}

void ASAICharacter::OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewValue,	float Delta)
{
	if (Delta < 0.0f)
	{
		// play hit flash
		GetMesh()->SetScalarParameterValueOnMaterials(TimeToHitParamName, GetWorld()->TimeSeconds);

		// set target to the actor that caused this damage
		if(InstigatorActor != this) // does not (yet?) check if instigator might be a team mate / fellow AI character
		{
			SetTargetActor(InstigatorActor);
		}

		// Add health bar to screen if it not exists yet and the character does not die immediately
		if (ActiveHealthBar == nullptr && NewValue > 0)
		{
			ActiveHealthBar = CreateWidget<USWorldUserWidget>(GetWorld(), HealthBarWidgetClass);
			if(ActiveHealthBar)
			{
				ActiveHealthBar->AttachedActor = this;
				ActiveHealthBar->AddToViewport(); // here the construct event is triggered
			}
		}

		// die
		if (NewValue <= 0.0f)
		{
			// Stop Behavior Tree
			AAIController* AIC = Cast<AAIController>(GetController());
			if(AIC)
			{
				AIC->GetBrainComponent()->StopLogic(TEXT("Killed"));
			}

			// Enable Ragdoll effects
			GetMesh()->SetAllBodiesSimulatePhysics(true);
			GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
			
			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			GetCharacterMovement()->DisableMovement();

			// destroy in 10 seconds
			SetLifeSpan(10.0f);
		}
	}
}

void ASAICharacter::SetTargetActor(AActor* NewTarget)
{
	AAIController* AIC = Cast<AAIController>(GetController());

	if (AIC)
	{
		AIC->GetBlackboardComponent()->SetValueAsObject(TargetActorKey, NewTarget);
	}
}

AActor* ASAICharacter::GetTargetActor() const
{
	AAIController* AIC = Cast<AAIController>(GetController());

	if (AIC)
	{
		return Cast<AActor>(AIC->GetBlackboardComponent()->GetValueAsObject(TargetActorKey));
	}

	return nullptr;
}

void ASAICharacter::MulticastSpawnPlayerSpottedWidget_Implementation()
{
	if ((GetWorldTimerManager().GetTimerRemaining(PlayerSpottedTimerHandle) <= 0.0) || !PlayerSpottedWidget)
	{
		PlayerSpottedWidget = CreateWidget<USWorldUserWidget>(GetWorld(), PlayerSpottedWidgetClass);
		if (PlayerSpottedWidget)
		{
			PlayerSpottedWidget->AttachedActor = this;

			// ZOrder > 0 in order to be above the health bar (in case that the minion has a health bar already)
			PlayerSpottedWidget->AddToViewport(1);
			// do not clear timer before setting it such that if minion sees two targets after each other
			// the duration is just as long as it would see one target

			// set timer to remove widget after some time again
			GetWorldTimerManager().SetTimer(PlayerSpottedTimerHandle, PlayerSpottedWidget, &UUserWidget::RemoveFromParent, 3.0);
		}
	}
}

