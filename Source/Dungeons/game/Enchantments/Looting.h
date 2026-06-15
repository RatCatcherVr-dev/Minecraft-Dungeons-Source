#pragma once

#include "CoreMinimal.h"
#include "DropIncreasingEnchantment.h"
#include "Looting.generated.h"

UCLASS()
class DUNGEONS_API ULooting : public UDropIncreasingEnchantment
{
	GENERATED_BODY()
	
public:
	ULooting();
	void AddGameplayCueParameters(FGameplayCueParameters& params) override;
};

