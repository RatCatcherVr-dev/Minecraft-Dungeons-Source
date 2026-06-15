#pragma once

#include "CoreMinimal.h"
#include "game/component/RangedAttackComponent.h"
#include "MultiOffsetRangedAttackComponent.generated.h"


UENUM()
enum class EAttackNormal : uint8 {
	AttackOrigin,
	OwningActor
};

UCLASS( ClassGroup = (Custom), meta = (BlueprintSpawnableComponent) )
class DUNGEONS_API UMultiOffsetRangedAttackComponent : public URangedAttackComponent
{
	GENERATED_BODY()

protected:
	void PerformAttack(TWeakObjectPtr<AActor> attackTarget, ABaseCharacter* attacker, FItemId ammoType, float rangedAttackSpeedMultiplier, int32 seed) override;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons|RangedAttackComponent")
	FComponentReference AttackOriginSceneComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons|RangedAttackComponent")
	EAttackNormal AttackNormal = EAttackNormal::AttackOrigin;
};
