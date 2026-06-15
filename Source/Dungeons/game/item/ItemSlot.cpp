#include "Dungeons.h"
#include "DungeonsGameMode.h"
#include "ItemSlot.h"
#include "game/item/instance/AItemInstance.h"
#include "game/component/EquipmentComponent.h"
#include "game/component/HealthComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/item/ItemUtil.h"
#include "instance/ArmorGearItemInstance.h"
#include "UnrealNetwork.h"
#include "game/ArmorProperties/ArmorPropertiesComponent.h"
#include "util/Algo.h"
#include "DungeonsGameInstance.h"
#include "util/telemetry/Analytics.h"
#include "AbilitySystemGlobals.h"
#include "game/UEffectBasedCooldownProvider.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/component/MobLoaderComponent.h"
#include "Engine/AssetManager.h"

UItemSlot::UItemSlot() {
	bReplicates = true;
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	CooldownProvider = CreateDefaultSubobject<UEffectBasedCooldownProvider>(TEXT("CooldownProvider"));
}

void UItemSlot::BeginPlay()
{
	Super::BeginPlay();
	auto abilitySystem = GetOwnerAbilitySystemComponent();
	abilitySystem->RegisterGameplayTagEvent(CooldownProvider->CooldownTag, EGameplayTagEventType::AnyCountChange).AddUObject(this, &UItemSlot::OnCooldownTagChange);
}

void UItemSlot::OnCooldownTagChange(const FGameplayTag tag, const int32 tagCount)
{
	const bool nowOnCooldown = tagCount > 0;
	if(nowOnCooldown != bWasOnCooldown) {
		OnItemSlotCooldownChanged.Broadcast(this, nowOnCooldown);
		bWasOnCooldown = nowOnCooldown;
		if (!nowOnCooldown) {
			SetCooldownSecondsReduction(0.0f);
		}
	}
	
	if(OnItemSlotCooldownUpdated.IsBound()){
		const auto cooldown = GetCooldownSecondsRemainingAndDuration();
		OnItemSlotCooldownUpdated.Broadcast(this, cooldown.Value, cooldown.Key, GetCooldownSecondsReduction());
	}
}


void UItemSlot::OnRep_ItemInstanceReplicated() {
	if (Instance)
	{
		Instance->SetCooldownProvider(this);
	}
	if (OnItemInstanceReplicated.IsBound()) {
		OnItemInstanceReplicated.Broadcast(this);
	}

	if (OnItemInstanceReplicatedInternal.IsBound()) {
		OnItemInstanceReplicatedInternal.Broadcast(this);
	}

	auto role = GetOwner()->Role;

	if (role != ROLE_Authority) {
		OnRep_ItemMaterialSettingsReplicated();
	}
	
	//We only need to listen for activation of a slot if we're the server or the possessing player...and the item actually can be activated.
	if (Instance && (role == ROLE_AutonomousProxy || role == ROLE_Authority)){
		if ( Instance->GetItemType().isActive() || Instance->GetItemType().isConsumable() ) {
			Instance->OnItemActivationPending.AddUObject(this, &UItemSlot::OnActivationPending);
			Instance->OnItemActivationCompleted.AddUObject(this, &UItemSlot::OnActivationCompleted);
		}
	}

	if(_InstanceInternal) {
		_InstanceInternal->OnItemActivationPending.RemoveAll(this);
		_InstanceInternal->OnItemActivationCompleted.RemoveAll(this);
	}
	

	//Make sure we have no straggling states
	_InstanceInternal = Instance;
	bPendingActivation = false;
}

UAbilitySystemComponent* UItemSlot::GetOwnerAbilitySystemComponent() const
{
	if(!CachedAbilitySystemComponent)
	{
		CachedAbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
	}

	return CachedAbilitySystemComponent;
}

TOptional<FItemMaterialSettings> UItemSlot::GetItemMaterialSettings() {
	if (SlotTypeId != ESlotType::Armor && SlotTypeId != ESlotType::MeleeWeapon && SlotTypeId != ESlotType::RangedWeapon) {
		return {};
	}

	if (!Instance) {
		return {};
	}

	auto& itemtype = Instance->GetItemType();

	return TOptional<FItemMaterialSettings>({
		EnchantmentMaterialPowerLevel,
		itemtype.shouldApplyArmorPropertyColor()
			? ArmorPropertyMaterialColor
			: TOptional<FLinearColor>(),
		itemtype.shouldApplyArmorPropertyColor() && itemtype.enchantmentColorMatchesArmorPropertyColor()
			? ArmorPropertyMaterialColor / 12.f
			: TOptional<FLinearColor>()
	});
}

void UItemSlot::OnRep_ItemMaterialSettingsReplicated() {
	const auto itemMaterialSettings = GetItemMaterialSettings();

	if (itemMaterialSettings.IsSet()) {
		OnItemMaterialSettingsChangedInternal.Broadcast(this, itemMaterialSettings.GetValue());
	}
}

void UItemSlot::RefreshItemMaterialSettings() {
	OnRep_ItemMaterialSettingsReplicated();
}

void UItemSlot::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);
	ClearInstance();
}

void UItemSlot::OnRep_CountReplicated() {
	if (OnCountReplicated.IsBound()) {
		OnCountReplicated.Broadcast(this);
	}
	if (OnDisplayCountChanged.IsBound()) {
		OnDisplayCountChanged.Broadcast(this);
	}
	if (OnReplicatedCountInteral.IsBound()) {
		OnReplicatedCountInteral.Broadcast(this);
	}
}

void UItemSlot::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UItemSlot, Count);
	DOREPLIFETIME(UItemSlot, Instance);
	DOREPLIFETIME(UItemSlot, EnchantmentMaterialPowerLevel);
	DOREPLIFETIME(UItemSlot, ArmorPropertyMaterialColor);
	DOREPLIFETIME(UItemSlot, cooldownReduction);
}

bool UItemSlot::CanEquip(const ItemType& itemType) const {
	return Instance == nullptr || 
		Instance->GetItemId() == itemType.getId();
}

AItemInstance* UItemSlot::GetItem() const {
	return Instance;
}

TOptional<FItemId> UItemSlot::ClearInstance() {
	if(LoadingHandle.IsValid() && (LoadingHandle->IsLoadingInProgress() || LoadingHandle->IsStalled())) {
		LoadingHandle->CancelHandle();
		LoadingHandle.Reset();	
	}
	
	if (!Instance) {
		return {};
	}

	//Clear old enchantments.
	const auto oldItemTypeId = Instance->GetItemId();
	if (auto enchantmentComponent = GetOwner()->FindComponentByClass<UEnchantmentComponent>()) {
		enchantmentComponent->RemoveEnchantments(Instance);
	}

	//Unequip
	Instance->RemoveEquippedEffects();

	//Deallocate old item instance.
	Instance->Destroy();
	Instance = nullptr;
	Count = 0;

	return static_cast<FItemId>(oldItemTypeId);
}

bool UItemSlot::ShouldSpawnNewInstance(const FItemId& type) const {
	const auto& itemType = GetItemRegistry().Get(type);
	return !(Instance != nullptr && Instance->GetItemId() == type && itemType.isConsumable());
}

void UItemSlot::OnInstanceClassLoaded(const FInventoryItemData& itemData, int count /*= 1*/, EEquipmentSource source) {
	const auto& itemType = GetItemRegistry().Get(itemData.GetItemId());

	const auto instance = game::item::util::spawnItemInstance(itemType, itemData.ItemPower, FVector::ZeroVector, GetOwner());

	Instance = instance;
	Count = count;

	if (Instance) {
		Instance->OptionalItemDataSource = itemData;

		if (auto armorInstance = Cast<AArmorGearItemInstance>(Instance)) {
			armorInstance->SetArmorProperties(itemData.ArmorProperties);
		}
		GetItem()->ApplyEquippedEffects();
	}

	if (GetOwner() && GetOwner()->Role == ROLE_Authority)
	{
		OnRep_ItemInstanceReplicated();
		OnRep_CountReplicated();
	}

	if (itemType.isGear() && Instance) {

		if (auto enchantmentComponent = GetOwner()->FindComponentByClass<UEnchantmentComponent>()) {
			auto enchantments = itemData.AllEnchantments();
			enchantmentComponent->AddEnchantments(enchantments, createEnchantmentSpawnConfig(Instance).SetOwnerItemRarity(itemData.Rarity));
			EnchantmentMaterialPowerLevel = algo::sum(enchantments, RETLAMBDA(it.Level));
		}

		if (SlotTypeId == ESlotType::Armor) {
			if (auto armorPropertiesComponent = GetOwner()->FindComponentByClass<UArmorPropertiesComponent>()) {
				ArmorPropertyMaterialColor = armorPropertiesComponent->DetermineArmorColor();
			}
			// D11.SSN
			if (APlayerCharacter* playerCharacter = Cast<APlayerCharacter>(GetOwner())) {
				if (firstEquip) {
					firstEquip = false;
					if (itemType.getId() == game::item::type::OcelotArmor.getId() || itemType.getId() == game::item::type::OcelotArmor_Unique1.getId()) {
						playerCharacter->OcelotArmourTracking = true;
						playerCharacter->OnRep_OcelotArmourTracking();
					}
					else if (itemType.getId() == game::item::type::EmeraldArmor.getId() || itemType.getId() == game::item::type::EmeraldArmor_Unique1.getId()) {
						playerCharacter->EmeraldArmourTracking = true;
						playerCharacter->OnRep_EmeraldArmourTracking();
					}
				}
				else {
					if (playerCharacter->OcelotArmourTracking && itemType.getId() != game::item::type::OcelotArmor.getId() && itemType.getId() != game::item::type::OcelotArmor_Unique1.getId()) {
						playerCharacter->OcelotArmourTracking = false;
						playerCharacter->OnRep_OcelotArmourTracking();
					}
					if (playerCharacter->EmeraldArmourTracking && itemType.getId() != game::item::type::EmeraldArmor.getId() && itemType.getId() != game::item::type::EmeraldArmor_Unique1.getId()) {
						playerCharacter->EmeraldArmourTracking = false;
						playerCharacter->OnRep_EmeraldArmourTracking();
					}
				}
			}
		}
		
		if (SlotTypeId == ESlotType::MeleeWeapon || SlotTypeId == ESlotType::RangedWeapon) {
			if (auto* playerCharacter = Cast<APlayerCharacter>(GetOwner())) {
				if (playerCharacter->IceWeaponTracking) {
					playerCharacter->IceWeaponTracking = false;
					playerCharacter->OnRep_IceWeaponTracking();
				}
			}
		}

		if (GetOwner() && GetOwner()->Role == ROLE_Authority) {
			OnRep_ItemMaterialSettingsReplicated();				
		}
	}

	if (source == EEquipmentSource::PickUp) {
		OnPickedUp.ExecuteIfBound(this);
	}

	LoadingHandle.Reset();
}

void UItemSlot::EquipItem_Implementation(const FInventoryItemData& itemData, int count /*= 1*/, EEquipmentSource source) {
	const auto& itemType = GetItemRegistry().Get(itemData.GetItemId());

	if (!ShouldSpawnNewInstance(itemData.GetItemId())) {
		Count += count;
		OnRep_CountReplicated();

		if (source == EEquipmentSource::PickUp) {
			OnPickedUp.ExecuteIfBound(this);
		}
	}
	else {
		ClearInstance();
		
		if(auto cls = itemType.getInstanceClass(false)) {
			OnInstanceClassLoaded(itemData, count, source);
		} else {
			
			LoadingHandle = game::item::util::loadItemInstance(itemType, FStreamableDelegate::CreateWeakLambda(this, [data = itemData, count, this, source]() {
					OnInstanceClassLoaded(data, count, source);
				}));
		}	
	}
}

bool UItemSlot::EquipItem_Validate(const FInventoryItemData& itemData, int count /*= 1*/, EEquipmentSource source)
{
	return true;
}

void UItemSlot::Clear_Implementation()
{
	if (!Instance) return;

	//First clear
	ClearInstance();

	//then trigger reps;
	if (GetOwner() && GetOwner()->Role == ROLE_Authority)
	{
		OnRep_ItemInstanceReplicated();
		OnRep_CountReplicated();
	}
}

bool UItemSlot::Clear_Validate() {
	return true;
}

int UItemSlot::GetCount() const {
	return Count;
}

int UItemSlot::GetDisplayCount() const {
	switch (SlotTypeId)
	{
		// better to normalise to 'currency'?
	case ESlotType::Arrow:
	case ESlotType::BurningArrow:
	case ESlotType::FireworksArrow:
	case ESlotType::TormentProjectile:
	case ESlotType::HeavyHarpoon:
	case ESlotType::Trident:
	case ESlotType::Conduit:
	case ESlotType::ThunderingArrow:
	case ESlotType::TNT:
	case ESlotType::VoidArrow:
	case ESlotType::PoisonArrow:
		return Count;
	case ESlotType::HealthPotion:
	case ESlotType::Consumable:
		return 0;
	default:
		return Instance ? Instance->GetDisplayCount() : Count;
	}
}

int UItemSlot::Consume() {
	auto amount = Instance ? (int32)Instance->GetItemType().getConsumeAmount() : 1;
	ConsumeAmount(amount);
	return amount;
}

void UItemSlot::ConsumeAmount(int32 amount) {
	if (!Count || (Instance && !Instance->GetItemType().isConsumable())) return;
	SetCount(Count-amount);
}

void UItemSlot::SetCount(int count) {
	Count = count;
	OnRep_CountReplicated();

	//Only the server may call clear when consuming last count
	if (Count <= 0 && GetOwner()->HasAuthority()) {
		Clear();
	}
}

bool UItemSlot::ActivateSlot() {
	if (CanActivateSlot()) {
		return Instance->TryActivate();
	}
	return false;
}

void UItemSlot::OnTriedToActivate() const {
	if (Instance) {
		Instance->OnTriedToActivate();
	}
}

bool UItemSlot::CanActivateSlot() const {
	
	if (!Instance || !Count || bPendingActivation) {
		return false;
	}

	auto HealthComponent = GetOwner()->FindComponentByClass<UHealthComponent>();
	if (HealthComponent && HealthComponent->IsNotAlive())
	{
		return false;
	}

	return Instance->CanActivate();
}

bool UItemSlot::IsBusy() const {
	if (!Instance) {
		return false;
	}

	return Instance->IsBusy();
}

bool UItemSlot::IsOnCooldown() const {
	return CooldownProvider->IsOnCooldown();
}

float UItemSlot::GetCooldownSecondsRemaining() const
{
	return CooldownProvider->GetCooldownSecondsRemaining();
}

float UItemSlot::GetCooldownFractionRemaining() const
{
	return CooldownProvider->GetCooldownFractionRemaining();
}

float UItemSlot::GetCooldownSecondsDuration() const
{
	return CooldownProvider->GetCooldownSecondsDuration();
}

float UItemSlot::GetCooldownSecondsReduction() const
{
	return cooldownReduction;
}

TPair<float, float> UItemSlot::GetCooldownSecondsRemainingAndDuration() const {
	return CooldownProvider->GetCooldownSecondsRemainingAndDuration();
}

void UItemSlot::TriggerCooldown(float seconds, float reduction, FPredictionKey key)
{
	SetCooldownSecondsReduction(reduction);
	CooldownProvider->TriggerCooldown(seconds, key);
}

void UItemSlot::TriggerCooldown(float seconds, FPredictionKey key)
{
	SetCooldownSecondsReduction(0.0f);
	CooldownProvider->TriggerCooldown(seconds, key);
}

void UItemSlot::ModifyCooldown(float offsetSeconds) {
	CooldownProvider->ModifyCooldown(offsetSeconds);
}

void UItemSlot::ResetCooldown()
{
	CooldownProvider->ResetCooldown();
	SetCooldownSecondsReduction(0.0f);
}

bool UItemSlot::GetWantsToActivate() const {
	return bWantsToActivate;
}

bool UItemSlot::SetWantsToActivate(bool WantsToActivate){
	bWantsToActivate = WantsToActivate;
	OnItemSlotWantsToActivateChanged.Broadcast();

	if (bWantsToActivate && !IsOnCooldown()) {
		if (CanActivateSlot()){
			ActivateSlot();
			return true;
		} else {
			OnTriedToActivate();
			return false;
		}
	}	

	return false;
}

bool UItemSlot::Swap_Validate(UItemSlot*)
{
	return true;
}

void UItemSlot::Swap_Implementation(UItemSlot* other)
{
	std::swap(Instance, other->Instance);
	std::swap(Count, other->Count);
	std::swap(EnchantmentMaterialPowerLevel, other->EnchantmentMaterialPowerLevel);
	std::swap(ArmorPropertyMaterialColor, other->ArmorPropertyMaterialColor);


	//Cooldown swapping
	float currentCooldown = GetCooldownSecondsRemaining();
	float otherCurrentCooldown = other->GetCooldownSecondsRemaining();

	auto fixupItem = [](AItemInstance* instance, ICooldownProvider* provider, UItemSlot* oldSlot, float oldCooldown)
	{
		if (instance)
		{
			instance->SetCooldownProvider(provider);
			instance->OnItemActivationCompleted.RemoveAll(oldSlot);
			instance->OnItemActivationPending.RemoveAll(oldSlot);

			if (provider->IsOnCooldown()) provider->ResetCooldown();
			if (oldCooldown > 0.f) provider->TriggerCooldown(oldCooldown);
		}
	};

	fixupItem(Instance, this, other, otherCurrentCooldown);
	fixupItem(other->Instance, other, this, currentCooldown);


	//Trigger relevant OnReps
	if (Count != other->Count)
	{
		OnRep_CountReplicated();
		other->OnRep_CountReplicated();
	}

	if (EnchantmentMaterialPowerLevel != other->EnchantmentMaterialPowerLevel || ArmorPropertyMaterialColor != other->ArmorPropertyMaterialColor)
	{
		OnRep_ItemMaterialSettingsReplicated();
		other->OnRep_ItemMaterialSettingsReplicated();
	}

	OnRep_ItemInstanceReplicated();
	other->OnRep_ItemInstanceReplicated();
}

void UItemSlot::ResetSlot() {
	bPendingActivation = false;
	if (Instance) {
		Instance->ResetInstance();
	}
}

void UItemSlot::OnActivationPending(bool pending) {
	bPendingActivation = true;
}

UEffectBasedCooldownProvider* UItemSlot::GetEffectsBasedCooldownProvider() const
{
	return CooldownProvider;
}

UAbilitySystemComponent* UItemSlot::GetAbilitySystemComponent() const
{
	return GetOwnerAbilitySystemComponent();
}

void UItemSlot::OnActivationCompleted(bool outcome) {
	bPendingActivation = false;
	check(Instance && "Must have instance when activation completes...");

	if (OnItemSlotActivationCompleted.IsBound()) {
		OnItemSlotActivationCompleted.Broadcast(this, outcome);
	}

	if (Instance->GetItemType().isConsumable() && outcome) {
		Consume();
	}
}


UItemslotCooldownEffect::UItemslotCooldownEffect(const FObjectInitializer& ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	FSetByCallerFloat durationMagnitude;
	durationMagnitude.DataName = FName(TEXT("Duration"));
	DurationMagnitude = durationMagnitude;

	StackingType = EGameplayEffectStackingType::None;
}

void UItemSlot::SetCooldownSecondsReduction(float reduction) {
	cooldownReduction = reduction;
}

void UItemSlot::OnRep_CooldownReduction() {
	if (OnItemSlotCooldownUpdated.IsBound()) {
		const auto cooldown = GetCooldownSecondsRemainingAndDuration();
		OnItemSlotCooldownUpdated.Broadcast(this, cooldown.Value, cooldown.Key, GetCooldownSecondsReduction());
	}
}