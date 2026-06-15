#pragma once

#include "CoreMinimal.h"
#include "../Entitlement.h"
#include "online/entitlements/EntitlementsValidator.h"
#include "MinecraftAPIEntitlementsValidator.generated.h"

UCLASS()
class UMinecraftAPIEntitlementsValidator : public UEntitlementsValidator {

	GENERATED_BODY()

public:	
	bool IsEntitlementTamperedWith(const FEntitlement&, FString expectedEntitlementName, FString expectedSigningUser) override;

private:
	bool HasCorrectSignature(const FEntitlement&, FString expectedSigningUser) const;
};
