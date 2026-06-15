#pragma once


#include "GameplayTagContainer.h"
#include "DamageFilter.generated.h"

USTRUCT(BlueprintType)
struct DUNGEONS_API FDamageMultiplier {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|DamageMultiplier")
	FGameplayTag Tag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|DamageMultiplier")
	float Multiplier;

	bool operator==(const FGameplayTag& tag) const;
};

USTRUCT(BlueprintType)
struct DUNGEONS_API FDamageFilter {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|DamageFilter")
	TArray<FDamageMultiplier> Multipliers;
};

namespace damageFilter {
	float getMultiplier(const FDamageFilter&, const FGameplayTagContainer&);
}
