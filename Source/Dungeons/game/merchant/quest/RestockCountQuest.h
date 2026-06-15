#pragma once
#include "MerchantCountQuestBase.h"
#include "RestockCountQuest.generated.h"

class AMerchantBase;

UCLASS()
class DUNGEONS_API URestockCountQuest : public UMerchantCountQuestBase {
	GENERATED_BODY()
protected:
	int GetProgressCount() const override;
public:
	static URestockCountQuest* CreateSubobject(AMerchantBase* merchant, const FName& name);
};
