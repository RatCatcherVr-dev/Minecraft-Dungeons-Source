#pragma once
#include "MerchantBase.h"
#include "AdventureHubMerchant.generated.h"

UCLASS()
class DUNGEONS_API AAdventureHubMerchant : public AMerchantBase {
	GENERATED_BODY()
public:
	AAdventureHubMerchant(const FObjectInitializer& ObjectInitializer);

	void OnPlayerInteraction(APlayerCharacter*) override;
};
