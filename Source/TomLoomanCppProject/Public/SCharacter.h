// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class USInteractionComponent;
class UAnimMontage;
class USAttributeComponent;
class UParticleSystem;
class USActionComponent;

UCLASS()
class TOMLOOMANCPPPROJECT_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASCharacter();

protected:

	// VisibleAnywhere: read only such that it is visible in Editor but not editable
	UPROPERTY(VisibleAnywhere, Category = "Attack")
	FName TimeToHitParamName;

	UPROPERTY(VisibleAnywhere, Category = "Attack")
	FName HandSocketName;

	UPROPERTY(EditAnywhere, Category = "Attack")
	TSubclassOf<AActor> PrimaryProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Attack")
	TSubclassOf<AActor> SecondaryProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Attack")
	TSubclassOf<AActor> TeleportProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Attack")
	UAnimMontage* AttackAnim;

	UPROPERTY(EditAnywhere, Category = "Attack")
	UParticleSystem* AttackVFX;

	FTimerHandle TimerHandle_Attack;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere)
	USInteractionComponent* InteractionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USAttributeComponent* AttributeComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USActionComponent* ActionComp;

	DECLARE_DELEGATE_OneParam(FProjectileDelegate, TSubclassOf<AActor>);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	UFUNCTION()
	void OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta);

	// Methods bound to input
	void MoveForward(float Value);
	void MoveRight(float Value);
	void PrimaryInteract();
	void Attack(TSubclassOf<AActor> ProjectileClass);
	UFUNCTION() // must be declared UFUNCTION for being delegated to the timer delegate
	void Attack_TimeElapsed(TSubclassOf<AActor> ProjectileClass);

	virtual FVector GetPawnViewLocation() const override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(Exec)
	void HealSelf(float Amount = 100.0f);

};
