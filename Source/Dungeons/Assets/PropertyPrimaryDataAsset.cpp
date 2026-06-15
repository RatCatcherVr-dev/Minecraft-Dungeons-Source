#include "PropertyPrimaryDataAsset.h"

#include <PackageName.h>
#include <UObjectBaseUtility.h>
#include <Engine/AssetManager.h>
#include "AbilitySystemGlobals.h"
#include "GameplayCueManager.h"
#include "GameplayCueSet.h"
#include "game/GameTypes.h"
#include "game/Enchantments/EnchantmentUtil.h"
#include "game/ArmorProperties/ArmorPropertyType.h"
#include "game/ArmorProperties/ArmorPropertyTypeDefs.h"
#include <AssetBundleData.h>
#include "Dungeons.h"
#include "Assets/EnchantmentAssetFinder.h"
#include "Assets/ArmorPropertyAssetFinder.h"
#include "game/item/ItemType.h"
#include "util/Algo.h"
#include "ItemPrimaryDataAsset.h"


const FName UPropertyPrimaryDataAsset::DecorBundle(TEXT("PropertyPrimaryData"));

namespace propertyprimaryasset {
	TSet<EArmorPropertyID> GetHardreferencedArmorProperties() {
		TSet<EArmorPropertyID> properties;
		for (const auto* item : GetItemRegistry().GetValues()) {
			if (game::item::asset::shouldPackageAssets(item->getId())) {
				properties.Append(item->getDefaultArmorPropertyIds());
			}
		}

		return properties;
	}
	TSet<EEnchantmentTypeID> GetHardReferencedEnchantments() {
		TSet<EEnchantmentTypeID> enchantments;
		for (const auto* item : GetItemRegistry().GetValues()) {
			if (game::item::asset::shouldPackageAssets(item->getId())) {
				enchantments.Append(algo::map_tarray(item->getDefaultEnchantments(), [](const auto& v) { return v.TypeID; }));
			}
		}

		return enchantments;
	}
}

UPropertyPrimaryDataAsset::UPropertyPrimaryDataAsset() {

	Rules.bApplyRecursively = false;
	Rules.Priority = 0;
	bIsRuntimeLabel = false;
}

#if WITH_EDITORONLY_DATA

void UPropertyPrimaryDataAsset::UpdateAssetBundleData() {
	Super::UpdateAssetBundleData();

	if (!UAssetManager::IsValid())
	{
		return;
	}

	// enchantments
	static TSet<EArmorPropertyID> seenProperties = propertyprimaryasset::GetHardreferencedArmorProperties();
	static TSet<EEnchantmentTypeID> seenEnchantments = propertyprimaryasset::GetHardReferencedEnchantments();
	
	UAssetManager& Manager = UAssetManager::Get();
	TArray<FSoftObjectPath> NewPaths = IDungeonsModule::Get().GetArmorPropertyAssetFinder()->GetArmorPropertyAssets([](EArmorPropertyID id) {
		bool disabled = game::armorproperties::type::getArmorPropertyType(id).isDisabled();
		bool used = seenProperties.Contains(id);

		if (disabled && used) {
			UE_LOG(LogDungeons, Error, TEXT("Disabled armor property %s is present on an item that will be packaged. Overriding disabled and packaging anyway."), *GetEnumValueToStringStripped(id));
		}

		return !disabled || used;
	});

	NewPaths.Append(IDungeonsModule::Get().GetEnchantmentAssetFinder()->GetEnchantmentAssets([](EEnchantmentTypeID id) {
		bool workInProgress = game::enchantment::type::getEnchantmentType(id).isWorkInProgress();
		bool used = seenEnchantments.Contains(id);
		
		if (workInProgress && used) {
			UE_LOG(LogDungeons, Error, TEXT("Work in progress enchantment %s is present on an item that will be packaged. Overriding work in progress and packaging anyway."), *GetEnumValueToStringStripped(id));
		}

		return !workInProgress || used;
	}));

	UGameplayCueManager* gameplayCueManager = UAbilitySystemGlobals::Get().GetGameplayCueManager();
	
	TArray<UGameplayCueSet*> gameplayCues = gameplayCueManager->GetGlobalCueSets();
	TSet<FSoftObjectPath> gameplayCuePaths;
	for (UGameplayCueSet* set : gameplayCues) {
		TArray<FString> cues;
		set->GetFilenames(cues);
		for (const auto& cue : cues) {
			FSoftObjectPath cueSoftPath = FSoftObjectPath(cue);
			if (cueSoftPath.IsValid())
				gameplayCuePaths.Add(MoveTemp(cueSoftPath));
		}
	}

	NewPaths.Append(gameplayCuePaths.Array());

	AssetBundleData.SetBundleAssets(DecorBundle, MoveTemp(NewPaths));

	FPrimaryAssetId PrimaryAssetId = GetPrimaryAssetId();
	Manager.SetPrimaryAssetRules(PrimaryAssetId, Rules);
}
#endif