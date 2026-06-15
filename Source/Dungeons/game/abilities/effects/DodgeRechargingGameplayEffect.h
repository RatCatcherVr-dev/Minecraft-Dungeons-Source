#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "DodgeRechargingGameplayEffect.generated.h"

UCLASS()
class DUNGEONS_API UDodgeRechargingGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UDodgeRechargingGameplayEffect(const FObjectInitializer& ObjectInitializer);

	static const FName DurationName;
	static const FName SpeedFactorName;
	static const FName FrictionFactorName;
	static const FName RotationFactorName;
};

