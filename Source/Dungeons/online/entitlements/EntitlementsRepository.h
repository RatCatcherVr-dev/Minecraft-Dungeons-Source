#pragma once

#include "CoreMinimal.h"
#include "Entitlement.h"
#include "EntitlementsValidator.h"
#include "CommonTypes.h"
#include "EntitlementsRepository.generated.h"

class APlayerCharacter;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnEntitlementsProvided, const TArray<FEntitlement>&)
DECLARE_MULTICAST_DELEGATE(FOnEntitlementsRequestFailed)

UCLASS()
class UEntitlementsRepository : public UObject {

	GENERATED_BODY()
	
public:
	virtual void RequestEntitlements();
	
	void Init(EEntitlementsSource);

	virtual bool IsEntitlementTamperedWith(const FEntitlement&, const FString& expectedEntitlementName, const FString& expectedSigningUser);

	TArray<FEntitlement> GetEntitlements() const;
	
	TOptional<FEntitlement> GetEntitlement(const FString& entitlementName) const;

	void AddEntitlement(const FEntitlement&);

	void AddEntitlement(const FString& entitlementName, EEntitlementsSource);

	void SetCachedEntitlements(const TArray<FEntitlement>&);
	const TArray<FEntitlement>& GetCachedEntitlements() const;

	FOnEntitlementsProvided OnEntitlementsProvided;
	FOnEntitlementsRequestFailed OnEntitlementsRequestFailed;

protected:
	virtual void ConfigureForPlatform();
	
	virtual UEntitlementsValidator* CreateValidator();

	void SetEntitlements(const TArray<FEntitlement>&);
	
	void AddEntitlementsForPlatform(const FString& platformSkuId, EEntitlementsSource);
	
	void AddEntitlements(const TArray<FEntitlement>&);

	void RemoveEntitlements(const Pred<FEntitlement>&);
	
	void RemoveAllEntitlements();

	EEntitlementsSource GetSource() const;

private:

	UEntitlementsValidator* GetValidator();

	TArray<FEntitlement> Entitlements;

	TArray<FEntitlement> CachedEntitlements;
	
	UPROPERTY()
	UEntitlementsValidator* Validator;

	TArray<FPlatformEntitlementMapping> EntitlementMapping;

	EEntitlementsSource Source;
};
