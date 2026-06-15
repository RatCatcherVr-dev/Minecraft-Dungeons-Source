#pragma once
#include "online/entitlements/APIEntitlementRepositoryParent.h"

#include "MinecraftAPIEntitlementsRepository.generated.h"

UCLASS()
class DUNGEONS_API UMinecraftAPIEntitlementsRepository : public UAPIEntitlementRepositoryParent {
public:
	void RequestEntitlements() override;

	GENERATED_BODY()
};

