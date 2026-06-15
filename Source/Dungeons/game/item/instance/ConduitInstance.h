#pragma once

#include "AItemInstance.h"
#include "game/actor/item/ConduitItem.h"
#include "ConduitInstance.generated.h"

UCLASS(Blueprintable, BlueprintType)
class DUNGEONS_API AConduitInstance : public AItemInstance {
	GENERATED_BODY()
public:
	bool CanActivate() const override;

	void Activate(const FPredictionKey& predictionKey) override;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AConduitItem> ClassToSpawn;

private:
	unsigned int GetConduitCount() const;
};