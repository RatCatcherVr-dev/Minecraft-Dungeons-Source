#pragma once

#include "CoreMinimal.h"
#include "game/component/RangedAttackComponent.h"
#include "AutoAimRangedAttackComponent.generated.h"

UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DUNGEONS_API UAutoAimRangedAttackComponent : public URangedAttackComponent
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons|RangedAttackComponent|AutoAim")
	bool autoAimDebug = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons|RangedAttackComponent|AutoAim")
	bool autoAim = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons|RangedAttackComponent|AutoAim")
	float autoAimAngleDegrees = 30.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons|RangedAttackComponent|AutoAim")
	float autoAimRangeUnits = 1000.f;

	AActor* GetAutoAimTarget(FVector& location) const;

	bool IsAutoAimEnabled() const;
protected:
	FVector CalculateAttackLocation(ABaseCharacter* attacker, const FVector& emitLocation, const FVector& emitDirection, AActor* attackTarget, const FRangedAttackProjectileSpawnDescription& definition, const FItemId& ammoType) const override;
	FVector GetAttackerBaseEmitVector(ABaseCharacter* attacker) const override;
};

