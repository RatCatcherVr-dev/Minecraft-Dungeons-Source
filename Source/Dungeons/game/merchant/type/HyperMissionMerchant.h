#pragma once
#include "MerchantBase.h"
#include "HyperMissionMerchant.generated.h"

class UMerchantSelectionBase;
class APlayerCharacter;

UCLASS()
class DUNGEONS_API AHyperMissionMerchant : public AMerchantBase {
	GENERATED_BODY()
public:
	AHyperMissionMerchant(const FObjectInitializer& ObjectInitializer);

	void RefreshOfferings();

	void OnPlayerInteraction(APlayerCharacter*) override;
};
