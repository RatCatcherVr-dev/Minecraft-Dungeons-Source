#pragma once

#include "game/component/BlockTriggerComponent.h"
#include "MagmaBlockTrigger.generated.h"

UCLASS()
class UMagmaBlockTrigger : public UBlockTrigger {
	GENERATED_BODY()

protected:
	void OnEnterStep(EMaterialTypeEnum& stepMaterial, ABaseCharacter* const character) const override;
	void OnExitStep(EMaterialTypeEnum& stepMaterial, ABaseCharacter* const character) const override;

	bool IsTrigger(const FullBlock& fullBlock, ABaseCharacter* const character) const override;
};