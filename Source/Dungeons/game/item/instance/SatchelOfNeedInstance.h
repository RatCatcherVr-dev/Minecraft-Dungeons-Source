#pragma once

#include "StorableDropBaseItemInstance.h"
#include "SatchelOfNeedInstance.generated.h"

UCLASS()
class DUNGEONS_API ASatchelOfNeedInstance : public AStorableDropBaseItemInstance {
	GENERATED_BODY()
public:
	void Activate(const FPredictionKey& predictionKey) override;
private:
	bool CanDropWaterBreathingPotion() const;
	bool ShouldDropWaterBreathingPotion() const;
};