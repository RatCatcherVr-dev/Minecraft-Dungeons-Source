#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "WeakenGameplayEffect.generated.h"


UCLASS()
class DUNGEONS_API UWeakenGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UWeakenGameplayEffect(const FObjectInitializer& ObjectInitializer);


	static const FName DurationName;
	static const FName DamageTakenName;
	static const FName DamageDealtName;
};


