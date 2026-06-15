#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "DamageWeakeningGameplayEffect.generated.h"


UCLASS()
class DUNGEONS_API UDamageWeakeningGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UDamageWeakeningGameplayEffect(const FObjectInitializer& ObjectInitializer);

	static const FName DurationName;	
	static const FName DamageDealtDividerName;
};


