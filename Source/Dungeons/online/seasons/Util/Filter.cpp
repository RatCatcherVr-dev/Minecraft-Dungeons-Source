#include "Filter.h"

#include <string>

#include "DungeonsGameInstance.h"
#include "util/StringUtil.h"
#include "util/EnumUtil.h"
#include "game/item/instance/HealthPotionInstance.h"
#include "GameplayEffectTypes.h"
#include "GameplayEffectExtension.h"
#include "game/Enchantments/Leeching.h"
#include "game/Enchantments/Radiance.h"
#include "game/item/instance/SoulHealer.h"
#include "game/item/instance/TotemOfRegeneration.h"
#include "modules/LiveOps/ChallengeResponse.h"
#include "online/seasons/ObjectiveTypes/SeasonsObjective.h"
#include "world/entity/MobTags.h"

namespace internal 
{
namespace util
{
bool lowerEquals(const std::string& a, const std::string& b) {
	return std::equal(a.begin(), a.end(),
		b.begin(), b.end(),
		[](char a, char b) {
		return tolower(a) == tolower(b);
	});
}
}
}


namespace online
{
namespace liveops
{
namespace filter
{

struct Result {
	TOptional<FilterPart> part;
	bool match;
};

namespace internal
{

auto considerOr(std::vector<Result>& vals) {
	auto trailing = vals.begin();
	bool orActive = false;
	for (auto it = vals.begin(); it != vals.end(); it++) {
		Result& result = *it;
		if (orActive) {
			orActive = false;
			trailing->match = trailing->match || result.match;
			result.match = trailing->match || result.match;
		}
		if (result.part.IsSet() && result.part->type == minecraft::api::FilterPartType::Operator) {
			if (result.part->value == "OR") {
				orActive = true;
			}
		} else {
			orActive = false;
			trailing = it;
		}
	}
}

std::vector<Result> getFilterResultGenerator(const std::vector<FilterPart>& filter, const std::function<TOptional<Result>(const FilterPart&)>& filterParts) {
	std::vector<Result> results;
	for (const auto& part : filter) {
		if (const auto& result = filterParts(part)) {
			results.push_back(result.GetValue());
		} else {
			results.push_back({{}, false});
		}
	}
	return results;
}

bool RunFilter(const std::vector<FilterPart>& filter, const std::function<TOptional<Result>(const FilterPart&)>& filterParts) {
	auto filterResults = getFilterResultGenerator(filter, filterParts);
	considerOr(filterResults);
	return std::all_of(filterResults.begin(), filterResults.end(), [](Result v) {return v.match;});
}

EGameDifficulty difficultyFromString(std::string difficulty) {
	if (difficulty == "Default") {
		return EGameDifficulty::Difficulty_1;
	}
	if (difficulty == "Adventure") {
		return EGameDifficulty::Difficulty_2;
	}
	if (difficulty == "Apocalypse") {
		return EGameDifficulty::Difficulty_3;
	}
	return EGameDifficulty::Invalid;
}

filter::Result Operator(const online::liveops::FilterPart& value) {
	return { value, true };
}

filter::Result Item(const online::liveops::FilterPart& value, const FName& itemId) {
	return { value, itemId.IsEqual(value.value.c_str()) };
}

filter::Result HasItemTag(const online::liveops::FilterPart& value, const ItemType& itemType) {
	return { value, itemType.hasTag(::internal::GetEnumValueFromString<ItemTag>(*FString("ItemTag"), FString(value.value.c_str())).Get(ItemTag::Unset)) };
}

}

namespace killobjective
{

namespace internal
{

filter::Result Mob(const online::liveops::FilterPart& value, EntityType mobType) {
	return { value, mobType == EntityTypeFromString(value.value)};
}

filter::Result MobTag(const online::liveops::FilterPart& value, EntityType mobType) {
	return { value, hasMobTag(mobType, value.value) };
}

}

bool isMatch(const std::vector<FilterPart>& filter, EntityType mobType) {
	auto filterParts = [&] (const FilterPart& part)->TOptional<Result> {
		switch (part.type) {
			case minecraft::api::FilterPartType::Operator:
				return filter::internal::Operator(part);
			case minecraft::api::FilterPartType::Mob:
				return internal::Mob(part, mobType);
			case minecraft::api::FilterPartType::MobTag:
				return internal::MobTag(part, mobType);
			default:
				break;
		}
		return {};
	};
	return filter::internal::RunFilter(filter, filterParts);
}

}

namespace missioncompleted
{

namespace internal
{

filter::Result Mission(const online::liveops::FilterPart& value, const ELevelNames& name) {
	return { value, name == level::fromString(value.value) };
}

filter::Result MinDifficulty(const online::liveops::FilterPart& value, const EGameDifficulty& difficulty) {
	return { value, difficulty >= filter::internal::difficultyFromString(value.value) };
}

filter::Result MinEndlessStruggle(const online::liveops::FilterPart& value, const FEndlessStruggle& struggle) {
	return { value, struggle >= FEndlessStruggle{ std::stoi(value.value) } };
}

filter::Result TrialType(const online::liveops::FilterPart& value, const DailyTrialFilterData& Data) {
	if (Data.Type.IsSet()) {
		return { value, ::internal::util::lowerEquals(value.value, stringutil::toStdString(Data.Type.GetValue())) };
	}
	return { value, false };
}

}

bool isMatch(const std::vector<online::liveops::FilterPart>& filter, ELevelNames name, EGameDifficulty difficulty, FEndlessStruggle endlessStruggle, const DailyTrialFilterData& data)  {
	auto filterParts = [&] (const online::liveops::FilterPart& part)->TOptional<Result> {
		switch (part.type) {
			case minecraft::api::FilterPartType::Operator:
				return filter::internal::Operator(part);
			case minecraft::api::FilterPartType::Mission:
				return internal::Mission(part, name);
			case minecraft::api::FilterPartType::MinDifficulty:
				return internal::MinDifficulty(part, difficulty);
			case minecraft::api::FilterPartType::MinEndlessStruggle:
				return internal::MinEndlessStruggle(part, endlessStruggle);
			case minecraft::api::FilterPartType::TrialType:
				return internal::TrialType(part, data);
			default:
				break;
		}
		return {};
	};
	return filter::internal::RunFilter(filter, filterParts);
}

}

namespace healingdone
{
namespace internal
{
template <typename T>
bool isEffect(const FOnAttributeChangeData& data) {
	return data.GEModData && data.GEModData->EffectSpec.Def && data.GEModData->EffectSpec.Def->IsA<T>();
}

filter::Result Operator(const online::liveops::FilterPart& value) {
	return { value, true };
}

filter::Result HealFromPotion(const online::liveops::FilterPart& value, const FOnAttributeChangeData& data) {	
	return { value, isEffect<UHealthPotionGameplayEffect>(data) };
}

filter::Result HealFromEnchantment(const online::liveops::FilterPart& value, const FOnAttributeChangeData& data) {
	if (value.value == "Leeching") {
		return { value, isEffect<ULeechingGameplayEffect>(data) };
	}
	else if(value.value == "Radiance") {
		return { value, isEffect<URadianceGameplayEffect>(data) };
	}
	return { value, false };
}

filter::Result HealFromArtifact(const online::liveops::FilterPart& value, const FOnAttributeChangeData& data) {
	if (value.value == "SoulHeal") {
		return { value, isEffect<USoulHealGameplayEffect>(data) };
	}
	else if (value.value == "TotemOfRegeneration") {
		return { value, isEffect<UTotemOfRegenerationGameplayEffect>(data) };
	}
	return { value, false };
}

}

bool isMatch(const std::vector<online::liveops::FilterPart>& filter, const FOnAttributeChangeData& data) {
	auto filterParts = [&](const online::liveops::FilterPart& part)->TOptional<Result> {
		switch (part.type) {
		case minecraft::api::FilterPartType::Operator:
			return internal::Operator(part);
		case minecraft::api::FilterPartType::Artifact:
			return internal::HealFromArtifact(part, data);
		case minecraft::api::FilterPartType::Potion:
			return internal::HealFromPotion(part, data);
		case minecraft::api::FilterPartType::Enchantment:
			return internal::HealFromEnchantment(part, data);
		default:
			break;
		}
		return {};
	};
	return filter::internal::RunFilter(filter, filterParts);
}

}

namespace currencycollected
{
namespace internal
{
filter::Result Operator(const online::liveops::FilterPart& value) {
	return { value, true };
}

filter::Result Currency(const online::liveops::FilterPart& value, UGameInstance* GameInstance, const FName& type, const int32& amount) {
	return { value, type.IsEqual(value.value.c_str()) };
}

filter::Result Mission(const online::liveops::FilterPart& value, UGameInstance* GameInstance, const FName& type, const int32& amount) {
	return { value, static_cast<UDungeonsGameInstance*>(GameInstance)->Configuration.GetLevelName() == level::fromString(value.value) };
}

}

bool isMatch(const std::vector<online::liveops::FilterPart>& filter, UGameInstance* GameInstance, const FName& type, const int32& amount) {
	auto filterParts = [&](const online::liveops::FilterPart& part)->TOptional<Result> {
		switch (part.type) {
		case minecraft::api::FilterPartType::Operator:
			return internal::Operator(part);
		case minecraft::api::FilterPartType::Currency:
			return internal::Currency(part, GameInstance, type, amount);
		case minecraft::api::FilterPartType::Mission:
			return internal::Mission(part, GameInstance, type, amount);
		default:
			break;
		}
		return {};
	};
	return filter::internal::RunFilter(filter, filterParts);
}

}
	
namespace revivefriend
{
bool isMatch(const std::vector<online::liveops::FilterPart>& filter){
	auto filterParts = [&](const online::liveops::FilterPart& part)->TOptional<Result> {
		switch (part.type) {
		case minecraft::api::FilterPartType::Operator:
			return internal::Operator(part);
		default:
			break;
		}
		return {};
	};
	return filter::internal::RunFilter(filter, filterParts);
}
}

namespace itemuse
{

bool isMatch(const std::vector<FilterPart>& filter, const ItemType& itemType) {
	auto filterParts = [&](const online::liveops::FilterPart& part)->TOptional<Result> {
		switch (part.type) {
		case minecraft::api::FilterPartType::Operator:
			return filter::internal::Operator(part);
		case minecraft::api::FilterPartType::Item:
			return filter::internal::Item(part, itemType.getNameId());
		case minecraft::api::FilterPartType::ItemTag:
			return filter::internal::HasItemTag(part, itemType);
		default:
			break;
		}
		return {};
	};
	return filter::internal::RunFilter(filter, filterParts);
}

}
}
}
}
