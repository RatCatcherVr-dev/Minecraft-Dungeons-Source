#pragma once

#include "util/FloatRange.h"
#include "game/component/MassComponent.h"
#include "Pushback.generated.h"

USTRUCT(BlueprintType)
struct DUNGEONS_API FPushback {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|Pushback")
	bool enablePushback = false;

	///Number of blocks we push a mob (at z factor 1, push multiplier 1)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|Pushback")
	float pushbackStrength = 1.0f;

	///XY is 1, Z is whatever you write here, then normalized
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|Pushback")
	float pushbackZFactor = 1.0f;

	///If true, use source's forward direction instead of using direction from source to target
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|Pushback")
	bool useForwardDirection = false;

	///How much should we take the affected mob's pushback multiplier into account, vs just ignoring it and treating everyone equally
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|Pushback", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float pushbackMultiplierRespectFactor = 1.0f;
	
	// Time we want to take to call that the montage just finished (If time > montagePlayTime, montagePlayTime will be used)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|Pushback")
	float pushbackTimeToCallFinish = 1.0f;
};

namespace pushback {
	bool _canPushback(const FPushback& push, const UMassComponent& massComponent, float externalPushbackMultiplier);
	float _calculatePushbackMultiplier(const FPushback& push, const UMassComponent& massComponent, float externalPushbackMultiplier, const AActor& affecting, bool bApplyResistance);
	void pushback(const FPushback& push, const AActor& by, const AActor& affecting, float externalPushbackMultiplier = 1.f, bool bApplyResistance = true, bool ignoreLevelClamps = false);
	void pushback(const FPushback& push, const FVector& direction, const AActor& affecting, float externalPushbackMultiplier = 1.f, bool bApplyResistance = true, bool ignoreLevelClamps = false);
	FVector getLaunchVector(const FPushback& push, const AActor& by, const AActor& affecting, float pushbackMultiplier = 1.f, float minimumPushbackStrength = 0.f);
	FVector getLaunchVector(const FPushback& push, const FVector& direction, const AActor& affecting, float pushbackMultiplier = 1.f, float minimumPushbackStrength = 0.f);
	FVector adjustDirectionZ(const FPushback& push, const FVector& direction, const AActor& affecting);

	const static FName PushbackZStrengthName = FName("PushbackZStrengthName");
}
