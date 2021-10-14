// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SMagicProjectile.h"
#include "STeleportProjectile.generated.h"

class UParticleSystem;
/**
 * 
 */
UCLASS()
class TOMLOOMANCPPPROJECT_API ASTeleportProjectile : public ASMagicProjectile
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, Category = "Teleport")
	float ProjectileFlightTime = 0.2f;


	UPROPERTY(EditAnywhere, Category = "Teleport")
	float PortalOpenDelay = 0.2f;


	UPROPERTY(EditAnywhere, Category="Particles")
	UParticleSystem* PortalEmitter;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void StartTeleportation();

protected:
	UFUNCTION()
	void TeleportInstigator();

	FTimerHandle TimerHandle_Teleport;

	virtual void PostInitializeComponents() override;

	UFUNCTION()
	void OnActorHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
