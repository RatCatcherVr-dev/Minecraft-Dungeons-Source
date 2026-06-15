#pragma once

#include "game/component/BlockTriggerComponent.h"
#include "WaterBlockTrigger.generated.h"

UCLASS()
class UWaterBlockTrigger : public UBlockTrigger
{
	GENERATED_BODY()

protected:
	void OnEnterOverlap(EMaterialTypeEnum& overlapMaterial, ABaseCharacter* const character) const override;
	void OnExitOverlap(EMaterialTypeEnum& overlapMaterial, ABaseCharacter* const character) const override;

	bool IsTrigger(const FullBlock& block, ABaseCharacter* const character) const override;
};