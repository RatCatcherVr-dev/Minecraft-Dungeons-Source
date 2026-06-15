#pragma once
#include "MerchantBase.h"
#include "VillageMerchant.generated.h"

UCLASS()
class DUNGEONS_API AVillageMerchant : public AMerchantBase {
	GENERATED_BODY()
private:
	static const FText RescueVillagerQuestText;
	static const FText RescueVillagerQuestExplainerText;
public:
	static const std::string RescueVillagerObjectiveTag;
	AVillageMerchant(const FObjectInitializer& ObjectInitializer);
};