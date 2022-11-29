// Copyright Gargowle. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMagicProjectile.h"
#include "STeleportProjectile.generated.h"

class UParticleSystem;
/**
 * 
 */
UCLASS()
class TOMLOOMANCPPPROJECT_API ASTeleportProjectile : public ASProjectileBase
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, Category = "Teleport")
	float ProjectileFlightTime = 0.2f;
	
	UPROPERTY(EditAnywhere, Category = "Teleport")
	float PortalOpenDelay = 0.2f;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void StartTeleportation();

protected:
	UFUNCTION()
	void TeleportInstigator();

	FTimerHandle TimerHandle_Teleport;
	
	// override _Implementation because event is marked as "BlueprintNativeEvent"
	virtual void Explode_Implementation() override;
};
