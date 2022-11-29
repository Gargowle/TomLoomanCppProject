// Copyright Gargowle. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SAICharacter.generated.h"


class USActionComponent;
class USAttributeComponent;
class UPawnSensingComponent;
class UUserWidget;
class USWorldUserWidget;

UCLASS(Abstract)
class TOMLOOMANCPPPROJECT_API ASAICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASAICharacter();

	AActor* GetTargetActor() const;

protected:

	virtual void PostInitializeComponents() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USAttributeComponent* AttributeComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USActionComponent* ActionComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UPawnSensingComponent* PawnSensingComp;

	// VisibleAnywhere: read only such that it is visible in Editor but not editable
	UPROPERTY(VisibleAnywhere, Category = "Attack")
	FName TimeToHitParamName;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> HealthBarWidgetClass;

	USWorldUserWidget* ActiveHealthBar;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> PlayerSpottedWidgetClass;

	USWorldUserWidget* PlayerSpottedWidget;

	FTimerHandle PlayerSpottedTimerHandle;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName TargetActorKey;

	UFUNCTION()
	void OnPawnSeen(APawn* Pawn);

	UFUNCTION()
	void OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewValue, float Delta);

	void SetTargetActor(AActor* NewTarget);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastSpawnPlayerSpottedWidget();
};
