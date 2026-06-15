#pragma once
#include "game/merchant/MerchantContext.h"
#include "game/merchant/MerchantSession.h"
#include "MerchantSubobjectBase.generated.h"

class AMerchantBase;

UCLASS(BlueprintType, Abstract)
class DUNGEONS_API UMerchantSubobjectBase : public UObject {
	GENERATED_BODY()
protected:
	UPROPERTY()
	AMerchantBase* mMerchant;
	const merchant::Context& GetContext() const;	
	const merchant::Session& GetSession() const;	

	AMerchantBase* GetMerchantMutable() const;	

public:
	void PostInitProperties() override;

	const AMerchantBase* GetMerchant() const;

	virtual void OnSetupSession();
	virtual void OnCleanupSession();
};
