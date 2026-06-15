#include "Dungeons.h"
#include "EquipmentDisplayComponent.h"
#include "game/item/instance/GearItemInstance.h"
#include <Components/ChildActorComponent.h>
#include "HealthComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/actor/item/GearActor.h"
#include "game/actor/character/BaseCharacter.h"

static const TMap<ESlotType, bool> WeaponVisibility_HIDDEN = {
	{ESlotType::MeleeWeapon, false},
	{ESlotType::RangedWeapon, false}
};

FName UEquipmentDisplayComponent::SlotTypeToComponentTag(ESlotType type) {
	switch (type) {
	case ESlotType::MeleeWeapon:
		return "MeleeWeapon";
	case ESlotType::RangedWeapon:
		return "RangedWeapon";
	case ESlotType::Armor:
		return "Armor";
	case ESlotType::ActivePermanent:
		return "ActivePermanent";
	default:
		checkNoEntry();
		return "";
	}
}

UEquipmentDisplayComponent::UEquipmentDisplayComponent() {
	bReplicates = true;
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	bEditableWhenInherited = true;
	SlotActorManager = CreateDefaultSubobject<USlotActorManager>(TEXT("SlotManager"));
}

void UEquipmentDisplayComponent::EndPlay(EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);
	//Bind to nothing
	BindToGearSlots({});
	SlotActorManager->EmptyActors();
}

void UEquipmentDisplayComponent::BeginPlay() {
	Super::BeginPlay();

	if (auto* player = Cast<APlayerCharacter>(GetOwner())) {
		player->OnPlayerDeath.AddUObject(this, &UEquipmentDisplayComponent::OnPlayerDeath);
		if (auto* healthComponent = GetOwner()->FindComponentByClass<UHealthComponent>()) {
			healthComponent->OnRevive.AddUObject(this, &UEquipmentDisplayComponent::OnRespawn);
		}
	}

	if (auto* character = Cast<ABaseCharacter>(GetOwner())) {
		character->OnAttackStateChanged.AddUObject(this, &UEquipmentDisplayComponent::OnAttackStateChanged);
		OnAttackStateChanged(character->GetAttackState());
	}
}

void UEquipmentDisplayComponent::SetDefaultAnimationsInstance(const TSubclassOf<UAnimInstance> defaultAnimationsInstance) {
	DefaultAnimationsInstance = defaultAnimationsInstance;
}

void UEquipmentDisplayComponent::BindToGearSlots(const TArray<UItemSlot*> gearSlots) {
	UnbindSlots();

	BindSlots(gearSlots);
	if (const auto* character = Cast<ABaseCharacter>(GetOwner())) {		
		OnAttackStateChanged(character->GetAttackState());
	}
}

void UEquipmentDisplayComponent::UnbindSlots() {
	for (auto* slot : BoundGearSlots) {
		if (slot) {
			slot->OnItemInstanceReplicatedInternal.RemoveAll(this);
			slot->OnItemMaterialSettingsChangedInternal.RemoveAll(this);
			SlotActorManager->DestroyActor(slot);
			SlotActorManager->RemoveSlot(slot);			
		}
	}
	BoundGearSlots.Reset();
}

void UEquipmentDisplayComponent::BindSlots(TArray<UItemSlot*> gearSlots) {
	BoundGearSlots = gearSlots;
	for (auto* slot : gearSlots) {
		if (slot) {
			SlotActorManager->Register(GetOwner(), slot);
			UpdateGearActorForSlot(slot);

			slot->OnItemInstanceReplicatedInternal.AddUObject(this, &UEquipmentDisplayComponent::OnGearItemSlotUpdated);
			slot->OnItemMaterialSettingsChangedInternal.AddUObject(this, &UEquipmentDisplayComponent::OnUpdateItemMaterialSettings);

			slot->RefreshItemMaterialSettings();
		}		
	}
	UpdatePlayerAnimation();
}

TArray<UItemSlot*> UEquipmentDisplayComponent::GetBoundGearSlots() const {
	return BoundGearSlots;
}

USkeletalMeshComponent* UEquipmentDisplayComponent::GetPlayerMesh() const {
	return OverridingPlayerMesh ? OverridingPlayerMesh : Cast<APlayerCharacter>(GetOwner())->GetMesh();
}

void UEquipmentDisplayComponent::OnGearItemSlotUpdated(UItemSlot* itemSlot) {
	UpdateGearActorForSlot(itemSlot);	
	UpdatePlayerAnimation();
}

void UEquipmentDisplayComponent::UpdateGearActorForSlot(UItemSlot* itemSlot) const {
	SlotActorManager->DestroyActor(itemSlot);

	const auto* gearItem = Cast<AGearItemInstance>(itemSlot->GetItem());
	
	if (gearItem && gearItem->GearActor) {
		auto* component = SlotActorManager->AssignActorClass(itemSlot, gearItem->GearActor);
		auto* gearActor = Cast<AGearActor>(component->GetChildActor());
		gearActor->Paperdoll->AttachToPlayerMesh(GetPlayerMesh());
		UpdateComponentVisibility(component, IsSlotTypeVisible(itemSlot->SlotTypeId));
		
		const auto itemMaterialSettings = itemSlot->GetItemMaterialSettings();
		if (itemMaterialSettings.IsSet()) {
			OnUpdateItemMaterialSettings(itemSlot, itemMaterialSettings.GetValue());
		}
	}
}

void UEquipmentDisplayComponent::OnUpdateItemMaterialSettings(UItemSlot* itemSlot, const FItemMaterialSettings itemMaterialSettings) const {
	if (const auto skeletalMesh = GetOwner()->FindComponentByClass<USkeletalMeshComponent>()) {
		UpdateItemMaterialSettings(skeletalMesh, itemSlot->SlotTypeId, itemMaterialSettings);
	}
}

void UEquipmentDisplayComponent::UpdateItemMaterialSettings(USkeletalMeshComponent* skeletalMesh, const ESlotType slotType, FItemMaterialSettings itemMaterialSettings) {
	TArray<USceneComponent*> components;
	skeletalMesh->GetChildrenComponents(false, components);

	for (auto component : components) {
		if (const auto meshComponent = Cast<UStaticMeshComponent>(component)) {
			if (meshComponent->ComponentHasTag(SlotTypeToComponentTag(slotType))) {
				if (const auto material = meshComponent->GetMaterial(0)) {
					auto materialDynamic = Cast<UMaterialInstanceDynamic>(material);
					if (materialDynamic == nullptr) {
						materialDynamic = UMaterialInstanceDynamic::Create(material, skeletalMesh->GetOwner());
						meshComponent->SetMaterial(0, materialDynamic);
					}

					materialDynamic->SetScalarParameterValue("Power", itemMaterialSettings.power);

					if (itemMaterialSettings.emissiveColor.IsSet()) {
						materialDynamic->SetVectorParameterValue("Emissive", itemMaterialSettings.emissiveColor.GetValue());
					}
					if (itemMaterialSettings.enchantmentColor.IsSet()) {
						materialDynamic->SetVectorParameterValue("EnchantmentColor", itemMaterialSettings.enchantmentColor.GetValue());
					}
				}
			}
		}
	}
}

void UEquipmentDisplayComponent::UpdatePlayerAnimation() {
	if (auto* playerMesh = GetPlayerMesh()) {
		const auto overrideAnim = GetOverrideAnim();
		const auto anim = overrideAnim ? overrideAnim : DefaultAnimationsInstance;
		auto animInstance = playerMesh->GetAnimInstance();

		if (playerMesh->AnimClass != anim) {
			auto montage = animInstance ? animInstance->GetCurrentActiveMontage() : nullptr;
			const auto activeMontage = montage ? animInstance->GetActiveMontageInstance() : nullptr;

			/* moj.db: I'm not going to pretend this is a good fix. I managed to find a way to tickle the engine in just the right way to make it stop crashing.
			By looking at the 4.23.1 source code it seems Epic has fixed this crash too - but in a defensive manner. */
			playerMesh->FinalizeBoneTransform();
			playerMesh->SetAnimInstanceClass(anim);

			if (montage && activeMontage) {
				if (playerMesh->GetAnimInstance())
				{
					animInstance = playerMesh->GetAnimInstance();
					montage->BlendIn = 0.f;
					animInstance->Montage_Play(montage, activeMontage->GetPlayRate(), EMontagePlayReturnType::Duration, activeMontage->GetPosition());
				}
				/* After searching long and hard to some combination of function calls that would erase the frame lag the above code could cause, this seems to do the trick.
				 * I have not dug too deeply into why (but I am assuming it forces an animation tick AND synchronized transform update). */
				playerMesh->TickAnimation(0.f, false);
				playerMesh->RefreshBoneTransforms();
			}
		}
	}
}

TSubclassOf<UAnimInstance> UEquipmentDisplayComponent::GetOverrideAnim() const {
	if (SlotActorManager)
	{
		for (const auto& slotGear : SlotActorManager->GetActors()) {
			if (slotGear.Value->IsVisible()) {
				if (const auto* slot = Cast<UItemSlot>(slotGear.Key)) {
					if (const auto overrideAnim = GetOverrideAnim(slot)) {
						return overrideAnim;
					}
				}
			}
		}
	}
	return nullptr;
}

TSubclassOf<UAnimInstance> UEquipmentDisplayComponent::GetOverrideAnim(const UItemSlot* slot) const {
	if (const auto* item = Cast<AGearItemInstance>(slot->GetItem())) {
		return item->OverrideAnimationsInstance;
	}
	return nullptr;
}

void UEquipmentDisplayComponent::SetCurrentWeapon(const ESlotType slot) {
	CurrentWeapon = slot;
	ShowCurrentWeapon();
}

void UEquipmentDisplayComponent::RestoreWeaponVisibility() {
	ShowCurrentWeapon();
}

void UEquipmentDisplayComponent::ShowCurrentWeapon() {
	const auto visibleWeapon = CurrentWeapon.Get(ESlotType::MeleeWeapon);
	ToggleEquipmentVisibility({
		{ESlotType::MeleeWeapon, ESlotType::MeleeWeapon == visibleWeapon},
		{ESlotType::RangedWeapon, ESlotType::RangedWeapon == visibleWeapon}
	});
}

bool UEquipmentDisplayComponent::IsSlotTypeVisible(ESlotType slot) const {
	return SlotTypeVisibility.Contains(slot) ? SlotTypeVisibility[slot] : true;
}

void UEquipmentDisplayComponent::HideWeapons() {
	ToggleEquipmentVisibility(WeaponVisibility_HIDDEN);
}

void UEquipmentDisplayComponent::UnsetCurrentWeapon() {
	CurrentWeapon.Reset();
	ToggleEquipmentVisibility(WeaponVisibility_HIDDEN);
}

void UEquipmentDisplayComponent::ToggleEquipmentVisibility(const ESlotType slot, const bool visible) {
	ToggleEquipmentVisibility({ { slot, visible } });
}

void UEquipmentDisplayComponent::ToggleEquipmentVisibility(const TMap<ESlotType, bool>& visibilitySettings) {

	for (const auto& pair : visibilitySettings) {
		const bool visible = pair.Value;
		const ESlotType slotType = pair.Key;
		if (visible != IsSlotTypeVisible(slotType)) {
			if (SlotActorManager)
			{
				for (const auto& objectComponentPair : SlotActorManager->GetActors()) {
					const auto* itemSlot = Cast<UItemSlot>(objectComponentPair.Key);
					if (itemSlot && itemSlot->SlotTypeId == slotType) {
						auto* childActorComponent = objectComponentPair.Value;
						UpdateComponentVisibility(childActorComponent, visible);
					}
				}
			}
		}
		SlotTypeVisibility.Emplace(slotType, visible);
	}
	UpdatePlayerAnimation();
}

void UEquipmentDisplayComponent::UpdateComponentVisibility(UChildActorComponent* childComponentActor, const bool visible) const {
	childComponentActor->SetVisibility(visible, true);
	if (auto* gearActor = Cast<AGearActor>(childComponentActor->GetChildActor())) {
		gearActor->SetActorHiddenInGame(!visible);
		gearActor->SetVisibility(visible);
	}
}


void UEquipmentDisplayComponent::SetOverridingPlayerMesh(USkeletalMeshComponent* playerMesh) {
	OverridingPlayerMesh = playerMesh;
}

AGearActor* UEquipmentDisplayComponent::GetItemSlotCurrentGearActor(UItemSlot* itemSlot) const {
	const auto gearActors = SlotActorManager->GetActors();
	if (gearActors.Contains(itemSlot)) {
		const auto* component = gearActors[itemSlot];
		return Cast<AGearActor>(component->GetChildActor());
	}
	return nullptr;
}

void UEquipmentDisplayComponent::OnPlayerDeath() {
	HideWeapons();
}

void UEquipmentDisplayComponent::OnRespawn() {
	RestoreWeaponVisibility();
}

void UEquipmentDisplayComponent::OnAttackStateChanged(const EAttackState attackState) {
	if (attackState == EAttackState::Ranged || attackState == EAttackState::Aiming) {
		SetCurrentWeapon(ESlotType::RangedWeapon);
	} else if (attackState != EAttackState::Magic) {
		SetCurrentWeapon(ESlotType::MeleeWeapon);
	} else {
		UnsetCurrentWeapon();
	}
}
