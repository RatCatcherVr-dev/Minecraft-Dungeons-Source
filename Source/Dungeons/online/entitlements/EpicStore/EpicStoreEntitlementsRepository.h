#pragma once

#include "UObject/ObjectMacros.h"
#include "../EntitlementsRepository.h"
#include "EpicStoreEntitlementsRepository.generated.h"

UCLASS()
class DUNGEONS_API UEpicStoreEntitlementsRepository : public UEntitlementsRepository {

	GENERATED_BODY()
public:	
	void RequestEntitlements() override;
protected:
	void ConfigureForPlatform() override;
private:
	void OnEntitlementsReceived(TArray<FString>);
	void OnEntitlementsReqFailed();
};
