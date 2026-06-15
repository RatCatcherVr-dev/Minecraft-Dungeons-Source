#include "Dungeons.h"
#include "ItemStashComponent.h"
#include "DungeonsGameInstance.h"
#include "EnchantmentComponent.h"
#include "EquipmentComponent.h"
#include "DungeonsGameState.h"
#include "game/component/PlayerExperienceComponent.h"
#include "game/component/WalletComponent.h"
#include "game/component/CharacterSerializeComponent.h"
#include "game/component/CharacterLazySaveComponent.h"
#include "game/item/ItemType.h"
#include "game/item/ItemUtil.h"
#include "game/util/ValueFormat.h"
#include "game/item/generator/ItemGenerator.h"
#include "game/item/generator/ItemGeneratorConfigs.h"
#include "game/item/generator/ItemGeneratorLooterState.h"
#include "game/Enchantments/Enchantment.h"
#include "game/Enchantments/EnchantmentUtil.h"
#include "game/ArmorProperties/ArmorPropertyData.h"
#include "game/ArmorProperties/ArmorPropertyTypeDefs.h"
#include "game/item/salvage/ItemSalvageUtil.h"
#include "game/item/power/ItemPowerStats.h"
#include "game/item/power/ItemPowerUtil.h"
#include "game/mission/MissionDefs.h"
#include "game/mission/MissionDef.h"
#include "game/GameBP.h"
#include "Engine/AssetManager.h"
#include <vector>
#include "game/item/instance/AItemInstance.h"
#include "game/item/ItemFunctionLibrary.h"
#include "Assets/ItemAssetFinder.h"
#include "game/merchant/transaction/GiftWrapItem.h"
#include "game/inventory/InventoryItemUtil.h"
#include "game/actor/item/ItemEmitterActor.h"


using namespace game::item::type;

extern TAutoConsoleVariable<int32> CVarEnableOpenInventoryWithKey;

bool isHotbarSlot(EEquipmentSlot slot) {
	return slot >= EEquipmentSlot::HotbarSlot1 && slot <= EEquipmentSlot::HotbarSlot3;
}

ESlotType SlotTypeFromEquipmentSlot(EEquipmentSlot equipmentSlotType) {
	switch (equipmentSlotType) {
	case EEquipmentSlot::MeleeGear:
		return ESlotType::MeleeWeapon;
	case EEquipmentSlot::RangedGear:
		return ESlotType::RangedWeapon;
	case EEquipmentSlot::ArmorGear:
		return ESlotType::Armor;
	case EEquipmentSlot::HotbarSlot1:
	case EEquipmentSlot::HotbarSlot2:
	case EEquipmentSlot::HotbarSlot3:
		return ESlotType::ActivePermanent;
	default:
		return ESlotType::None;
	}
}

EEquipmentSlot EquipmentSlotter::operator()(ESlotType type) {
	switch (type) {
	case ESlotType::MeleeWeapon:
		return EEquipmentSlot::MeleeGear;
	case ESlotType::RangedWeapon:
		return EEquipmentSlot::RangedGear;
	case ESlotType::Armor:
		return EEquipmentSlot::ArmorGear;
	case ESlotType::ActivePermanent:
		++HotBarCount;
		if (HotBarCount == 1) {
			return EEquipmentSlot::HotbarSlot1;
		}
		if (HotBarCount == 2) {
			return EEquipmentSlot::HotbarSlot2;
		}
		if (HotBarCount == 3) {
			return EEquipmentSlot::HotbarSlot3;
		}
	default:
		return EEquipmentSlot::Invalid;
	}
}

namespace {
	int SlotScorer(const UInventoryItemSlot* slot) {
		if (slot->Item) {
			return UInventoryItemUtil::GetTotalInvestedEnchantmentPoints(slot->Item->Item);
		}
		return 0;
	}
}


int UInventoryItemSlot::GetChangeIndex() const {
	return mChangeIndex;
}

UInventoryItem* UInventoryItemSlot::GetItem() const {
	return Item;
}

bool UInventoryItemSlot::AcceptsItem(const UInventoryItem* item) const {
	if (IsLocked()) return false;
	
	if (!item || SlotType == ESlotType::Any) {
		return true;
	}
	return GetItemRegistry().Get(item->Item.GetItemId()).accepts(SlotType);
}

bool UInventoryItemSlot::CanSwapWith(const UInventoryItemSlot* other) const {
	return other && other != this && AcceptsItem(other->Item) && other->AcceptsItem(Item);
}

void UInventoryItemSlot::IncrementChangeIndex() {
	mChangeIndex++;
	if (ItemStashComponent) {
		ItemStashComponent->IncrementChangeIndex();
	}
}

bool UInventoryItemSlot::Store(UInventoryItem* item, bool force /* = true */) {
	if (force || AcceptsItem(item)) {
		Item = item;
		IncrementChangeIndex();
		return true;
	}
	
	return false;
}

bool UInventoryItemSlot::Remove() {
	if (Item) {
		Item = nullptr;
		IncrementChangeIndex();
		return true;
	}
	return false;
}

TArray< UTexture2D* > UInventoryItemSlot::s_IconInventoryTextures;

int UInventoryItemSlot::s_IconInventoryTexturesLoading = 1; //set to 1 to indicate he havent began loading yet

TArray< TArray< UTexture2D* > > UInventoryItemSlot::s_IconTagTextures;

int UInventoryItemSlot::s_IconTagTexturesLoading = 1; //set to 1 to indicate he havent began loading yet

bool UInventoryItemSlot::Swap(UInventoryItemSlot* other) {
	if (other != nullptr && CanSwapWith(other)) {
		UInventoryItem* tmpItem = Item;
		Store(other->Item);
		other->Store(tmpItem);
		OnItemSwapped.Broadcast();
		return true;
	}

	return false;
}

void UInventoryItemSlot::PreloadIconInventoryTextures()
{
	const auto& itemTypes = GetItemRegistry().GetValues();
	s_IconInventoryTextures.SetNumZeroed(itemTypes.Num());
	s_IconInventoryTexturesLoading = 0;
	
	for (int i(0); i < itemTypes.Num(); ++i)
	{
		auto& thisItemType = *itemTypes[i];

		FString IconPath = IDungeonsModule::Get().GetItemAssetFinder()->IconPath(thisItemType.getId()).Get(FSoftObjectPath()).ToString();
		
		if (IconPath.Len() > 0)
		{
			FSoftObjectPath SoftPath = IconPath;
			++s_IconInventoryTexturesLoading;

			UAssetManager::GetStreamableManager().RequestAsyncLoad(SoftPath,
				[i, SoftPath]() {

				s_IconInventoryTextures[i] = Cast<UTexture2D>(SoftPath.ResolveObject());
				if (s_IconInventoryTextures[i])
				{
					s_IconInventoryTextures[i]->AddToRoot();
				}
				--s_IconInventoryTexturesLoading;

			}
				, FStreamableManager::AsyncLoadHighPriority,
				false,
				false,
				TEXT("PreloadIconInventoryTextures"));
		}

	}

	//
	UDataTable* pDataTable = LoadObject<UDataTable>(NULL, TEXT("DataTable'/Game/UI/UIItemTagData.UIItemTagData'"), NULL, LOAD_None, NULL);;
	
	if (pDataTable)
	{
		s_IconTagTexturesLoading = 0;

		s_IconTagTextures.SetNum((int)ItemTag::Last);
		for (int i(0); i < (int)ItemTag::Last; ++i)
		{
			s_IconTagTextures[i].SetNumZeroed((int)ItemTagLevel::Last);
		}
				
		//read soft paths from data table
		pDataTable->ForeachRow<FItemTagData>(TEXT("FItemTagData"), [](const FName& Key, const FItemTagData& Value){

			int iItemType = (int)Value.TagType;

			for (auto TagImgs : Value.TagLevelIconsIcon)
			{
				int iItemLvl = (int)TagImgs.Key;
				FSoftObjectPath SoftPath = TagImgs.Value.ToSoftObjectPath();
				++s_IconTagTexturesLoading;
				//send async load for soft paths

				UAssetManager::GetStreamableManager().RequestAsyncLoad(SoftPath,
					[iItemType, iItemLvl, SoftPath]() {

					s_IconTagTextures[iItemType][iItemLvl] = Cast<UTexture2D>(SoftPath.ResolveObject());
					
					if (s_IconTagTextures[iItemType][iItemLvl])
					{
						s_IconTagTextures[iItemType][iItemLvl]->AddToRoot();
					}
					--s_IconTagTexturesLoading;

				}
					, FStreamableManager::AsyncLoadHighPriority,
					false,
					false,
					TEXT("PreloadIconTagTextures"));
			}
		});

	}

}

UTexture2D* UInventoryItemSlot::GetIconTextureForItemId(const FSerializableItemId& type) {
	const auto index = GetItemRegistry().IndexOf(type);
	if (s_IconInventoryTextures.IsValidIndex(index))
	{
		if (s_IconInventoryTextures[index])
		{
			return s_IconInventoryTextures[index];
		}
		else
		{
			//somehow, this has failed to load in this icon, lets try a static load this time
			s_IconInventoryTextures[index] = UItemFunctionLibrary::GetIconTextureForItemType(type);
		}
	}

	return UItemFunctionLibrary::GetIconTextureForItemType(type);
}

bool UInventoryItemSlot::GetIconTexturesStillLoading()
{
	return s_IconInventoryTexturesLoading != 0;
}

UTexture2D* UInventoryItemSlot::GetIconTextureForItemTag(ItemTag type, ItemTagLevel level)
{
	if (s_IconTagTextures.Num())
	{
		return s_IconTagTextures[(int)type][(int)level];
	}
	return nullptr;
}

ItemTag UInventoryItemSlot::GetTagForSlotType(ESlotType type) {	
	switch (type) {
	case ESlotType::ActivePermanent:
		return ItemTag::Item;
	case ESlotType::MeleeWeapon:
		return ItemTag::MeleeWeapon;
	case ESlotType::RangedWeapon:
		return ItemTag::RangedWeapon;
	case ESlotType::Armor:
		return ItemTag::Armor;
	default:
		return ItemTag::Unset;
	}	
}

bool UInventoryItemSlot::GetTagIconsStillLoading()
{
	return s_IconTagTexturesLoading != 0;
}


void UInventoryItemSlot::WasSelectedInUI() const {
	ItemStashComponent->ItemSlotSelected(this);
}

bool UInventoryItemSlot::HasSlotChanged() const
{
	return OldItem != Item;
}

void UInventoryItemSlot::FinishedSlotChanged()
{
	// D11.DH
	
	OldItem = Item;
}

bool UInventoryItemSlot::IsNew() const{
	if (Item) {
		return Item->IsNew();
	}
	return false;
}

bool UInventoryEquipmentItemSlot::Store(UInventoryItem* item, bool force /* = true */) {
	bool stored = StoreInternal(item, force);

	if(stored)
	{
		if (!Item) Slot->Clear();
		else Bind();
	}

	return stored;
}

bool UInventoryEquipmentItemSlot::StoreInternal(UInventoryItem* item, bool force)
{
	if (Item) {
		Item->OnItemChangedInternal.Remove(DelegateHandle);
	}


	bool stored = Super::Store(item, force);

	if (Item) {
		DelegateHandle = Item->OnItemChangedInternal.AddUObject(this, &UInventoryEquipmentItemSlot::Bind);
	}

	return stored;
}


bool UInventoryEquipmentItemSlot::IsLocked() const
{
	return Slot->IsOnCooldown();
}

bool UInventoryEquipmentItemSlot::Remove() {
	if (Item) {
		Slot->Clear();
	}

	return Super::Remove();
}

void UInventoryEquipmentItemSlot::WasSelectedInUI() const
{
	UInventoryItemSlot::WasSelectedInUI();
	Slot->OnWasSelectedInUIInternal.Broadcast(Slot);
}

void UInventoryEquipmentItemSlot::Bind() {
	Slot->EquipItem(Item->Item);
}

void UInventoryEquipmentItemSlot::OnCooldownChanged(UItemSlot* ,bool shouldBeLocked)
{
	OnSlotLockedChanged.Broadcast(shouldBeLocked);
}

void UInventoryEquipmentItemSlot::SetSlot(UItemSlot* slot)
{
	if(Slot)
	{
		Slot->OnItemSlotCooldownChanged.RemoveAll(this);
	}

	Slot = slot;
	SlotType = slot->SlotTypeId;
	Slot->OnItemSlotCooldownChanged.AddDynamic(this, &UInventoryEquipmentItemSlot::OnCooldownChanged);
}

bool UInventoryEquipmentItemSlot::Swap(UInventoryItemSlot* other)
{
	auto equipmentOther = Cast<UInventoryEquipmentItemSlot>(other);
	if (!equipmentOther) return Super::Swap(other);

	if(CanSwapWith(other))
	{
		Slot->Swap(equipmentOther->Slot);
		auto tmpItem = Item;
		StoreInternal(equipmentOther->Item, false);
		equipmentOther->StoreInternal(tmpItem, false);
		OnItemSwapped.Broadcast();
		return true;
	}

	return false;
}

//D11.PS - changed to float
//static int s_delay = 0.1; //seconds
static float s_delay = 0.0; //seconds

// Sets default values for this component's properties
UItemStashComponent::UItemStashComponent()
	: mDelay(s_delay)
	, mSoundsQueue(0)
{
	//We will need tick later when closing re-usable chests
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}


void UItemStashComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	bool bDisableTick = true;

	if (PayoutSound) {
		bDisableTick = false;
	}
	if (mSoundsQueue > 0)
	{
		bDisableTick = false;
		if (mDelay < 0)
		{
			UGameplayStatics::PlaySoundAtLocation(this, PayoutSound, GetOwner()->GetActorLocation());
			mSoundsQueue -= 1;
			mDelay = s_delay;
		}
		else
			mDelay -= DeltaTime;
	}

	if (mCachePointsRequireRefresh)
	{
		RefreshCachedPoints();
	}

	if (bDisableTick)
	{
		SetComponentTickEnabled(false);
	}

}


int UItemStashComponent::GetChangeIndex() const {
	return mChangeIndex;
}

bool UItemStashComponent::HasPreviouslyFoundItem(const FSerializableItemId& itemType) const {
	if (const auto* characterSave = GetOwner()->FindComponentByClass<UCharacterSerializeComponent>()) {
		return characterSave->ReadItemsFound().Contains(itemType);
	}

	return false;
}

bool UItemStashComponent::HasPreviouslyFoundCurrency(const FSerializableItemId& itemType) const {
	if (const auto* characterSave = GetOwner()->FindComponentByClass<UCharacterSerializeComponent>()) {
		return characterSave->ReadCurrenciesFound().Contains(itemType);
	}

	return false;
}

void UItemStashComponent::ItemSlotSelected(const UInventoryItemSlot* inventoryItemSlot) {
	OnInventoryItemSlotSelected.Broadcast(inventoryItemSlot);
}

bool UItemStashComponent::IsItemInInventorySlots(const UInventoryItem* item) const {
	return algo::any_of(InventorySlots, RETLAMBDA(it->Item == item));
}

bool UItemStashComponent::IsItemInEquipmentSlots(const UInventoryItem* item) const {
	for (const auto& pair : EquipmentSlots) {
		if (pair.Value->Item == item) {
			return true;
		}
	}
	return false;
}

bool UItemStashComponent::HasItem(const UInventoryItem* item) const {
	return IsItemInInventorySlots(item) || IsItemInEquipmentSlots(item);
}

EItemPowerComparison UItemStashComponent::CompareItemPowerWithEquipped(const FInventoryItemData& compareItem) const {
	const auto& compareItemType = GetItemRegistry().Get(compareItem.GetItemId());

	auto slots = [&](const FInventoryItemData& compareItem) {
		TArray<UInventoryItemSlot*> slots;
		const auto compareSlotType = GetItemRegistry().Get(compareItem.GetItemId()).getSlotType();
		for (auto slot : EquipmentSlots) {
			if (SlotTypeFromEquipmentSlot(slot.Key) == compareSlotType) {
				slots.Add(slot.Value);
			}
		}
		return slots;
	};
	
	auto lowestEquippedPower = 0.f;
	bool first = true;
	for (auto slot : slots(compareItem)) {
		const auto* item = slot->Item;
		const auto testPower = item ? item->Item.ItemPower : 0.f;
		lowestEquippedPower = first || (testPower < lowestEquippedPower) ? testPower : lowestEquippedPower;
		first = false;
	}

	const auto comparePower = compareItem.ItemPower;
	const auto equippedPower = lowestEquippedPower;

	if (comparePower > equippedPower) {
		return EItemPowerComparison::Better;
	}
	if (comparePower < equippedPower) {
		return EItemPowerComparison::Worse;
	}
	return EItemPowerComparison::Same;
}

bool UItemStashComponent::GetCanOpenWithKeyCommand() const {
	if (CVarEnableOpenInventoryWithKey.GetValueOnGameThread() <= 0) {


		const auto game = actorquery::getFirstActor<AGameBP>(GetWorld());

		if (!game) return true;

		if (auto state = Cast<ADungeonsGameState>(GetWorld()->GetGameState())) {
			if (state->IsLobby()) {
				return true;
			}
		}		
	}

	return CVarEnableOpenInventoryWithKey.GetValueOnGameThread() <= 0 ? false : true;
}

// Called when the game starts
void UItemStashComponent::OnLocalPawnPossessed()
{
	EquipmentSlots.Empty();
	InventorySlots.Empty();	

	if (const UEquipmentComponent* equipmentComponent = GetOwner()->FindComponentByClass<UEquipmentComponent>()) {
		TArray<UItemSlot*> slots(equipmentComponent->GetSlotsOfType(ESlotType::Any));

		EquipmentSlotter slotter;

		for (UItemSlot* slot : slots) {
			EEquipmentSlot slotType = slotter(slot->SlotTypeId);
			if (slotType != EEquipmentSlot::Invalid && !EquipmentSlots.Contains(slotType)) {
				auto slotObject = NewObject<UInventoryEquipmentItemSlot>();
				slotObject->SetSlot(slot);
				slotObject->ItemStashComponent = this;
				EquipmentSlots.Add(slotType, slotObject);

				slotObject->OnItemSwapped.AddUObject(this, &UItemStashComponent::DesireRefreshCachedPoints);
			}
		}
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("equipment componentn missing on owner"));
	}

	DeserializeSaveState();
	if (GrantDefaultItems(SlotsUsableByItems())) {
		SerializeSaveState();
	}

	if (auto playerExperienceComponent = GetOwner()->FindComponentByClass<UPlayerExperienceComponent>()) {
		playerExperienceComponent->OnLevelUp.AddDynamic(this, &UItemStashComponent::OnLevelUp);
	}

	RefreshCachedPoints();
	RefreshNumNewItems();
	IncrementChangeIndex();
}

void UItemStashComponent::OnLevelUp(const int32 newLevel) {
	if (GetOwner()->GetGameInstance()->IsDedicatedServerInstance()) {
		return;
	}
	
	RefreshCachedPoints(newLevel);
}

TOptional<FMissionOfferings> UItemStashComponent::GetMaybeMissionOfferings(ELevelNames LevelName) const {
	if (HasMissionOfferings(LevelName)){
		return mMissionOfferings[LevelName];
	}
	return {};
}

FMissionOfferings UItemStashComponent::GetMissionOfferings(ELevelNames LevelName) const {
	return GetMaybeMissionOfferings(LevelName).Get({});
}

bool UItemStashComponent::HasMissionOfferings(ELevelNames LevelName) const {
	return mMissionOfferings.Contains(LevelName);
}

void UItemStashComponent::SetMissionOfferings(ELevelNames LevelName, const FMissionOfferings& missionOfferings) {	
	checkf(missions::get(LevelName).isHyperMission(), TEXT("We are setting mission offerings of a non-hypermission, this does not match our current design"));
	if (!missions::get(LevelName).isHyperMission()) {
		return;
	}

	mMissionOfferings.Add(LevelName, missionOfferings);
	OnMissionOfferingsChanged.Broadcast(LevelName);
}

void UItemStashComponent::ClearMissionOfferings(ELevelNames LevelName) {
	if (mMissionOfferings.Remove(LevelName)) {
		OnMissionOfferingsChanged.Broadcast(LevelName);
	}
}

TOptional<FMissionOfferings> UItemStashComponent::TryConsumeMissionOfferings(ELevelNames LevelName) {
	if (auto offering = GetMaybeMissionOfferings(LevelName)) {

		checkf(missions::get(LevelName).isHyperMission(), TEXT("We are now consuming mission offerings of a non-hypermission, this does not match our current design"));
		if (!missions::get(LevelName).isHyperMission()) {
			return {};
		}

		//validate whole offering - this can fail.
		if (!algo::any_of(offering->offeredItems, RETLAMBDA(IsItemInInventorySlots(it)))) {
			return {};
		}
		if (offering->offeredEnchantmentPoints > 0 && offering->offeredEnchantmentPoints > AvailableEnchantmentPoints()) {
			return {};
		}

		//Consume whole offering - this should be considered a transaction and we should no longer allow failure:
		for (const auto* item : offering->offeredItems) {
			auto slot = InventorySlots.FindByPredicate(RETLAMBDA(it->Item == item));
			checkf(slot, TEXT("could not find slot while consuming mission offering, failed to consume item"));
			if(slot){
				auto success = RemoveItem(*slot);
				checkf(success, TEXT("could not remove item while consuming mission offering, failed to consume item"));
			}
		}

		if(offering->offeredEnchantmentPoints > 0){
			if (auto experienceComponent = GetOwner()->FindComponentByClass<UPlayerExperienceComponent>()) {
				auto success = experienceComponent->ConsumeLevels_OnlyFromOwningClient(offering->offeredEnchantmentPoints);
				checkf(success, TEXT("Failed to consume offered enchantment points"));
			} else {
				checkf(offering->offeredEnchantmentPoints <= 0, TEXT("UPlayerExperienceComponent needed to consume enchantment points"));
			}
		}

		mMissionOfferings.Remove(LevelName);
		OnMissionOfferingsChanged.Broadcast(LevelName);
		return offering;
	}
	return {};
}

bool UItemStashComponent::IsItemBeingOffered(const UInventoryItem* item) const {
	for (auto& pair : mMissionOfferings) {
		if (algo::any_of(pair.Value.offeredItems, RETLAMBDA(it == item))) {
			return true;
		}
	}
	return false;
}

void UItemStashComponent::RefreshNumNewItems() {
	OnNumNewItemsChanged.Broadcast();
}

TSet<EEquipmentSlot> UItemStashComponent::SlotsUsableByItems() {

	TSet<EEquipmentSlot> seenSlots;

	for (const auto& entry : EquipmentSlots) {
		if (entry.Value->Item) {
			seenSlots.Add(entry.Key);
		}
	}

	for (const auto& slot : InventorySlots) {
		if (slot->Item) {
			for (const auto& entry : EquipmentSlots) {
				if (entry.Value->AcceptsItem(slot->Item)) {
					seenSlots.Add(entry.Key);
				}
			}
		}
	}

	return seenSlots;
}

bool UItemStashComponent::GrantDefaultItems(TSet<EEquipmentSlot> existingSlots) {
	bool itemsGranted = false;

	for (const auto& entry : DefaultItems) {
		if (!existingSlots.Contains(entry.Key)) {
			UInventoryItemSlot* slot = nullptr;
			if (EquipmentSlots.Contains(entry.Key) && EquipmentSlots[entry.Key]->Item == nullptr) {
				slot = EquipmentSlots[entry.Key];
			}
			else if(auto it = InventorySlots.FindByPredicate([](const UInventoryItemSlot* v) { return v->Item == nullptr; })) {
				slot = *it;
			}

			const auto config = itemgen::configs::Item(entry.Value.GetItemId(), entry.Value.ItemPower);
			
			UInventoryItem* inventoryItem = NewObject<UInventoryItem>();
			inventoryItem->ItemStashComponent = this;
			inventoryItem->Item = itemgen::generate({}, itemgen::looters::Unknown(), config).GetValue();
			if (inventoryItem->Item.Enchantments.Num() > 3) {
				inventoryItem->Item.Enchantments.SetNum(3);
			}

			if (!slot) {				
				slot = NewObject<UInventoryItemSlot>();
				slot->ItemStashComponent = this;
				InventorySlots.Add(slot);

				slot->Item = inventoryItem;
			} else {
				slot->Store(inventoryItem);
			}
			itemsGranted = true;
		}
	}

	
	return itemsGranted;
}

void UItemStashComponent::DeserializeSaveState()
{
	const auto gameInstance = Cast<UDungeonsGameInstance>(GetOwner()->GetGameInstance());
	if (gameInstance->IsDedicatedServerInstance()) return;

	InventorySlots.Empty();
	//Empty equipment slots too!
	for (auto& entry : EquipmentSlots) {
		entry.Value->Store(nullptr);
	}

	mDeferredInventoryItems.Reset();

	// copy for sorting
	UCharacterSerializeComponent* serializer = GetOwner()->FindComponentByClass<UCharacterSerializeComponent>();
	if (!serializer) {
		UE_LOG(LogTemp, Error, TEXT("Parent cant deserialize, cant load items"));
		return;
	}
	// D11.DH
	// Fix for attempt to read null save data when signing out during the victory countdown
	uint32 numItems = 0;
	std::vector<ItemSaveData> itemsCopy;
	if (serializer->HasProfile())
	{
		serializer->GetItemsCopy(itemsCopy);
		numItems = itemsCopy.size();
	}
	if (numItems) {
		std::sort(itemsCopy.begin(), std::partition(itemsCopy.begin(), itemsCopy.end(), [](const ItemSaveData& item) { return !item.Equipped(); }), [](const ItemSaveData& item1, const ItemSaveData& item2) {
			return item1.GetIndex() > item2.GetIndex();
		});

		//Clamp inventory slots to a certain limit over number of items in inventory to not allocate too many empty slots.
		auto highestSlottedItem = itemsCopy.begin();
		uint32 highestIndex = highestSlottedItem->equipped ? numItems : Math::max(highestSlottedItem->GetIndex() + 1, numItems);
		uint32 calculatedNumItems = Math::max(numItems, highestIndex);

		// #D11.CM - Cap our save slots to the max inventory size
		int slotsToAllocate = Math::min(MAX_INVENTORY_SIZE, ClampToEvenColumns(int(calculatedNumItems + InventoryDesiredFreeSlots)));

		Expand(slotsToAllocate);
	
		for (const auto& item : itemsCopy) {
			UInventoryItemSlot* slot = nullptr;

			if (!item.equipped) {
				if (item.GetIndex() < static_cast<uint32>(InventorySlots.Num())) {
					slot = InventorySlots[item.GetIndex()];
				}
				else {
					auto it = InventorySlots.FindByPredicate([](const UInventoryItemSlot* v) { return v->Item == nullptr; });
					check(it && "Should have allocated enough slots");
					slot = *it;
				}
			}
			else {
				if (EquipmentSlots.Contains(item.GetEquipmentSlot()) && !EquipmentSlots[item.GetEquipmentSlot()]->Item) {
					slot = EquipmentSlots[item.GetEquipmentSlot()];
				}
				else {
					auto it = InventorySlots.FindByPredicate([](const UInventoryItemSlot* v) { return v->Item == nullptr; });
					check(it && "Should have allocated enough slots");
					slot = *it;
				}
			}
			
			check(slot && "A slot must have been found...");

			UInventoryItem* inventoryItem = NewObject<UInventoryItem>();
			inventoryItem->ItemStashComponent = this;
			inventoryItem->Meta.MarkedNew = item.markedNew;			
			inventoryItem->Meta.Cloned = item.cloned;
			TArray<FEnchantmentData> enchantmentData;
			for (auto enchantmentSaveData : item.enchantments) {
				enchantmentData.Emplace(enchantmentSaveData.id, enchantmentSaveData.level);
			}
			TArray<FArmorPropertyData> propertyData;
			for (auto armorPropertySaveData : item.armorProperties) {
				propertyData.Emplace(FArmorPropertyData( armorPropertySaveData.id, armorPropertySaveData.rarity) );
			}
			TOptional<FEnchantmentData> netheriteEnchant = item.netheriteEnchant ? FEnchantmentData(item.netheriteEnchant->id, item.netheriteEnchant->level) : TOptional<FEnchantmentData>();
			inventoryItem->Item = FInventoryItemData(item.type, item.power, enchantmentData, netheriteEnchant, propertyData, item.rarity, item.upgraded, item.gifted);

			slot->Store(inventoryItem);
		}
	}
	else
	{
		//make sure we have empty slots even if no items are in them
		Expand(GetDesiredInventorySlotAmount());
	}

	ClearCachedInventoryDataArray();
	mInventoryUIRequiresRefresh = true;
}

int UItemStashComponent::InventorySize() const
{
	return InventorySlots.Num();
}

bool UItemStashComponent::ServerVendorGiftItemTo_Validate(APlayerCharacter* recipient, const FInventoryItemData & item) {
	return true;
}

void UItemStashComponent::ServerVendorGiftItemTo_Implementation(APlayerCharacter* recipient, const FInventoryItemData & item) {
	if (!recipient) {
		return;
	}
	if (auto itemStash = recipient->GetItemStashComponent()) {
		itemStash->ClientVendorGiftItem(item);
	}
}

void UItemStashComponent::ClientVendorGiftItem_Implementation(const FInventoryItemData & item) {
	auto itemCopy = item;
	if(UGiftWrapItem::rerollItemPower_LocalOnly(GetOwner(), itemCopy)){
		ClientGiftItem(itemCopy);
	}
}

int UItemStashComponent::GetNumUsedSlots() const {
	int count = 0;

	for (const auto& slot : InventorySlots) {
		if (slot->Item != nullptr) ++count;
	}

	return count;

}
int UItemStashComponent::GetNumFreeSlots() const {
	int count = 0;

	for (const auto& slot : InventorySlots) {
		if (!slot->Item) ++count;
	}

	return count;
}

int UItemStashComponent::GetNumEquippedItems() const {
	int count = 0;

	for (const auto& it : EquipmentSlots) {
		if (it.Value->Item) {
			++count;
		}
	}
	return count;
}

int UItemStashComponent::ShiftItemsToMatch(ItemSlotMatchPredicate predicate, int startingIndex, int numLimit)
{
	int numShifted = 0;
	for (int i = startingIndex; i < InventorySlots.Num() - 1; i++) {		
		auto putSlot = InventorySlots[i];
		if (!predicate(*putSlot)) {
			for (int j = i+1; j < InventorySlots.Num(); j++) {
				auto seekSlot = InventorySlots[j];
				if (predicate(*seekSlot)) {
					for (int k = j; k > i; k--) {
						auto swapSlotA = InventorySlots[k-1];
						auto swapSlotB = InventorySlots[k];
						swapSlotA->Swap(swapSlotB);
					}
					break;
				}
			}
			numShifted++;
			if (numLimit >= 0 && numShifted >= numLimit) {
				break;
			}
		}
	}
	return numShifted;
}

int UItemStashComponent::SwapItemsToMatch(ItemSlotMatchPredicate predicate, int startingIndex, int numLimit)
{
	int putIndex = startingIndex;
	int lookIndex = startingIndex + 1;
	int suffledCount = 0;
	for (int i = startingIndex; i < InventorySlots.Num() - 1; i++) {
		if (numLimit >= 0 && suffledCount >= numLimit) {
			//limit exceeded
			break;
		}

		auto putSlot = InventorySlots[putIndex];

		if ( !predicate(*putSlot) ) {
			//Found non-matching slot
			auto lookSlot = InventorySlots[lookIndex];
			if ( predicate(*lookSlot) ) {
				//found item to move, try to swap
				putSlot->Swap(lookSlot);
				suffledCount++;
			}
		}

		if (predicate(*putSlot)) {
			putIndex++;
			lookIndex++;
			//If put index is blocked, move both put and look forward
		}
		else {
			lookIndex++;
			//We are looking for a candidate to put in the put index..
		}
	}
	return suffledCount;
}

void UItemStashComponent::ShuffleItemsToFillHoles(int initialNumEmpty)
{ 

	static auto slotIsEmpty = [](const UInventoryItemSlot& slot) -> bool {
		return slot.Item == nullptr;
	};

	static auto slotHasItem = [](const UInventoryItemSlot& slot) -> bool {
		return slot.Item != nullptr;
	};

	int index = 0;

	index += initialNumEmpty > 0 ? ShiftItemsToMatch(slotIsEmpty, index, initialNumEmpty) : 0;
	SwapItemsToMatch(slotHasItem, index);
}

DECLARE_CYCLE_STAT(TEXT("UItemStashComponent::AddItem_"), UItemStashComponent_AddItem_, STATGROUP_PlayerController);
DECLARE_CYCLE_STAT(TEXT("UItemStashComponent::AddItem_Expand"), UItemStashComponent_AddItem_Expand, STATGROUP_PlayerController);
DECLARE_CYCLE_STAT(TEXT("UItemStashComponent::AddItem_FirstSlot"), UItemStashComponent_AddItem_FirstSlot, STATGROUP_PlayerController);
DECLARE_CYCLE_STAT(TEXT("UItemStashComponent::AddItem_Store"), UItemStashComponent_AddItem_Store, STATGROUP_PlayerController);
DECLARE_CYCLE_STAT(TEXT("UItemStashComponent::AddItem_SerializeSaveState"), UItemStashComponent_AddItem_SerializeSaveState, STATGROUP_PlayerController);
DECLARE_CYCLE_STAT(TEXT("UItemStashComponent::AddItem_OnNumNewItemsChanged"), UItemStashComponent_AddItem_OnNumNewItemsChanged, STATGROUP_PlayerController);



bool UItemStashComponent::AddItem(const FInventoryItemData& item) 
{
	SCOPE_CYCLE_COUNTER(UItemStashComponent_AddItem_);

	if (IsInventoryFull()) return false;

	mDeferredInventoryItems.Push(item);
	mInventoryUIRequiresRefresh = true;
	{
		// mark item found as found
		UCharacterSerializeComponent* characterSave = GetOwner()->FindComponentByClass<UCharacterSerializeComponent>();
		if (characterSave) {
			TSet<FItemId>& itemsFoundCopy = characterSave->GetItemsFound();
			itemsFoundCopy.Add(item.GetItemId());
		}
	}
		
	{
		SCOPE_CYCLE_COUNTER(UItemStashComponent_AddItem_SerializeSaveState);
		SerializeSaveState();
	}

	if (auto gameInstance = Cast<UDungeonsGameInstance>(GetWorld()->GetGameInstance())) {
		analytics::Analytics::GetInstance().FireItemAcquired(*Cast<APlayerCharacter>(GetOwner()), item);
	}

	// D11.BC
	if (const auto* character = Cast<APlayerCharacter>(GetOwner())) {
		if (auto* tracker = character->GetStatTracker()) {
			tracker->GotItem(item.GetItemId());
		}
	}
	return true;
}

void UItemStashComponent::AddInventoryItem(const FInventoryItemData& item, bool bSerialise)
{

	auto inventoryItem = NewObject<UInventoryItem>();
	inventoryItem->ItemStashComponent = this;
	inventoryItem->Item = item;
	inventoryItem->Meta = { true }; //Marked 'New' in inventory

	Expand(GetDesiredInventorySlotAmount());

	//Algo assumed that there is at least 1 slot free somewhere
	//Because of the above expand.
	auto firstSlot = InventorySlots[0];
	if (firstSlot->Item != nullptr) {
		SCOPE_CYCLE_COUNTER(UItemStashComponent_AddItem_FirstSlot);
		//Start at beginning, find first free index
		int foundFree = -1;
		for (int i = 0; i < InventorySlots.Num(); ++i) {
			if (InventorySlots[i]->Item == nullptr) {
				foundFree = i;
				break;
			}
		}

		if (foundFree >= 0) {
			//Iterate from first free, and swap backwards.
			for (int i = foundFree; i > 0; --i) {
				auto targetSlot = InventorySlots[i];
				auto fetchSlot = InventorySlots[i - 1];
				if (targetSlot->Item == nullptr && fetchSlot->Item != nullptr) {
					//Only swap in one direction.
					targetSlot->Swap(fetchSlot);
				}
			}

			//Should have a free slot at index 0 now.
			DEBUG_ASSERT(firstSlot->Item == nullptr, "algorithm is wrong!, index 0 should be free");
		}
	}

	auto position = InventorySlots.FindByPredicate([](const UInventoryItemSlot* v) { return v->Item == nullptr; });

	if (position) {
		SCOPE_CYCLE_COUNTER(UItemStashComponent_AddItem_Store);
		(*position)->Store(inventoryItem);
	}

	if (bSerialise)
	{
		{
			SCOPE_CYCLE_COUNTER(UItemStashComponent_AddItem_SerializeSaveState);
			SerializeSaveState();
		}
		{
			SCOPE_CYCLE_COUNTER(UItemStashComponent_AddItem_OnNumNewItemsChanged);
			OnNumNewItemsChanged.Broadcast();
		}
	}

	inventoryItem->Item.OnWasAdded(inventoryItem);
}

void UItemStashComponent::AddAllDeferredItems()
{
	if (mDeferredInventoryItems.Num())
	{
		mInventoryUIRequiresRefresh = true;
		Expand(GetDesiredInventorySlotAmount());

		for (auto& val : mDeferredInventoryItems)
		{
			AddInventoryItem(val, false);
		}

		{
			SCOPE_CYCLE_COUNTER(UItemStashComponent_AddItem_SerializeSaveState);
			SerializeSaveState();
		}

		{
			SCOPE_CYCLE_COUNTER(UItemStashComponent_AddItem_OnNumNewItemsChanged);
			OnNumNewItemsChanged.Broadcast();
		}


		mDeferredInventoryItems.Reset();
	}
}

void UItemStashComponent::GiftItem(const FInventoryItemData & item) {
	const auto success = AddItem(item);
	if (success) {
		OnItemGifted.Broadcast(item);
	}
	else {
		
		TArray<FInventoryItemData> sourceItemList = { item };
		AItemEmitterActor::SpawnSimpleItemEmitter(GetWorld(), sourceItemList, GetOwner()->GetActorLocation(), GetOwner(), true, true, false, FItemEmitterActorCompleteDelegate::CreateLambda([](TArray< AStorableItem* > & SpawnedItems) {

			for (auto item : SpawnedItems)
			{
				item->ApplyDropEffect(nullptr);
			}
		}));
	}
}

void UItemStashComponent::ClientGiftItem_Implementation(const FInventoryItemData& item) {
	GiftItem(item);
}

void UItemStashComponent::ClientAddItem_Implementation(const FInventoryItemData& item) {
	AddItem(item);
}

void UItemStashComponent::ClientPickupItem_Implementation(const FInventoryItemData& item) {
	const auto inventoryItem = AddItem(item);
	if (inventoryItem && OnItemPickupInternal.IsBound()) {
		OnItemPickupInternal.Broadcast(item);
	}
	IncrementDirty(10);
}

const UInventoryItem* UItemStashComponent::GetInventoryItem(const int index) const {
	const auto createIterator = [=]() {
		return EquipmentSlots.CreateConstIterator();
	};

	return GetInventoryItem(index, createIterator);
}

void UItemStashComponent::IncrementDirty(int32 inValue)
{
	auto* lazySaver = GetOwner()->FindComponentByClass<UCharacterLazySaveComponent>();
	if (!lazySaver) {
		UE_LOG(LogTemp, Error, TEXT("missing serializer, failed to save item stash"));
		return;
	}

	if (GetOwner()->GetGameInstance()->IsDedicatedServerInstance()) {
		return;
	}

	lazySaver->DirtyItems(inValue);
}

UInventoryItem* UItemStashComponent::GetMutableInventoryItem(const int index) {
	const auto createIterator = [=]() {
		return EquipmentSlots.CreateIterator();
	};

	return GetInventoryItem(index, createIterator);
}

template <typename F>
UInventoryItem* UItemStashComponent::GetInventoryItem(const int index, F& createIterator) const {
	UInventoryItem* item = nullptr;
	
	if (index > -1) {
		if (index < EquipmentSlots.Num()) {
			auto It = createIterator();
			auto i = 0;
			while (i < index) {
				++i;
				++It;
			}

			item = It->Value->Item;
		}
		else if (index - EquipmentSlots.Num() < InventorySlots.Num()) {
			const auto slot = InventorySlots[index - EquipmentSlots.Num()];
			item = slot->Item;
		}
	}

	return item;
}

bool UItemStashComponent::SetItemEnchantments(const int index, const TArray<FEnchantmentData>& newEnchantments) {
	if (auto item = GetMutableInventoryItem(index)) {
		item->Item.Enchantments = newEnchantments;
		SerializeSaveState();
		return true;
	}
	return false;
}

bool UItemStashComponent::ResetItemEnchantments(const int index) {
	auto success = false;

	if (auto item = GetMutableInventoryItem(index)) {
		auto& enchantments = item->Item.Enchantments;
		for (auto& enchantment : enchantments) {
			enchantment.Level = 0;
		}
			
		success = enchantments.Num() > 0;
	}

	SerializeSaveState(); // updates inventory pane
	return success;
}

void UItemStashComponent::QueueSalvageSounds(const int amount) {
	mSoundsQueue = amount;
	SetComponentTickEnabled(true);
}

int UItemStashComponent::ClampToEvenColumns(int count) const {
	const auto rest = count % INVENTORY_COLUMN_COUNT;
	return count - rest;
}

void UItemStashComponent::EnterInventoryUI()
{
	AddAllDeferredItems();
	mInventoryUIRequiresRefresh = false;
}

bool UItemStashComponent::HasDeferredItems()
{
	return mDeferredInventoryItems.Num() > 0;
}

void UItemStashComponent::ExitInventoryUI()
{
	SerializeSaveState();
}

UInventoryItemSlot* UItemStashComponent::GetLowestPoweredItem() {
	UInventoryItemSlot* returnSlot = nullptr;

	if (InventorySlots.Num() > 0) {
		 returnSlot = InventorySlots[0];
		for (auto slot : InventorySlots) {
			if (slot->Item && slot->Item->GetItemPower() < returnSlot->Item->GetItemPower()) {
				returnSlot = slot;
			}
		}
	}

	return returnSlot;
}

TArray<FInventoryItemData> UItemStashComponent::GetEquippedItemsOfSlotType(ESlotType type)
{
	TArray<FInventoryItemData> items;
	for (auto equipSlot : EquipmentSlots) {
		if (equipSlot.Value->SlotType == type) {
			if (UInventoryItem* item = equipSlot.Value->Item) {
				items.Add(item->Item);
			}
		}
	}

	return items;
}

bool UItemStashComponent::RemoveItem(UInventoryItemSlot* slot) {
	if(slot->Remove()){

		RefreshNumNewItems();
		RefreshCachedPoints();
		SerializeSaveState();
		return true;
	}
	return false;
}



FItemSalvageInfo UItemStashComponent::GetSalvageInfo(UInventoryItem* item) {
	return UItemSalvageUtil::GetSalvageItemInfo(item->Item, item->IsCloned() ? 0.0f : 1.0f);
}

void UItemStashComponent::SalvageItemData(const FInventoryItemData& item) {
	ExecuteItemSalvageInfo(UItemSalvageUtil::GetSalvageItemInfo(item));
}

void UItemStashComponent::ExecuteItemSalvageInfo(const FItemSalvageInfo& salvageInfo)
{
	int maxGain = 0;
	if (UWalletComponent* wallet = GetOwner()->FindComponentByClass<UWalletComponent>()) {
		for (auto gain : salvageInfo.currencies) {
			wallet->ClientAdd(gain.Key, gain.Value);
			maxGain = FMath::Max(maxGain, gain.Value);
		}
	}
	
	// D11.DH
	IncrementDirty(10);

	QueueSalvageSounds(maxGain);
	RefreshCachedPoints();	
}

bool UItemStashComponent::SalvageItemUndo(const FItemSalvageUndoInfo& undoInfo) {
	if (undoInfo.slot && !undoInfo.slot->Item && undoInfo.item){
		if (UWalletComponent* wallet = GetOwner()->FindComponentByClass<UWalletComponent>()) {
			for (auto gain : undoInfo.salvageInfo.currencies) {
				if (wallet->Balance(gain.Key) < gain.Value) {
					//We don't have enough
					return false;
				}
			}
			for (auto gain : undoInfo.salvageInfo.currencies) {
				wallet->Deduct(gain.Key, gain.Value);
				// D11.SSN
				const auto* character = Cast<APlayerCharacter>(GetOwner());
				if (auto* tracker = character->GetStatTracker()) {
					tracker->GotCurrency(gain.Key, -gain.Value);
				}
			}
		}

		undoInfo.slot->Store(undoInfo.item, true);
		RefreshCachedPoints();

		return true;
	}

	return false;
}

FItemSalvageUndoInfo UItemStashComponent::SalvageItemInSlot(UInventoryItemSlot* slot, bool& success) {
	if (const auto item = slot->Item) {

		auto salvageInfo = GetSalvageInfo(slot->Item);
		ExecuteItemSalvageInfo(salvageInfo);
		RemoveItem(slot);
		// D11.DH
		IncrementDirty(10);		
		RefreshCachedPoints();
		success = true;

		return { slot, item, salvageInfo };
	}
	success = false;
	return {};
}

void UItemStashComponent::Expand(int newSize)
{
	SCOPE_CYCLE_COUNTER(UItemStashComponent_AddItem_Expand);

	if (InventorySlots.Num() < newSize) 
	{
		int oldSize = InventorySize();
		int numToadd = newSize - oldSize;
		while (numToadd--) 
		{
			auto slot = NewObject<UInventoryItemSlot>();
			slot->ItemStashComponent = this;
			InventorySlots.Add(slot);
		}
		IncrementChangeIndex();
	}
}



const TMap<EEquipmentSlot, UInventoryItemSlot*>& UItemStashComponent::GetEquipmentSlots() const
{
	return EquipmentSlots;
}

const TArray< UInventoryItemSlot*>& UItemStashComponent::GetInventorySlots() const
{
	return InventorySlots;
}

void UItemStashComponent::SerializeSaveState() {
	auto* characterSerializer = GetOwner()->FindComponentByClass<UCharacterSerializeComponent>();
	if (!characterSerializer || !characterSerializer->HasProfile()) {
		UE_LOG(LogTemp, Error, TEXT("missing serializer, failed to save item stash"));
		return;
	}

	if (GetOwner()->GetGameInstance()->IsDedicatedServerInstance()) {
		return;
	}

	auto& items = characterSerializer->GetItems();
	//d11.gm - save after any inventory use
	if(auto* lazySaver = Cast<UCharacterLazySaveComponent>(characterSerializer)){
		lazySaver->DirtyItems(100);
	}
	items.clear();

	items.reserve(EquipmentSlots.Num() + InventorySlots.Num() + mDeferredInventoryItems.Num());

	for (auto it = EquipmentSlots.CreateConstIterator(); it; ++it) {
		if (it->Value->Item) {
			const auto& item = it->Value->Item->Item;
			const auto& meta = it->Value->Item->Meta;
			const auto& itemType = GetItemRegistry().Get(item.GetItemId());
			auto netheriteEnchant = item.IsNetherite() ? EnchantmentSaveData{ item.NetheriteEnchant().TypeID, item.NetheriteEnchant().Level } : TOptional<EnchantmentSaveData>();

			items.emplace_back(item.GetItemId(), item.ItemPower, item.Rarity, item.bIsUpgraded, item.bIsGifted, item.bIsModified
				, true, meta.MarkedNew, meta.Cloned, it->Key, 0
				, algo::map_vector(item.Enchantments, [](const auto& entry) -> EnchantmentSaveData { return { entry.TypeID, entry.Level }; })
				, algo::map_vector(item.ArmorProperties, [](const auto& entry) -> ArmorPropertySaveData { return { entry }; })
				, netheriteEnchant
			);
		}
	}

	auto index = 0;
	for (auto it = InventorySlots.CreateConstIterator(); it; ++it, ++index) 
	{
		if ((*it)->Item) 
		{
			const auto& item = (*it)->Item->Item;
			const auto& meta = (*it)->Item->Meta;
			const auto& itemType = GetItemRegistry().Get(item.GetItemId());
			auto netheriteEnchant = item.IsNetherite() ? EnchantmentSaveData{ item.NetheriteEnchant().TypeID, item.NetheriteEnchant().Level } : TOptional<EnchantmentSaveData>();

			items.emplace_back(item.GetItemId(), item.ItemPower, item.Rarity, item.bIsUpgraded, item.bIsGifted, item.bIsModified
				, false, meta.MarkedNew, meta.Cloned, EEquipmentSlot::Invalid, index
				, algo::map_vector(item.Enchantments, [](const auto& entry) -> EnchantmentSaveData { return { entry.TypeID, entry.Level }; })
				, algo::map_vector(item.ArmorProperties, [](const auto& entry) -> ArmorPropertySaveData { return { entry }; })
				, netheriteEnchant
			);
		}
	}

	//D11.SC Deferred objects
	for (auto& DeferredInst : mDeferredInventoryItems)
	{
		auto netheriteEnchant = DeferredInst.IsNetherite() ? EnchantmentSaveData{ DeferredInst.NetheriteEnchant().TypeID, DeferredInst.NetheriteEnchant().Level } : TOptional<EnchantmentSaveData>();

		items.emplace_back(DeferredInst.GetItemId(), DeferredInst.ItemPower, DeferredInst.Rarity, DeferredInst.bIsUpgraded, DeferredInst.bIsGifted, DeferredInst.bIsModified
			, false, true, false, EEquipmentSlot::Invalid, -1
			, algo::map_vector(DeferredInst.Enchantments, [](const auto& entry) -> EnchantmentSaveData { return { entry.TypeID, entry.Level }; })
			, algo::map_vector(DeferredInst.ArmorProperties, [](const auto& entry) -> ArmorPropertySaveData { return { entry }; })
			, netheriteEnchant
		);
	}

	ClearCachedInventoryDataArray();
}

void UItemStashComponent::RefreshCachedPoints() {
	if (GetOwner()->GetGameInstance()->IsDedicatedServerInstance()) {
		return;
	}

	auto level = 0;
	if (auto* characterSerializer = GetOwner()->FindComponentByClass<UCharacterSerializeComponent>()) {
		level = UDungeonsGameInstance::createCharacterLevel(characterSerializer->ReadXP()).level;
	}

	RefreshCachedPoints(level);
}

int UItemStashComponent::CalculateEnchantmentPointsGrantedByLevel(const int32 level) {
	return FMath::Max(0, level - 1);	
}

void UItemStashComponent::RefreshCachedPoints(const int32 level) {
	const auto oldPoints = AvailableEnchantmentPoints();
	
	CachedGrantedPoints = CalculateEnchantmentPointsGrantedByLevel(level);
	CachedInvestedPoints = 0;

	for (auto slot : InventorySlots) {
		CachedInvestedPoints += SlotScorer(slot);
	}

	for (const auto slot : EquipmentSlots) {
		CachedInvestedPoints += SlotScorer(slot.Value);
	}

	const auto newPoints = AvailableEnchantmentPoints();
	if (oldPoints != newPoints) {
		OnAvailableEnchangmentPointsChanged.Broadcast(newPoints, this);
		OnAvailableEnchangmentPointsChangedInternal.Broadcast();
	}

	mCachePointsRequireRefresh = false;
}

void UItemStashComponent::DesireRefreshCachedPoints()
{
	 mCachePointsRequireRefresh = true; 
	 SetComponentTickEnabled(true);
}

int UItemStashComponent::EnchantmentPointsGrantedFromExperience() const {
	return CachedGrantedPoints;
}

int UItemStashComponent::InvestedEnchantmentPointsTotal() const {
	return CachedInvestedPoints;
}

int UItemStashComponent::AvailableEnchantmentPoints() const {
	return EnchantmentPointsGrantedFromExperience() - InvestedEnchantmentPointsTotal();
}

int UItemStashComponent::GetNumNewItems() const {
	int count = 0;	
	for (auto slot : InventorySlots) {
		if (slot->IsNew()) {
			count++;
		}
	}
	for (auto slot : EquipmentSlots) {
		if (slot.Value->IsNew()) {
			count++;
		}
	}
	return count;
}

bool UItemStashComponent::CanInvestEnchantmentPointInItemAtIndex(const FInventoryItemData& item, int index) const {
	return false;
}

bool UItemStashComponent::InvestEnchantmentPointInItemAtIndex(FInventoryItemData& item, int index) const {
	return false;
}

void UItemStashComponent::ClearCachedInventoryDataArray() {
	cachedInventoryData.Reset();
}
///Put all inventory items in an array containing only the InventoryItemData.
const TArray<FInventoryItemData>& UItemStashComponent::GetAsInventoryDataArray() {
	//Only populate when empty - reset to 0 when we serialize save state.
	if(cachedInventoryData.Num() <= 0){
	
		for (auto slot : InventorySlots) {
			if (slot->Item) {
				cachedInventoryData.Add(slot->Item->Item);
			}
		}

		for (auto slot : EquipmentSlots) {
			if (slot.Value->Item) {
				cachedInventoryData.Add(slot.Value->Item->Item);
			}
		}
	}

	return cachedInventoryData;
}

//D11.JP
int UItemStashComponent::GetDesiredInventorySlotAmount()
{
	int usedSlots = GetNumEquippedItems() + GetNumUsedSlots() + mDeferredInventoryItems.Num();
	int availableSlots = MAX_INVENTORY_SIZE - usedSlots;
	int neededSlots = Math::min(usedSlots + InventoryDesiredFreeSlots, usedSlots + availableSlots);

	return ClampToEvenColumns(neededSlots);
}

void UItemStashComponent::IncrementChangeIndex() {
	mChangeIndex++;
	OnInventoryChanged.Broadcast();
}
