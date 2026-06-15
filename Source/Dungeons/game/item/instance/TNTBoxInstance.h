#pragma once

#include "AItemInstance.h"
#include "game/actor/item/TNTBoxItem.h"
#include "TNTBoxInstance.generated.h"

UCLASS(Blueprintable, BlueprintType)
class DUNGEONS_API ATNTBoxInstance : public AItemInstance {
	GENERATED_BODY()
public:
	bool CanActivate() const override;

	void Activate(const FPredictionKey& predictionKey) override;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ATNTBoxItem> ClassToSpawn;
	
private:
	unsigned int GetTntCount() const;
};