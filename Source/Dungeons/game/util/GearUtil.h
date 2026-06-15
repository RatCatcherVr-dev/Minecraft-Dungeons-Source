#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "game/Enchantments/EnchantmentType.h"
#include "GameplayTagContainer.h"
#include "GameplayPrediction.h"
#include "game/component/MeleeAttackComponent.h"
#include <SharedPointer.h>
#include "game/util/ValueFormat.h"
#include "game/item/drop/ItemDropData.h"
#include "game/item/ItemBulletPoint.h"
#include <AbilitySystemInterface.h>
#include <GameplayEffectTypes.h>
#include "game/item/drop/ItemDropGenerator.h"
#include "game/item/InventoryItemData.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "GearUtil.generated.h"

struct FRandomStream;
struct FDropCategoryDescription;
class ABaseProjectile;

UCLASS(Blueprintable, BlueprintType)
class DUNGEONS_API UGearUtil : public UActorComponent, public IAbilitySystemInterface
{
 	GENERATED_BODY()

public:	
	UGearUtil();

public:	
	virtual void OnBeforeReceivedDamage(bool& outAttackMissed, struct FGameplayEffectModCallbackData &data, UAbilitySystemComponent* targetComponent, FRandomStream& randStream, FPredictionKey key) {};

	virtual void OnAfterReceivedDamage(const struct FGameplayEffectModCallbackData &data, FRandomStream& randStream) {};

	virtual void OnBeforeDealtMeleeDamage(float &outPushbackMultiplier, FGameplayEffectSpec& mutableSpec, UAbilitySystemComponent* targetComponent, FRandomStream& randStream, FSharedPredictionContext window) {};

	virtual void OnAfterDealtMeleeDamage(float Damage, bool missedAttack, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream, FSharedPredictionContext window) {};

	virtual void OnBeforeMeleeAttack(AActor* attackTarget, FVector attackVector, int32 index, FRandomStream& randStream, FSharedPredictionContext window) {};

	virtual void OnAfterMeleeAttack(AActor* attackTarget, int32 hitCount, FVector attackVector, int32 index, FRandomStream& randStream, FSharedPredictionContext window) {};

	virtual void OnBeforeAoeAttack(AActor* attackTarget, FRandomStream& randStream, FSharedPredictionContext window) {};

	virtual void OnBeforeDealtAoeDamage(AActor* attackTarget, FRandomStream& randStream, FSharedPredictionContext window) {}; // D11.DB

	virtual void OnAfterDealtAoeDamage(AActor* attackTarget, FRandomStream& randStream, FSharedPredictionContext window) {}; // D11.DB

	virtual void OnProjectileOverlap(AActor *what, AActor *whom, FVector fromLocation, FVector atLocation, FRandomStream& randStream) {};

	virtual void OnProjectileLaunch(ABaseProjectile* fromProjectile) {}; // #D11.CM

	virtual void OnAfterRangedAttackPerProjectile(AActor* attackTarget, const FVector& attackTargetLocation, ABaseProjectile* projectile, TSubclassOf<ABaseProjectile> ProjectileClass, FVector spawnLocation, FRotator rotation, uint32 currentArrowCount, const FRandomStream& randStream, FPredictionKey key) {};

	virtual void OnBeforeRangedAttack(AActor * attackTarget, bool& attackDenied, FPredictionKey key) {}

	virtual void OnAfterRangedAttack(AActor* attackTarget, TSubclassOf<ABaseProjectile> ProjectileClass, bool charged, FVector spawnLocation, FRotator rotation, uint32 currentArrowCount, const FRandomStream& randStream, FPredictionKey key) {};
	//virtual void OnAfterRangedAttack(const ABaseCharacter* Character, AActor* attackTarget, TSubclassOf<ABaseProjectile> ProjectileClass, bool charged, FVector spawnLocation, FRotator rotation, uint32 currentArrowCount, const FRandomStream& randStream, FPredictionKey key = FPredictionKey()) {};

	virtual void OnRangedAttackEnded(bool completed, bool didSpawnProjectiles) {};

	virtual void OnBeforeDealtRangedDamage(float &outDamage, FGameplayTag& damageType, ABaseProjectile* fromProjectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream) {};

	virtual void OnAfterDealtRangedDamage(float Damage, ABaseProjectile* fromProjectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream) {};

	virtual void OnResetRangedProjectile(ABaseProjectile* fromProjectile) {};

	virtual void OnOverlappedByProjectile(ABaseProjectile* projectile, const FRandomStream& randStrteam) {};

	virtual void OnAfterAmmoConsumed(TSubclassOf<ABaseProjectile> projectileDefinition) {}

	virtual void OnDodgeRollStart(const FVector& dodgeDirection, FPredictionKey) {};

	virtual void OnDodgeRollEnd(FPredictionKey) {};

	virtual bool OnAfterDropRolled(const FDropCategoryDescription& dropData, const FItemDropSource& dropSource, float probability) { return false; };

	virtual void OnEmeraldsCollected(int32 amount) {};

	virtual void OnAfterResurrection() {};

	virtual void OnLuckChanged(FPredictionKey) {};

	virtual bool ShouldTriggerOnProjectile(const ABaseProjectile* Projectile) const { return true; };

	virtual void OnInvisibilityRemoved() {};

	virtual void OnItemActivated(const AItemInstance*, FPredictionKey key) {};

	virtual void OnPlayerIdleChange(const ABasePlayerController* player, EPlayerIdleState idle) {};

	virtual void OnInvisibilityGained() {};

	UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	FORCEINLINE bool IsPredictive() const { return PredictiveExecution; }

	FORCEINLINE bool CanExecuteOnClient() const { return !ServerOnlyExecution; }

	FORCEINLINE void SetIsCurrentlyRolling(bool isRolling) { mIsCurrentlyRolling = isRolling; }

	virtual TOptional<UGearUtil*> ChangeItemRarity(EItemRarity& rarity) { return TOptional<UGearUtil*>(); };
protected:
	bool PredictiveExecution = false;

	//This can only be executed on the server. Uses non shared random stream.
	bool ServerOnlyExecution = false;

	bool IsCurrentlyRolling() const { return mIsCurrentlyRolling; }

private:
	bool mIsCurrentlyRolling = false; // FIX hack: won't create an infinite loop for enchantment calls
};
