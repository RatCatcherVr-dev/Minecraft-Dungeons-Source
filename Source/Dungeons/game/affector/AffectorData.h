#pragma once

#include "CoreMinimal.h"
#include "game/item/ItemType.h"
#include "AffectorData.generated.h"

enum class EItemType : uint8;
enum class EAffectorId : uint8;

UENUM(BlueprintType)
enum class EAffectorApplicationType : uint8
{
	None,
	DefaultApplication,
	MidGame,
};

USTRUCT()
struct DUNGEONS_API FAffectorData {
	GENERATED_BODY()

	FAffectorData() = default;
	FAffectorData(FString);
	FAffectorData(const char*);
	FAffectorData(int);
	FAffectorData(float);
	FAffectorData(bool);

	FAffectorData& SetDefault();
	FAffectorData& SetApplicationType(EAffectorApplicationType);


	int   AsInt() const;
	float AsFloat() const;
	bool  AsBoolean() const;
	const FString& AsString() const;
	TOptional<FItemId> AsItemType() const;

	bool IsDefault() const;
	bool IsMidGame() const;
	EAffectorApplicationType GetApplicationType() const;

	operator int() const { return AsInt(); }
	operator float() const { return AsFloat(); }
	operator bool() const { return AsBoolean(); }
	operator const FString&() const { return AsString(); }
	operator const TOptional<FItemId>() const { return AsItemType(); };

private:
	UPROPERTY()
	FString Data;

	UPROPERTY()
	int mInt;

	UPROPERTY()
	float mFloat;

	UPROPERTY()
	bool mBool;

	//A Default Affector does not show up in the UI. Some basic rules
	UPROPERTY()
	EAffectorApplicationType ApplicationType = EAffectorApplicationType::None;
};

namespace affector {

struct RuleData {
	RuleData() {}
	RuleData(EAffectorId, FAffectorData);
	RuleData(TMap<EAffectorId, FAffectorData>);

	bool Has(EAffectorId) const;

	const FAffectorData& Get(EAffectorId) const;

	RuleData& Set(EAffectorId, FAffectorData);

	TMap<EAffectorId, FAffectorData> Data;
};

}