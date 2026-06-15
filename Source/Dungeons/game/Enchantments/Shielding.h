#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include "game/component/AreaBuffComponent.h"
#include "Shielding.generated.h"

UCLASS()
class DUNGEONS_API UShieldingGameplayEffect : public UAreaBuffGameplayEffect {
	GENERATED_BODY()
public:
	UShieldingGameplayEffect(const FObjectInitializer& ObjectInitializer);
};


UCLASS()
class DUNGEONS_API UShielding : public UEnchantment
{
	GENERATED_BODY()
	
	UShielding();

	void OnStart() override;

	void OnEnd() override;
protected:
	FActiveGameplayEffectHandle PlayerOwnerEffectHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float ShieldMultiplier = 0.95f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float ShieldRadius = 1000.0f;
private:
	TWeakObjectPtr<UAreaBuffComponent> OwnerAreaBuffComponent;
};
