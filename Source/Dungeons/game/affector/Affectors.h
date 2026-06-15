#pragma once

#include "AffectorData.h"
#include "AffectorTypes.h"
#include "AffectorEnchantmentUtil.h"
#include "game/item/ItemType.h"
#include "game/mobspawn/MobSpawnTypes.h"
#include "../actor/character/BaseCharacter.h"

enum class EGameDifficulty : uint8;

namespace game { class Game; }


namespace affector {

struct AffectorInstance {
	AffectorInstance(const AffectorType&, const FAffectorData&);
	EAffectorModus GetModus() const;

	const AffectorType& Type;
	FAffectorData Data;
};


class Affectors {
public:
	Affectors();

	void Set(EAffectorId, const FString& affectorData);
	void Set(const RuleData&);

	void Clear();

	int GetStartupLives(EGameDifficulty) const;
	int GetPetSpawnCount() const;
	bool IsInstantGameOver() const;
	int GetSoulCountMultiplier() const;
	float GetChestProbabilityMultiplier() const;
	bool IsMobInvisibility() const;

	const game::mobspawn::EntityTypeMapper& GetEntityTypeMapper() const;

	TOptional<TArray<FEnchantmentData>> GetPlayerEnchantments() const;
	TOptional<TArray<FEnchantmentData>> GetMobEnchantments(AMobCharacter&) const;

	const TArray<AffectorInstance>& GetActive() const;
	TArray<AffectorInstance> GetActiveWithEffect(EAffectorTarget) const;
	TArray<FAffectorInfo> GetActiveAffectorsInfo() const;
	TOptional<FItemId> GetPlayerArrowType() const;
	TOptional<float> GetEmeraldHealthAdd() const;
	bool IsNightMode() const;
	int MidGameAffectorsNum() const;

	const AffectorInstance* Find(const AffectorType&) const;
private:
	void Register(const RuleData&, const AffectorType&);
	AffectorInstance* FindMutable(const AffectorType&) const;
	TOptional<game::mobspawn::EntityTypeMapper> CreateEntityTypeMapper() const;

	template<class T>
	using IntRet1 = std::function<int(T)>;

	IntRet1<EGameDifficulty> _GetStartupLives;
	TArray<AffectorInstance> Active;

	mutable TOptional<game::mobspawn::EntityTypeMapper> TypeMapper;
	EnchantmentCategoryCollector mEnchantmentCategoryCollector;
};

const Affectors& get(game::Game*);
const Affectors& get(UWorld*);
Affectors& getDebug(UWorld*);

void applyAffectorsForCharacter(ABaseCharacter&);
void applyRandomAffector(TArray<ABaseCharacter*>& characters);
}