#include "ClientEventHub.h"


#include "DungeonsGameInstance.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/item/ItemType.h"
#include "game/component/AwardsGeneratorComponent.h"
#include "online/seasons/LiveOps.h"
#include "online/sessions/OnlineUtil.h"

UClientEventHubComponent::UClientEventHubComponent() {
}

void UClientEventHubComponent::UseItem(const FSerializableItemId& itemId) const {
	Execute([&]() { DoUseItem(itemId); }, [&]() { Client_DoUseItem(itemId); });
}

void UClientEventHubComponent::Client_DoUseItem_Implementation(const FSerializableItemId& itemId) const {
	DoUseItem(itemId);
}

void UClientEventHubComponent::DoUseItem(const FSerializableItemId& itemId) const {
	const ItemType& itemType = GetItemRegistry().Get(itemId);
	TrackLiveOps([&](online::liveops::LiveOps* liveOps) { liveOps->OnItemUsed.Broadcast(itemType); });
}

void UClientEventHubComponent::OpenChest() const {
	Execute([&]() { DoOpenChest(); }, [&]() { Client_DoOpenChest(); });
}

void UClientEventHubComponent::Client_DoOpenChest_Implementation() const {
	DoOpenChest();
}

void UClientEventHubComponent::DoOpenChest() const {
	TrackLiveOps([&](online::liveops::LiveOps* liveOps) { liveOps->OnOpenChest.Broadcast(); });
	TrackStat([&](UStatTrackerComponent* tracker) { tracker->OpenChest(); });
}

void UClientEventHubComponent::XPChanged(int amount) const {
	TrackLiveOps([&, amount](online::liveops::LiveOps* liveOps) { liveOps->OnXPChanged.Broadcast(amount); });
}

void UClientEventHubComponent::ReviveFriend() const {
	TrackLiveOps([&](online::liveops::LiveOps* liveOps) { liveOps->OnRevive.Broadcast(); });
	TrackStat([&](UStatTrackerComponent* tracker) { tracker->ReviveFriend(); });
}

void UClientEventHubComponent::ReceivedCurrency(const FSerializableItemId& type, int amount) const {
	TrackLiveOps([&](online::liveops::LiveOps* liveOps) { liveOps->OnCurrencyChanged.Broadcast(type.GetSerializedId(), amount); });
	TrackStat([&](UStatTrackerComponent* tracker) { tracker->GotCurrency(type, amount); });
}

void UClientEventHubComponent::KilledMob(const EntityType mobType, bool enchanted) const {
	TrackLiveOps([&](online::liveops::LiveOps* liveOps) { liveOps->OnKilledOther.Broadcast(mobType); });
	TrackStat([&](UStatTrackerComponent* tracker) { tracker->RegisterKill(mobType, enchanted); });
}

void UClientEventHubComponent::Moved(float deltaSeconds, const FVector& oldLocation, const FVector& oldVelocity) const {
	TrackLiveOps([&](online::liveops::LiveOps* liveOps) { liveOps->OnMovement.Broadcast(deltaSeconds, oldLocation, oldVelocity); });

	const float distance = oldVelocity.Size() * deltaSeconds / 100;  // Convert to meters
	TrackStat([&](UStatTrackerComponent* tracker) { tracker->UpdateTravel(distance); });
}

void UClientEventHubComponent::MissionFinished(ELevelNames name, EGameDifficulty difficulty, FEndlessStruggle endlessStruggle, EExtraChallenge extraChallenge, bool hyperMission, ELevelVariationType levelVariation, int collectedTokens) const {
	TrackStat([&](UStatTrackerComponent* tracker) { tracker->CompleteLevel(name, difficulty, endlessStruggle, extraChallenge, hyperMission, levelVariation, collectedTokens); });
	TrackLiveOps([&](online::liveops::LiveOps* liveOps) { liveOps->OnMissionFinished.Broadcast(name, difficulty, endlessStruggle, extraChallenge, hyperMission, levelVariation, collectedTokens, GetWorld()->GetGameInstance<UDungeonsGameInstance>()->GetCurrentTrialId(name)); });
}

template <typename WhenLocallyControlled, typename WhenNonLocallyControlled>
void UClientEventHubComponent::Execute(WhenLocallyControlled localAction, WhenNonLocallyControlled nonLocalAction) const {
	if (!OwnerHasAuthority()) {
		return;
	}

	if (OwnerIsLocallyControlled()) {
		localAction();
	}
	else {
		nonLocalAction();
	}
}

template <typename StatTrackerCall>
void UClientEventHubComponent::TrackStat(StatTrackerCall call) const {
	if (const auto character = Cast<APlayerCharacter>(GetOwner())) {
		if (auto tracker = character->GetStatTracker()) {
			call(tracker);
		}
	}
}

template <typename LiveOpsCall>
void UClientEventHubComponent::TrackLiveOps(LiveOpsCall call) const {
	if (OwnerIsLocallyControlled() && ShouldTrackLiveOpsProgress()) {
		call(online::getLiveOps(GetOwner()->GetGameInstance()));
	}
}

bool UClientEventHubComponent::OwnerIsLocallyControlled() const {
	const auto character = GetBaseCharacterOwner();
	return character && character->IsLocallyControlled();
}

bool UClientEventHubComponent::OwnerHasAuthority() const {
	return GetOwner()->HasAuthority();
}

bool UClientEventHubComponent::ShouldTrackLiveOpsProgress() const {
	return OwnerIsPrimaryLocalPlayer();
}

ABaseCharacter* UClientEventHubComponent::GetBaseCharacterOwner() const {
	return Cast<ABaseCharacter>(GetOwner());
}

bool UClientEventHubComponent::OwnerIsPrimaryLocalPlayer() const {
	if (const auto character = GetBaseCharacterOwner()) {
		if (const auto controller = Cast<APlayerController>(character->GetController())) {
			if (const auto localPlayer = controller->GetLocalPlayer()) {
				return localPlayer->IsPrimaryPlayer();
			}
		}
	}
	return false;
}
