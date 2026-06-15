#pragma once

#include "AffectorData.h"
#include "game/util/ReplicatableMap.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AffectorTypes.generated.h"

class AActor;

UENUM(BlueprintType)
enum class EAffectorModus : uint8 {
	Easier,
	Harder,
	Any,
};
ENUM_NAME(EAffectorModus)

USTRUCT(BlueprintType)
struct DUNGEONS_API FAffectorInfo {
	GENERATED_BODY()

		UPROPERTY(BlueprintReadOnly)
		FText DisplayText;

	UPROPERTY(BlueprintReadOnly)
		EAffectorModus Modus;
};

UENUM(BlueprintType)
enum class EAffectorId : uint8 {
	EmeraldHealthAdd,
	PlayerArrowType,
	PlayerDamage,
	PlayerHealth,
	PlayerSpeed,
	PlayerArtifactCooldown,
	MobDamage,
	MobHealth,
	MobSpeed,
	MobInvisible,
	NightMode,
	PetCount,
	InstantGameOver,
	SoulCount,
	StartupLives,
	ReplaceMeleeMobs,
	ReplaceRangedMobs,
	PlayerEnchantment,
	MobEnchantment,
	ChestProbability,
	PlayerUnderwater,
	MobUnderwater,
	Last,
};
ENUM_NAME(EAffectorId)

USTRUCT(BlueprintType)
struct DUNGEONS_API FReplicatableModifier {
	GENERATED_BODY()

		FReplicatableModifier() = default;
	FReplicatableModifier(EAffectorId affectorId, FAffectorData affectorData)
		: AffectorId(affectorId)
		, AffectorData(std::move(affectorData)) {
	}

	UPROPERTY()
		EAffectorId AffectorId;

	UPROPERTY()
		FAffectorData AffectorData;
};

USTRUCT(BlueprintType)
struct DUNGEONS_API FReplicatableAffectorsRules {
	GENERATED_BODY()

		FReplicatableAffectorsRules() = default;

	UPROPERTY()
		TArray<FReplicatableModifier> modifiers;

	affector::RuleData ToRuleData() const;
};

struct FAffectorData;

namespace affector { namespace effect { struct EffectType; } }

namespace affector {

	enum EAffectorTarget {
		Game,
		Player,
		Mob
	};

	struct DataGeneratorState {
		class Random& Rnd;
	};
	struct ProviderState {
		const class AffectorType& self;
		const FAffectorData& data;
	};

	using LazyEffectType = std::function<const effect::EffectType*()>;
	using DataGenerator = std::function<FAffectorData(DataGeneratorState)>;
	using HardnessProvider = std::function<float(ProviderState)>;
	using DescriptionProvider = std::function<FText(ProviderState)>;

	class AffectorType {
	public:
		AffectorType(EAffectorId id, FString ruleId, FString description, EAffectorTarget, LazyEffectType);

		AffectorType(const AffectorType&) = delete;
		AffectorType(AffectorType&&) = default;

		const AffectorType& operator=(const AffectorType&) = delete;
		AffectorType& operator=(AffectorType&&) = default;

		FText GetDescription(const FAffectorData&) const;
		FString GetBasicDescription() const;

		EAffectorModus GetModusForData(const FAffectorData&) const;
		FAffectorInfo GetInfo(const FAffectorData& data) const;
		bool HasDataGenerator(EAffectorModus) const;
		const DataGenerator& GetDataGenerator(EAffectorModus = EAffectorModus::Any, bool preferEasier = true) const;
		bool CanApplyMidGame() const;

		EAffectorId Id;
		FString RuleId;
		EAffectorTarget Target;
		LazyEffectType EffectType;
	protected:
		FString mBasicDescription;
		DescriptionProvider mDescriptionGenerator;
		HardnessProvider mHardnessProvider;

		const DataGenerator* DataGeneratorPtr(EAffectorModus) const;
		DataGenerator mEasierDataGenerator;
		DataGenerator mHarderDataGenerator;
		bool mIsApplicationAllowedMidgame = true;
	};

	extern const AffectorType& PlayerArrowType;
	extern const AffectorType& PlayerDamageMultiplierType;
	extern const AffectorType& PlayerHealthMultiplierType;
	extern const AffectorType& PlayerSpeedMultiplierType;
	extern const AffectorType& PlayerArtifactCooldownMultiplierType;

	extern const AffectorType& MobDamageMultiplierType;
	extern const AffectorType& MobHealthMultiplierType;
	extern const AffectorType& MobSpeedMultiplierType;
	extern const AffectorType& MobInvisibilityType;

	extern const AffectorType& PetCountType;
	extern const AffectorType& InstantGameOverType;
	extern const AffectorType& SoulCountType;
	extern const AffectorType& StartupLivesType;
	extern const AffectorType& EmeraldHealthAddType;
	extern const AffectorType& NightModeType;
	extern const AffectorType& ChestProbabilityMultiplierType;
	extern const AffectorType& PlayerUnderwaterType;
	extern const AffectorType& MobUnderwaterType;

	extern const AffectorType& ReplaceMeleeMobsType;
	extern const AffectorType& ReplaceRangedMobsType;

	extern const AffectorType& PlayerEnchantmentType;
	extern const AffectorType& MobEnchantmentType;

	const AffectorType* findType(const FString& ruleId);
	TArray<const AffectorType*> getTypes();

	EAffectorId idFromString(const FString& id);

}

UCLASS()
class DUNGEONS_API UAffectorFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Items|ItemTypeID")
	static int GetNumOfMidGameAffectors(TArray<FReplicatableModifier> replicatableModifiers);
};
