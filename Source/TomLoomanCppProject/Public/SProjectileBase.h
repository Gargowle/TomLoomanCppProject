// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SProjectileBase.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UParticleSystemComponent;
class UAudioComponent;
class USoundCue;
class UCameraShakeBase;

UCLASS(ABSTRACT)
class TOMLOOMANCPPPROJECT_API ASProjectileBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASProjectileBase();

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* SphereComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UProjectileMovementComponent* MovementComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UParticleSystemComponent* EffectComp;

	UPROPERTY(EditAnywhere, Category = "Effects")
	UAudioComponent* AudioComp;

	UPROPERTY(EditAnywhere, Category = "Effects")
	UParticleSystem* ImpactVFX;

	UPROPERTY(EditAnywhere, Category = "Effects")
	USoundCue* ImpactSound;

	UPROPERTY(EditAnywhere, Category = "Effects")
	TSubclassOf<UCameraShakeBase> CameraShake;

	UPROPERTY(EditAnywhere, Category = "Effects|Shake")
	float ImpactShakeInnerRadius;

	UPROPERTY(EditAnywhere, Category = "Effects|Shake")
	float ImpactShakeOuterRadius;
		
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called just before BeginPlay
	virtual void PostInitializeComponents() override;
	
	UFUNCTION()
	virtual void OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Explode();
};
