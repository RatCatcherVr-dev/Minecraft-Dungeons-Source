#pragma once

#include "CoreMinimal.h"
#include "game/item/instance/AItemInstance.h"
#include "game/Enchantments/Enchantment.h"
#include "Synergy.h"
#include "BeastSurge.generated.h"


UCLASS()
class DUNGEONS_API UBeastSurgeGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UBeastSurgeGameplayEffect(const FObjectInitializer&);
};


UCLASS()
class DUNGEONS_API UBeastSurge : public UHealthPotionSynergy {
	GENERATED_BODY()
	
public:
	UBeastSurge();

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UBeastSurgeGameplayEffect> Effect;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float BaseModifier = 1.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float Level2Modifier = 2.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float Level3Modifier = 2.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float PerLevelModifier = 0.3f;

protected:
	void OnHealthPotionUsed() override;
};
