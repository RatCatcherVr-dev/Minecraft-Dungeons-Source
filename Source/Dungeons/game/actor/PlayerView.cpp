#include "Dungeons.h"
#include "game/component/EquipmentComponent.h"
#include "game/item/instance/GearItemInstance.h"
#include "game/avatar/PlayerAvatarComponent.h"
#include "game/component/EquipmentDisplayComponent.h"
#include "PlayerView.h"
#include "character/player/PlayerCharacterSaveSlot.h"
#include "game/component/cosmetics/CosmeticsComponent.h"
#include "game/component/cosmetics/CosmeticsDisplayComponent.h"
#include "game/util/DungeonsEffectLibrary.h"
#include "game/skins/SkinsUtil.h"
#include "online/sessions/OnlineUtil.h"

namespace game {
	namespace playerView {
		FRotator calculateNewRelativeRotation(const FRotator currentRelativeRotation, const float additionalRotation) {
			return currentRelativeRotation + FRotator(0, additionalRotation, 0);
		}
	}
}

const float APlayerView::SPIN_RESTORE_SPEED = 1.0f;

void APlayerView::RefreshWasDamagedBind()
{
	auto playerToBind = bActive ? ViewedPlayer : nullptr;
	if (playerToBind != BoundPlayerHitFlash) {
		if (BoundPlayerHitFlash) {
			if (auto HC = BoundPlayerHitFlash->GetHealthComponent()) {
				HC->OnHealthFractionChanged.RemoveAll(this);
			}
			for (auto* playerCharacter : InstanceTracker<APlayerCharacter>::GetList(GetWorld())) {
				playerCharacter->GetHealthComponent()->OnHealthFractionChanged.RemoveAll(this);
			}
		}
		BoundPlayerHitFlash = playerToBind;

		if (BoundPlayerHitFlash) {
			if (auto HC = BoundPlayerHitFlash->GetHealthComponent()) {
				HC->OnHealthFractionChanged.AddUObject(this, &APlayerView::OnPlayerHealthFractionChanged);
			}
			for (auto* playerCharacter : InstanceTracker<APlayerCharacter>::GetList(GetWorld())) {
				if (playerCharacter != BoundPlayerHitFlash && playerCharacter->IsLocallyControlled()) {
					if (!playerCharacter->GetHealthComponent()->OnHealthFractionChanged.IsBoundToObject(this)) {
						playerCharacter->GetHealthComponent()->OnHealthFractionChanged.AddUObject(this, &APlayerView::OnOtherPlayerHealthFractionChanged);
					}
				}
			}
		}
	}
}

APlayerView::APlayerView(const FObjectInitializer& objectInitializer)
	: Super(objectInitializer)
		, pDeferredPlayerCharacterSaveSlot(nullptr)
		, bActive(false)
		, SpinRestoreInterpolation(0.0f) {
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	EquipmentDisplayComponent = CreateDefaultSubobject<UEquipmentDisplayComponent>(TEXT("EquipmentDisplayComponent"));		
	CosmeticsDisplayComponent = CreateDefaultSubobject<UCosmeticsDisplayComponent>(TEXT("CosmeticsDisplayComponent"));
}

void APlayerView::PostInitializeComponents() {
	Super::PostInitializeComponents();

	{
		const auto components = GetComponentsByTag(USkeletalMeshComponent::StaticClass(), "3d-mesh");
		if (components.Num() > 0) {
			PlayerMesh = Cast<USkeletalMeshComponent>(components[0]);
		}
	}
	
	EquipmentDisplayComponent->SetDefaultAnimationsInstance(DefaultAnimationsInstance);
}

void APlayerView::BeginPlay() {
	Super::BeginPlay();
	TearOff();
	
	EquipmentDisplayComponent->SetOverridingPlayerMesh(PlayerMesh);

	CosmeticsDisplayComponent->SetOverridingPlayerMesh(PlayerMesh);

	if (pDeferredPlayerCharacterSaveSlot) {
		BindDeferredCharacterSaveSlot();
	}

}

void APlayerView::Tick(const float deltaSeconds) {
	Super::Tick(deltaSeconds);

	if (bViewedPlayerUpdated) {
		BindPlayerAvatarComponent(ViewedPlayer ? ViewedPlayer->GetPlayerAvatarComponent() : nullptr);
		BindEquipmentComponent(ViewedPlayer ? ViewedPlayer->GetEquipmentComponent() : nullptr);
		BindCosmeticsComponent(ViewedPlayer ? ViewedPlayer->GetCosmeticsComponent() : nullptr);
		RefreshWasDamagedBind();
		bViewedPlayerUpdated = false;
		
		//D11.RR - Cape hack for paused inventory
		if (ViewedPlayer)
		{
			if (!UOnlineUtil::IsOnlineSession() && ViewedPlayer->IsInInventory())
			{
				ReEquipCape();
				EquipCapeAfterDelay();
			}
		}				
	}
	
	if (bActive) {
		RestoreCharacterSpin(deltaSeconds);
	} else {
		SetActorTickEnabled(false);	
	}	
}

void APlayerView::RestoreCharacterSpin(const float deltaSeconds) {
	if (SpinRestoreInterpolation > 0) {		
		if (auto* playerRootPosition = GetPlayerRootPosition()) {
			playerRootPosition->SetRelativeRotation(CalculateSpinRestoreRotation(playerRootPosition));
		}		
	}

	SpinRestoreInterpolation = CalculateUpdatedSpinRestoreInterpolation(deltaSeconds);
}

FRotator APlayerView::CalculateSpinRestoreRotation(USceneComponent* playerRootPosition) const {
	const auto alpha = FMath::InterpEaseIn(0.0f, 1.0f, SpinRestoreInterpolation, 2.0f);
	return FMath::Lerp(playerRootPosition->RelativeRotation, FRotator(0.f, mPreferredRotation, 0.f), alpha);
}

float APlayerView::CalculateUpdatedSpinRestoreInterpolation(const float deltaSeconds) const {
	return FMath::Min(SpinRestoreInterpolation + deltaSeconds * SPIN_RESTORE_SPEED, 1.0f);
}

void APlayerView::BindPlayerAvatarComponent(UPlayerAvatarComponent* avatarComponent) {
	if (BoundAvatarComponent) {
		BoundAvatarComponent->OnApplyMeshMaterial.RemoveAll(this);
	}
	BoundAvatarComponent = avatarComponent;
	if (BoundAvatarComponent) {
		BoundAvatarComponent->OnApplyMeshMaterial.AddUObject(this, &APlayerView::ApplyMaterialToMesh);
		if (auto* material = USkinsUtil::GetSkinAvatarMaterialInstance(this, BoundAvatarComponent->GetSkinId())) {
			ApplyMaterialToMesh(material);
		}
	}
}

void APlayerView::ApplyMaterialToMesh(UMaterialInstance* material) const {
	if (PlayerMesh) {
		PlayerMesh->SetMaterial(0, material);
	}
}

void APlayerView::BindEquipmentComponent(UEquipmentComponent* equipmentComponent) {
	for (auto slot : BoundEquippableSlots) {
		if (slot) {
			slot->OnItemInstanceReplicatedInternal.RemoveAll(this);
			slot->OnWasSelectedInUIInternal.RemoveAll(this);
		}
	}
	BoundEquippableSlots.Reset();

	if (equipmentComponent) {
		EquipmentDisplayComponent->BindToGearSlots(equipmentComponent->GetGearSlots());		
	}

	CountReportedItemTypeCounts();

	if (equipmentComponent) {
		BoundEquippableSlots = equipmentComponent->GetEquippableSlots();
		for (auto slot : BoundEquippableSlots) {
			if (slot) {
				RefreshSlotItemTypeCounts(slot);
				slot->OnItemInstanceReplicatedInternal.AddUObject(this, &APlayerView::OnEquippableItemSlotChanged);
				slot->OnWasSelectedInUIInternal.AddUObject(this, &APlayerView::OnEquippableItemSlotSelected);
			}
		}
	}

	RefreshPrioritizedSlotVisibility();
}

void APlayerView::BindCosmeticsComponent(UCosmeticsComponent* cosmeticsComponent) {
	for (auto slot : BoundCosmeticsSlots) {
		if (slot) {
			slot->OnCosmeticsSlotUpdatedInternal.RemoveAll(this);
			slot->OnCosmeticsSlotEquippedInternal.RemoveAll(this);
			slot->OnCosmeticsSlotHighlightChangedInternal.RemoveAll(this);	
		}
	}

	BoundCosmeticsSlots.Reset();

	CountReportedCosmeticCounts();

	if (cosmeticsComponent) {
		BoundCosmeticsSlots = cosmeticsComponent->GetSlots();
		CosmeticsDisplayComponent->BindToSlots(BoundCosmeticsSlots);
		for(auto* slot : BoundCosmeticsSlots){
			RefreshSlotCosmeticCounts(slot);
			slot->OnCosmeticsSlotUpdatedInternal.AddUObject(this, &APlayerView::OnCosmeticSlotUpdated);
			slot->OnCosmeticsSlotEquippedInternal.AddUObject(this, &APlayerView::OnCosmeticSlotEquipped);
			slot->OnCosmeticsSlotHighlightChangedInternal.AddUObject(this, &APlayerView::OnCosmeticSlotHighlightChanged);			
		}

	}
}

void APlayerView::SetPlayerCharacter(APlayerCharacter* playerCharacter) {
	ViewedPlayer = playerCharacter;	
	bViewedPlayerUpdated = true;
	SetActorTickEnabled(true);
};

void APlayerView::SetPlayerCharacterSaveSlot(APlayerCharacterSaveSlot* playerCharacterSaveSlot) {
	
	//dont multiply attempt to bind the same deferred slot
	if (pDeferredPlayerCharacterSaveSlot == playerCharacterSaveSlot)
	{
		return;
	}

	pDeferredPlayerCharacterSaveSlot = playerCharacterSaveSlot;

	if (HasActorBegunPlay()) {
		BindDeferredCharacterSaveSlot();
	}
};

void APlayerView::BindDeferredCharacterSaveSlot() {
	BindPlayerAvatarComponent(pDeferredPlayerCharacterSaveSlot ? pDeferredPlayerCharacterSaveSlot->GetPlayerAvatarComponent() : nullptr);
	BindEquipmentComponent(pDeferredPlayerCharacterSaveSlot ? pDeferredPlayerCharacterSaveSlot->GetEquipmentComponent() : nullptr);
	BindCosmeticsComponent(pDeferredPlayerCharacterSaveSlot ? pDeferredPlayerCharacterSaveSlot->GetCosmeticsComponent() : nullptr);
	mPreferredRotation = 0.f;
}

bool APlayerView::IsSlotTypeDisplayable(ESlotType slot) const {
	switch (slot) {
	case ESlotType::MeleeWeapon:
	case ESlotType::RangedWeapon:
		return true;
	default:
		return false;
	}
}

void APlayerView::OnEquippableItemSlotSelected(const UItemSlot* itemSlot) {
	if (itemSlot->GetItem() != nullptr) {
		OnItemTypeSlotSelected(itemSlot->GetItem()->GetItemId());
		if(IsSlotTypeDisplayable(itemSlot->SlotTypeId)){
			SetPrioritizedSlotType(itemSlot->SlotTypeId);		
		}
	}
}

void APlayerView::SetPrioritizedSlotType(const ESlotType slot) {
	PrioritizedSlotType = slot;
	RefreshPrioritizedSlotVisibility();
}

void APlayerView::RefreshPrioritizedSlotVisibility() const {
	if (EquipmentDisplayComponent && EquipmentDisplayComponent->HasBegunPlay() && IsSlotTypeDisplayable(PrioritizedSlotType)) {		
		EquipmentDisplayComponent->SetCurrentWeapon(PrioritizedSlotType);		
	}
}

void APlayerView::OnUpdateItemMaterialSettings(UItemSlot* itemSlot, const FItemMaterialSettings itemMaterialSettings) const {
	if (PlayerMesh != nullptr) {
		UEquipmentDisplayComponent::UpdateItemMaterialSettings(PlayerMesh, itemSlot->SlotTypeId, itemMaterialSettings);
	}
}

void APlayerView::OnEquippableItemSlotChanged(UItemSlot* itemSlot) {
	RefreshSlotItemTypeCounts(itemSlot);
	SetPrioritizedSlotType(itemSlot->SlotTypeId);
	EquipmentDisplayComponent->UpdatePlayerAnimation();
	OnGearItemSlotChanged(itemSlot);
}

void APlayerView::OnPlayerHealthFractionChanged(float newFraction, float oldFraction) {
	if (newFraction > oldFraction) return;
	
	if (PlayerMesh){
		UDungeonsEffectLibrary::HitFlashOnMesh(PlayerMesh, FLinearColor(50.0f, 0.0f, 0.0f, 1.0f), 0.2f);
	}
	OnWasDamaged();
}

void APlayerView::OnOtherPlayerHealthFractionChanged(float newFraction, float oldFraction) {
	if (newFraction > oldFraction) return;
	OnOtherWasDamaged();
}

int APlayerView::CountTotalOfItemType(const FItemId& itemType) const {
	auto total = 0;
	if (BoundEquippableSlots.Num()) {
		for (const auto* slot : BoundEquippableSlots) {
			if (slot) {
				if (const auto* item = slot->GetItem()) {
					if (item->GetItemId() == itemType) {
						total++;
					}
				}
			}
		}
	}
	return total;
}

int APlayerView::CountTotalOfCosmetic(const FName& cosmeticName) const {
	auto total = 0;
	if (BoundCosmeticsSlots.Num()) {
		for (const auto* slot : BoundCosmeticsSlots) {
			if (slot) {
				if (slot->GetEquippedCosmetic() == cosmeticName) {
					total++;
				}
			}
		}
	}
	return total;
}

void APlayerView::CountReportedCosmeticCounts() {
	//Recount old reported counts
	//ensure calls of (OnCosmeticCountChanged > 0) will eventually get a (OnCosmeticCountChanged == 0) in blueprints.
	for (auto it = CosmeticCounts.CreateIterator(); it; ++it) {
		const auto newCount = CountTotalOfCosmetic(it->Key);
		SetCosmeticCount(it->Key, newCount);
		//Safely iterate over table remove zero counted types.
		if (newCount <= 0) {
			it.RemoveCurrent();
		}
	}
}

void APlayerView::RefreshSlotCosmeticCounts(UCosmeticsSlot* cosmeticSlot) {
	CountReportedCosmeticCounts();
	if (!cosmeticSlot->GetEquippedCosmetic().IsNone()) {
		SetCosmeticCount(cosmeticSlot->GetEquippedCosmetic(), CountTotalOfCosmetic(cosmeticSlot->GetEquippedCosmetic()));
	}
}

void APlayerView::CountReportedItemTypeCounts() {
	//Recount old reported counts
	//ensure calls of (OnItemTypeCountChanged > 0) will eventually get a (OnItemTypeCountChanged == 0) in blueprints.
	for (auto it = ItemTypeCounts.CreateIterator(); it; ++it) {
		const FItemId ItemKey = it->Key;
		const auto newCount = CountTotalOfItemType(ItemKey);
		SetItemTypeCount(ItemKey, newCount);
		//Safely iterate over table remove zero counted types.
		if (newCount <= 0) {
			it.RemoveCurrent();
		}
	}
}



void APlayerView::RefreshSlotItemTypeCounts(UItemSlot* itemSlot) {
	CountReportedItemTypeCounts();
	if (const auto* item = itemSlot->GetItem()) {
		SetItemTypeCount(item->GetItemId(), CountTotalOfItemType(item->GetItemId()));
	}
}

void APlayerView::OnCosmeticSlotUpdated(UCosmeticsSlot* cosmeticSlot) {
	RefreshSlotCosmeticCounts(cosmeticSlot);
}

void APlayerView::OnCosmeticSlotEquipped(UCosmeticsSlot* cosmeticSlot) {	
	if (auto def = cosmeticSlot->FindEquippedCosmeticDef()) {
		OnCosmeticItemEquipped(def);
	}
}

void APlayerView::OnCosmeticSlotHighlightChanged(UCosmeticsSlot* cosmeticSlot, UCosmeticItemDef* cosmetic) {
	OnCosmeticSlotItemHighlightChanged(cosmeticSlot, cosmetic);
}

void APlayerView::SetItemTypeCount(const FItemId& itemType, const int count) {
	auto* existingCount = ItemTypeCounts.Find(itemType);
	if (!existingCount || *existingCount != count) {
		ItemTypeCounts.Add(itemType, count);
		OnItemTypeCountChanged(itemType, count);
	}
}

void APlayerView::SetCosmeticCount(const FName& cosmeticName, int count) {
	auto* existingCount = CosmeticCounts.Find(cosmeticName);
	if (!existingCount || *existingCount != count) {
		CosmeticCounts.Add(cosmeticName, count);
		OnCosmeticCountChanged(cosmeticName, count);
	}
}

void APlayerView::ToggleActive(const bool active) {
	const auto activated = !bActive && active;

	bActive = active;

	if (activated) {
		OnActivated();
	} else {
		OnDeactivated();
	}
	RefreshWasDamagedBind();
}

void APlayerView::OnActivated() {
	SetActorTickEnabled(true);
}

void APlayerView::OnDeactivated() {
	if (!bViewedPlayerUpdated) {
		SetActorTickEnabled(false);
	}
	EquipmentDisplayComponent->BindToGearSlots({});
}

void APlayerView::SpinCharacter(const float rotation) {
	if (auto* playerRootPosition = GetPlayerRootPosition()) {
		const auto newRelativeRotation = game::playerView::calculateNewRelativeRotation(playerRootPosition->RelativeRotation, rotation);
		playerRootPosition->SetRelativeRotation(newRelativeRotation);
		ResetSpinRestoreInterpolation();
	}
}

void APlayerView::SetPreferredCharacterRotation(float rotation){
	mPreferredRotation = rotation;
	SpinRestoreInterpolation = 0.0f;
}

void APlayerView::ReEquipCape()
{
	for (UCosmeticsSlot* cosmetic : CosmeticsDisplayComponent->GetBoundSlots())
	{
		if (cosmetic->GetType() == ECosmeticType::Cape)
		{
			capeCosmeticSlot = cosmetic;
			capeCosmeticID = capeCosmeticSlot->GetEquippedCosmetic();
			if (capeCosmeticID != "None")
			{			
				capeCosmeticSlot->UnequipCosmetic();
			}
		}
	}	
}

void APlayerView::EquipCapeAfterDelay()
{	
	if(capeCosmeticID != "None")
		capeCosmeticSlot->EquipCosmetic(capeCosmeticID);		
}

void APlayerView::ResetSpinRestoreInterpolation() {
	SpinRestoreInterpolation = 0.0f;
}

USceneComponent* APlayerView::GetPlayerRootPosition() {
	return Cast<USceneComponent>(GetDefaultSubobjectByName(TEXT("PlayerRootPosition")));
}
