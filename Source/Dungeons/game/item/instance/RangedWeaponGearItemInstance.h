#pragma once

#include "CoreMinimal.h"
#include "game/item/instance/GearItemInstance.h"
#include "game/actor/item/BaseProjectile.h"
#include "game/item/stats/ItemStatsTypes.h"
#include "game/component/RangedAttackComponent.h"
#include "RangedWeaponGearItemInstance.generated.h"

class UBaseRangedWeaponAnimNotifyState;

USTRUCT()
struct DUNGEONS_API FAnimNotifyStateData {
	GENERATED_USTRUCT_BODY()

	FAnimNotifyStateData();
	FAnimNotifyStateData(FString notifyStateName, USoundCue* sound);

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	FString NotifyStateName;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	USoundCue* TriggerSound;
};

UCLASS()
class DUNGEONS_API ARangedWeaponGearItemInstance : public AGearItemInstance
{
	GENERATED_BODY()
public:
	ARangedWeaponGearItemInstance();
	
	TSubclassOf<ABaseProjectile> GetProjectileOverrideForItemType(const FItemId& itemType) const;

	FORCEINLINE const FRangedAttack& GetRangedAttackDefintion() const { return AttackDefintion; }

	int GetProjectileStackCount() const;

	float GetStats(EItemStats stat) const override;

	UFUNCTION(BlueprintCallable)
	RangedWeaponType GetRangedWeaponType() const;

	FGameplayTag GetChargeShootEffectTag() const;

	UFUNCTION(BlueprintCallable)
	USoundCue* GetAnimNotifyStateSound(FString stateName) const;

	TArray< TSubclassOf< ABaseProjectile > > & GetOverrides(){ return ProjectileOverrides; }

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	FGameplayTag ChargeShootEffectTag;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	FRangedAttack AttackDefintion;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	int ProjectileStackCount = 30;
	
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	TArray<TSubclassOf<ABaseProjectile>> ProjectileOverrides;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	TArray<FAnimNotifyStateData> AnimNotifyStateData;
};
