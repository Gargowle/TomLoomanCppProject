// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SInteractionComponent.h"
#include "SAttributeComponent.h"
#include "Kismet/GameplayStatics.h"
#include "SActionComponent.h"
// for drawing
#include "DrawDebugHelpers.h"

static TAutoConsoleVariable<bool> CVarDebugDrawMovementArrows(TEXT("su.DebugDrawMovementArrow"), false, TEXT("Enable debug arrows for movement and input direction."), ECVF_Cheat);

// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>("SpringArmComp");
	SpringArmComp->SetupAttachment(RootComponent);
	// enable camera angles different from horizontal alignment (driven by the Control Rotation)
	SpringArmComp->bUsePawnControlRotation = true;

	// The rotation of spring arm is controlled with pawn control rotation already. This disables a subtle side effect where rotating the
	// CapsuleComponent (eg. caused by bOrientRotationToMovement in Character Movement) will rotate the spring arm until it self corrects later in the update.
	// This may cause unwanted effects when using CameraLocation during Tick as it may be slightly offset from the final camera position.
	SpringArmComp->SetUsingAbsoluteRotation(true);

	CameraComp = CreateDefaultSubobject<UCameraComponent>("CameraComp");
	CameraComp->SetupAttachment(SpringArmComp);

	InteractionComp = CreateDefaultSubobject<USInteractionComponent>("InteractionComponent");

	AttributeComp = CreateDefaultSubobject<USAttributeComponent>("AttributeComp");

	ActionComp = CreateDefaultSubobject<USActionComponent>("ActionComp");

	// Use "Action RPG style camera" in which camera can be rotated independently of the character's direction
	bUseControllerRotationYaw = false;

	// disable "strafing" but instead make the character turn into the direction of movement
	GetCharacterMovement()->bOrientRotationToMovement = true;

	TimeToHitParamName = TEXT("HitFlashTimeToHit");
}

void ASCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	AttributeComp->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);
}

void ASCharacter::OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewValue, float Delta)
{
	if(Delta < 0.0f)
	{
		if (NewValue <= 0.0f)
		{
			APlayerController* PC = Cast<APlayerController>(GetController());
			DisableInput(PC);

			SetLifeSpan(5.0f);
		}
		GetMesh()->SetScalarParameterValueOnMaterials(TimeToHitParamName, GetWorld()->TimeSeconds);
	}
}

void ASCharacter::MoveForward(float Value)
{
	FRotator ControlRot = GetControlRotation();
	ControlRot.Pitch = 0.0f;
	ControlRot.Roll = 0.0f;
	AddMovementInput(ControlRot.Vector(), Value);
}

void ASCharacter::MoveRight(float Value)
{
	FRotator ControlRot = GetControlRotation();
	ControlRot.Pitch = 0.0f;
	ControlRot.Roll = 0.0f;
	// adapted from UE_5.0EA\Engine\Source\Runtime\Engine\Private\KismetMathLibrary.cpp
	FVector RightVector = FRotationMatrix(ControlRot).GetScaledAxis(EAxis::Y);
	AddMovementInput(RightVector, Value);
}

void ASCharacter::SprintStart()
{
	ActionComp->StartActionByName(this, TEXT("Sprint"));
}

void ASCharacter::SprintStop()
{
	ActionComp->StopActionByName(this, TEXT("Sprint"));
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CVarDebugDrawMovementArrows.GetValueOnGameThread())
	{
		// -- Rotation Visualization -- //
		constexpr float DrawScale = 100.0f;
		constexpr float Thickness = 5.0f;

		FVector LineStart = GetActorLocation();
		// Offset to the right of pawn
		LineStart += GetActorRightVector() * 100.0f;
		// Set line end in direction of the actor's forward
		FVector ActorDirection_LineEnd = LineStart + (GetActorForwardVector() * 100.0f);
		// Draw Actor's Direction
		DrawDebugDirectionalArrow(GetWorld(), LineStart, ActorDirection_LineEnd, DrawScale, FColor::Yellow, false, 0.0f, 0, Thickness);

		FVector ControllerDirection_LineEnd = LineStart + (GetControlRotation().Vector() * 100.0f);
		// Draw 'Controller' Rotation ('PlayerController' that 'possessed' this character)
		DrawDebugDirectionalArrow(GetWorld(), LineStart, ControllerDirection_LineEnd, DrawScale, FColor::Green, false, 0.0f, 0, Thickness);
	}
}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction<FAttackActionNameDelegate>("PrimaryAttack", IE_Pressed, this, &ASCharacter::Attack, FName("PrimaryAttack"));
	PlayerInputComponent->BindAction<FAttackActionNameDelegate>("SecondaryAttack", IE_Pressed, this, &ASCharacter::Attack, FName("Blackhole"));
	PlayerInputComponent->BindAction<FAttackActionNameDelegate>("TeleportAttack", IE_Pressed, this, &ASCharacter::Attack, FName("Teleport"));
	PlayerInputComponent->BindAction("PrimaryInteract", IE_Pressed, this, &ASCharacter::PrimaryInteract);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ASCharacter::SprintStart);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ASCharacter::SprintStop);

}

void ASCharacter::HealSelf(float Amount /*= 100.0f*/)
{
	AttributeComp->ApplyHealthChange(this, Amount);
}

void ASCharacter::PrimaryInteract()
{
	//check not strictly necessary
	if (InteractionComp)
	{
		InteractionComp->PrimaryInteract();
	}
}

void ASCharacter::Attack(FName ActionName)
{
	ActionComp->StartActionByName(this, ActionName);
}

FVector ASCharacter::GetPawnViewLocation() const
{
	return CameraComp->GetComponentLocation();
}

