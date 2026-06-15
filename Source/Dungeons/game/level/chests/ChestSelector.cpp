#include "Dungeons.h"
#include "ChestSelector.h"
#include "util/SharedRandom.h"
#include "util/FloatWeighedRandom.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "util/AssetUtil.h"
#include <utility>
#include "game/component/MissionSelectorComponent.h"

namespace game { namespace tile { namespace chest {

	static const EChestType default_chest_type = EChestType::Wooden;
	
	static const ChestTypeMapping wooden_chest(EChestType::Wooden, FString("/Game/Decor/Prefabs/RewardChest/BP_WoodenChest"));
	static const ChestTypeMapping fancy_chest(EChestType::Fancy, FString("/Game/Decor/Prefabs/RewardChest/BP_FancyChest"));
	static const ChestTypeMapping deluxe_chest(EChestType::Deluxe, FString("/Game/Decor/Prefabs/RewardChest/BP_DeluxeChest"));

	static const TArray<ChestTypeMapping> chest_type_mappings = {wooden_chest, fancy_chest, deluxe_chest};

	ChestTypeProbability::ChestTypeProbability(const EChestType chestType, const float weight)
		: mChestType(chestType)
	    , mWeight(weight) {
	}

	ChestTypeProbabilityConfig::ChestTypeProbabilityConfig(std::vector<ChestTypeProbability> probabilities) 
		: mProbabilities(std::move(probabilities)) {
	}

	void ChestTypeProbabilityConfig::addProbability(const ChestTypeProbability& probability) {		
		mProbabilities.emplace_back(probability);
	}

	ChestTypeMapping::ChestTypeMapping(const EChestType chestType, FString blueprintPath)
		: mChestType(chestType)
		, mBlueprintPath(std::move(blueprintPath)) {
	}

	EChestType selectRandomizedTypeUsing(const ChestTypeProbabilityConfig& config) {
		const auto randomChestChanceIndex = FloatWeighedRandom::getRandomItemIndex(&Util::sharedRandom(), config.mProbabilities);

		if (randomChestChanceIndex >= 0) {
			return config.mProbabilities[randomChestChanceIndex].mChestType;	
		}

		return default_chest_type;
	}

	ChestTypeMapping findMappingFor(const EChestType& selectedType) {
		const auto predicate = [=](const ChestTypeMapping chestTypeMapping) { return chestTypeMapping.mChestType == selectedType; };
		const auto selectedMapping = chest_type_mappings.FindByPredicate(predicate);

		if (selectedMapping) {
			return *selectedMapping;
		}

		return wooden_chest;
	}

	ChestTypeMapping selectRandomizedChestFor(const ChestTypeProbabilityConfig& config) {
		const auto selectedType = selectRandomizedTypeUsing(config);		
		return findMappingFor(selectedType);
	}

	void PreloadChestTypes()
	{		
		TArray<FSoftObjectPath> Names = TArray<FSoftObjectPath>();
		
		for (auto& Mapping : chest_type_mappings)
		{
			FString PathName = Mapping.mBlueprintPath;
			
			
			int index = INDEX_NONE;
			if (PathName.FindLastChar('/', index))
			{
				FString BaseAssetName = PathName.RightChop(index + 1);
				FString BaseAssetName_Reward = BaseAssetName + TEXT("_Reward");
				FString BaseAssetName_Hidden = BaseAssetName + TEXT("_Hidden");
				
				PathName = PathName.Left(index + 1);
				
				Names.Add(PathName + BaseAssetName + TEXT(".") + BaseAssetName);
				Names.Add(PathName + BaseAssetName + TEXT(".") + BaseAssetName + TEXT("_C"));

				Names.Add(PathName + BaseAssetName_Reward + TEXT(".") + BaseAssetName_Reward);
				Names.Add(PathName + BaseAssetName_Reward + TEXT(".") + BaseAssetName_Reward + TEXT("_C"));

				Names.Add(PathName + BaseAssetName_Hidden + TEXT(".") + BaseAssetName_Hidden);
				Names.Add(PathName + BaseAssetName_Hidden + TEXT(".") + BaseAssetName_Hidden + TEXT("_C"));

			}
			
		}
		
		UAssetManager::GetStreamableManager().RequestAsyncLoad(Names, [Names]() {
			for (const auto& name : Names) {
				if (auto object = name.ResolveObject()) {
					object->AddToRoot();
				}
			}
		});
	}
	


}}}
