#pragma once

#include "Components/ActorComponent.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/GameBP.h"
#include "client/renderer/block/BlockGraphicsPack.h"
#include "BlockTriggerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBlockTriggerTick, float, DeltaTime, ABaseCharacter* const, Character);

UCLASS()
class UBlockTrigger : public UObject
{
	GENERATED_BODY()
public:
	void TryOverlapUpdate(EMaterialTypeEnum& overlapMaterial, const FullBlock& previousOverlapBlock, const FullBlock& currentOverlapBlock, ABaseCharacter* const character, FOnBlockTriggerTick& overlapTick) const;
	void TryStepUpdate(EMaterialTypeEnum& stepMaterial, const FullBlock& previousStepBlock, const FullBlock& currentStepBlock, ABaseCharacter* const character, FOnBlockTriggerTick& stepTick) const;

protected:
	virtual void OnEnterOverlap(EMaterialTypeEnum& overlapMaterial, ABaseCharacter* const character) const {};
	virtual void OnExitOverlap(EMaterialTypeEnum& overlapMaterial, ABaseCharacter* const character) const {};

	virtual void OnEnterStep(EMaterialTypeEnum& stepMaterial, ABaseCharacter* const character) const {};
	virtual void OnExitStep(EMaterialTypeEnum& stepMaterial, ABaseCharacter* const character) const {};

	virtual bool IsTrigger(const FullBlock& block, ABaseCharacter* const character) const { return false; };

	UFUNCTION()
	virtual void TickStep(float DeltaTime, ABaseCharacter* const character) {};

	UFUNCTION()
	virtual void TickOverlap(float DeltaTime, ABaseCharacter* const character) {};

private:
	bool TriggerHasChanged(bool& wasPreviousTrigger, const FullBlock& previousStepBlock, const FullBlock& currentStepBlock, ABaseCharacter* const character) const;

};

UCLASS(Within = BaseCharacter)
class DUNGEONS_API UBlockTriggerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBlockTriggerComponent();

private:
	void BeginPlay() override;
	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void TryApplyBlockTriggers(float DeltaTime);

	TWeakObjectPtr<AGameBP> Game;

	BlockPos previousStepPosition;
	BlockPos previousOverlapPosition;

	FOnBlockTriggerTick stepTick;
	FOnBlockTriggerTick overlapTick;
};
