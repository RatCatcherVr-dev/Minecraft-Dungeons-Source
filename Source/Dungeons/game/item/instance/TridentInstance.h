#pragma once

#include "AItemInstance.h"
#include "game/actor/item/TridentItem.h"
#include "TridentInstance.generated.h"

UCLASS(Blueprintable, BlueprintType)
class DUNGEONS_API ATridentInstance : public AItemInstance {
	GENERATED_BODY()
public:
	bool CanActivate() const override;

	void Activate(const FPredictionKey& predictionKey) override;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ATridentItem> ClassToSpawn;
	
private:
	unsigned int GetTridentCount() const;
};

