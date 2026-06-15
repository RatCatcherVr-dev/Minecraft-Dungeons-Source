#pragma once

#include "CoreMinimal.h"
#include "game/item/instance/GearItemInstance.h"
#include "ArmorGearItemInstance.generated.h"

UCLASS()
class DUNGEONS_API AArmorGearItemInstance : public AGearItemInstance
{
	GENERATED_BODY()
public:
	AArmorGearItemInstance();

	void SetArmorProperties(const TArray<FArmorPropertyData>&);

	void ApplyEquippedEffects() override;

	void RemoveEquippedEffects() override;

	void ActivateEquippedEffects() override;

	void DeactivateEquippedEffects() override;
private:
	TArray<FArmorPropertyData> armorProperties;
};
