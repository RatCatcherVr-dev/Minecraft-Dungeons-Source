#pragma once

#include "Engine/DataTable.h"
#include "Curves/CurveFloat.h"
#include "EndlessStruggle.generated.h"

const size_t NumberOfEndlessStruggleLevels = 25;

USTRUCT(BlueprintType)
struct DUNGEONS_API FEndlessStruggle {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Value = 0;

	int compare(const FEndlessStruggle& rhs) const;
	bool operator< (const FEndlessStruggle& rhs) const { return compare(rhs) < 0; }
	bool operator<= (const FEndlessStruggle& rhs) const { return compare(rhs) <= 0; }
	bool operator> (const FEndlessStruggle& rhs) const { return compare(rhs) > 0; }
	bool operator>= (const FEndlessStruggle& rhs) const { return compare(rhs) >= 0; }
	bool operator== (const FEndlessStruggle& rhs) const { return compare(rhs) == 0; }
	FEndlessStruggle operator+(const int addition) const { return { Value + addition }; }
	FEndlessStruggle operator-(const int subtraction) const { return { Value - subtraction }; }

	static const FEndlessStruggle HIGHEST;
	static const FEndlessStruggle ZERO;
	static const int UnlockedLevelsAboveCompleted;
};

USTRUCT(BlueprintType)
struct DUNGEONS_API FEndlessStruggleRow : public FTableRowBase {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MinValue = 1.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MaxValue = 10.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int IntroducedAt = 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UCurveFloat* Curve;
};

USTRUCT(BlueprintType)
struct DUNGEONS_API FEndlessStruggleConfiguration {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float XPMultiplier = 1.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float LootPowerMultiplier = 1.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float LootQualityBoost = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float ShopPriceMultiplier = 1.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MobCountMultiplier = 1.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MobMaxHealthMultiplier = 1.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MobPerformHealingMultiplier = 1.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MobMaxEnduranceMultiplier = 1.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MobRecoverEnduranceMultiplier = 1.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MobDamageMultiplier = 1.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MobStunDurationMultiplier = 1.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MobSpeedMultiplier = 1.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MobResurrectionChance = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MobPushbackMultiplier = 1.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MobDeflectChance = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float EnchantedMobChanceMultiplier = 1.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float EnchantedMobPowerMultiplier = 1.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float PlayerReceiveHealingMultiplier = 1.0f;
};

UCLASS()
class UEndlessStruggleLibrary : public UObject {
	GENERATED_BODY()

public:
	UEndlessStruggleLibrary();

	FEndlessStruggleRow* GetRow(FName name);
	FEndlessStruggleConfiguration GetConfiguration(int value = 0);
	
private:
	UDataTable* EndlessStruggleTable;
};
