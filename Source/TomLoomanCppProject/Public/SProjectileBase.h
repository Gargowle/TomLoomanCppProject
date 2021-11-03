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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USphereComponent* SphereComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UProjectileMovementComponent* MovementComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Particles")
	UParticleSystemComponent* EffectComp;

	UPROPERTY(EditAnywhere, Category = "Audio")
	UAudioComponent* AudioComp;

	UPROPERTY(EditAnywhere, Category = "Particles")
	UParticleSystem* ImpactVFX;

	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundCue* ImpactSound;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UCameraShakeBase> CameraShake;
		
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called just before BeginPlay
	virtual void PostInitializeComponents() override;
	
	UFUNCTION()
	virtual void OnActorHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Explode();
};
