// © 2020 Mojang Synergies AB. TM Microsoft Corporation.

#pragma once

#include "CoreMinimal.h"
#include "repgraph/Actor_RepSpatializeStatic.h"
#include "character/BaseCharacter.h"
#include "ConduitButton.generated.h"

/**
 *
 */
UCLASS(Abstract)
class DUNGEONS_API AConduitButton : public APropActor_RepAlways
{
	GENERATED_BODY()
public:
	AConduitButton();

	virtual void BeginPlay() override;

	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable)
	void GiveConduitToCharacter(ACharacter* Character);

	UFUNCTION(BlueprintCallable)
	void DropConduitStorable();

	UFUNCTION(BlueprintCallable)
	void CompleteDarkConduit();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	USceneComponent* DefaultSceneRootInternal;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UReplicatedInteractableComponent* Interactable;

	UPROPERTY(BlueprintReadWrite)
	bool RequiresHeldConduitToBeActive;

	UFUNCTION()
	void OnConduitHeld();
	UFUNCTION()
	void OnConduitDropped();
};
