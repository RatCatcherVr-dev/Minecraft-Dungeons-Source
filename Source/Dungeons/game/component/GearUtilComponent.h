#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayPrediction.h"
#include "game/component/MeleeAttackComponent.h"
#include "game/Enchantments/EnchantmentType.h"
#include "game/item/ItemRarity.h"
#include "GearUtilComponent.generated.h"

struct FItemDropSource;
struct FDropCategoryDescription;
class AGearItemInstance;
class UEnchantment;
struct FGameplayEffectModCallbackData;
class ABaseProjectile;
class UGearUtil;
class AItemInstance;
struct FInventoryItemData;
class ABasePlayerController;

UENUM(BlueprintType)
enum class EPlayerIdleState : uint8 {
	Active,
	Inactive,
	Idle
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONS_API UGearUtilComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGearUtilComponent();

	void OnBeforeMeleeDamageDealt(float &outPushbackMultiplier, FGameplayEffectSpec& mutableSpec, class UAbilitySystemComponent* targetComponent, FRandomStream* overrideRandom = nullptr, FSharedPredictionContext context = FSharedPredictionContext());

	void OnAfterDealtMeleeDamage(float damage, bool missedAttack, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream* overrideRandom = nullptr, FSharedPredictionContext context = FSharedPredictionContext());

	void OnBeforeMeleeAttack(AActor* attackTarget, FVector attackVector, int32 index, FRandomStream* overrideRandom, FSharedPredictionContext context = FSharedPredictionContext());

	void OnAfterMeleeAttack(AActor* attackTarget, int32 hitCount, FVector attackVector, int32 index, FRandomStream* overrideRandom, FSharedPredictionContext context = FSharedPredictionContext());

	void OnBeforeAoeAttack(AActor* attackTarget, FRandomStream* overrideRandom = nullptr, FSharedPredictionContext context = FSharedPredictionContext());
	
	void OnBeforeAoeAttackDamage(AActor* attackTarget, FRandomStream* overrideRandom, FSharedPredictionContext context = FSharedPredictionContext());

	void OnAfterAoeAttackDamage(AActor* attackTarget, FRandomStream* overrideRandom, FSharedPredictionContext context = FSharedPredictionContext());

	/**
	 * Only called on the server
	 */
	void OnProjectileOverlap(AActor *what, AActor *whom, FVector fromLocation, FVector atLocation, ABaseProjectile* fromProjectile);

	/**
	 * Only called on the server
	 */
	void OnProjectileLaunched(ABaseProjectile* fromProjectile);

	/**
	 * Only called on the server
	 */
	void OnBeforeRangedDamageDealt(float &outDamage, FGameplayTag& damageType, ABaseProjectile* fromProjectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation);

	/**
	 * Only called on the server
	 */
	void OnAfterDealtRangedDamage(float Damage, ABaseProjectile* fromProjectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation);

	/**
	 * Called on both server and all clients. Calls will match on client and server.
	 */
	void OnAfterRangedAttackPerProjectile(AActor* attackTarget, const FVector& attackLocation, ABaseProjectile* projectile, TSubclassOf<ABaseProjectile> ProjectileClass, FVector spawnLocation, FRotator rotation, uint32 currentArrowCount, const FRandomStream& randStream, FPredictionKey key = FPredictionKey());


	/**
	 * Called on both server and all clients. Calls will match on client and server.
	 */
	void OnAfterRangedAttack(AActor* attackTarget, TSubclassOf<ABaseProjectile> ProjectileClass, bool charged, FVector spawnLocation, FRotator rotation, uint32 currentArrowCount, const FRandomStream& randStream, FPredictionKey key = FPredictionKey());


	/**
	* Called on both server and owning client.
	*/
	void OnBeforeRangedAttack(AActor* attackTarget, bool& attackDenied, FPredictionKey key = FPredictionKey());

	/**
	* Called only on server.
	*/
	void OnAfterAmmoConsumed(TSubclassOf<ABaseProjectile> projectileDefinition);

	/**
	* Called only on server.
	*/
	void OnRangedAttackEnded(bool completed, bool didSpawnProjectiles);
	
	/**
	 * Called on both server and clients.
	 */
	void OnResetRangedProjectile(ABaseProjectile* projectile);


	void OnOverlappedByProjectile(ABaseProjectile* projectile);

	void OnBeforeDamageReceived(bool& outAttackMissed, FGameplayEffectModCallbackData& data, UAbilitySystemComponent* targetComponent, FRandomStream* overrideRandom = nullptr, FPredictionKey key = FPredictionKey());

	void OnAfterReceivedDamage(const FGameplayEffectModCallbackData &data, FRandomStream* overrideRandom = nullptr);


	void OnDodgeRollStart(const FVector& dodgeDirection, FPredictionKey);

	void OnDodgeRollEnd(FPredictionKey);

	bool OnAfterDropRolled(const FDropCategoryDescription& dropData, const FItemDropSource& dropSource, float probability);

	void OnEmeraldsCollected(int32 amount);

	void OnAfterResurrection();

	void OnInvisibilityRemoved();

	void OnInvisibilityGained();

	void OnPlayerIdleChange(const ABasePlayerController* player, EPlayerIdleState idle);

	TOptional<UGearUtil*> ChangeItemRarity(EItemRarity& rarity);

	//Only called on *owning* client, i.e if is locally controlled.
	void OnHasStoredItem(class AStorableItem* item);

	void OnItemActivated(const AItemInstance* item, FPredictionKey key);

protected:
	virtual TArray<UGearUtil*> GetValidGearUtils() const { return {}; };
private:
	FRandomStream DefaultRandom;
};
