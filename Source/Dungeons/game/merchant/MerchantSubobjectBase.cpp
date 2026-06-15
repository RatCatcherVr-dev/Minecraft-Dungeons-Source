#include "Dungeons.h"
#include "type/MerchantBase.h"
#include "MerchantSubobjectBase.h"

const merchant::Context& UMerchantSubobjectBase::GetContext() const {
	return mMerchant->GetCurrentContext();
}

const merchant::Session& UMerchantSubobjectBase::GetSession() const {
	return mMerchant->GetCurrentSession();
}

AMerchantBase* UMerchantSubobjectBase::GetMerchantMutable() const {
	return mMerchant;
}



void UMerchantSubobjectBase::PostInitProperties() {
	Super::PostInitProperties();
	if (this != GetClass()->GetDefaultObject(false)) {
		mMerchant = Cast<AMerchantBase>(GetOuter());
		check(mMerchant && "outer is not a AMerchantBase");
		mMerchant->RegisterSubObject(this);
	}
}

const AMerchantBase* UMerchantSubobjectBase::GetMerchant() const {
	return mMerchant;
}

void UMerchantSubobjectBase::OnSetupSession() {}
void UMerchantSubobjectBase::OnCleanupSession() {}
