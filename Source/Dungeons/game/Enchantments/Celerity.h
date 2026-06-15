#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include <GameplayEffect.h>
#include "Celerity.generated.h"

UCLASS()
class DUNGEONS_API UCelerityGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UCelerityGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UCelerity : public UEnchantment
{
	GENERATED_BODY()
	
	UCelerity();

	void OnStart() override;

	void OnEnd() override;
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UCelerityGameplayEffect> Effect;

	FActiveGameplayEffectHandle Handle;
};
