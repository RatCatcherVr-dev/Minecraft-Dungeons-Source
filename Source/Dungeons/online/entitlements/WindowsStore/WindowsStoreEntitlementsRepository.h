#pragma once
#include "online/entitlements/APIEntitlementRepositoryParent.h"

#include "WindowsStoreEntitlementsRepository.generated.h"

UCLASS()
class DUNGEONS_API UWindowsStoreEntitlementsRepository : public UAPIEntitlementRepositoryParent {
public:
	void RequestEntitlements() override;

	GENERATED_BODY()
};

