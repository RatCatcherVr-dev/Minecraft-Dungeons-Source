#include "Dungeons.h"
#include "AffectorTypes.h"
#include "AffectorData.h"
#include "AffectorTypesUtil.h"
#include "game/abilities/effects/AffectorGameplayEffect.h"
#include "game/Enchantments/Enchantment.h"
#include "game/Enchantments/EnchantmentUtil.h"
#include "game/item/ItemTypeDefs.h"
#include "game/mob/MobTypeDefs.h"
#include "game/util/ValueFormat.h"
#include "util/Algo.h"
#include "util/CollectionUtils.h"
#include "util/FloatRange.h"
#include "util/Random.h"
#include "util/RandomUtil.h"
#include "util/StringUtil.h"
#include "world/entity/EntityTypes.h"

namespace affector {

AffectorType::AffectorType(EAffectorId id, FString ruleId, FString basicDescription, EAffectorTarget target, LazyEffectType effectType)
	: Id(id)
	, RuleId(std::move(ruleId))
	, mBasicDescription(std::move(basicDescription))
	, Target(target)
	, EffectType(effectType)
{
	mDescriptionGenerator = [fmt = FTextFormat::FromString(TEXT("{0}: {1}")), text = FText::FromString(mBasicDescription)](const ProviderState& it) {
		return FText::Format(fmt, text, FText::FromString(it.data.AsString()));
	};
}

FText AffectorType::GetDescription(const FAffectorData& data) const {
	return mDescriptionGenerator({ *this, data }); // @todo: Localization
}

FString AffectorType::GetBasicDescription() const {
	return mBasicDescription;
}

EAffectorModus AffectorType::GetModusForData(const FAffectorData& data) const {
	const auto harderIfElseEasier = [](bool harder) { return harder ? EAffectorModus::Harder : EAffectorModus::Easier; };

	if (mHardnessProvider) {
		return harderIfElseEasier(mHardnessProvider({ *this, data }) >= 0);
	}
	// We can't really know right now, so just assume it's "Harder" if we have multiple generators
	return harderIfElseEasier(static_cast<bool>(mHarderDataGenerator));
}

FAffectorInfo AffectorType::GetInfo(const FAffectorData& data) const {
	return FAffectorInfo{ GetDescription(data), GetModusForData(data) };
}

bool AffectorType::HasDataGenerator(EAffectorModus modus) const {
	return DataGeneratorPtr(modus) != nullptr;
}

const DataGenerator& AffectorType::GetDataGenerator(EAffectorModus modus /*= EAffectorModus::Any*/, bool preferEasier /*= true*/) const {
	if (modus == EAffectorModus::Any) {
		if (auto preferred = DataGeneratorPtr(preferEasier ? EAffectorModus::Easier : EAffectorModus::Harder)) {
			return *preferred;
		}
	}
	const auto generator = DataGeneratorPtr(modus);
	check(generator);
	return *generator;
}

bool AffectorType::CanApplyMidGame() const
{
	return mIsApplicationAllowedMidgame;
}

const DataGenerator* AffectorType::DataGeneratorPtr(EAffectorModus modus) const {
	if (mEasierDataGenerator && modus != EAffectorModus::Harder) {
		return &mEasierDataGenerator;
	}
	if (mHarderDataGenerator && modus != EAffectorModus::Easier) {
		return &mHarderDataGenerator;
	}
	return nullptr;
}

//
// MutableAffectorType to be used in affector type declarations
//
class MutableAffectorType : public AffectorType {
public:
	using AffectorType::AffectorType;

	MutableAffectorType& SetHard(Pred<ProviderState> g) { return SetHardness([p = std::move(g)](ProviderState it) { return p(it) ? 1 : -1; }); }
	MutableAffectorType& SetHardness(HardnessProvider provider) { mHardnessProvider = std::move(provider); return *this; }

	MutableAffectorType& SetDescription(DescriptionProvider g) { mDescriptionGenerator = std::move(g); return *this; }
	MutableAffectorType& SetHarderEasierDescriptions(DescriptionProvider harder, DescriptionProvider easier) {
		return SetDescription([h = std::move(harder), e = std::move(easier)](ProviderState state) {
			return state.self.GetModusForData(state.data) == EAffectorModus::Harder ? h(state) : e(state);
		});
	}
	MutableAffectorType& SetEasierGenerator(DataGenerator g) { mEasierDataGenerator = std::move(g); return *this; }
	MutableAffectorType& SetHarderGenerator(DataGenerator g) { mHarderDataGenerator = std::move(g); return *this; }
	MutableAffectorType& DisallowMidGameSpawn() { mIsApplicationAllowedMidgame = false;  return *this; };
};

static TArray<MutableAffectorType> s_affectorTypes = Util::createZeroedTArrayOfSize<MutableAffectorType>(enum_cast(EAffectorId::Last));

MutableAffectorType& create(EAffectorId id, FString ruleId, FString description, const EAffectorTarget target, LazyEffectType et) {
	const auto index = enum_cast(id);
	s_affectorTypes[index] = MutableAffectorType(id, std::move(ruleId), std::move(description), target, et);
	return s_affectorTypes[index];
}

MutableAffectorType& game(EAffectorId id, FString ruleId, FString description) {
	return create(id, std::move(ruleId), std::move(description), Game, {});
}

MutableAffectorType& player(EAffectorId id, FString ruleId, FString description, LazyEffectType et = {}) {
	return create(id, std::move(ruleId), std::move(description), Player, std::move(et));
}

MutableAffectorType& mob(EAffectorId id, FString ruleId, FString description, LazyEffectType et = {}) {
	return create(id, std::move(ruleId), std::move(description), Mob, std::move(et));
}

const AffectorType* findType(const FString& ruleId) {
	const auto* at = s_affectorTypes.FindByPredicate([&ruleId](const AffectorType& at){ return at.RuleId == ruleId; });
	if (at == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("Could not find affector type, '%s'"), *ruleId);
	}
	return at;
}

TArray<const AffectorType*> getTypes() {
	return algo::map_tarray(s_affectorTypes, RETLAMBDA(static_cast<const AffectorType*>(&it)));
}

EAffectorId idFromString(const FString& id) {
	return EnumValueFromString(EAffectorId, id).Get(EAffectorId::Last);
}

DataGenerator RandomInt(int a, int b) {
	return [=](DataGeneratorState state) { return state.Rnd.nextInt(a, b + 1); };
}

DataGenerator Choice(const std::vector<FString>& choices) {
	return [=](DataGeneratorState state) { return *Util::randomChoice(choices, &state.Rnd); };
}

DataGenerator NextFloat(float a, float b) {
	return [=](DataGeneratorState state) { return state.Rnd.nextFloat(a, b); };
}

DataGenerator Round(float rounding, DataGenerator gen) {
	return [g = std::move(gen), rounding](DataGeneratorState state) { return Math::roundTo(g(state).AsFloat(), rounding); };
}

DataGenerator Avoid(FloatRange excludedRange, DataGenerator gen) {
	return [g = std::move(gen), excludedRange](DataGeneratorState state) {
		for (int i = 0; i < 50; ++i) {
			const float value = g(state).AsFloat();
			if (!excludedRange.inRange(value)) { return value; }
		}
		return state.Rnd.nextBoolean() ? excludedRange.min() : excludedRange.max();
	};
}

DataGenerator Avoid(float value, DataGenerator gen, float minDistance = 0.15f) {
	return Avoid(FloatRange(value - minDistance, value + minDistance), std::move(gen));
}

DataGenerator Avoid0(DataGenerator gen) {
	return Avoid(0, std::move(gen));
}

DataGenerator Avoid1(DataGenerator gen) {
	return Avoid(1, std::move(gen));
}

DataGenerator True() {
	return RETLAMBDA("1");
}

namespace fmt {

FText percentChange(float r, bool sign = false) {
	return FText::FromString(sign? valueformat::asRelativeMultiplierPercentageChange(r) : valueformat::asMultiplierPercentageChange(r));
}

FText literal(int r, bool sign = false) {
	return FText::FromString(valueformat::asConstant(sign ? r : (r < 0 ? -r : r)));
}

//
// Description providers
//
DescriptionProvider AsPercentChange(FText text, bool sign = false) {
	return[text = std::move(text), sign](ProviderState it) { return FText::Format(text, percentChange(it.data, sign)); };
}

DescriptionProvider AsInt(FText text, bool sign = false) {
	return[text = std::move(text), sign](ProviderState it) { return FText::Format(text, literal(it.data, sign)); };
}

DescriptionProvider AsDisplayHealth(FText text) {
	return[text = std::move(text)](ProviderState it) { return FText::Format(text, literal(valueformat::healthAsDisplayAmount(it.data))); };
}

DescriptionProvider Literal(FText text) {
	return[text = std::move(text)](ProviderState it) { return FText::Format(text, FText::FromString(it.data)); };
}

}

FText undefined() { return FText::FromString("<undefined>"); }

#define LOCTEXT_NAMESPACE "Affector"

const AffectorType& PlayerDamageMultiplierType = player(EAffectorId::PlayerDamage, "playerDamageMul", "Player damage modified", []{return &effect::damageEffectType();})
	.SetHard(RETLAMBDA1(it.data.AsFloat() < 1.0f)) //D11.PS - added new RETLAMBDA
	.SetHarderEasierDescriptions(
		fmt::AsPercentChange(LOCTEXT("player_damage_decreased", "Player damage is decreased by {0}")),
		fmt::AsPercentChange(LOCTEXT("player_damage_increased", "Player damage is increased by {0}")))
	.SetHarderGenerator(Round(0.2, NextFloat(0.4, 0.8)))
	.SetEasierGenerator(Round(0.2, NextFloat(1.2, 2.0)));

const AffectorType& PlayerHealthMultiplierType = player(EAffectorId::PlayerHealth, "playerHealthMul", "Player health modified", []{return &effect::maxHealthEffectType();})
	.SetHard(RETLAMBDA1(it.data.AsFloat() < 1.0f)) //D11.PS - added new RETLAMBDA
	.SetHarderEasierDescriptions(
		fmt::AsPercentChange(LOCTEXT("player_health_decreased", "Player health is decreased by {0}")),
		fmt::AsPercentChange(LOCTEXT("player_health_increased", "Player health is increased by {0}")))
	.SetHarderGenerator(Round(0.2, NextFloat(0.4, 0.8)))
	.SetEasierGenerator(Round(0.2, NextFloat(1.2, 2.0)));

const AffectorType& PlayerArrowType = player(EAffectorId::PlayerArrowType, "playerArrowType", "The player arrows are special")
	.SetEasierGenerator(Choice({"BurningArrow", "FireworksArrow", "TormentProjectile", "HeavyHarpoon"}))
	.SetDescription([] (ProviderState ps) {
		const auto it = ps.data.AsItemType();
		if (it == game::item::type::BurningArrow.getId()) return LOCTEXT("player_burning_arrows", "The player only has Burning Arrows");
		if (it == game::item::type::FireworksArrow.getId()) return LOCTEXT("player_firework_arrows", "The player only has Firework Arrows");
		if (it == game::item::type::TormentProjectile.getId()) return LOCTEXT("player_torment_arrows", "The player only has Torment Arrows");
		if (it == game::item::type::HeavyHarpoon.getId()) return LOCTEXT("player_heavy_harpoons", "The player only has Heavy Duty Harpoons");
		if (it == game::item::type::VoidArrow.getId()) return LOCTEXT("player_void_arrows", "The player only has Void Arrows");
		if (it == game::item::type::PoisonArrow.getId()) return LOCTEXT("player_poison_arrows", "The player only has poison Arrows");
		return FText::FromString("Player arrow type unrecognized: " + ps.data.AsString());
	});

const AffectorType& PlayerSpeedMultiplierType = player(EAffectorId::PlayerSpeed, "playerSpeedMul", "Player speed modified", []{return &effect::speedEffectType();})
	.SetDescription(fmt::AsPercentChange(LOCTEXT("player_speed_increased", "Player speed is increased by {0}")))
	.SetEasierGenerator(Round(0.1, NextFloat(1.1, 1.7)));

const AffectorType& PlayerArtifactCooldownMultiplierType = player(EAffectorId::PlayerArtifactCooldown, "playerArtifactCooldownMul", "Artifact cooldown modified", [] {return &effect::artifactCooldownType(); })
	.SetHard(RETLAMBDA1(it.data.AsFloat() > 1.0f))
	.SetHarderEasierDescriptions(
		fmt::AsPercentChange(LOCTEXT("artifact_cooldown_increased", "Artifact cooldown is increased by {0}")),
		fmt::AsPercentChange(LOCTEXT("artifact_cooldown_decreased", "Artifact cooldown is decreased by {0}")))
	.DisallowMidGameSpawn()
	.SetHarderGenerator(Round(0.2, NextFloat(1.2, 2)))
	.SetEasierGenerator(Round(0.2, NextFloat(0.2, 0.8)));

const AffectorType& MobDamageMultiplierType = mob(EAffectorId::MobDamage, "mobDamageMul", "Mob damage modified", []{return &effect::damageEffectType();})
	.SetDescription(fmt::AsPercentChange(LOCTEXT("mob_damage_increased", "Mob damage is increased by {0}")))
	.SetHarderGenerator(Round(0.1, NextFloat(1.1, 3)));

const AffectorType& MobHealthMultiplierType = mob(EAffectorId::MobHealth, "mobHealthMul", "Mob health modified", []{return &effect::maxHealthEffectType();})
	.SetDescription(fmt::AsPercentChange(LOCTEXT("mob_health_increased", "Mob health is increased by {0}")))
	.SetHarderGenerator(Round(0.1, NextFloat(1.1, 3)));

const AffectorType& MobSpeedMultiplierType = mob(EAffectorId::MobSpeed, "mobSpeedMul", "Mob speed modified", []{return &effect::speedEffectType();})
	.SetDescription(fmt::AsPercentChange(LOCTEXT("mob_speed_increased", "Mob speed is increased by {0}")))
	.SetHarderGenerator(Round(0.1, NextFloat(1.1, 2)));

const AffectorType& MobInvisibilityType = mob(EAffectorId::MobInvisible, "mobInvisible", "Mob invisibility")
	.SetDescription(fmt::Literal(LOCTEXT("mob_invisible", "Mobs are invisible and cannot be targeted unless they are attacking or casting")))
	.SetHarderGenerator(True())
	.DisallowMidGameSpawn();

const AffectorType& PetCountType = game(EAffectorId::PetCount, "petCount", "Pet count modified")
	.SetDescription(fmt::Literal(LOCTEXT("pet_count", "Pet count is set to {0}")))
	.SetEasierGenerator(RandomInt(2, 4));

const AffectorType& InstantGameOverType = game(EAffectorId::InstantGameOver, "instantGameOver", "Instant game over")
	.SetDescription(fmt::Literal(LOCTEXT("instant_gameover", "Game over when any player is downed")))
	.SetHarderGenerator(True())
	.DisallowMidGameSpawn();

const AffectorType& SoulCountType = game(EAffectorId::SoulCount, "soulCountMul", "Soul count modified")
	.SetDescription(fmt::Literal(LOCTEXT("soul_count", "Players collect {0} times as many souls")))
	.SetEasierGenerator(RandomInt(2, 4));

const AffectorType& StartupLivesType = game(EAffectorId::StartupLives, "startupLives", "Startup lives modified")
	.SetDescription(fmt::Literal(LOCTEXT("startup_lives_count", "Initial life count is set to {0}")))
	.SetHarderGenerator(RandomInt(0, 4))
	.DisallowMidGameSpawn();

const AffectorType& EmeraldHealthAddType = game(EAffectorId::EmeraldHealthAdd, "emeraldHealthAdd", "Emeralds give damage or health")
	.SetHard(RETLAMBDA1(it.data.AsFloat() < 0))	//D11.PS - added new RETLAMBDA
	.SetHarderGenerator(Round(5, RandomInt(-40, -5)))
	.SetEasierGenerator(Round(5, RandomInt(5, 40)))
	.SetHarderEasierDescriptions(
		fmt::AsInt(LOCTEXT("emeralds_damage_player", "Emeralds damage the player by {0} hp")),
		fmt::AsDisplayHealth(LOCTEXT("emeralds_heal_player", "Emeralds heal the player by {0} hp")))
	.DisallowMidGameSpawn();

const AffectorType& NightModeType = game(EAffectorId::NightMode, "nightMode", "Night mode")
	.SetDescription(fmt::Literal(LOCTEXT("night_mode", "Night mode")))
	.SetHarderGenerator(True());

const AffectorType& ChestProbabilityMultiplierType = game(EAffectorId::ChestProbability, "chestProbabilityMul", "Chest probability multiplier")
	.SetDescription(fmt::Literal(LOCTEXT("chest_probability_increased", "Chance to spawn chests is {0} times higher")))
	.SetEasierGenerator(RandomInt(2, 10))
	.DisallowMidGameSpawn();

const AffectorType& PlayerUnderwaterType = player(EAffectorId::PlayerUnderwater, "playerUnderwater", "Player Underwater", [] {return &effect::underwaterEffectType(); })
	.SetDescription(fmt::Literal(LOCTEXT("player_underwater", "Player Underwater")))
	.SetHarderGenerator(True())
	.DisallowMidGameSpawn();

const AffectorType& MobUnderwaterType = mob(EAffectorId::MobUnderwater, "mobUnderwater", "Mob Underwater", [] {return &effect::underwaterEffectType(); })
	.SetDescription(fmt::Literal(LOCTEXT("mob_underwater", "Mob Underwater")))
	.SetHarderGenerator(True())
	.DisallowMidGameSpawn();

const AffectorType& ReplaceMeleeMobsType = game(EAffectorId::ReplaceMeleeMobs, "replaceMeleeMobs", "Melee mobs are replaced")
	.SetHarderGenerator([](DataGeneratorState state) {
		const auto probability = Math::roundTo(state.Rnd.nextFloat(0.1f, 0.5f), 0.05f);
		const auto targetType = "creeper";
		return FAffectorData(FString::SanitizeFloat(probability) + " -> " + targetType);
	})
	.SetDescription([] (ProviderState ps) {
		const auto info = parse::replaceMobTypeInfo(ps.data);
		const auto percent = FText::FromString(info ? valueformat::asPercentage(info->probability) : FString("0"));
		const auto targetType = info ? mob::type::get(info->target).GetName() : undefined();
		return FText::Format(LOCTEXT("replace_melee_mobs", "{0} of melee mobs are replaced with {1}"), percent, targetType);
	});

const AffectorType& ReplaceRangedMobsType = game(EAffectorId::ReplaceRangedMobs, "replaceRangedMobs", "Ranged mobs are replaced")
	.SetHarderGenerator([](DataGeneratorState state) {
		const auto probability = Math::roundTo(state.Rnd.nextFloat(0.1f, 0.5f), 0.05f);
		const auto targetType = "pillager";
		return FAffectorData(FString::SanitizeFloat(probability) + " -> " + targetType);
	})
	.SetDescription([] (ProviderState ps) {
		const auto info = parse::replaceMobTypeInfo(ps.data);
		const auto percent = FText::FromString(info ? valueformat::asPercentage(info->probability) : FString("0"));
		const auto targetType = info ? mob::type::get(info->target).GetName() : undefined();
		return FText::Format(LOCTEXT("replace_ranged_mobs", "{0} of ranged mobs are replaced with {1}"), percent, targetType);
	});

const AffectorType& PlayerEnchantmentType = player(EAffectorId::PlayerEnchantment, "playerEnchantment", "Player enchantment")
	.SetEasierGenerator([](DataGeneratorState state) {
		const auto& allEnchantments = game::enchantment::type::getAvailableEnchantments();
		const auto& enchantmentCandidates = algo::copy_if(allEnchantments, RETLAMBDA(!it.isWorkInProgress() && !it.isDisabledForPlayers()));
		const auto index = state.Rnd.nextInt(enchantmentCandidates.Num());
		return FAffectorData{ GetEnumValueToStringStripped(enchantmentCandidates[index].getEnchantmentTypeID()) };
	})
	.SetDescription([](ProviderState ps) {
		const auto typeId = GetEnumValueFromStringT<EEnchantmentTypeID>(ps.data.AsString());
		const auto name = typeId ? UEnchantment::GetNameForEnchantmentType(typeId.GetValue()) : undefined();
		return FText::Format(LOCTEXT("player_enchantment", "Players have the {0} enchantment equipped"), name);
	});

const AffectorType& MobEnchantmentType = mob(EAffectorId::MobEnchantment, "mobEnchantment", "Mob enchantment")
	.SetHarderGenerator([](DataGeneratorState state) {
		const auto& allEnchantments = game::enchantment::type::getAvailableEnchantments();
		const auto& enchantmentCandidates = algo::copy_if(allEnchantments, RETLAMBDA(!it.isWorkInProgress() && it.canBeUsedByMobs()));
		const auto index = state.Rnd.nextInt(enchantmentCandidates.Num());
		const auto probability = Math::roundTo(state.Rnd.nextFloat(0.1f, 0.5f), 0.05f);
		return FAffectorData{ FString::SanitizeFloat(probability) + " -> " + GetEnumValueToStringStripped(enchantmentCandidates[index].getEnchantmentTypeID()) };
	})
	.SetDescription([](ProviderState ps) {
		const auto info = parse::mobEnchantmentInfo(ps.data);
		const auto percent = FText::FromString(info ? valueformat::asPercentage(info->probability) : FString("0"));
		const auto enchantmentName = info ? UEnchantment::GetNameForEnchantmentType(info->target) : undefined();
		const auto enchantmentCategory = info ? UEnchantment::GetEnchantmentTypeCategory(info->target) : EEnchantmentCategory::Unset;
		switch (enchantmentCategory) {
			case EEnchantmentCategory::Melee:  return FText::Format(LOCTEXT("mob_enchantment_melee", "{0} of melee mobs have the {1} enchantment"), percent, enchantmentName);
			case EEnchantmentCategory::Ranged: return FText::Format(LOCTEXT("mob_enchantment_ranged", "{0} of ranged mobs have the {1} enchantment"), percent, enchantmentName);
			default: return FText::Format(LOCTEXT("mob_enchantment", "{0} of mobs have the {1} enchantment"), percent, enchantmentName);
		};
		
	});

#undef LOCTEXT_NAMESPACE

}

affector::RuleData FReplicatableAffectorsRules::ToRuleData() const
{
	affector::RuleData affectors;
	for (const auto& modifier : modifiers) {
		affectors.Set(modifier.AffectorId, modifier.AffectorData);
	}
	return affectors;
}

int UAffectorFunctionLibrary::GetNumOfMidGameAffectors(TArray<FReplicatableModifier> replicatableModifiers)
{
	return algo::copy_if(replicatableModifiers,
		RETLAMBDA(it.AffectorData.GetApplicationType() == EAffectorApplicationType::MidGame)).Num();
}
