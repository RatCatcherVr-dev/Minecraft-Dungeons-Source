#pragma once

#include "CoreMinimal.h"
#include "Entitlement.generated.h"

UENUM(BlueprintType)
enum class EEntitlementsSource : uint8 {
	Invalid,
	Development,
	MinecraftNet,
	WindowsStore,
	XBL,
	PSN,
	Nintendo,
	EpicStore,
	Steam,
	CachedLocally
};
ENUM_NAME(EEntitlementsSource);

USTRUCT(BlueprintType)
struct DUNGEONS_API FEntitlement {
	
	GENERATED_BODY()

	FEntitlement() = default;
	explicit FEntitlement(FString name);
	explicit FEntitlement(FString name, EEntitlementsSource entitlementsSource);
	explicit FEntitlement(FString name, FString signature, FString signingUserId, EEntitlementsSource entitlementsSource);

	bool IsSignedBy(FString userId) const;
	
	const FString GetName() const {
		return Name;
	}
	const FString GetSignature() const {
		return Signature;
	}
	const FString GetSigningUserId() const {
		return SigningUserId;
	}
	const EEntitlementsSource GetEntitlementsSource() const {
		return EntitlementsSource;
	}

	bool IsRoaming() const;

private:
	UPROPERTY()
	FString Name;
	
	UPROPERTY()
	FString Signature;
	
	UPROPERTY()
	FString SigningUserId;
	
	UPROPERTY()
	EEntitlementsSource EntitlementsSource;
};

struct FPlatformEntitlementMapping {
	FString EntitlementName;
	FString PlatformSkuId;

	bool IsMatchingPlatformSkuId(const FString& platformSkuId) const;

private:
	bool IsPartiallyMatchingPlatformSkuId(const FString& platformSkuId) const;
};

namespace online { namespace entitlements {
	EEntitlementsSource getPrimaryEntitlementsSource();
}}
