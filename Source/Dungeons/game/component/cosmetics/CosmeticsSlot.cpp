#include "Dungeons.h"
#include "CosmeticsComponent.h"
#include "CosmeticsSlot.h"
#include "game/cosmetics/CosmeticItemDef.h"
#include "game/cosmetics/CosmeticsItemSpawner.h"
#include "UnrealNetwork.h"
#include "online/entitlements/Entitlement.h"
#include <Themida/Anticheat.hpp>

UCosmeticsSlot::UCosmeticsSlot()
	: Type(ECosmeticType::Invalid)	
	, Item(nullptr) {
	bReplicates = true;
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UCosmeticsSlot::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UCosmeticsSlot, Cosmetic);
}

ECosmeticType UCosmeticsSlot::GetType() const {
	return Type;
}

void UCosmeticsSlot::SetType(const ECosmeticType type) {
	Type = type;
}

ACosmeticItemInfo* UCosmeticsSlot::GetItem() const {
	return Item;
}

const FName& UCosmeticsSlot::GetEquippedCosmetic() const {	
	return Cosmetic.CosmeticId;	
}

const FEntitlement& UCosmeticsSlot::GetEntitlement() const {
	return Cosmetic.Entitlement;
}

bool UCosmeticsSlot::IsCosmeticEquipped(UCosmeticItemDef* cosmeticDefinition) const {
	return (cosmeticDefinition ? cosmeticDefinition->GetCosmeticId() : NAME_None) == Cosmetic.CosmeticId;
}

UCosmeticItemDef* UCosmeticsSlot::FindEquippedCosmeticDef() const {
	if (!Cosmetic.CosmeticId.IsNone()) {
		if (auto* cosmeticsComponent = GetOwner()->FindComponentByClass<UCosmeticsComponent>()) {			
			return cosmeticsComponent->FindCosmeticDefinitionById(Cosmetic.CosmeticId);
		}
	}
	return nullptr;
}

UCosmeticItemDef* UCosmeticsSlot::FindCosmeticDef(const FName& cosmeticId) const {
	if (auto* cosmeticsComponent = GetOwner()->FindComponentByClass<UCosmeticsComponent>()) {			
		return cosmeticsComponent->FindCosmeticDefinitionById(cosmeticId);
	}
	return nullptr;
}

TArray<UCosmeticItemDef*> UCosmeticsSlot::GetAvailableCosmetics() const {
	if (auto* cosmeticsComponent = GetOwner()->FindComponentByClass<UCosmeticsComponent>()) {
		return cosmeticsComponent->FindAvailableCosmeticTypeDefinitions(GetType());
	}
	return {};
}

void UCosmeticsSlot::EquipCosmetic(const FName& cosmeticId) {
	EquippingCosmeticId = cosmeticId;
	SetCosmetic(cosmeticId);
}

ANTICHEAT_NO_OPTIMIZATION_BEGIN
TOptional<FEntitlement> UCosmeticsSlot::GetEntitlementForAvailableCosmetic(const FName& cosmeticId) const {
	ANTICHEAT_OBFUSCATE_BEGIN
	if (const auto* def = FindCosmeticDef(cosmeticId)) {
		return GetWorld()->GetGameInstance<UDungeonsGameInstance>()->GetEntitlementsRepository()->GetEntitlement(def->GetEntitlementName());
	}
	return TOptional<FEntitlement>();
	ANTICHEAT_OBFUSCATE_END
}
ANTICHEAT_NO_OPTIMIZATION_END

void UCosmeticsSlot::UnequipCosmetic(){
	EquippingCosmeticId = NAME_None;
	ServerSetCosmetic(NAME_None, {});
}

void UCosmeticsSlot::ClearCosmetic() {	
	ServerSetCosmetic(NAME_None, {});
}

void UCosmeticsSlot::SetCosmetic(const FName& cosmeticId) {
	ANTICHEAT_OBFUSCATE_BEGIN

	if (IgnoreEntitlement) //D11.PS we want to ignore it in situations like Cloud save preview.
	{
		Cosmetic.CosmeticId = cosmeticId;
	}
	else if (const auto entitlement = GetEntitlementForAvailableCosmetic(cosmeticId)) {
		ServerSetCosmetic(cosmeticId, entitlement.GetValue());
	}
	ANTICHEAT_OBFUSCATE_END
}

bool UCosmeticsSlot::IsCosmeticNew(UCosmeticItemDef* cosmeticDefinition) const {
	if (cosmeticDefinition){
		if (auto* cosmeticsComponent = GetOwner()->FindComponentByClass<UCosmeticsComponent>()) {
			return !cosmeticsComponent->HasEverEquippedCosmetic(cosmeticDefinition->GetCosmeticId());
		}
	}
	return false;
}

void UCosmeticsSlot::OnHighlightedInUi(UCosmeticItemDef* cosmeticDefinition) {
	OnCosmeticsSlotHighlightChangedInternal.Broadcast(this, cosmeticDefinition);	
}

void UCosmeticsSlot::ServerSetCosmetic_Implementation(const FName& equippedCosmeticId, const FEntitlement& entitlement) {
	Cosmetic = FEntitledCosmetic(equippedCosmeticId, entitlement);
	OnRep_CosmeticReplicated();
}

ANTICHEAT_NO_OPTIMIZATION_BEGIN
bool UCosmeticsSlot::ServerSetCosmetic_Validate(const FName& equippedCosmeticId, const FEntitlement& entitlement) {
	ANTICHEAT_OBFUSCATE_BEGIN
	return equippedCosmeticId.IsNone() || IsValidEntitlement(equippedCosmeticId, entitlement);
	ANTICHEAT_OBFUSCATE_END
}
ANTICHEAT_NO_OPTIMIZATION_END

ANTICHEAT_NO_OPTIMIZATION_BEGIN
void UCosmeticsSlot::OnRep_CosmeticReplicated() {
	ANTICHEAT_OBFUSCATE_BEGIN
	const auto validateEntitlement = Cosmetic.CosmeticId != NAME_None && !GetOwner()->HasAuthority();
	if (validateEntitlement && !IsValidEntitlement(Cosmetic.CosmeticId, Cosmetic.Entitlement)) {
		UnequipCosmetic();
	} else {
		UpdateItem();
	}
	ANTICHEAT_OBFUSCATE_END
}
ANTICHEAT_NO_OPTIMIZATION_END

void UCosmeticsSlot::UpdateItem() {
	if (Item) {
		Item->Unequip();
		Item->Destroy();		
		Item = nullptr;
	}
	if (const auto* def = FindEquippedCosmeticDef()) {
		Item = game::cosmetics::spawnCosmeticItemInfo(GetWorld(), GetOwner(), def->GetBlueprintClass());
	}	
	OnCosmeticsSlotUpdatedInternal.Broadcast(this);
	OnCosmeticsSlotItemUpdated.Broadcast();

	if (EquippingCosmeticId.IsSet() && EquippingCosmeticId.GetValue() == GetEquippedCosmetic()) {
		OnCosmeticsSlotEquippedInternal.Broadcast(this);
	}	
}

ANTICHEAT_NO_OPTIMIZATION_BEGIN
bool UCosmeticsSlot::IsValidEntitlement(const FName& cosmeticId, const FEntitlement& entitlement) const {
	ANTICHEAT_OBFUSCATE_BEGIN
	if (const auto* itemDefinition = FindCosmeticDef(cosmeticId)) {
		if (const auto* player = GetPlayerOwner()) {
			if (const auto playerId = player->GetOnlineUserId()) {
				auto* repository = GetWorld()->GetGameInstance<UDungeonsGameInstance>()->GetEntitlementsRepository();
				return !repository->IsEntitlementTamperedWith(entitlement, itemDefinition->GetEntitlementName(), playerId.GetValue());
			}			
		}
		return true; //Not a player, or not online - always valid.
	}	
	return false;
	ANTICHEAT_OBFUSCATE_END
}
ANTICHEAT_NO_OPTIMIZATION_END

APlayerCharacter* UCosmeticsSlot::GetPlayerOwner() const {
	return Cast<APlayerCharacter>(GetOwner());		
}
