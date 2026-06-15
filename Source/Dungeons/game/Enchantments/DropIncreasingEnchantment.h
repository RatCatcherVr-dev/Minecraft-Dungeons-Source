#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include "DropIncreasingEnchantment.generated.h"

UCLASS()
class DUNGEONS_API UDropIncreasingEnchantment : public UEnchantment {
	GENERATED_BODY()

public:
	UDropIncreasingEnchantment();

	void OnStart() override;
	bool OnAfterDropRolled(const FDropCategoryDescription& dropData, const FItemDropSource& dropSource, float probability) override;

protected:
	virtual void AddGameplayCueParameters(FGameplayCueParameters& params);

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	FGameplayTag DropGameplayCue;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float DropRateIncreaseBaseChance = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float DropRateIncreasePerLevel = 0.05f;

	EDropCategory DropCategory = EDropCategory::None;

private:
	void ExecuteGamePlayCue(FVector location);
	bool IsDropQualified(const FDropCategoryDescription& dropData, const FItemDropSource& dropSource) const;
	bool IsSuccessfulReroll(float probability) const;
	float GetRerollProbability(float probability) const;
};
