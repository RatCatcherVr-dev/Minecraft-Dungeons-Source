#pragma once
#include "ProgressStatQuest.h"
#include "DynamicProgressStatQuest.generated.h"

class AMerchantBase;

UCLASS()
class DUNGEONS_API UDynamicProgressStatQuest : public UProgressStatQuest {
	GENERATED_BODY()
public:
	using ProgressStatProvider = std::function<EProgressStat()>;
	static UDynamicProgressStatQuest* CreateSubobject(AMerchantBase* merchant, const FName& name, EProgressStat fallbackStat, int requiredCount, ProgressStatProvider statProvider);
	void ResetProgress() const override;

protected:
	EProgressStat GetProgressStat() const override;
	ProgressStatProvider mProgressStatProvider;
};
