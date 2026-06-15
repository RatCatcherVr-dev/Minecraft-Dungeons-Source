#include "Dungeons.h"
#include "game/item/ItemTypeDefs.h"
#include "game/component/EquipmentComponent.h"
#include "ArrowCraftingItemInstance.h"
#include "game/item/power/ItemPowerEffectDefs.h"
#include "game/actor/character/BaseCharacter.h"
#include <AbilitySystemComponent.h>
#include <UnrealNetwork.h>
#include "AbilitySystemGlobals.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/attributes/RangedAttributeSet.h"
#include "game/abilities/effects/calculations/ItemPowerModCalculations.h"
#include "game/abilities/effects/calculations/DamageModCalculations.h"

AArrowCraftingItemInstance::AArrowCraftingItemInstance() : craftedArrowItemType(game::item::type::Arrow.getId()) {
	bHasManualCooldownActivation = true;
	SharedPassiveCooldown = 0;
	Effect = UArrowCraftingItemPowerGameplayEffect::StaticClass();
}

void AArrowCraftingItemInstance::BeginPlay() {
	Super::BeginPlay();
	LazyInitializeEquipmentSlot();
}

void AArrowCraftingItemInstance::RemoveEquippedEffects() {
	Super::RemoveEquippedEffects();

	if (bHasCraftedArrows && ArrowSlot.IsValid()) {
		const auto countToRemove = FMath::Min(craftedArrowCount, ArrowSlot->GetCount() - mHasCraftedArrowsAtCount);
		if(countToRemove > 0){
			ArrowSlot->ConsumeAmount(craftedArrowCount);
		}
		bHasCraftedArrows = false;
	}
}

void AArrowCraftingItemInstance::OnItemSlotCountChanged(const UItemSlot* itemSlot) {
	if (bHasCraftedArrows && itemSlot->GetCount() <= mHasCraftedArrowsAtCount) {
		bHasCraftedArrows = false;
		const auto cooldown = CalculateCooldown() * (bHasItemPowerCooldownScaling ? GetPowerEffect()->GetMultiplier(ItemPower) : 1);
		Cooldown().TriggerCooldown(cooldown);
	}
}

int AArrowCraftingItemInstance::GetDisplayCount() const {
	return 0;
}

bool AArrowCraftingItemInstance::IsBusy() const {
	return bHasCraftedArrows;	
}

bool AArrowCraftingItemInstance::CanActivate() const {
	if (ArrowSlot.IsValid()) {
		if (ArrowSlot->GetCount() > 0) {
			return false;
		}
	}

	return Super::CanActivate() && !IsBusy() && OwnerHasRangedWeaponEquipped();
}

bool AArrowCraftingItemInstance::OwnerHasRangedWeaponEquipped() const {
	if (EquipmentComponent) {
		if (const auto* slot = EquipmentComponent->GetFirstSlotOfType(ESlotType::RangedWeapon)) {
			return slot->GetItem() != nullptr;
		}
	}
	return false;
}

void AArrowCraftingItemInstance::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AArrowCraftingItemInstance, bHasCraftedArrows);
}

void AArrowCraftingItemInstance::Activate(const FPredictionKey& predictionKey) {
	if (!ArrowSlot.IsValid()) {
		LazyInitializeEquipmentSlot();
	}
	
	if (ArrowSlot.IsValid() && ArrowSlot->GetCount() <= 0) {
		if (HasAuthority()) {
			bHasCraftedArrows = true;
			mHasCraftedArrowsAtCount = ArrowSlot->GetCount();
			ActivateOnServer();
		}
	}
	Super::Activate(predictionKey);
}

void AArrowCraftingItemInstance::OnCooldownProviderAssigned(ICooldownProvider* provider) {
	if(IsBusy() && provider->IsOnCooldown()) {
		provider->ResetCooldown();
	}
}

void AArrowCraftingItemInstance::ActivateOnServer() {
	auto abilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
	auto spec = effects::CreateGameplayEffectSpecFromSubClass(abilitySystem, Effect, ItemPower);
	spec.GetContext().AddInstigator(GetOwner(), GetOwner());
	ArrowSlot->AddSourceItem(FSourceItemStack(spec, craftedArrowCount), craftedArrowItemType, static_cast<bool>(craftedArrowCount), EEquipmentSource::Crafted);
	ApplyEquipEffectOnOwner();
}

void AArrowCraftingItemInstance::ApplyEquipEffectOnOwner() {
	if (const auto* ownerCharacter = Cast<ABaseCharacter>(GetOwner())) {
		auto* ownerAbilitySystem = ownerCharacter->GetAbilitySystemComponent();

		FGameplayCueParameters params;
		params.Location = GetOwner()->GetActorLocation();
		params.Instigator = GetOwner();

		ownerAbilitySystem->ExecuteGameplayCue(ArrowEquipEffectTag, params);
	}
}

void AArrowCraftingItemInstance::LazyInitializeEquipmentSlot() {
	if (const auto* owner = GetOwner()) {
		EquipmentComponent = owner->FindComponentByClass<UEquipmentComponent>();
		check(EquipmentComponent && "Should have equipment component");
		auto&& itemType = GetItemRegistry().Get(craftedArrowItemType);
		auto ArrowSlots = EquipmentComponent->GetSlotsOfType(itemType.slotType());
		if (ArrowSlots.IsValidIndex(0)) {
			ArrowSlot = Cast<UArrowItemSlot>(ArrowSlots[0]);
			if(owner->HasAuthority()) {
				if (ArrowSlot.IsValid()) {
					ArrowSlot.Get()->OnReplicatedCountInteral.AddUObject(this, &AArrowCraftingItemInstance::OnItemSlotCountChanged);
				}
			}
		}
	}
}


UArrowCraftingGameplayEffect::UArrowCraftingGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::Infinite;
	StackingType = EGameplayEffectStackingType::None;

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("PersistAfterDeath")));
}

UArrowCraftingItemPowerGameplayEffect::UArrowCraftingItemPowerGameplayEffect() {
	FCustomCalculationBasedFloat itemPowerScaling;
	itemPowerScaling.CalculationClassMagnitude = UItemPowerRangedDamageModCalculation::StaticClass();

	FGameplayModifierInfo ItemPowerModifier;
	ItemPowerModifier.ModifierOp = EGameplayModOp::Override;
	ItemPowerModifier.ModifierMagnitude = itemPowerScaling;
	ItemPowerModifier.Attribute = URangedAttributeSet::RangedAttackItemPowerFactorAttribute();
	Modifiers.Add(ItemPowerModifier);
}

UArrowCraftingRangedWeaponPowerGameplayEffect::UArrowCraftingRangedWeaponPowerGameplayEffect() {
}
