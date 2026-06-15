#pragma once

#include "CoreMinimal.h"
#include "game/component/drop/ItemDropComponent.h"
#include "game/actor/item/ItemDropChanceActor.h"
#include "RollingDropComponent.generated.h"

/**
 * Generates drops based on the rolling logic in ItemDropActor.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DUNGEONS_API URollingDropComponent : public UItemDropComponent
{
	GENERATED_BODY()

public:
	URollingDropComponent();

	void BeginPlay() override;

protected:
	TArray<FNetworkedItemDropData> GatherItemDropData(const FItemDropSource& dropSource) override;

private:
	AItemDropChanceActor* DropChanceActor;
};


