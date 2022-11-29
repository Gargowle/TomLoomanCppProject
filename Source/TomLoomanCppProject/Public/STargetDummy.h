// Copyright Gargowle. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "STargetDummy.generated.h"

class USAttributeComponent;

UCLASS()
class TOMLOOMANCPPPROJECT_API ASTargetDummy : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASTargetDummy();

protected:

	UPROPERTY(VisibleAnywhere)
	USAttributeComponent* AttributeComp;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* MeshComp;

	virtual void PostInitializeComponents() override;

	UFUNCTION()
	void OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewValue, float Delta);
};
