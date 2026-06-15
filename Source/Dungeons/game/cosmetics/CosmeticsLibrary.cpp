#include "Dungeons.h"
#include "CosmeticsLibrary.h"
#include "CosmeticsEntry.h"
#include "CosmeticItemDef.h"

const FString UCosmeticsLibrary::ContextString = FString(TEXT("Cosmetics Entry"));

UCosmeticsLibrary::UCosmeticsLibrary() {	
	static const auto objectFinder = ConstructorHelpers::FObjectFinder<UDataTable>(TEXT("DataTable'/Game/DataTables/Assets/Cosmetics.Cosmetics'"));
	CosmeticsLookupTable = objectFinder.Object;
}

UCosmeticItemDef* UCosmeticsLibrary::FindDefinition(const FName& name) const {
	auto* dataEntry = CosmeticsLookupTable->FindRow<FCosmeticsEntry>(name, ContextString);
	return UCosmeticItemDef::CreateDefinition(name, dataEntry);
}

TArray<UCosmeticItemDef*> UCosmeticsLibrary::FindDefinitions(const TArray<CosmeticsSaveData>& saveData) const {
	TArray<UCosmeticItemDef*> cosmeticItemDefs;

	for (const auto& cosmeticData : saveData) {
		if (auto cosmeticItemDef = FindDefinition(cosmeticData.id)) {
			if (cosmeticItemDef->IsCorrectType(cosmeticData.type)) {
				cosmeticItemDefs.Add(cosmeticItemDef);
			}
		}
	}

	return cosmeticItemDefs;
}

TArray<UCosmeticItemDef*> UCosmeticsLibrary::FindDefinitions(const CosmeticsDefFilterPredicate defFilter) const {
	TArray<UCosmeticItemDef*> cosmeticItemDefs;
	
	for (const auto& name : CosmeticsLookupTable->GetRowNames()) {
		if (auto* def = FindDefinition(name)) {
			if (defFilter(*def)) {
				cosmeticItemDefs.Add(def);
			}
		}
	}

	return cosmeticItemDefs;
}

UCosmeticItemDef* UCosmeticsLibrary::FindDefinitionByEntitlementName(const FString& entitlementName) const {
	TArray<UCosmeticItemDef*> results = FindDefinitions([&entitlementName](const UCosmeticItemDef& cosmetic) {
		return entitlementName.Equals(cosmetic.GetEntitlementName());
	});

	check(results.Num() == 1 && "Failed to find exactly one cosmetic for entitlement name");
	return results.Num() == 1 ? results[0] : nullptr;
}
