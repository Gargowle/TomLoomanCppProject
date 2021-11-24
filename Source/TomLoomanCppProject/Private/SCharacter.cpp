// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SInteractionComponent.h"
#include "SAttributeComponent.h"
// for drawing
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

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

	CameraComp = CreateDefaultSubobject<UCameraComponent>("CameraComp");
	CameraComp->SetupAttachment(SpringArmComp);

	InteractionComp = CreateDefaultSubobject<USInteractionComponent>("InteractionComponent");

	AttributeComp = CreateDefaultSubobject<USAttributeComponent>("AttributeComp");

	// Use "Action RPG style camera" in which camera can be rotated independently of the character's direction
	bUseControllerRotationYaw = false;

	// disable "strafing" but instead make the character turn into the direction of movement
	GetCharacterMovement()->bOrientRotationToMovement = true;

	TimeToHitParamName = TEXT("HitFlashTimeToHit");
	HandSocketName = TEXT("Muzzle_01");
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	AttributeComp->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);
}

void ASCharacter::OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta)
{
	if(Delta < 0.0f)
	{
		if (NewHealth <= 0.0f)
		{
			APlayerController* PC = Cast<APlayerController>(GetController());
			DisableInput(PC);
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

	PlayerInputComponent->BindAction<FProjectileDelegate>("PrimaryAttack", IE_Pressed, this, &ASCharacter::Attack, PrimaryProjectileClass);
	PlayerInputComponent->BindAction<FProjectileDelegate>("SecondaryAttack", IE_Pressed, this, &ASCharacter::Attack, SecondaryProjectileClass);
	PlayerInputComponent->BindAction<FProjectileDelegate>("TeleportAttack", IE_Pressed, this, &ASCharacter::Attack, TeleportProjectileClass);
	PlayerInputComponent->BindAction("PrimaryInteract", IE_Pressed, this, &ASCharacter::PrimaryInteract);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);

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

void ASCharacter::Attack(TSubclassOf<AActor> ProjectileClass)
{
	PlayAnimMontage(AttackAnim);
	
	FTimerDelegate TimerDel;
	TimerDel.BindUFunction(this, FName("Attack_TimeElapsed"), ProjectileClass);

	GetWorldTimerManager().SetTimer(TimerHandle_Attack, TimerDel, 0.17f, /*looping*/ false);

	// If we were to clear this timer in order for it not to go off (for example: when player dies), this is how you would do it:
	//GetWorldTimerManager().ClearTimer(TimerHandle_PrimaryAttack);

}

void ASCharacter::Attack_TimeElapsed(TSubclassOf<AActor> ProjectileClass)
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
		CollisionQueryParams.AddIgnoredActor(this);

		FHitResult AttackTargetHitResult;

		constexpr float LineTraceLength = 100000.0f; // 1000 meter

		FVector CameraPosition = CameraComp->GetComponentLocation();
		FVector LineTraceEnd = CameraPosition + GetControlRotation().Vector() * LineTraceLength;

		bool bHitSuccessful = GetWorld()->LineTraceSingleByObjectType(AttackTargetHitResult, CameraPosition, LineTraceEnd, ObjectQueryParams, CollisionQueryParams);

		FVector HandLocation = GetMesh()->GetSocketLocation(HandSocketName);

		FRotator RotationFromLineTrace = ((bHitSuccessful ? AttackTargetHitResult.Location : LineTraceEnd) - HandLocation).Rotation();

		//TM = Transform Matrix
		// Spawn at the location of the Muzzle_01 location into the yielded by the line trace
		FTransform SpawnTM = FTransform(RotationFromLineTrace, HandLocation);

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		// add the information on who shot this projectile
		SpawnParams.Instigator = this;

		GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnTM, SpawnParams);

		UGameplayStatics::SpawnEmitterAttached(AttackVFX, GetMesh(), HandSocketName);
	}
}

