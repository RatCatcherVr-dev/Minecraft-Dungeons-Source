#pragma once
#include "LevelCommon.generated.h"

UENUM(BlueprintType)
enum class EMaterialTypeEnum : uint8
{
	Air,
	Dirt,
	Wood,
	Stone,
	Metal,
	Water,
	Lava,
	Leaves,
	Plant,
	ReplaceablePlant,
	Sponge,
	Cloth,
	Bed,
	Fire,
	Sand,
	Decoration,
	Glass,
	Explosive,
	Ice,
	PackedIce,
	TopSnow,
	Snow,
	Cactus,
	Clay,
	Vegetable,
	Portal,
	Cake,
	Web,
	RedstoneWire,
	Carpet,
	BuildableGlass,
	Slime,
	Piston,
	Allow,
	Deny,
	Void,
	Grass,
	GrassPath,
	Gravel,


	SlippyFloor,

	SurfaceTypeTotal,
	Any
};
ENUM_NAME(EMaterialTypeEnum)

USTRUCT(BlueprintType)
struct DUNGEONS_API FLevelHitResult
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Result)
		bool isHit = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Result)
		bool isLiquidHit = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Result)
		EMaterialTypeEnum materialType;

	FLevelHitResult(bool isHit = false, bool isLiquidHit = false, EMaterialTypeEnum materialType = EMaterialTypeEnum::Air) : isHit(isHit), isLiquidHit(isLiquidHit), materialType(materialType) {}
};
