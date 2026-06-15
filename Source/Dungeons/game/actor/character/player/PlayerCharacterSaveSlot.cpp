#include "Dungeons.h"
#include "game/component/EquipmentComponent.h"
#include "game/component/CharacterSerializeComponent.h"
#include "game/avatar/PlayerAvatarComponent.h"
#include "PlayerCharacterSaveSlot.h"
#include "save/CharacterSaveData.h"
#include "DungeonsGameInstance.h"
#include "game/component/cosmetics/CosmeticsComponent.h"
#include "game/skins/SkinsUtil.h"


APlayerCharacterSaveSlot::APlayerCharacterSaveSlot(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)

{
	// for each actor that needs an Equipment component :<<<<
	CreateDefaultSubobject<UItemSlot>(TEXT("MeleeWeaponItemSlot"))->SlotTypeId = ESlotType::MeleeWeapon;
	CreateDefaultSubobject<UItemSlot>(TEXT("ArmorItemSlot"))->SlotTypeId = ESlotType::Armor;
	CreateDefaultSubobject<UItemSlot>(TEXT("RangedWeaponItemSlot"))->SlotTypeId = ESlotType::RangedWeapon;
	CreateDefaultSubobject<UItemSlot>(TEXT("ArtifactItemSlot1"))->SlotTypeId = ESlotType::ActivePermanent;
	CreateDefaultSubobject<UItemSlot>(TEXT("ArtifactItemSlot2"))->SlotTypeId = ESlotType::ActivePermanent;
	CreateDefaultSubobject<UItemSlot>(TEXT("ArtifactItemSlot3"))->SlotTypeId = ESlotType::ActivePermanent;

	CosmeticCapeSlot = CreateDefaultSubobject<UCosmeticsSlot>(TEXT("CapeSlot"));
	CosmeticCapeSlot->SetType(ECosmeticType::Cape);
	CosmeticPetSlot = CreateDefaultSubobject<UCosmeticsSlot>(TEXT("PetSlot"));
	CosmeticPetSlot->SetType(ECosmeticType::Pet);

	// #D11.CM - These save slots don't need collision data.
	SetActorEnableCollision(false);

	CharacterSerializeComponent = CreateDefaultSubobject<UCharacterSerializeComponent>(TEXT("CharacterSerializeComponent"));
	ItemStashComponent = CreateDefaultSubobject<UItemStashComponent>(TEXT("ItemStashComponent"));
	EquipmentComponent = CreateDefaultSubobject<UEquipmentComponent>(TEXT("EquipmentComponent"));
	PlayerAvatarComponent = CreateDefaultSubobject<UPlayerAvatarComponent>(TEXT("PlayerAvatarComponent"));
	CosmeticsComponent = CreateDefaultSubobject<UCosmeticsComponent>(TEXT("CosmeticsComponent"));

	bReplicates = false;
	PrimaryActorTick.bCanEverTick = false;

	mCloudUploadeDateTime = FDateTime::UtcNow();
}

void APlayerCharacterSaveSlot::AssignSaveData(UCharacterSaveData* data) const {
	CharacterSerializeComponent->AssignCharacter(data);

	// we aren't a pawn, but this is the same intent!
	ItemStashComponent->OnLocalPawnPossessed();
	CosmeticsComponent->OnLocalPawnPossessed();
	PlayerAvatarComponent->OnLocalPawnPossessed();
}

TArray<UCosmeticItemDef*> APlayerCharacterSaveSlot::GetCosmeticDefinitions()
{
	auto pCharacterSerializeComponent = GetCharacterSerializeComponent();
	UCharacterSaveData* pSaveData = pCharacterSerializeComponent->GetSaveData();
	auto pInstance = GetWorld()->GetGameInstance<UDungeonsGameInstance>();
	auto pCosmeticsLibrary = pInstance->GetCosmeticsLibrary();
	auto CosmeticDefinitions = pCosmeticsLibrary->FindDefinitions(pSaveData->mRecordedData.cosmetics);
	return CosmeticDefinitions;
}

bool APlayerCharacterSaveSlot::IsDLCRequired() {
	FString FreeDLCEntitlement = "game_dungeons";
	for (auto pCosmeticDefinition : GetCosmeticDefinitions())
	{
		auto EntitlementName = pCosmeticDefinition->GetEntitlementName();
		if (!EntitlementName.IsEmpty() && EntitlementName != FreeDLCEntitlement )
			return true;
	}
	bool bSkinRequried = USkinsUtil::IsEntitlementRequired(GetWorld(), GetCharacterSkinId_Implementation());
	return bSkinRequried;
}

void APlayerCharacterSaveSlot::StripDLCNotEntitled()
{
	auto pInstance = GetWorld()->GetGameInstance<UDungeonsGameInstance>();
	auto pEntitlements = pInstance->GetEntitlementsRepository();
	
	auto pCharacterSerializeComponent = GetCharacterSerializeComponent();
	UCharacterSaveData* pSaveData = pCharacterSerializeComponent->GetSaveData();
	auto& RecordededCosmectics = pSaveData->mRecordedData.cosmetics;

	auto CosmeticDefintions = GetCosmeticDefinitions();
	for (auto pCosmeticDefinition : CosmeticDefintions)
	{
		auto EntitlementName = pCosmeticDefinition->GetEntitlementName();
		auto HasEntitlement = pEntitlements->GetEntitlement(EntitlementName);
		if (!HasEntitlement)
		{
			auto CosmeticId = pCosmeticDefinition->GetCosmeticId();
			auto pFound = RecordededCosmectics.FindByPredicate([&](const CosmeticsSaveData &c) { return c.id == CosmeticId; });
			if (pFound)
				RecordededCosmectics.RemoveAt(pFound-&RecordededCosmectics[0]);
		}
	}
	auto ThisSkin = GetCharacterSkinId_Implementation();
	bool bIsSkinEntitled = USkinsUtil::IsSkinEntitled(GetWorld(), ThisSkin);
	if (!bIsSkinEntitled)
	{
		auto ThatSkin = rand() % 2 == 0 ? "Steve" : "Alex";
		ChangeSkinId(ThatSkin);
	}
}

bool APlayerCharacterSaveSlot::IsDLCEntitled() {
	auto pInstance = GetWorld()->GetGameInstance<UDungeonsGameInstance>();
	auto pEntitlements = pInstance->GetEntitlementsRepository();
	for (auto pCosmeticDefinition : GetCosmeticDefinitions())
	{
		auto EntitlementName = pCosmeticDefinition->GetEntitlementName();
		auto HasEntitlement = pEntitlements->GetEntitlement(EntitlementName);
		if (!HasEntitlement)
			return false;
	}
	bool bIsSkinEntitled = USkinsUtil::IsSkinEntitled(GetWorld(), GetCharacterSkinId_Implementation());
	return bIsSkinEntitled;
}

int32 APlayerCharacterSaveSlot::GetLevel_Implementation() const {
	return UDungeonsGameInstance::createCharacterLevel(CharacterSerializeComponent->ReadXP()).level;
}

int32 APlayerCharacterSaveSlot::GetTotalEquippedGearPower_Implementation() const {
	return EquipmentComponent->GetTotalEquippedDisplayItemPower();
}

int32 APlayerCharacterSaveSlot::GetOwnedEmeralds_Implementation() const {
	return static_cast<int32>(CharacterSerializeComponent->ReadCurrencyFor(game::item::type::Emerald.getId()));
}

int32 APlayerCharacterSaveSlot::GetOwnedGold_Implementation() const {
	return static_cast<int32>(CharacterSerializeComponent->ReadCurrencyFor(game::item::type::Gold.getId()));
}

FText APlayerCharacterSaveSlot::GetCharacterName_Implementation() const {
	return FText::FromString(CharacterSerializeComponent->ReadName());
}

FName APlayerCharacterSaveSlot::GetCharacterSkinId_Implementation() const {
	return CharacterSerializeComponent->ReadSkin();
}

FDateTime APlayerCharacterSaveSlot::GetCloudUploadeDateTime() const {
	return mCloudUploadeDateTime;
}

FDateTime APlayerCharacterSaveSlot::GetDateTime() const {
	return GetCloudUploadeDateTime();
}

void APlayerCharacterSaveSlot::SetCloudUploadeDateTime(FDateTime DateTime){
	mCloudUploadeDateTime = DateTime;
}

void APlayerCharacterSaveSlot::UpdateDateTimeToNow()
{
	SetCloudUploadeDateTime(FDateTime::UtcNow());
}

bool APlayerCharacterSaveSlot::IsLocallyControlled() const {
	return true;
}

UEquipmentComponent* APlayerCharacterSaveSlot::GetEquipmentComponent() const {
	return EquipmentComponent;
}

UPlayerAvatarComponent* APlayerCharacterSaveSlot::GetPlayerAvatarComponent() const {
	return PlayerAvatarComponent;
}

UCosmeticsComponent* APlayerCharacterSaveSlot::GetCosmeticsComponent() const {
	return CosmeticsComponent;
}

void APlayerCharacterSaveSlot::ChangeSkinId(FName skinId) {
	CharacterSerializeComponent->SetSkin(skinId);
	PlayerAvatarComponent->OnLocalPawnPossessed();
	OnSaveSlotDataChanged.Broadcast();
	analytics::Analytics::GetInstance().FireEventSkinSelected(GetName(), skinId.ToString());
}

void APlayerCharacterSaveSlot::ChangeName(FString Name){
	CharacterSerializeComponent->SetName(Name);
	OnSaveSlotDataChanged.Broadcast();
}

void APlayerCharacterSaveSlot::ChangeLegendaryStatus(ELegendaryStatus status) {
	CharacterSerializeComponent->SetLegendaryStatus(status);
	OnSaveSlotDataChanged.Broadcast();
}

ELegendaryStatus APlayerCharacterSaveSlot::GetLegendaryStatus() {
	return CharacterSerializeComponent->GetLegendaryStatus();
}

UItemStashComponent* APlayerCharacterSaveSlot::GetItemStashComponent() const {
	return ItemStashComponent;
}

FGuid APlayerCharacterSaveSlot::GetCloudPlayerId()
{
	return CharacterSerializeComponent->GetCloudPlayerId();
}


void APlayerCharacterSaveSlot::IgnoreEntitlements()
{
	GetPlayerAvatarComponent()->SetIgnoreEntitlement(true);
	CosmeticCapeSlot->SetIgnoreEntitlement(true);
	CosmeticPetSlot->SetIgnoreEntitlement(true);
}

UCharacterSerializeComponent* APlayerCharacterSaveSlot::GetCharacterSerializeComponent()
{
	return CharacterSerializeComponent;
}

