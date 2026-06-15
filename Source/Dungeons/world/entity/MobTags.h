#pragma once
#include "CommonTypes.h"
#include "EntityTypes.h"
#include "lovika/Tags.h"
#include "game/GameSettings.h"

class MobTags {
public:
	static const size_t
		HashTag_Friendly,
		HashTag_Animal,
		HashTag_Monster,
		HashTag_Melee,
		HashTag_Ranged,
		HashTag_Disable,
		HashTag_Aoe,
		HashTag_Fast,
		HashTag_Baby,
		HashTag_Weak,
		HashTag_Strong,
		HashTag_Miniboss,
		HashTag_Centerpiece,
		HashTag_Caster,
		HashTag_Summoner,
		HashTag_EventMob,
		HashTag_Passive,
		HashTag_DLC,
		HashTag_Unbuffable,
		HashTag_Undead,
		HashTag_Immobile,
		HashTag_Unlovable,
		HashTag_Floating,
		HashTag_LowPriority,
		HashTag_Special,
		HashTag_Pet,
		HashTag_Unenchantable,
		HashTag_MountainAnimal,
		HashTag_OceanAnimal,
		HashTag_Illager,
		HashTag_Ancient,
		HashTag_Underwater,
		HashTag_WinterAnimal,
		HashTag_JungleAnimal,
		HashTag_BluffAnimal,
		HashTag_Cosmetic,
		HashTag_NoRes,
		HashTag_Unbubbled,
		HashTag_Ender,
		HasTag_NoEnchantedName,
		HashTag_RaidCaptain,
		HashTag_NoHyperMission,
		HashTag_Unchainable;

	const Tags& tags(EntityType) const;
	const Tags& tags(const std::string&) const;

	float difficultyThreshold(EntityType, EGameDifficulty) const;
	float difficultyThreshold(const std::string&, EGameDifficulty) const;

	static const MobTags& singleton();
private:
	static const std::string
		Friendly,
		Animal,
		Monster,
		Melee,
		Ranged,
		Disable,
		Aoe,
		Fast,
		Baby,
		Weak,
		Strong,
		Miniboss,
		Centerpiece,
		Caster,
		Summoner,
		EventMob,
		Passive,
		Immobile,
		Unlovable,
		DLC,
		Unbuffable,
		Undead,
		Floating,
		LowPriority,
		Special,
		Pet,
		Unenchantable,
		Illager,
		NoRes,
		MountainAnimal,
		OceanAnimal,
		Ancient,
		Underwater,
		WinterAnimal,
		JungleAnimal,
		BluffAnimal,
		Cosmetic,
		Ender,
		NoEnchantedName,
		Unbubbled,
		RaidCaptain,
		NoHyperMission,
		Unchainable
		;

	MobTags();
	MobTags(MobTags const&) = delete;

	void operator=(MobTags const&) = delete;

	struct Item {
		Tags tags;
		std::array<float, enum_cast(EGameDifficulty::ENUM_COUNT)> difficultyThresholds;
	};

	MobTags& tag(EntityType, std::initializer_list<float> difficultyThresholds, std::initializer_list<std::string> = {});

	Item& getOrCreate(EntityType);
	void _onCreated(EntityType, Tags&);
	const Item& item(EntityType) const;

	static const Item Empty;
	std::unordered_map<EntityType, Unique<Item>, EnumClassHash> mTags;
};

bool hasMobTag(EntityType, const std::string& tag);
bool hasMobTag(EntityType, size_t tag_hash);
bool hasMobTag(const AActor*, size_t tag_hash);
bool hasMobTag_alreadyLowerCase(EntityType, const std::string& lowerCaseTag);
bool hasMobTag_alreadyLowerCase(const AActor*, const std::string& lowerCaseTag);
