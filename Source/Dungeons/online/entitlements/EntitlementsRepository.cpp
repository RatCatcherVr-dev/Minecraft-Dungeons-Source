#include "Dungeons.h"
#include "EntitlementsRepository.h"
#include "EntitlementsValidator.h"
#include "save/GlobalSaveData.h"
#include "util/ConfigFileUtil.h"
#include "util/Algo.hpp"

namespace internal
{

bool IsDynamicEntitlement(const EEntitlementsSource entitlementSource) {
	return entitlementSource == EEntitlementsSource::MinecraftNet;
}

}

void UEntitlementsRepository::RequestEntitlements() {
}

TArray<FEntitlement> UEntitlementsRepository::GetEntitlements() const {
	TArray<FEntitlement> mergedEntitlements = Entitlements;
	mergedEntitlements += CachedEntitlements;
	return mergedEntitlements;
}

TOptional<FEntitlement> UEntitlementsRepository::GetEntitlement(const FString& entitlementName) const {
	return UEntitlementsValidator::FindEntitlement(GetEntitlements(), entitlementName);
}

bool UEntitlementsRepository::IsEntitlementTamperedWith(const FEntitlement& entitlement, const FString& expectedEntitlementName, const FString& expectedSigningUser) {
	return GetValidator()->IsEntitlementTamperedWith(entitlement, expectedEntitlementName, expectedSigningUser);
}

void UEntitlementsRepository::Init(const EEntitlementsSource source) {
	Source = source;
	EntitlementMapping = configfile::EntitlementMappings();
	ConfigureForPlatform();
}

void UEntitlementsRepository::AddEntitlementsForPlatform(const FString& platformSkuId, const EEntitlementsSource entitlementSource) {
	const auto mappedEntitlements = EntitlementMapping.FilterByPredicate(RETLAMBDA(it.IsMatchingPlatformSkuId(platformSkuId)));

	if (mappedEntitlements.Num() > 0) {
		for (const auto& mappedEntitlement : mappedEntitlements) {
			AddEntitlement(mappedEntitlement.EntitlementName, entitlementSource);
		}
	} else {
		// If no mappings exist the platform ID and the game ID are the same.
		AddEntitlement(platformSkuId, entitlementSource);
	}
}

void UEntitlementsRepository::AddEntitlement(const FEntitlement& entitlement) {
	if (entitlement.GetEntitlementsSource() != EEntitlementsSource::CachedLocally) {
		Entitlements.Add(entitlement);
	}
	UE_LOG(LogDungeonsEntitlements, Log, TEXT("Entitlement added: %s"), *entitlement.GetName());
	if (internal::IsDynamicEntitlement(entitlement.GetEntitlementsSource())) {
		if (!CachedEntitlements.ContainsByPredicate([entitlement] (const FEntitlement& localEntitlement) {
			return entitlement.GetName().Equals(localEntitlement.GetName());
		})) {
			UE_LOG(LogDungeonsEntitlements, Log, TEXT("Cached entitlement added: %s"), *entitlement.GetName());
			CachedEntitlements.Add(FEntitlement(entitlement.GetName(), entitlement.GetSignature(), entitlement.GetSigningUserId(), EEntitlementsSource::CachedLocally));
		}
	}
}

void UEntitlementsRepository::AddEntitlement(const FString& entitlementName, const EEntitlementsSource entitlementSource) {
	AddEntitlement(FEntitlement(entitlementName, entitlementSource));
}

void UEntitlementsRepository::AddEntitlements(const TArray<FEntitlement>& entitlements) {
	for (const auto& entitlement : entitlements) {
		AddEntitlement(entitlement);
	}
}

void UEntitlementsRepository::RemoveEntitlements(const Pred<FEntitlement>& predicate) {
	Entitlements.RemoveAll(predicate);
}

void UEntitlementsRepository::RemoveAllEntitlements() {
	Entitlements.Empty();
}

EEntitlementsSource UEntitlementsRepository::GetSource() const {
	return Source;
}

UEntitlementsValidator* UEntitlementsRepository::GetValidator() {
	if (!Validator) {
		Validator = CreateValidator();
	}
	return Validator;
}

void UEntitlementsRepository::ConfigureForPlatform() {
}

UEntitlementsValidator* UEntitlementsRepository::CreateValidator() {
	return NewObject<UEntitlementsValidator>();
}

void UEntitlementsRepository::SetEntitlements(const TArray<FEntitlement>& entitlements) {
	RemoveAllEntitlements();
	for (const auto& entitlement : entitlements) {
		AddEntitlement(entitlement);
	}
}

void UEntitlementsRepository::SetCachedEntitlements(const TArray<FEntitlement>& entitlements) {
	CachedEntitlements = entitlements;
}

const TArray<FEntitlement>& UEntitlementsRepository::GetCachedEntitlements() const {
	return CachedEntitlements;
}
