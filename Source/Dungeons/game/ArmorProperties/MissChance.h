#pragma once

#include "CoreMinimal.h"
#include "game/ArmorProperties/ArmorProperty.h"
#include "game/util/ValueFormat.h"
#include "MissChance.generated.h"

UCLASS()
class DUNGEONS_API UMissChance : public UArmorProperty
{
	GENERATED_BODY()
public:
	UMissChance();
	
	void OnBeforeReceivedDamage(bool& outAttackMissed, struct FGameplayEffectModCallbackData& data, UAbilitySystemComponent* targetComponent, FRandomStream& randStream, FPredictionKey key) override;
protected:
	FString createFormattedValueString() const override { return valueformat::asPercentageChance(Chance); };

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float Chance = 0.2f;

private:
	static bool IsBlockableDamage(const FGameplayEffectModCallbackData& data);
};
