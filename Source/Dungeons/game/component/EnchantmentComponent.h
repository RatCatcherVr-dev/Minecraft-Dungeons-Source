#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayPrediction.h"
#include "game/component/MeleeAttackComponent.h"
#include "game/item/enchantment/EnchantmentData.h"
#include "GearUtilComponent.h"
#include "EnchantmentComponent.generated.h"

struct FItemDropSource;
struct FDropCategoryDescription;
class AGearItemInstance;
class UEnchantment;
struct FGameplayEffectModCallbackData;
class ABaseProjectile;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnchantmentsUpdated);

struct EnchantmentSpawnConfig {
	AActor* source = nullptr;
	bool overriding = false;
	TOptional<bool> treatAsInherited;
	TOptional<EItemRarity> ownerItemRarity;

	EnchantmentSpawnConfig& SetOverriding();
	EnchantmentSpawnConfig& SetTreatAsInherited();
	EnchantmentSpawnConfig& SetOwnerItemRarity(EItemRarity);
};

EnchantmentSpawnConfig createEnchantmentSpawnConfig(AActor* source);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONS_API UEnchantmentComponent : public UGearUtilComponent
{
	GENERATED_BODY()

public:
	UEnchantmentComponent();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static int HighestAvailableIndex(const TArray<FEnchantmentData>& enchantments);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static int HighestIndex(const TArray<FEnchantmentData>& enchantments);

	void TryRegisterEnchantment(UEnchantment* enchantment);
	void TryUnregisterEnchantment(UEnchantment* enchantment);

	void AddEnchantments(const TArray<FEnchantmentData>&, EnchantmentSpawnConfig = {});
	void RemoveEnchantments(const AActor* source = nullptr);

	UFUNCTION(BlueprintPure, BlueprintPure, Category = "Dungeons")
	UEnchantment* GetEnchantment(EEnchantmentTypeID id);

	FORCEINLINE bool HasEnchantment(EEnchantmentTypeID id) { return GetEnchantment(id) != nullptr; }

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	TArray<FEnchantmentData> GetEnchantments() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	TArray<FEnchantmentDataWithRarity> GetEnchantmentsWithRarity() const;

	TArray<UEnchantment*> GetEnchantmentsBySource(const AActor* source) const;

	TArray<UEnchantment*> GetValidEnchantments() const;

	TArray<UGearUtil*> GetValidGearUtils() const override;

	UPROPERTY(BlueprintAssignable)
	FOnEnchantmentsUpdated OnEnchantmentsUpdated;

	virtual void OnEnchantmentRegistered(UEnchantment*) {};
protected:
	virtual class UEnchantment* SpawnEnchantment(const FEnchantmentData&, EnchantmentSpawnConfig);

	void BeginPlay() override;

	UFUNCTION()
	virtual void OnRep_Enchantments();

	void GetLifetimeReplicatedProps(TArray < FLifetimeProperty > & OutLifetimeProps) const override;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_Enchantments)
	TArray<UEnchantment*> Enchantments;
private:
	void OnDeath();
	void OnRespawn();
	void OnRevived(AActor* revivingActor);

	FRandomStream DefaultRandom;
};
