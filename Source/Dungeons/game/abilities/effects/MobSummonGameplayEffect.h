#pragma once


#include "CoreMinimal.h"
#include <GameplayEffect.h>
#include "MobSummonGameplayEffect.generated.h"


UCLASS()
class DUNGEONS_API UMobSummonGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UMobSummonGameplayEffect(const FObjectInitializer& ObjectInitializer);
};
