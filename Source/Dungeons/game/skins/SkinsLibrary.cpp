#include "Dungeons.h"
#include "SkinsLibrary.h"
#include "SkinEntry.h"
#include "SkinDef.h"

const FString USkinsLibrary::ContextString = FString(TEXT("Skins Entry"));

USkinsLibrary::USkinsLibrary() {	
	static const auto objectFinder = ConstructorHelpers::FObjectFinder<UDataTable>(TEXT("DataTable'/Game/DataTables/Assets/Skins.Skins'"));
	SkinsLookupTable = objectFinder.Object;
}

USkinDef* USkinsLibrary::GetOrCreateDefinition(const FName& dataName, const FSkinEntry& dataEntry) {
	if (!dataEntry.IsReleased)
		return nullptr;

	if (!SkinsDefsLoadedMap.Contains(dataName)) {
		auto* skinDef = USkinDef::CreateDefinition(dataName, dataEntry);
		SkinsDefsLoaded.Add(skinDef);
		SkinsDefsLoadedMap.Add(dataName, skinDef);
	}

	return *SkinsDefsLoadedMap.Find(dataName);
}

USkinDef* USkinsLibrary::GetOrCreateDefinition(const FName& name) {
	if (!name.IsNone()) {
		if (auto* dataEntry = SkinsLookupTable->FindRow<FSkinEntry>(name, ContextString)){
			return GetOrCreateDefinition(name, *dataEntry);
		}	
	}
	return nullptr;
}

TArray<USkinDef*> USkinsLibrary::GetOrCreateDefinitions(const SkinsDefFilterPredicate defFilter) {
	TArray<USkinDef*> cosmeticItemDefs;
	
	for (const auto& name : SkinsLookupTable->GetRowNames()) {
		if (auto* def = GetOrCreateDefinition(name)) {
			if (defFilter(*def)) {
				cosmeticItemDefs.Add(def);
			}
		}
	}

	return cosmeticItemDefs;
}
