#pragma once
#include "game/Enchantments/Enchantment.h"
#include "CoreMinimal.h"
#include "../item/instance/BackstabbersBrewInstance.h"

#include "ShadowShot.generated.h"


UCLASS()
class DUNGEONS_API UShadowShot : public UEnchantment
{
	GENERATED_BODY()
public:
	UShadowShot();

	void OnAfterDealtRangedDamage(float Damage, ABaseProjectile* fromProjectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream) override;

	void OnBeforeRangedAttack(AActor * attackTarget, bool& attackDenied, FPredictionKey key) override;

	UPROPERTY(EditDefaultsOnly)
		float DurationMagnitude = 3.0f;

	UPROPERTY(EditDefaultsOnly)
		float MeleePowerBoostAmount = 8.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		TSubclassOf<UBackstabbersBrewGameplayEffect> Effect;

	UPROPERTY(EditDefaultsOnly)
		float TriggerChance = 0.5f;
private:
	bool bTriggerShadowShot = true;
};