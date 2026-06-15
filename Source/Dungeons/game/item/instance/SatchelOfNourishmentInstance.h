#pragma once

#include "StorableDropBaseItemInstance.h"
#include "SatchelOfNourishmentInstance.generated.h"

UCLASS()
class DUNGEONS_API ASatchelOfNourishmentInstance : public AStorableDropBaseItemInstance {
	GENERATED_BODY()
public:
	void Activate(const FPredictionKey& predictionKey) override;
};