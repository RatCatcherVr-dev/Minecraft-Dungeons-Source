#pragma once
#include "ItemType.h"
#include "game/ArmorProperties/ArmorPropertyData.h"
#include "game/component/EnchantmentComponent.h"
#include "game/component/ItemStashComponent.h"
#include "AbilitySystemInterface.h"
#include "game/CooldownProvider.h"
#include "ItemSlot.generated.h"

struct FStreamableHandle;
class UItemSlot;
class AItemInstance;
class UEffectBasedCooldownProvider;

UENUM()
enum class EEquipmentSource : uint8 {
	PickUp,
	Inventory,
	Crafted
};

USTRUCT()
struct DUNGEONS_API FItemMaterialSettings
{
	GENERATED_BODY()

	float power;
	TOptional<FLinearColor> emissiveColor;
	TOptional<FLinearColor> enchantmentColor;
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnReplicatedItemInstanceInternal, UItemSlot*);

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnItemMaterialSettingsChangedInternal, UItemSlot*, FItemMaterialSettings);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReplicatedItemInstance, UItemSlot*, slot);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnReplicatedCountInteral, const UItemSlot*);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReplicatedCount, UItemSlot*, slot);

DECLARE_DELEGATE_OneParam(FOnPickup, UItemSlot*);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDisplayCountChanged, UItemSlot*, slot);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemSlotActivationCompleted, UItemSlot*, slot, bool, success);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemSlotCooldownChanged, UItemSlot*, slot, bool, nowInCooldown);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnItemSlotCooldownUpdated, UItemSlot*, slot, float, totalDuration, float, remainingDuration, float, durationReduction);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnItemSlotWantsToActivateChanged);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnWasSelectedInUIInternal, const UItemSlot*);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DUNGEONS_API UItemSlot : public UActorComponent, public ICooldownProvider, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	UItemSlot();

	void BeginPlay() override;

	FOnReplicatedItemInstanceInternal OnItemInstanceReplicatedInternal;
	FOnItemMaterialSettingsChangedInternal OnItemMaterialSettingsChangedInternal;
	FOnReplicatedCountInteral OnReplicatedCountInteral;
	FOnWasSelectedInUIInternal OnWasSelectedInUIInternal;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnReplicatedItemInstance OnItemInstanceReplicated;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnReplicatedCount OnCountReplicated;

	FOnPickup OnPickedUp;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnDisplayCountChanged OnDisplayCountChanged;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnItemSlotActivationCompleted OnItemSlotActivationCompleted;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnItemSlotCooldownChanged OnItemSlotCooldownChanged;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnItemSlotCooldownUpdated OnItemSlotCooldownUpdated;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnItemSlotWantsToActivateChanged OnItemSlotWantsToActivateChanged;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons")
	ESlotType SlotTypeId = ESlotType::None;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	EEquipmentSlot EquipmentSlotId = EEquipmentSlot::Invalid;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	int GetDisplayCount() const;
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	virtual int GetCount() const;
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	AItemInstance* GetItem() const;
	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Dungeons")
	void EquipItem(const FInventoryItemData& itemData, int count = 1, EEquipmentSource source = EEquipmentSource::Inventory);
	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Dungeons")
	void Clear();
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	int Consume();
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	virtual void ConsumeAmount(int32 amount);

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool CanActivateSlot() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool IsBusy() const;
	
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool GetWantsToActivate() const;


	UFUNCTION(Server, Reliable, WithValidation)
	void Swap(UItemSlot* other);
	
	bool SetWantsToActivate(bool WantsToActivate);	

	void ResetSlot();
	bool ActivateSlot();

	void OnTriedToActivate() const;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual bool CanEquip(const ItemType& itemType) const;

	UFUNCTION()
	void OnRep_ItemMaterialSettingsReplicated();

	void RefreshItemMaterialSettings();

	void SetCount(int count);

	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	TOptional<FItemMaterialSettings> GetItemMaterialSettings();

	UFUNCTION()
	bool IsOnCooldown() const override;

	UFUNCTION()
	float GetCooldownSecondsRemaining() const override;
	UFUNCTION()
	float GetCooldownFractionRemaining() const override;
	UFUNCTION()
	float GetCooldownSecondsDuration() const override;
	UFUNCTION()
	float GetCooldownSecondsReduction() const;

	void SetCooldownSecondsReduction(float reduction);

	TPair<float, float> GetCooldownSecondsRemainingAndDuration() const override;

	void TriggerCooldown(float seconds, float reduction, FPredictionKey key = FPredictionKey()) override;
	void TriggerCooldown(float seconds, FPredictionKey key = FPredictionKey()) override;
	void ModifyCooldown(float offsetSeconds) override;
	void ResetCooldown();

	UEffectBasedCooldownProvider* GetEffectsBasedCooldownProvider() const;

private:
	void OnInstanceClassLoaded(const FInventoryItemData&, int, EEquipmentSource);

	UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	void OnActivationCompleted(bool outcome);

	void OnActivationPending(bool pending);

	UFUNCTION()
	void OnRep_ItemInstanceReplicated();

protected:
	UAbilitySystemComponent* GetOwnerAbilitySystemComponent() const;

	UPROPERTY(ReplicatedUsing = OnRep_CountReplicated, Transient)
	int Count = 0;

	UFUNCTION()
	void OnRep_CountReplicated();

	bool ShouldSpawnNewInstance(const FItemId& type) const;
private:
	mutable UAbilitySystemComponent* CachedAbilitySystemComponent = nullptr;

	UPROPERTY(ReplicatedUsing = OnRep_ItemInstanceReplicated, Transient)
	AItemInstance* Instance = nullptr;

	TOptional<FItemId> ClearInstance();

	UPROPERTY(ReplicatedUsing = OnRep_ItemMaterialSettingsReplicated, Transient)
	float EnchantmentMaterialPowerLevel = 0.0f;
	UPROPERTY(ReplicatedUsing = OnRep_ItemMaterialSettingsReplicated, Transient)
	FLinearColor ArmorPropertyMaterialColor = FLinearColor::White;


	bool bPendingActivation = false;

	bool bWantsToActivate = false;

	bool bWasOnCooldown = false;

	FDelegateHandle InstanceTypeHandle;

	void OnCooldownTagChange(const FGameplayTag tag, const int32 tagCount);

	UPROPERTY(EditDefaultsOnly, Instanced, meta = (ShowOnlyInnerProperties))
	UEffectBasedCooldownProvider* CooldownProvider;

	//Used to clear callbacks when onrep is called. Need another member as OnRep is called post change.
	UPROPERTY(transient)
	AItemInstance* _InstanceInternal = nullptr;

	TSharedPtr<FStreamableHandle> LoadingHandle;

	bool firstEquip = true;

	// D11.SSN - for cooldowns taht are reduce mid-cooldown
	// The number of seconds reduced by, plus the number of seconds elapsed since the cooldown initially started.
	UPROPERTY(ReplicatedUsing = OnRep_CooldownReduction)
	float cooldownReduction = 0.0f;

	UFUNCTION()
	void OnRep_CooldownReduction();
};


UCLASS()
class DUNGEONS_API UItemslotCooldownEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UItemslotCooldownEffect(const FObjectInitializer& ObjectInitializer);
};
