#pragma once

#include "game/item/instance/AItemInstance.h"
#include "game/item/generator/ItemGeneratorTypes.h"
#include "game/item/ItemTypeDefs.h"
#include "game/item/ItemRarityChance.h"
#include "game/item/ItemBulletPoint.h"
#include "game/item/drop/ItemDropGenerator.h"
#include "MysteryBoxInstance.generated.h"

UCLASS()
class DUNGEONS_API AMysteryBoxInstance : public AItemInstance
{
	GENERATED_BODY()
	AMysteryBoxInstance();
	void Activate(const FPredictionKey& predictionKey) override;

	void PopulateBulletPoints(TArray<FItemBulletPoint>&, const ABaseCharacter& owner) const override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	TSet<ESlotType> GeneratedItemSlotTypes;
		
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	EItemRarityChanceCategory GeneratedItemRarityChanceCategory;

private: 
	game::item::drop::DropGenerationInput GetDropGenerationInput(const ABaseCharacter& owner) const;
};
