// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SGameplayInterface.h"
#include "SPowerUp.generated.h"

class UStaticMeshComponent;

UCLASS(Abstract)
class TOMLOOMANCPPPROJECT_API ASPowerUp : public AActor, public ISGameplayInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASPowerUp();

	virtual void Interact_Implementation(APawn* InstigatorPawn) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* MeshComp;

	UPROPERTY(EditAnywhere)
	float SecondsToRespawn;

	UFUNCTION()
	virtual void ApplyEffect(APawn* InstigatorPawn) PURE_VIRTUAL(ASPowerUp::ApplyEffect, );

	virtual bool IsInstigatorEligible(APawn * InstigatorPawn) PURE_VIRTUAL(ASPowerUp::IsInstigatorEligible, return false;);

	virtual void Respawn();
};
