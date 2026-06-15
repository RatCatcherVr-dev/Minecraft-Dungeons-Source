#include "Dungeons.h"
#include "Affectors.h"
#include "AffectorTypesUtil.h"
#include "game/Game.h"
#include "game/GameBP.h"
#include "game/abilities/effects/AffectorGameplayEffect.h"
#include "game/component/MeleeAttackComponent.h"
#include "game/component/RangedAttackComponent.h"
#include "game/difficulty/Difficulty.h"
#include "game/Enchantments/EnchantmentType.h"
#include "game/Enchantments/mobs/MobEnchantmentUtil.h"
#include "game/mobspawn/EntityTypeMappers.h"
#include "game/util/ActorQuery.h"
#include "util/Algo.h"
#include "util/RandomUtil.h"
#include "world/entity/MobTags.h"
#include "game/Enchantments/Invisible.h"

namespace affector {

AffectorInstance::AffectorInstance(const AffectorType& type, const FAffectorData& data) :
	Type(type), Data(data) {
}

EAffectorModus AffectorInstance::GetModus() const {
	return Type.GetModusForData(Data);
}

//
// Affectors
//
Affectors::Affectors() {
	_GetStartupLives = [](auto difficulty) { return difficultyquery::getLivesForDifficulty(difficulty); };

	RuleData def;
	def.Set(PetCountType.Id, FAffectorData(1).SetDefault());
	def.Set(SoulCountType.Id, FAffectorData(1).SetDefault());
	def.Set(InstantGameOverType.Id, FAffectorData(false).SetDefault());
	def.Set(NightModeType.Id, FAffectorData(false).SetDefault());
	def.Set(ChestProbabilityMultiplierType.Id, FAffectorData(1).SetDefault());
	def.Set(MobInvisibilityType.Id, FAffectorData(false).SetDefault());
	Set(def);
}

void Affectors::Set(EAffectorId affectorId, const FString& affectorData) {
	Set(RuleData{ affectorId, affectorData });
}

void Affectors::Set(const RuleData& ruleData) {
	if (ruleData.Has(StartupLivesType.Id)) {
		_GetStartupLives = [lives = ruleData.Get(StartupLivesType.Id).AsInt()](auto difficulty) { return lives; };
		Register(ruleData, StartupLivesType);
	}

	Register(ruleData, EmeraldHealthAddType);
	Register(ruleData, PetCountType);
	Register(ruleData, InstantGameOverType);
	Register(ruleData, PlayerArrowType);
	Register(ruleData, SoulCountType);
	Register(ruleData, NightModeType);
	Register(ruleData, ChestProbabilityMultiplierType);

	Register(ruleData, PlayerDamageMultiplierType);
	Register(ruleData, MobDamageMultiplierType);

	Register(ruleData, PlayerHealthMultiplierType);
	Register(ruleData, MobHealthMultiplierType);

	Register(ruleData, PlayerSpeedMultiplierType);
	Register(ruleData, MobSpeedMultiplierType);

	Register(ruleData, ReplaceMeleeMobsType);
	Register(ruleData, ReplaceRangedMobsType);

	Register(ruleData, PlayerEnchantmentType);
	Register(ruleData, MobEnchantmentType);

	Register(ruleData, PlayerArtifactCooldownMultiplierType);
	Register(ruleData, MobInvisibilityType);

	Register(ruleData, PlayerUnderwaterType);
	Register(ruleData, MobUnderwaterType);
}

void Affectors::Clear() {
	Active.Empty();
	TypeMapper.Reset();
}

TArray<AffectorInstance> Affectors::GetActiveWithEffect(EAffectorTarget target) const {
	return Active.FilterByPredicate([target](const AffectorInstance& ai) { return ai.Type.EffectType && target == ai.Type.Target; });
}

const TArray<AffectorInstance>& Affectors::GetActive() const {
	return Active;
}

int Affectors::GetStartupLives(EGameDifficulty difficulty) const {
	if (IsInstantGameOver()) {
		return 0;
	}
	return _GetStartupLives(difficulty);
}

int Affectors::GetPetSpawnCount() const {
	return Find(PetCountType)->Data.AsInt();
}

bool Affectors::IsInstantGameOver() const {
	return Find(InstantGameOverType)->Data.AsBoolean();
}

int Affectors::GetSoulCountMultiplier() const {
	return Find(SoulCountType)->Data.AsInt();
}

float Affectors::GetChestProbabilityMultiplier() const {
	return Find(ChestProbabilityMultiplierType)->Data.AsFloat();
}

bool Affectors::IsMobInvisibility() const {
	return Find(MobInvisibilityType)->Data.AsBoolean();
}

const game::mobspawn::EntityTypeMapper& Affectors::GetEntityTypeMapper() const {
	if (!TypeMapper.IsSet()) {
		TypeMapper = CreateEntityTypeMapper();
	}
	return TypeMapper.GetValue();
}

TOptional<game::mobspawn::EntityTypeMapper> Affectors::CreateEntityTypeMapper() const {
	const auto createMapper = [](const AffectorInstance* affector, size_t mobTag) -> TOptional<game::mobspawn::EntityTypeMapper> {
		if (affector) {
			if (auto replaceMobTypeInfo = parse::replaceMobTypeInfo(affector->Data)) {
				return game::mobspawn::FromPredicateAndMapper(
					[probability = replaceMobTypeInfo->probability, mobTag](EntityType type) { return hasMobTag(type, mobTag) && Util::sharedRandom().testProbability(probability); },
					game::mobspawn::Only(replaceMobTypeInfo->target));
			}
		}
		return {};
	};

	std::vector<game::mobspawn::EntityTypeMapper> mappers;

	if (auto meleeMapper = createMapper(Find(ReplaceMeleeMobsType), MobTags::HashTag_Melee)) {
		mappers.push_back(meleeMapper.GetValue());
	}
	if (auto rangedMapper = createMapper(Find(ReplaceRangedMobsType), MobTags::HashTag_Ranged)) {
		mappers.push_back(rangedMapper.GetValue());
	}
	if (!mappers.empty()) {
		return game::mobspawn::CascadedBreakWhenTypeChanged(mappers);
	}
	return { [](const EntityType entityType) { return entityType; } };
}

TOptional<TArray<FEnchantmentData>> Affectors::GetPlayerEnchantments() const {
	if (const auto* ai = Find(PlayerEnchantmentType)) {
		if (const auto enchantmentType = GetEnumValueFromStringT<EEnchantmentTypeID>(ai->Data.AsString())) {
			return TArray<FEnchantmentData> { {enchantmentType.GetValue(), 3} };
		}
	}
	return {};
}

TOptional<TArray<FEnchantmentData>> Affectors::GetMobEnchantments(AMobCharacter& mob) const {
	if (const auto* ai = Find(MobEnchantmentType)) {
		if (const auto enchantmentInfo = parse::mobEnchantmentInfo(ai->Data)) {

			if (Util::sharedRandom().testProbability(enchantmentInfo->probability)) {
				const auto enchantmentType = enchantmentInfo->target;
				if (mobench::isEnchantmentApplicableFor(enchantmentType, mEnchantmentCategoryCollector.getCategories(mob), game::FDifficulty::HIGHEST)) {
					return TArray<FEnchantmentData> { {enchantmentInfo->target, 3} };
				}
			}
		}
	}
	return {};
}

bool Affectors::IsNightMode() const {
	return Find(NightModeType)->Data.AsBoolean();
}

int Affectors::MidGameAffectorsNum() const
{
	return algo::count_if(Active, RETLAMBDA(it.Data.IsMidGame()));
}

TOptional<FItemId> Affectors::GetPlayerArrowType() const {
	if (const auto* ai = Find(PlayerArrowType)) {
		return ai->Data;
	}
	return {};
}

TOptional<float> Affectors::GetEmeraldHealthAdd() const {
	if (const auto* ai = Find(EmeraldHealthAddType)) {
		return ai->Data.AsFloat();
	}
	return {};
}

const AffectorInstance* Affectors::Find(const AffectorType& type) const {
	return Active.FindByPredicate([&type](const AffectorInstance& ai) { return type.Id == ai.Type.Id; });
}

TArray<FAffectorInfo> Affectors::GetActiveAffectorsInfo() const {
	return algo::copy_if_map_tarray(Active,
		RETLAMBDA(!it.Data.IsDefault()),
		RETLAMBDA(it.Type.GetInfo(it.Data))
	);
}

AffectorInstance* Affectors::FindMutable(const AffectorType& type) const {
	return const_cast<AffectorInstance*>(Find(type));
}

void Affectors::Register(const RuleData& ruleData, const AffectorType& type) {
	const auto& id = type.Id;
	if (!ruleData.Has(id)) {
		return;
	}
	const auto& data = ruleData.Get(id);
	if (auto* ai = FindMutable(type)) {
		ai->Data = data;
	}
	else {
		Active.Add(AffectorInstance(type, data));
	}

	if (id == EAffectorId::ReplaceMeleeMobs || id == EAffectorId::ReplaceRangedMobs) {
		TypeMapper.Reset();
	}
}

const Affectors& get(game::Game* game) {
	if (game == nullptr) {
		static affector::Affectors affectors;
		return affectors;
	}
	return game->affectors();
}

const Affectors& get(UWorld* world) {
	return get(actorquery::getGame(world));
}

Affectors& getDebug(UWorld* world) {
	return const_cast<Affectors&>(get(world));
}

bool isEnchantableByAffectors(AMobCharacter* mob) {
	return !hasMobTag(mob->EntityType, MobTags::HashTag_Unenchantable) &&
		!hasMobTag(mob->EntityType, MobTags::HashTag_Miniboss);
}

void applyAffectorEnchantments(AGameBP* gameBp, ABaseCharacter& character) {
	if (!gameBp || !gameBp->GetGame()) {
		return;
	}
	if (!character.GetEnchantmentComponent()) {
		return;
	}

	const auto maybeEnchantmentsForCharacter = [&]() -> TOptional<TArray<FEnchantmentData>> {
		if (character.IsA<APlayerCharacter>()) {
			return gameBp->GetGame()->affectors().GetPlayerEnchantments();
		}
		if (auto* mob = Cast<AMobCharacter>(&character)) {
			if (isEnchantableByAffectors(mob)) {
				return gameBp->GetGame()->affectors().GetMobEnchantments(*mob);
			}
		}
		return {};
	}();

	if (maybeEnchantmentsForCharacter) {
		character.GetEnchantmentComponent()->AddEnchantments(maybeEnchantmentsForCharacter.GetValue(), createEnchantmentSpawnConfig(gameBp).SetOverriding());
	}

	if (gameBp->GetGame()->affectors().IsMobInvisibility() && invisible::isAllowedMob(&character)) {
		character.GetEnchantmentComponent()->AddEnchantments({ {EEnchantmentTypeID::Invisible, 1} }, createEnchantmentSpawnConfig(gameBp).SetOverriding().SetTreatAsInherited());
	}
}

void applyAffectorsForCharacter(ABaseCharacter& character) {
	effect::applyEffects(character.GetAbilitySystemComponent());
	applyAffectorEnchantments(UActorQuery::GetGameBP(character.GetWorld()), character);
}

void applyRandomAffector(TArray<ABaseCharacter*>& characters) {

	if (characters.Num() <= 0) {
		return;
	}

	auto& WorldAffectors = getDebug(characters[0]->GetWorld());
	const TArray<const AffectorType*>& availableAffectors = getTypes().FilterByPredicate([=](const AffectorType* affector)
	{
		return affector->CanApplyMidGame() && affector->HasDataGenerator(EAffectorModus::Harder) && (!WorldAffectors.Find(*affector) || WorldAffectors.Find(*affector)->Data.IsDefault());
	});

	if (availableAffectors.Num() > 0)
	{
		const AffectorType* randomAffector = availableAffectors[FMath::RandRange(0, availableAffectors.Num() - 1)];

		Random& rnd = Util::sharedRandom();
		auto data = randomAffector->GetDataGenerator(EAffectorModus::Harder)({ rnd });
		RuleData def;
		def.Set(randomAffector->Id, data.SetApplicationType(EAffectorApplicationType::MidGame));
		WorldAffectors.Set(def);

		if (randomAffector->Target != EAffectorTarget::Game) {
			for (auto character : characters) {
				applyAffectorsForCharacter(*character);
			}
		}

		actorquery::getFirstActor<AGameBP>(characters[0]->GetWorld())->OnAffectorsChanged();
	}
}

}