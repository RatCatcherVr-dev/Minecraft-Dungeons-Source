#include "Dungeons.h"
#include "CosmeticItemDef.h"
#include "util/Algo.hpp"
#include "util/ClassUtil.h"
#include <Sound/SoundCue.h>
#include <Animation/AnimSequenceBase.h>

UCosmeticItemDef::UCosmeticItemDef() {
}

UCosmeticItemDef* UCosmeticItemDef::CreateDefinition(const FName& dataName, FCosmeticsEntry* dataEntry) {
	if (dataEntry && dataEntry->IsReleased) {
		return CreateDefinition(dataName, *dataEntry);
	}
	return nullptr;
}

UCosmeticItemDef* UCosmeticItemDef::CreateDefinition(const FName& dataName, const FCosmeticsEntry& dataEntry) {
	const auto cosmeticItemDef = NewObject<UCosmeticItemDef>();
	cosmeticItemDef->DataName = dataName;
	cosmeticItemDef->DataEntry = dataEntry;
	return cosmeticItemDef;
}

TArray<UCosmeticItemDef*> UCosmeticItemDef::CreateDefinitions(const TMap<FName, FCosmeticsEntry>& dataEntries) {
	TArray<UCosmeticItemDef*> cosmeticItemDefs;

	for (const auto& pair : dataEntries) {
		cosmeticItemDefs.Add(CreateDefinition(pair.Key, pair.Value));
	}

	return cosmeticItemDefs;
}

bool UCosmeticItemDef::IsCorrectType(const ECosmeticType& type) const {
	const auto cosmeticType = GetCosmeticType();
	return cosmeticType != ECosmeticType::Invalid && cosmeticType == type;
}

FString UCosmeticItemDef::GetEntitlementName() const {
	return DataEntry->Entitlement;
}

ECosmeticType UCosmeticItemDef::GetCosmeticType() const {
	return DataEntry->Type;
}

TSubclassOf<ACosmeticItemInfo> UCosmeticItemDef::GetBlueprintClass() const {
	return DataEntry->BlueprintClass.LoadSynchronous();
}

UTexture2D* UCosmeticItemDef::GetCosmeticIconTexture() const {
	return DataEntry->IconTexture.LoadSynchronous();
}

USoundCue* UCosmeticItemDef::GetCosmeticEquippedSound() const {
	return DataEntry->EquippedSound.LoadSynchronous();
}

UAnimSequenceBase* UCosmeticItemDef::GetCosmeticEquippedAnimation() const {
	return DataEntry->EquippedAnimation.LoadSynchronous();
}


float UCosmeticItemDef::GetCosmeticInventoryRotation() const {
	return DataEntry->PreferredInventoryCharacterRotation;
}

const FText& UCosmeticItemDef::GetCosmeticDisplayDescription() const{
	return DataEntry->DisplayDescription;
}

const FText& UCosmeticItemDef::GetCosmeticDisplayName() const {
	return DataEntry->DisplayName;
}

const FName& UCosmeticItemDef::GetCosmeticId() const {
	return DataName;
}
