#include "Dungeons.h"
#include "CosmeticsComponent.h"
#include "game/component/CharacterSerializeComponent.h"
#include "DungeonsGameInstance.h"
#include "game/cosmetics/CosmeticItemDef.h"
#include "game/cosmetics/CosmeticsEntitlementChecker.h"
#include "save/CharacterSaveData.h"
#include <Themida/Anticheat.hpp>

UCosmeticsComponent::UCosmeticsComponent() {
	bReplicates = true;
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UCosmeticsComponent::BeginPlay() {
	Super::BeginPlay();

	LoadSlots();
	
	if (IsOwnedByLocalPlayer()) {
		GetDungeonsGameInstance()->GetEntitlementsRepository()->OnEntitlementsProvided.AddUObject(this, &UCosmeticsComponent::OnEntitlementsProvided);
	}

	LoadSlots();
}

bool UCosmeticsComponent::IsOwnedByLocalPlayer() const {
	if (const auto* owner = Cast<APawn>(GetOwner())) {
		return owner->IsLocallyControlled();
	}	
	return false;
}

void UCosmeticsComponent::EndPlay(const EEndPlayReason::Type endPlayReason) {
	Super::EndPlay(endPlayReason);
	ClearSlots();
}

void UCosmeticsComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UCosmeticsComponent, Slots);
}

void UCosmeticsComponent::OnLocalPawnPossessed() {
	LoadSlots();
	
	DeserializeSaveState();

	for (auto* slot : GetSlots()) {
		slot->OnCosmeticsSlotEquippedInternal.AddUObject(this, &UCosmeticsComponent::OnCosmeticSlotEquippedCosmetic);
	}
}

void UCosmeticsComponent::LoadSlots() {
	if (Slots.Num() == 0) {
		for (auto component : GetOwner()->GetComponentsByClass(UCosmeticsSlot::StaticClass())) {
			Slots.Add(Cast<UCosmeticsSlot>(component));
		}
	}
}

const TArray<UCosmeticsSlot*>& UCosmeticsComponent::GetSlots() const {
	return Slots;
}

ANTICHEAT_NO_OPTIMIZATION_BEGIN
const TArray<UCosmeticItemDef*> UCosmeticsComponent::FindAvailableCosmeticTypeDefinitions(ECosmeticType cosmeticType) const {
	ANTICHEAT_OBFUSCATE_BEGIN
	if (const auto* cosmeticsLibrary = GetDungeonsGameInstance()->GetCosmeticsLibrary()) {
		const auto predicate = [cosmeticType](const UCosmeticItemDef& def) {
			return def.GetCosmeticType() == cosmeticType;
		};
		const auto cosmetics = cosmeticsLibrary->FindDefinitions(predicate);
		return game::cosmetics::filterByEntitlements(cosmetics, GetEntitlements());
	}
	return {};
	ANTICHEAT_OBFUSCATE_END
}
ANTICHEAT_NO_OPTIMIZATION_END

void UCosmeticsComponent::Equip(const FName& cosmeticId) const {
	if (const auto* cosmetic = FindCosmeticDefinitionById(cosmeticId)){
		if (auto* slot = GetFirstSlotOfType(cosmetic->GetCosmeticType())) {
			slot->EquipCosmetic(cosmeticId);
		}
	}
}

void UCosmeticsComponent::UnequipAll() {
	UnequipSlots();
}

void UCosmeticsComponent::Unequip(const ECosmeticType cosmeticType) {
	for (auto* slot : Slots) {
		if (slot->GetType() == cosmeticType) {
			slot->UnequipCosmetic();
		}
	}	
}

void UCosmeticsComponent::OnCosmeticSlotEquippedCosmetic(UCosmeticsSlot* slot){	
	if (auto* characterSerializer = GetOwner()->FindComponentByClass<UCharacterSerializeComponent>()) {
		const auto cosmeticId = slot->GetEquippedCosmetic();
		if (!cosmeticId.IsNone()){
			characterSerializer->SetCosmeticEquipped(cosmeticId);
		}
	}
	SerializeSaveState();
}

bool UCosmeticsComponent::HasEverEquippedCosmetic(const FName& cosmeticId) const {
	if (auto* characterSerializer = GetOwner()->FindComponentByClass<UCharacterSerializeComponent>()) {
		return characterSerializer->HasEverEquippedCosmetic(cosmeticId);
	}
	return false;
}

bool UCosmeticsComponent::HasAnyCosmetics() const {
	return Slots.ContainsByPredicate(RETLAMBDA(it->GetAvailableCosmetics().Num() > 0));
}

void UCosmeticsComponent::OnEntitlementsProvided(const TArray<FEntitlement>& entitlements) const {
	DeserializeSaveState();
}

UCosmeticsSlot* UCosmeticsComponent::GetIndexedSlotOfType(const ECosmeticType cosmeticType, int index) const {	
	const auto filteredSlots = algo::copy_if(Slots, RETLAMBDA(it->GetType() == cosmeticType));	
	if (filteredSlots.IsValidIndex(index)) {
		return filteredSlots[index];
	}	
	return nullptr;
}

UCosmeticsSlot* UCosmeticsComponent::GetFirstSlotOfType(const ECosmeticType cosmeticType) const {
	return GetIndexedSlotOfType(cosmeticType, 0);
}

void UCosmeticsComponent::SerializeSaveState() {
	TArray<CosmeticsSaveData> saveData;
	for (auto& slot : Slots) {
		auto& equippedCosmetic = slot->GetEquippedCosmetic();
		if(!equippedCosmetic.IsNone()){
			saveData.Add(CosmeticsSaveData(equippedCosmetic, slot->GetType()));
		}
	}
	
	SerializeSaveState(saveData);
}

void UCosmeticsComponent::SerializeSaveState(const TArray<UCosmeticItemDef*>& cosmetics) const {
	TArray<CosmeticsSaveData> saveData;
	for (auto& cosmetic : cosmetics) {
		saveData.Add(CosmeticsSaveData(cosmetic->GetCosmeticId(), cosmetic->GetCosmeticType()));
	}
	
	SerializeSaveState(saveData);
}

void UCosmeticsComponent::SerializeSaveState(const TArray<CosmeticsSaveData>& cosmetics) const {
	if (GetOwner()->GetGameInstance()->IsDedicatedServerInstance()) {
		return;	
	}

	if (auto* characterSerializer = GetOwner()->FindComponentByClass<UCharacterSerializeComponent>()) {		
		characterSerializer->SetCosmetics(cosmetics);
	} else {
		UE_LOG(LogTemp, Error, TEXT("Missing serializer, failed to save cosmetics."));
	}
}

void UCosmeticsComponent::DeserializeSaveState() const {
	if (GetOwner()->GetGameInstance()->IsDedicatedServerInstance()) {
		return;
	}

	if (auto* characterSerializer = GetOwner()->FindComponentByClass<UCharacterSerializeComponent>()) {		
		LoadCosmeticsIntoSlots(characterSerializer->GetCosmetics());
	} else {
		UE_LOG(LogTemp, Error, TEXT("Missing serializer, failed to load cosmetics."));
	}
}

void UCosmeticsComponent::ClearSlots() {
	for (auto* slot : Slots) {
		slot->ClearCosmetic();
	}
}

void UCosmeticsComponent::UnequipSlots() {
	for (auto* slot : Slots) {
		slot->UnequipCosmetic();
	}
}

///Cosmetics of the same type try to equip to multiple different cosmetics slots of the same type if they exist
void UCosmeticsComponent::LoadCosmeticsIntoSlots(const TArray<CosmeticsSaveData>& cosmetics) const {
	TMap<ECosmeticType, int> loadedCosmeticMap;
	for (const auto& cosmetic : cosmetics) {
		auto cosmeticType = cosmetic.type;

		int numLoadedOfType = 0;
		auto* foundValue = loadedCosmeticMap.Find(cosmeticType);
		if (foundValue) {
			numLoadedOfType = *foundValue;
		}

		if (auto* slot = GetIndexedSlotOfType(cosmeticType, numLoadedOfType)) {
			slot->SetCosmetic(cosmetic.id);
			loadedCosmeticMap.Add(cosmeticType, numLoadedOfType + 1);
		}
	}
}

UCosmeticItemDef* UCosmeticsComponent::FindCosmeticDefinitionById(const FName& cosmeticId) const {
	if (const auto* cosmeticsLibrary = GetDungeonsGameInstance()->GetCosmeticsLibrary()){
		return cosmeticsLibrary->FindDefinition(cosmeticId);
	}
	return nullptr;
}

TArray<FEntitlement> UCosmeticsComponent::GetEntitlements() const {
	return GetDungeonsGameInstance()->GetEntitlementsRepository()->GetEntitlements();
}

UDungeonsGameInstance* UCosmeticsComponent::GetDungeonsGameInstance() const {
	return GetWorld()->GetGameInstance<UDungeonsGameInstance>();
}

bool UCosmeticsComponent::DoesOwnerHaveAuthority() const {
	return GetOwner()->HasAuthority();
}
