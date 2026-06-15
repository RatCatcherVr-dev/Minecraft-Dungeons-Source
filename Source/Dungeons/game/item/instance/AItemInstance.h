#pragma once
#include "CommonTypes.h"
#include "Sound/SoundCue.h"
#include "game/item/power/ItemPowerEffect.h"
#include "game/item/InventoryItemData.h"
#include "game/item/ItemTypeDefs.h"
#include "game/item/stats/ItemStatsTypes.h"
#include "game/item/stats/ItemStatsUtil.h"
#include "game/difficulty/DifficultyStats.h"
#include <GameplayPrediction.h>
#include <Array.h>
#include <SubclassOf.h>
#include "game/CooldownProvider.h"
#include "game/actor/item/GearActor.h"
#include "game/item/SerializableItemId.h"
#include "GameFramework/Info.h"
#include "game/item/ItemBulletPoint.h"
#include "AItemInstance.generated.h"

class AActor;
class ABaseCharacter;

DECLARE_MULTICAST_DELEGATE(FOnItemSuccess);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemActivationPending, bool);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemActivationCompleted, bool);
DECLARE_MULTICAST_DELEGATE(FOnItemPowerChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAlternativeItemChanged);


/**
 *
 */
UCLASS()
class DUNGEONS_API UDummyCooldownProvider : public UObject, public ICooldownProvider
{
	GENERATED_BODY()
};

UCLASS(Blueprintable, BlueprintType)
class DUNGEONS_API AItemInstance : public AInfo {
	GENERATED_BODY()
public:
	AItemInstance();

	virtual int GetDisplayCount() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	virtual bool CanActivate() const;
	const ItemType& GetItemType() const;	
	void SetItemPower(float itemPower);
	float GetItemPower() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool ItemIsBigWeapon() const {return bIsBigWeapon;};

	bool IsAutomaticDestructible();

	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	virtual const FSerializableItemId& GetItemId() const;

	// Some ItemInstance can spawn items and we may want to show it on the UI
	// The UI will ask for this to know which texture to show
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	virtual const FSerializableItemId& GetAlternativeItemId() const;
	
	virtual bool TryActivate();


	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons")
	USoundCue* InventoryDropSound = nullptr;

	FOnItemSuccess OnItemSuccess;
	FOnItemActivationPending OnItemActivationPending;
	FOnItemActivationCompleted OnItemActivationCompleted;
	FOnItemPowerChanged OnItemPowerChanged;	
	UPROPERTY(BlueprintAssignable)
	FOnAlternativeItemChanged OnAlternativeItemChanged;
	
	virtual bool IsBusy() const;

	virtual void OnTriedToActivate() const;

	UFUNCTION(BlueprintPure, Category = "Dungeons", meta = (WorldContext = "WorldContextObject"))
	static USoundCue* GetInventoryDropSoundForItemId(UObject* WorldContextObject, const FSerializableItemId& itemId);

	virtual void ResetInstance();

	virtual float GetStats(EItemStats stat) const;
	EItemStats PrimaryDamageStat = EItemStats::Invalid;

	virtual void ApplyEquippedEffects();
	virtual void RemoveEquippedEffects();
	virtual void ActivateEquippedEffects();
	virtual void DeactivateEquippedEffects();	

	virtual TArray<FText> CreatePowerEffectDescription(const ABaseCharacter& owner, float power) const;
	virtual void PopulateBulletPoints(TArray<FItemBulletPoint>& intoList, const ABaseCharacter& owner) const;

	TOptional<FInventoryItemData> OptionalItemDataSource;

	void SetCooldownProvider(ICooldownProvider*);
	virtual void OnCooldownProviderAssigned(ICooldownProvider*) {};
	
	virtual void ForceActivate();

	UPROPERTY(AssetRegistrySearchable)
	FName ItemIdName;

	int GetSoulGatherCount() const;
	
	virtual float GetSoulActivationCost() const;

protected:
	void ActivationSucceeded(FPredictionKey);

	virtual void OnSetupWithValidOwner();
	virtual void OnOwnerAndOwnerControllerReplicated();

	TArray<FActiveGameplayEffectHandle> EffectHandles;
	
	//For tracking if we should reapply when ItemPower changes.
	bool bEffectsApplied;

	UPROPERTY()
	TArray<TSubclassOf<UItemPowerEffect>> PowerEffects;

	UItemPowerEffect* GetPowerEffect(int32 index = 0) const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons", Transient, Replicated)
	float ItemPower = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float SharedPassiveCooldown = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons", Transient, Replicated)
	bool bAutomaticDestruction = true;

	UFUNCTION(Server, Reliable, WithValidation, Category = "Dungeons")
	virtual void ActivateServer(FPredictionKey predictionKey);

	virtual void Activate(const FPredictionKey& predictionKey);

	bool bHasManualCooldownActivation = false;
	bool bHasManualSoulConsumption = false;

	UFUNCTION(BlueprintImplementableEvent)
	void OnActivated();

	float CalculateCooldown() const;	

	class USoulComponent* GetSoulComponent() const;
	class APlayerCharacter* GetPlayerOwner() const;
	class ABaseCharacter* GetCharacterOwner() const;

	game::CachedDifficultyStats mDifficultyStats;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	FName ActivationAnimationSlot = "UpperBody";

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	class UAnimSequenceBase* ActivationAnimation = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	TSubclassOf<AItemActor> ItemActorClass = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	bool bIsBigWeapon;

	bool bCanFail = false;
	bool bManualPredictionHandling = false;
	bool bUsePrediction = true;
	
	ICooldownProvider& Cooldown() const { return *CooldownProvider; }

	
	virtual void HandleItemActorSpawn();

	virtual void HandleItemActorRemoval() const;

	void SpawnItemActor();
	void RemoveItemActor() const;
	class UEquipmentDisplayComponent* GetOwnerEquipmentDisplayComponent() const;

	virtual void OnPlayerMeleeAttack(const APlayerCharacter* player);

	virtual void OnPlayerBeginRangedAttack(const APlayerCharacter* player);

private:
	void Serialize(FArchive& Ar) override;
#if WITH_EDITOR
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
	ICooldownProvider* CooldownProvider = nullptr;
	
	void OnRep_Owner() final;

	UFUNCTION()
	void OnOwnerControllerReplicated();

	UPROPERTY()
	TWeakObjectPtr<class AItemActor> ItemActor = nullptr;

	FTimerHandle RemoveItemActorTimerHandle;

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastHandleItemActorSpawn();

	UFUNCTION(Server, Unreliable, WithValidation)
	void ServerHandleItemActorRemoval();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastHandleItemActorRemoval();

	void RemoveItemActorFromLocal();
	
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	FSerializableItemId ItemId;
};
