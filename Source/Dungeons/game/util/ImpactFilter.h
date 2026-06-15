#pragma once


#include "GameplayTagContainer.h"
#include "ImpactFilter.generated.h"

USTRUCT(BlueprintType)
struct DUNGEONS_API FImpactFilterEntry {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|ImpactFilter")
	FGameplayTag Tag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|ImpactFilter")
	bool Allow;
};

USTRUCT(BlueprintType)
struct DUNGEONS_API FImpactFilter {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|ImpactFilter")
	TArray<FImpactFilterEntry> Entries;
};

namespace impactFilter {
	bool shouldImpact(const FImpactFilter&, const FGameplayTag&);
}
