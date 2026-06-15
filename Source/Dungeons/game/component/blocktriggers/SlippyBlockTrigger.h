#pragma once

#include "game/component/BlockTriggerComponent.h"
#include "SlippyBlockTrigger.generated.h"

UCLASS(Abstract)
class USlippyBlockTrigger : public UBlockTrigger
{
	GENERATED_BODY()

protected:
	void OnEnterStep(EMaterialTypeEnum& stepMaterial, ABaseCharacter* const character) const override;
	void OnExitStep(EMaterialTypeEnum& stepMaterial, ABaseCharacter* const character) const override;

	bool IsTrigger(const FullBlock& block, ABaseCharacter* const character) const override { return false; };
};

UCLASS()
class UFrozenFjordSlippyBlockTrigger : public USlippyBlockTrigger
{
	GENERATED_BODY()

	bool IsTrigger(const FullBlock& block, ABaseCharacter* const character) const override;
};


UCLASS()
class ULonelyFortressSlippyBlockTrigger : public USlippyBlockTrigger
{
	GENERATED_BODY()

	bool IsTrigger(const FullBlock& block, ABaseCharacter* const character) const override;
};

UCLASS()
class ULostSettlementSlippyBlockTrigger : public USlippyBlockTrigger
{
	GENERATED_BODY()

	bool IsTrigger(const FullBlock& block, ABaseCharacter* const character) const override;
};

UCLASS()
class UWindsweptPeaksSlippyBlockTrigger : public USlippyBlockTrigger
{
	GENERATED_BODY()

	bool IsTrigger(const FullBlock& block, ABaseCharacter* const character) const override;
};
