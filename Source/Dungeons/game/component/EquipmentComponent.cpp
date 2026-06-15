#include "Dungeons.h"
#include "EquipmentComponent.h"
#include "UnrealNetwork.h"
#include "HealthComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/item/power/ItemPowerUtil.h"
#include "game/item/power/ItemPowerStats.h"
#include "game/mission/MissionDefs.h"
#include "game/item/instance/AItemInstance.h"
#include "mode/RespawnAsTeamUsingLivesComponent.h"
#include "game/GameBP.h"

UEquipmentComponent::UEquipmentComponent() {
	bReplicates = true;
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UEquipmentComponent::BeginPlay() {
	Super::BeginPlay();

	for (auto slot : GetEquippableSlots()) {
		slot->OnItemInstanceReplicatedInternal.AddUObject(this, &UEquipmentComponent::OnEquippableItemSlotUpdated);		
	}

	if (auto player = Cast<APlayerCharacter>(GetOwner())) {		
		player->OnPlayerDown.AddUObject(this, &UEquipmentComponent::OnPlayerDown);
		if (auto* healthComponent = GetOwner()->FindComponentByClass<UHealthComponent>()) {
			healthComponent->OnRevive.AddUObject(this, &UEquipmentComponent::OnRespawn);
		}		
	}

	if (AGameBP* game = actorquery::getFirstActor<AGameBP>(GetWorld())) {
		game->OnDifficultyIncreased.AddDynamic(this, &UEquipmentComponent::OnMissionDifficultyIncreased);
	}

	RefreshTotalEquippedDisplayItemPower();
}

int UEquipmentComponent::CalculateTotalEquippedDisplayItemPower() const {
	float summedPower = 0.0f;
	float totalItemWeights = 0.0f;
	for (auto&& slot : GetEquippableSlots()) {
		float powerWeight = 1.0f;
		if (slot->SlotTypeId == ESlotType::ActivePermanent) {
			powerWeight = game::item::power::ARTIFACT_POWER_IMPORTANCE;
		}
		totalItemWeights += powerWeight;

		if (slot->GetItem()) {
			summedPower += UItemPowerUtil::GetItemPowerDisplayInt(slot->GetItem()->GetItemPower()) * powerWeight;
		}
	}

	const float averagePower = summedPower / totalItemWeights;

	return FMath::RoundToInt(averagePower);	
}

void UEquipmentComponent::RefreshTotalEquippedDisplayItemPower() const {
	int roundedPower = CalculateTotalEquippedDisplayItemPower();
	if (roundedPower != mCachedEquippedDisplayItemPower) {
		mCachedEquippedDisplayItemPower = roundedPower;
		OnTotalEquippedItemPowerChanged.Broadcast();
		OnTotalEquippedItemPowerChangedInternal.Broadcast();
	}
}

void UEquipmentComponent::EndPlay(const EEndPlayReason::Type endPlayReason) {
	Super::EndPlay(endPlayReason);
	EquipmentSlots.Empty();
}

void UEquipmentComponent::LazyLoadSlots() const {
	EquipmentSlots.Empty();
	const auto owner = GetOwner();
	const TSet<UActorComponent*>& equipmentComps = owner->GetComponents();
	for (auto component : equipmentComps) {
		if (component->IsA(UItemSlot::StaticClass())) {
			EquipmentSlots.Add(Cast<UItemSlot>(component));
		}
	}
}

void UEquipmentComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	DOREPLIFETIME(UEquipmentComponent, EquipmentSlots);
}

TArray<UItemSlot*> UEquipmentComponent::GetSlotsOfType(ESlotType type) const {
	if (!EquipmentSlots.Num()) {
		LazyLoadSlots();
	}

	if (type == ESlotType::Any) {
		return EquipmentSlots;
	}

	return EquipmentSlots.FilterByPredicate([&](const UItemSlot* v) { return (v && v->SlotTypeId == type); });
}

UItemSlot& UEquipmentComponent::GetSlot(const ESlotType& slotType) const {
	check(slotType != ESlotType::Any && slotType != ESlotType::None);
	return *GetSlotsOfType(slotType)[0];
}

UItemSlot* UEquipmentComponent::GetFirstSlotOfType(const ESlotType& slotType) const {
	auto slots = GetSlotsOfType(slotType);
	if (slots.IsValidIndex(0)) {
		return slots[0];
	}
	return nullptr;
}

TArray<UItemSlot*> UEquipmentComponent::GetEquipmentSlots() const {
	return EquipmentSlots;
}

TArray<UItemSlot*> UEquipmentComponent::GetGearSlots() const {
	return GetSlots(IsGearSlot);
}

TArray<UItemSlot*> UEquipmentComponent::GetEquippableSlots() const {
	return GetSlots(IsEquippableSlot);
}

void UEquipmentComponent::OnPawnPossessed() const {
	if (!EquipmentSlots.Num()) {
		LazyLoadSlots();
	}
}

void UEquipmentComponent::ResetEquipment() const {
	for (auto slot : EquipmentSlots) {
		slot->ResetSlot();
	}
}

void UEquipmentComponent::OnEquippableItemSlotUpdated(UItemSlot* itemSlot) const {
	RefreshTotalEquippedDisplayItemPower();
	OnAnySlotChanged.Broadcast(itemSlot);
}

void UEquipmentComponent::OnPlayerDown() const {
	auto player = Cast<APlayerCharacter>(GetOwner());

	if (!(player && URespawnAsTeamUsingLivesComponent::HasPlayerForcedDownState(GetWorld(), player))) {
		for (auto slot : EquipmentSlots) {
			auto* owner = GetOwner();
			if (slot->GetItem() && owner && owner->HasAuthority()) {
				slot->GetItem()->DeactivateEquippedEffects();
			}
		}
	}
}

void UEquipmentComponent::OnRespawn() const {
	auto* character = Cast<ABaseCharacter>(GetOwner());
	check(character && "owner of equipmentcomponent must be character!");

	auto player = Cast<APlayerCharacter>(character);

	if (!(player && URespawnAsTeamUsingLivesComponent::HasPlayerForcedDownState(GetWorld(), player))) {
		for (auto slot : EquipmentSlots) {
			if (slot->GetItem() && character->HasAuthority()) {
				slot->GetItem()->ActivateEquippedEffects();
			}
		}
	}
}

TArray<UItemSlot*> UEquipmentComponent::GetSlots(std::function<bool(ESlotType type)> predicate) const {
	if (!EquipmentSlots.Num()) {
		LazyLoadSlots();
	}

	return EquipmentSlots.FilterByPredicate([&](const UItemSlot* v) { return (v && predicate(v->SlotTypeId)); });
}

bool UEquipmentComponent::IsGearSlot(const ESlotType& type) {
	switch (type) {
	case ESlotType::MeleeWeapon:
	case ESlotType::RangedWeapon:
	case ESlotType::Armor:
		return true;
	default:
		return false;
	}
}

bool UEquipmentComponent::IsEquippableSlot(const ESlotType& type) {
	switch (type) {
	case ESlotType::MeleeWeapon:
	case ESlotType::RangedWeapon:
	case ESlotType::Armor:
	case ESlotType::ActivePermanent:
		return true;
	default:
		return false;
	}
}

int UEquipmentComponent::GetTotalEquippedDisplayItemPower() const {
	return mCachedEquippedDisplayItemPower;
}

int UEquipmentComponent::GetMissionStartingEquippedGearPower() const {
	return mMissionStartingGearPower;
}

bool UEquipmentComponent::HasHealthPotionEquipped() const 
{
	return GetSlot(ESlotType::HealthPotion).GetItem() != nullptr;
}

void UEquipmentComponent::SetMissionStartingEquippedGearPower(int startingGearPower) {
	mMissionStartingGearPower = startingGearPower;
	RefreshDifficultyRecommendation();
}

void UEquipmentComponent::RefreshDifficultyRecommendation() {
	TOptional<FEmergentDifficulty> emergentDifficulty;
	if (AGameBP* gameBP = actorquery::getFirstActor<AGameBP>(GetWorld())) {
		emergentDifficulty = gameBP->GetReplicatedEmergentDifficulty();
	}
	mDifficultyRecommendation = game::DifficultyRecommendation(mMissionStartingGearPower, emergentDifficulty);
}

const game::DifficultyRecommendation& UEquipmentComponent::GetDifficultyRecommendation() {	
	if (!mDifficultyRecommendation) {
		SetMissionStartingEquippedGearPower(CalculateTotalEquippedDisplayItemPower());
	}
	return mDifficultyRecommendation.GetValue();
}

void UEquipmentComponent::ResetDifficultyRecommendation() {
	mDifficultyRecommendation.Reset();
}

void UEquipmentComponent::OnMissionDifficultyIncreased(int difficultyLevelIncreased)
{
	RefreshDifficultyRecommendation();
}

TArray<float> UEquipmentComponent::GetCooldowns() const
{
	TArray<float> cooldowns;

	//D11.KS - Post level transition for clients don't have a valid state, so it's impossible to get any reference of time, at this point the reconnects are about to be cleared anyways due to character re-creation.
	if (GetWorld() && GetWorld()->GetGameState())
	{
		cooldowns.Push(GetSlot(ESlotType::HealthPotion).GetCooldownSecondsRemaining());

		for (UItemSlot* slot : GetSlotsOfType(ESlotType::ActivePermanent))
		{
			cooldowns.Push(slot->GetCooldownSecondsRemaining());
		}
	}

	return cooldowns;
}
