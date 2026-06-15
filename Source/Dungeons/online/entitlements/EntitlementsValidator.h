#pragma once

#include "CoreMinimal.h"
#include "Entitlement.h"
#include "EntitlementsValidator.generated.h"

UCLASS()
class UEntitlementsValidator : public UObject {

	GENERATED_BODY()

public:
	virtual bool IsEntitlementTamperedWith(const FEntitlement&, FString expectedEntitlementName, FString expectedSigningUser);

	static bool HasEntitlement(const TArray<FEntitlement>&, FString);
	static bool HasAnyEntitlement(const TArray<FEntitlement>&, const TArray<FString>&);
	static TOptional<FEntitlement> FindEntitlement(const TArray<FEntitlement>&, FString);
};