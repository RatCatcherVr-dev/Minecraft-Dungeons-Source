// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "ArrowItemSlot.h"
#include "game/component/EquipmentComponent.h"
#include "game/item/ItemSlot.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"

static int RANGED_WEAPON_REFERENCE_STACK_SIZE = 30;
static int convertStackCount(int currentStoreSize, int previousStoreCount, int count) {
	return FMath::FloorToInt((float)currentStoreSize / (float)previousStoreCount * (float)count);
}

UArrowItemSlot::UArrowItemSlot() 
	: ProjectileStackCount(RANGED_WEAPON_REFERENCE_STACK_SIZE)
	, DefaultStackCount(RANGED_WEAPON_REFERENCE_STACK_SIZE)
{}

TSubclassOf<class UUserWidget> UArrowItemSlot::GetAmmoCounterWidgetClass() const{
	return AmmoCounterWidgetClass;
}

TSubclassOf<class UUserWidget> UArrowItemSlot::GetAmmoCounterLiteContentWidgetClass() const {
	return AmmoCounterLiteContentWidgetClass;
}


TSubclassOf<class UUserWidget> UArrowItemSlot::GetAmmoCounterLiteOverlayWidgetClass() const {
	return AmmoCounterLiteOverlayWidgetClass;
}

int UArrowItemSlot::GetCount() const {
	return EquippedRangedWeapon.IsValid() ? Count : 0;
}

void UArrowItemSlot::UpdateGameplayEffectSpec() {
	auto abilitySystem = GetOwnerAbilitySystem();
	if (CurrentEffectHandle.IsValid()) {
		abilitySystem->RemoveActiveGameplayEffect(CurrentEffectHandle);
		CurrentEffectHandle.Invalidate();
	}

	if (IsActive && SourceItemStacks.Num()) {
		CurrentEffectHandle = abilitySystem->ApplyGameplayEffectSpecToSelf(SourceItemStacks.Top().SpecToApply);
	}
}

void UArrowItemSlot::ConsumeAmount(int32 amount) {
	Super::ConsumeAmount(amount);
	int consumeAmount = amount;

	bool HasChanged = false;
	while (consumeAmount > 0  && SourceItemStacks.Num()) {
		auto& top = SourceItemStacks.Top();
		top.Stack -= consumeAmount;

		if (top.Stack <= 0) {
			consumeAmount = FMath::Abs(top.Stack);
			SourceItemStacks.Pop();
			HasChanged = true;
		}
		else {
			break;
		}
	}
	
	if(HasChanged) UpdateGameplayEffectSpec();
}

void UArrowItemSlot::SetAsActiveArrowSlot() {
	if (IsActive) return;
	IsActive = true;

	UpdateGameplayEffectSpec();
}

void UArrowItemSlot::SetAsInactiveArrowSlot() {
	IsActive = false;
	UpdateGameplayEffectSpec();
}

UAbilitySystemComponent* UArrowItemSlot::GetOwnerAbilitySystem() {
	if (!OwnerAbilitySystem.IsValid()) {
		OwnerAbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
	}
	
	return OwnerAbilitySystem.Get();
}

void UArrowItemSlot::EmptySourceItems() {
	SourceItemStacks.Empty();
	UpdateGameplayEffectSpec();
}

void UArrowItemSlot::AddSourceItem(FSourceItemStack&& itemStack, const FItemId& type, bool fixedAmount, EEquipmentSource source) {
	if (ShouldSpawnNewInstance(type)) {
		EmptySourceItems();
	}

	SourceItemStacks.Emplace(std::move(itemStack));
	auto& entry = SourceItemStacks.Top();

	int count = entry.Stack;
	if (!fixedAmount) {
		entry.Stack = convertStackCount(ProjectileStackCount, DefaultStackCount, count);
	}

	UpdateGameplayEffectSpec();

	Super::EquipItem_Implementation(FInventoryItemData(type, 1.f), entry.Stack, source);
}

const FSourceItemStack* UArrowItemSlot::GetTopSourceItem() const {
	if (SourceItemStacks.Num() > 0) {
		return &SourceItemStacks.Top();
	}
	return nullptr;
}

void UArrowItemSlot::BeginPlay() {
	Super::BeginPlay();

	if (const auto equipmentComponent = GetOwner()->FindComponentByClass<UEquipmentComponent>()) {
		const auto rangedWeaponSlots = equipmentComponent->GetSlotsOfType(ESlotType::RangedWeapon);
		if (rangedWeaponSlots.Num()) {
			rangedWeaponSlots[0]->OnItemInstanceReplicatedInternal.AddUObject(this, &UArrowItemSlot::OnRangedWeaponUpdated);
			OnRangedWeaponUpdated(rangedWeaponSlots[0]);
		}
	}
}

float UArrowItemSlot::GetStackConversionFactor() const {
	return static_cast<float>(ProjectileStackCount) / DefaultStackCount;
}

void UArrowItemSlot::EquipFixedAmount(const FInventoryItemData& itemData, int count /*= 1*/, EEquipmentSource source) {
	if (ShouldSpawnNewInstance(itemData.GetItemId())) {
		EmptySourceItems();
	}
	else if (SourceItemStacks.Num() > 0) {
		SourceItemStacks.Top().Stack += convertStackCount(ProjectileStackCount, DefaultStackCount, count);
	}

	Super::EquipItem_Implementation(itemData, count, source);
}

void UArrowItemSlot::EquipItem_Implementation(const FInventoryItemData& itemData, int count/* = 1*/, EEquipmentSource source) {
	const int convertedCount = convertStackCount(ProjectileStackCount, DefaultStackCount, count);

	if (ShouldSpawnNewInstance(itemData.GetItemId())) {
		EmptySourceItems();
	}
	else if (SourceItemStacks.Num() > 0) {
		SourceItemStacks.Top().Stack += convertedCount;
	}

	Super::EquipItem_Implementation(itemData, convertedCount, source);
}

void UArrowItemSlot::OnRangedWeaponUpdated(UItemSlot* itemSlot) {
	EquippedRangedWeapon = Cast<ARangedWeaponGearItemInstance>(itemSlot->GetItem());

	if (GetOwner()->HasAuthority()) {
		int newStackCount = EquippedRangedWeapon.IsValid() && !ForceUnscaledStackCount ? EquippedRangedWeapon->GetProjectileStackCount() : DefaultStackCount;
		Count = convertStackCount(newStackCount, ProjectileStackCount, Count);
		ProjectileStackCount = newStackCount;
		OnRep_CountReplicated();
	}
}

FSourceItemStack::FSourceItemStack()
	: Stack(0) {
}
FSourceItemStack::FSourceItemStack(FGameplayEffectSpec&& spec, int stack) 
: SpecToApply(std::move(spec)), Stack(stack) {
}
FSourceItemStack::FSourceItemStack(const FGameplayEffectSpec& spec, int stack)
	: SpecToApply(spec), Stack(stack) {

}