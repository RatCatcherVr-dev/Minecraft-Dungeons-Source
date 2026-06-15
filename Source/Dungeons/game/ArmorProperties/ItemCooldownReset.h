#pragma once

#include "CoreMinimal.h"
#include "game/ArmorProperties/ArmorProperty.h"
#include "ItemCooldownReset.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONS_API UItemCooldownReset : public UArmorProperty
{
	GENERATED_BODY()
public:
	UItemCooldownReset();

	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type reason) override;
private:
	UFUNCTION()
	void OnHealthPotionActivated(UItemSlot* slot, bool success);

	TArray<TWeakObjectPtr<UItemSlot>> SlotsToReset;
};
