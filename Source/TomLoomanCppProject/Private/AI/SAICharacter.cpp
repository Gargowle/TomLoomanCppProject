// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SAICharacter.h"

#include "AIController.h"
#include "BrainComponent.h"
#include "DrawDebugHelpers.h"
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
	SetTargetActor(Pawn);
	if(CVarDebugDrawAIStrings.GetValueOnGameThread())
	{
		DrawDebugString(GetWorld(), GetActorLocation(), TEXT("PLAYER SPOTTED"), nullptr, FColor::White, 4.0f, true);
	}
}

void ASAICharacter::OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth,	float Delta)
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
		if (ActiveHealthBar == nullptr && NewHealth > 0)
		{
			ActiveHealthBar = CreateWidget<USWorldUserWidget>(GetWorld(), HealthBarWidgetClass);
			if(ActiveHealthBar)
			{
				ActiveHealthBar->AttachedActor = this;
				ActiveHealthBar->AddToViewport(); // here the construct event is triggered
			}
		}

		// die
		if (NewHealth <= 0.0f)
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
		AIC->GetBlackboardComponent()->SetValueAsObject("TargetActor", NewTarget);
	}
}

