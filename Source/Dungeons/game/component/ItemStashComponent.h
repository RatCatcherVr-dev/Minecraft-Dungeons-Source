#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "game/inventory/InventoryItem.h"
#include "game/item/InventoryItemData.h"
#include "EnchantmentComponent.h"
#include "IDelegateInstance.h"
#include "game/item/drop/ItemDropData.h"
#include "game/item/ItemBulletPoint.h"
#include "game/item/salvage/ItemSalvageInfo.h"
#include "game/item/salvage/ItemSalvageUndoInfo.h"
#include "game/item/SerializableItemId.h"
#include "game/item/ItemCharacteristic.h"
#include "game/mission/offerings/MissionOfferings.h"
#include "ItemStashComponent.generated.h"

class UItemSlot;
class UItemStashComponent;
class AItemInstance;
class ABaseCharacter;

UENUM(BlueprintType)
enum class EItemPowerComparison : uint8 {
	NONE,
	Better,
	Same,
	Worse
};
ENUM_NAME(EItemPowerComparison);

UENUM(BlueprintType)
enum class EEquipmentSlot : uint8 {
	HotbarSlot1, 
	HotbarSlot2, 
	HotbarSlot3, 
	MeleeGear, 
	RangedGear,
	ArmorGear,
	Invalid
};
ENUM_NAME(EEquipmentSlot);

bool isHotbarSlot(EEquipmentSlot slot);

struct EquipmentSlotter {
	int HotBarCount = 0;

	EEquipmentSlot operator()(ESlotType);
};


/** Structure that defines a ItemTagImage table entry */
USTRUCT(BlueprintType)
struct FItemTagData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:

	FItemTagData()
		: TagType(ItemTag::Unset)
	{}

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	ItemTag TagType;
	
	/** Icon to use for Normal */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	TMap< ItemTagLevel, TSoftObjectPtr<UTexture> > TagLevelIconsIcon;	

};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSlotLockedChanged, bool, locked);
DECLARE_MULTICAST_DELEGATE(FItemSwapped);

UCLASS(BlueprintType)
class DUNGEONS_API UInventoryItemSlot  : public UObject {
	GENERATED_BODY()

private:
	int mChangeIndex = 0;
	void IncrementChangeIndex();
public:
	UPROPERTY(BlueprintReadOnly)
	ESlotType SlotType = ESlotType::Any;
	UPROPERTY(BlueprintReadOnly)
	UInventoryItem* Item = nullptr;
	
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	int GetChangeIndex() const;

	UPROPERTY()
	UInventoryItem* OldItem = nullptr;

	UItemStashComponent* ItemStashComponent = nullptr;

	UInventoryItem* GetItem() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	virtual bool AcceptsItem(const UInventoryItem* otherItem) const;
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool CanSwapWith(const UInventoryItemSlot* other) const;
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	virtual bool Swap(UInventoryItemSlot* other);
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	virtual bool IsLocked() const { return false; }
		
	virtual bool Store(UInventoryItem* otherItem, bool force = false);
	
	virtual bool Remove();

	static TArray< UTexture2D* >			s_IconInventoryTextures;
	static int								s_IconInventoryTexturesLoading;

	static TArray< TArray< UTexture2D* > >	s_IconTagTextures;
	static int								s_IconTagTexturesLoading;

	static void PreloadIconInventoryTextures();


	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	static UTexture2D* GetIconTextureForItemId(const FSerializableItemId& type);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static bool GetIconTexturesStillLoading();

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	static UTexture2D* GetIconTextureForItemTag(ItemTag type, ItemTagLevel level);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static ItemTag GetTagForSlotType(ESlotType type);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static bool GetTagIconsStillLoading();

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	virtual void WasSelectedInUI() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	virtual bool HasSlotChanged() const ;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	virtual void FinishedSlotChanged();

	bool IsNew() const;	

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnSlotLockedChanged OnSlotLockedChanged;	
	
	FItemSwapped OnItemSwapped;
};


UCLASS()
class DUNGEONS_API UInventoryEquipmentItemSlot : public UInventoryItemSlot {
	GENERATED_BODY()
	class UEnchantmentComponent *EnchantmentComponent;
public:
	bool Store(UInventoryItem* item, bool force = false) override;
	bool IsLocked() const override;
	bool Remove() override;

	void WasSelectedInUI() const override;
	void SetSlot(UItemSlot* Slot);

	bool Swap(UInventoryItemSlot* other) override;
	
private:
	bool StoreInternal(UInventoryItem* item, bool force);
	
	UFUNCTION()
	void OnCooldownChanged(UItemSlot* slot, bool shouldBeLocked);
	
	UItemSlot* Slot = nullptr;
	
	void Bind();
	FDelegateHandle DelegateHandle;
};


DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemPickup, const FInventoryItemData&);
DECLARE_MULTICAST_DELEGATE(FOnInventoryChanged);
DECLARE_MULTICAST_DELEGATE(FAvailableEnchantmentPointsChangedInternal);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemGifted, const FInventoryItemData&, item);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAvailableEnchantmentPointsChanged, int, NewAmount, UItemStashComponent*, ItemStashComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMissionOfferingsChanged, ELevelNames, mission);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNumNewItemsChanged);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnInventoryItemSlotSelected, const UInventoryItemSlot*);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryFull);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONS_API UItemStashComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	// #D11.CM - Maximum inventory size for each player.
	const static int MAX_INVENTORY_SIZE = 300;

	const static int INVENTORY_COLUMN_COUNT = 3;

	// Sets default values for this component's properties
	UItemStashComponent();

	// #D11.CM
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	static int GetMaxInventoryCount() { return MAX_INVENTORY_SIZE; };

	// #D11.CM
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool IsInventoryFull() const { return GetNumItemsInInventory() >= MAX_INVENTORY_SIZE; } ;

	// #D11.CM - Returns how many items the player currently has. This includes deferred items that haven't technically been added to the inventory yet.
	UFUNCTION(BlueprintCallable, Category = "Dungones")
	int GetNumItemsInInventory() const { return GetNumUsedSlots() + GetNumEquippedItems() + mDeferredInventoryItems.Num(); } ;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	int InventorySize() const;

	UFUNCTION(Server, Reliable, Category = "Dungeons", WithValidation)
	void ServerVendorGiftItemTo(APlayerCharacter* recipient, const FInventoryItemData & item);

	UFUNCTION(Client, Reliable, Category = "Dungeons")
	void ClientVendorGiftItem(const FInventoryItemData & item);

	void GiftItem(const FInventoryItemData & item);

	UFUNCTION(BlueprintCallable, Client, Reliable, Category = "Dungeons")
	void ClientGiftItem(const FInventoryItemData & item);

	UFUNCTION(BlueprintCallable, Client, Reliable, Category = "Dungeons")
	void ClientPickupItem(const FInventoryItemData & item);

	UFUNCTION(BlueprintCallable, Client, Reliable, Category = "Dungeons")
	void ClientAddItem(const FInventoryItemData & item);

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void EnterInventoryUI();

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool HasDeferredItems();

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void ExitInventoryUI();

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	UInventoryItemSlot* GetLowestPoweredItem();

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	TArray<FInventoryItemData> GetEquippedItemsOfSlotType(ESlotType type);

	// used for item salvage 
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool RemoveItem(UInventoryItemSlot* slot);

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	FItemSalvageUndoInfo SalvageItemInSlot(UInventoryItemSlot* slot, bool& success);

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool SalvageItemUndo(const FItemSalvageUndoInfo& undoInfo);

	UFUNCTION(BlueprintCallable, Category = "Dungeons", BlueprintPure)
	static FItemSalvageInfo GetSalvageInfo(UInventoryItem* item);
	
	void SalvageItemData(const FInventoryItemData& item);

	void ExecuteItemSalvageInfo(const FItemSalvageInfo& salvageInfo);

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void Expand(int newSize);

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void ShuffleItemsToFillHoles(int initialNumEmpty = 0);

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void SerializeSaveState();
	void DeserializeSaveState();

	bool ResetItemEnchantments(int index);
	bool SetItemEnchantments(int index, const TArray<FEnchantmentData>& newEnchantments);
	const UInventoryItem* GetInventoryItem(int index) const;

	const TArray<FInventoryItemData>& GetAsInventoryDataArray();

	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	void AddAllDeferredItems();

private:
	using ItemSlotMatchPredicate = std::function<bool(const UInventoryItemSlot&)>;

	int ShiftItemsToMatch(ItemSlotMatchPredicate predicate, int startingIndex = 0, int numLimit = -1);
	int SwapItemsToMatch(ItemSlotMatchPredicate predicate, int startingIndex = 0, int numLimit = -1);

	// D11.DH
	void IncrementDirty(int32 inValue);

	UInventoryItem* GetMutableInventoryItem(int index);
	
	template<typename F>
	UInventoryItem* GetInventoryItem(int index, F &createIterator) const;	

	UPROPERTY()
	TArray<FInventoryItemData> cachedInventoryData;
	void ClearCachedInventoryDataArray();

	bool			AddItem(const FInventoryItemData& item);
	void			AddInventoryItem(const FInventoryItemData& item, bool bSerialise = true);
	

	UPROPERTY()
	TArray< FInventoryItemData > mDeferredInventoryItems;

	//D11.JP
	int GetDesiredInventorySlotAmount();

	int mChangeIndex = 0;

public:
	void IncrementChangeIndex();

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	int GetChangeIndex() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	const TArray<UInventoryItemSlot*>& GetInventorySlots() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	const TMap<EEquipmentSlot, UInventoryItemSlot*>& GetEquipmentSlots() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool HasPreviouslyFoundItem(const FSerializableItemId& itemType) const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool HasPreviouslyFoundCurrency(const FSerializableItemId& itemType) const;

	void ItemSlotSelected(const UInventoryItemSlot*);

	bool IsItemInInventorySlots(const UInventoryItem* item) const;
	bool IsItemInEquipmentSlots(const UInventoryItem* item) const;
	bool HasItem(const UInventoryItem*) const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	EItemPowerComparison CompareItemPowerWithEquipped(const FInventoryItemData& compareItem) const;

	FOnInventoryItemSlotSelected OnInventoryItemSlotSelected;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnItemGifted OnItemGifted;
	
	FOnItemPickup OnItemPickupInternal;

	FOnInventoryChanged OnInventoryChanged;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FAvailableEnchantmentPointsChanged OnAvailableEnchangmentPointsChanged;
	
	FAvailableEnchantmentPointsChangedInternal OnAvailableEnchangmentPointsChangedInternal;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnNumNewItemsChanged OnNumNewItemsChanged;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	TMap<EEquipmentSlot, FInventoryItemData> DefaultItems;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnInventoryFull OnInventoryFull;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Audio)
	class USoundCue* PayoutSound;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool GetCanOpenWithKeyCommand() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool InventoryUIRequiresRefresh() const { return mInventoryUIRequiresRefresh; };

	void OnLocalPawnPossessed();

	int EnchantmentPointsGrantedFromExperience() const;
	int InvestedEnchantmentPointsTotal() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	int AvailableEnchantmentPoints() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	int GetNumNewItems() const;

	void RefreshNumNewItems();

	static int CalculateEnchantmentPointsGrantedByLevel(const int32 level);

	bool CanInvestEnchantmentPointInItemAtIndex(const FInventoryItemData& item, int index) const;
	bool InvestEnchantmentPointInItemAtIndex(FInventoryItemData& item, int index) const;
	void RefreshCachedPoints();
	void RefreshCachedPoints(int32 level);
	void DesireRefreshCachedPoints();
	
	UFUNCTION()
	void OnLevelUp(int32 newLevel);

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnMissionOfferingsChanged OnMissionOfferingsChanged;

	TOptional<FMissionOfferings> GetMaybeMissionOfferings(ELevelNames) const;	

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	FMissionOfferings GetMissionOfferings(ELevelNames mission) const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool HasMissionOfferings(ELevelNames mission) const;

	void SetMissionOfferings(ELevelNames, const FMissionOfferings&);
	void ClearMissionOfferings(ELevelNames);
	TOptional<FMissionOfferings> TryConsumeMissionOfferings(ELevelNames);

	bool IsItemBeingOffered(const UInventoryItem* item) const;

private:
	TSet<EEquipmentSlot> SlotsUsableByItems();

	int GetNumUsedSlots() const;
	int GetNumFreeSlots() const;
	int GetNumEquippedItems() const;

	bool GrantDefaultItems(TSet<EEquipmentSlot> existingSlots);
	void QueueSalvageSounds(int amount);

	int ClampToEvenColumns(int count) const;

	mutable int CachedGrantedPoints = 0;
	mutable int CachedInvestedPoints = 0;

	int InventoryDesiredFreeSlots = 12;

	int mSoundsQueue;
	float mDelay;

	bool mCachePointsRequireRefresh = false;
	bool mInventoryUIRequiresRefresh = true;

	TMap<ELevelNames, FMissionOfferings> mMissionOfferings;
	

	UPROPERTY()
	TArray<UInventoryItemSlot*> InventorySlots;

	UPROPERTY()
	TMap<EEquipmentSlot, UInventoryItemSlot*> EquipmentSlots;
};
