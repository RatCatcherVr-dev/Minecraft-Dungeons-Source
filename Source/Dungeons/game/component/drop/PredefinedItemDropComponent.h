#pragma once

#include "game/component/drop/ItemDropComponent.h"
#include "PredefinedItemDropComponent.generated.h"

/**
 * Generates drops based on its predefined settings.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DUNGEONS_API UPredefinedItemDropComponent : public UItemDropComponent {
	GENERATED_BODY()

public:
	void SetDropData(const FItemDrop& dropData);

protected:
	TArray<FNetworkedItemDropData> GatherItemDropData(const FItemDropSource& dropSource) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	FItemDrop DropData;
};
