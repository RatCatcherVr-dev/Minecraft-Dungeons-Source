// © 2020 Mojang Synergies AB. TM Microsoft Corporation.
#include "AdventureHubComponent.h"
#include "online/sessions/OnlineUtil.h"
#include "online/seasons/LiveOps.h"
#include "game/AdventureHubUtil.h"

UAdventureHubComponent::UAdventureHubComponent() {
}

void UAdventureHubComponent::BeginPlay() {
	Super::BeginPlay();
	auto gameInstance = GetOwner()->GetGameInstance();
	if (auto liveOps = online::getLiveOps(gameInstance)) {
		DataUpdateHandle = liveOps->OnDataUpdate.AddUObject(this, &UAdventureHubComponent::OnDataUpdate);
	}
}

void UAdventureHubComponent::EndPlay(EEndPlayReason::Type reason) {
	Super::EndPlay(reason);
	auto gameInstance = GetOwner()->GetGameInstance();
	if (auto liveOps = online::getLiveOps(gameInstance)) {
		liveOps->OnDataUpdate.Remove(DataUpdateHandle);
	}
}

void UAdventureHubComponent::OnDataUpdate(online::liveops::UpdateRequestStatus status) {
	if (status == online::liveops::UpdateRequestStatus::Success) {
		auto gameInstance = GetOwner()->GetGameInstance();
		if (auto liveOps = online::getLiveOps(gameInstance)) {
			OnUpdateAdventureHubData.Broadcast(FAdventureHubInfo(liveOps->GetSeasonView()));
		}
	}
}
