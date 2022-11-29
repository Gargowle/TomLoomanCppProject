// Copyright Gargowle. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SGameplayInterface.h"
#include "SItemChest.generated.h"

class UStaticMeshComponent;

UCLASS()
class TOMLOOMANCPPPROJECT_API ASItemChest : public AActor, public ISGameplayInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	float TargetPitch;

	// implements the Interact Method of the SGameplay Interface
	// the suffix "_Implementation" comes from the fact that this method was tagged with UFUNCTION(BlueprintNativeEvent)
	void Interact_Implementation(APawn* InstigatorPawn);
	
	// Sets default values for this actor's properties
	ASItemChest();

	virtual void OnActorLoaded_Implementation() override;

protected:

	UPROPERTY(ReplicatedUsing="OnRep_LidOpened", BlueprintReadOnly, SaveGame) //ReplicatedUsing is the same as RepNotify in BP
	bool bLidOpened;

	UFUNCTION()
	void OnRep_LidOpened();

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) // BlueprintReadOnly such that we can set the relative rotation in the BP
	UStaticMeshComponent* LidMesh;

};
