#pragma once

#include "CoreMinimal.h"
#include "EntitlementsRepository.h"
#include "CompositeEntitlementsRepository.generated.h"

UENUM()
enum class ESourceState : uint8 {
	Pending,
	RequestSent,
	Failed,
	Succeeded
};

USTRUCT()
struct FRegisteredSource {
	GENERATED_BODY()

	FRegisteredSource();
	FRegisteredSource(UEntitlementsRepository*, ESourceState);

	UPROPERTY()
	UEntitlementsRepository* Repository;

	ESourceState State;

	FDelegateHandle OnEntitlementProvidedHandle;

	FDelegateHandle OnEntitlementsRequestFailedHandle;
};

UCLASS()
class DUNGEONS_API UCompositeEntitlementsRepository final : public UEntitlementsRepository {

	GENERATED_BODY()

public:	
	void RegisterRepository(EEntitlementsSource, UEntitlementsRepository*);	

	void RequestEntitlements() override;

	bool IsEntitlementTamperedWith(const FEntitlement&, const FString& expectedEntitlementName, const FString& expectedSigningUser) override;
private:
	void RequestEntitlements(EEntitlementsSource, FRegisteredSource&);
	
	void OnEntitlementsProvidedByRegisteredRepository(const TArray<FEntitlement>&, EEntitlementsSource);

	void OnRegisteredRepositoryEntitlementsRequestFailed(EEntitlementsSource);
	
	void BindDelegates(EEntitlementsSource);

	void ResetSourceStates();

	void HandleConsoleEntitlement(const EEntitlementsSource);

	void BroadcastIfAllSourcesReceived();

	UPROPERTY()
	TMap<EEntitlementsSource, FRegisteredSource> RegisteredSources;
};