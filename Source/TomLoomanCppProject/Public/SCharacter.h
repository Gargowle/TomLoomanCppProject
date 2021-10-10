// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class USInteractionComponent;
class UAnimMontage;

UCLASS()
class TOMLOOMANCPPPROJECT_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASCharacter();

protected:

	UPROPERTY(EditAnywhere, Category = "Attack")
	TSubclassOf<AActor> PrimaryProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Attack")
	TSubclassOf<AActor> SecondaryProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Attack")
	UAnimMontage* AttackAnim;

	FTimerHandle TimerHandle_Attack;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere)
	USInteractionComponent* InteractionComp;

	DECLARE_DELEGATE_OneParam(FProjectileDelegate, TSubclassOf<AActor>);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Methods bound to input
	void MoveForward(float Value);
	void MoveRight(float Value);
	void PrimaryInteract();
	void Attack(TSubclassOf<AActor> ProjectileClass);
	UFUNCTION() // must be declared UFUNCTION for being delegated to the timer delegate
	void Attack_TimeElapsed(TSubclassOf<AActor> ProjectileClass);
	void Teleport();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
