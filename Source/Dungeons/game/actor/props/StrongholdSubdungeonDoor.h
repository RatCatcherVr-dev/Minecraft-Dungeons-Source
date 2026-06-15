#pragma once

#include "game/actor/PropActor.h"
#include "game/component/EyeOfEnderDropComponent.h"

#include "repgraph/Actor_RepSpatializeStatic.h"
#include "StrongholdSubdungeonDoor.generated.h"


UCLASS()
class DUNGEONS_API AStrongholdSubdungeonDoor : public APropActor_RepAlways {
	GENERATED_BODY()
public:
	AStrongholdSubdungeonDoor();
	void BeginPlay() override;
	void Tick(float DeltaSeconds) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	USceneComponent* DefaultSceneRootInternal;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class UReplicatedInteractableComponent* InteractableComponent;

	UPROPERTY(EditDefaultsOnly, Category="Dungeons")
	TSet<EEyeOfEnderType> EyeOfEnderUnlockCombination;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int EyeOfEnderUnlockCount;

private:
	void SetupLockedBehavior(ABasePlayerController*);
	bool mInitialized = false;
};
