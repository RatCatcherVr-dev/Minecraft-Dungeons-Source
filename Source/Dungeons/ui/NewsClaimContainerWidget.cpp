// © 2020 Mojang Synergies AB. TM Microsoft Corporation.


#include "NewsClaimContainerWidget.h"
#include "online/sessions/OnlineUtil.h"
#include "online/seasons/LiveOps.h"
#include "LiveOpsClient.h"
#include "TextBlock.h"
#include "online/seasons/RewardsHandler.h"
#include "online/seasons/EventView.h"
#include "game/component/ItemStashComponent.h"
#include "DungeonsGameInstance.h"

TMap<FString, int> RewardMappings = {
	{ "SpookyFallEasy", 1 },
	{ "SpookyFallHard", 2 },
};

TMap<FString, FText> DescriptionMappings = {
	{ "SpookyFallEasy", LocTableFromFile::Get("SpookyFallEventLabels.csv", "Easy_Challenge_Desc")},
	{ "SpookyFallHard", LocTableFromFile::Get("SpookyFallEventLabels.csv", "Hard_Challenge_Desc")},
};


void UNewsClaimContainerWidget::NativeConstruct() {
	Super::NativeConstruct();
	GameInstance = GetGameInstance();
	
	if (auto liveOps = online::getLiveOps(GameInstance)) {
		EventViewHandle = liveOps->OnEventViewUpdate.AddLambda([&](online::liveops::UpdateRequestStatus status, const TOptional<online::liveops::EventView>& eventView) {
			if (status == online::liveops::UpdateRequestStatus::Success) {
				UpdateProgress(eventView);
			}
		});
	}
}

void UNewsClaimContainerWidget::ClaimReward(FName rewardId) {
	auto rewardsHandler = online::getRewardsHandlerInterface(GameInstance);
	auto liveOps = online::getLiveOps(GameInstance);
	rewardsHandler->Claim({ { stringutil::toStdString(rewardId) }, online::getSelectedSeason(GameInstance), nullptr, [&, liveOps](online::liveops::ClaimResult result) { 
			UpdateProgress(liveOps->GetEventView());
			liveOps->RequestEventViewUpdateAsync();
		} 
	});
}

void UNewsClaimContainerWidget::UpdateProgress(const TOptional<online::liveops::EventView>& eventView) {
	TArray<FNewsRewardInfo> rewards;
	if (eventView) {
		for (auto challenge : eventView->ChallengeView.Challenges) {
			auto& registry = GetItemRegistry();
			if (auto reward = GetRewardFromAP(eventView.GetValue(), RewardMappings.Contains(challenge.Name) ? RewardMappings[challenge.Name] : 0)) {
				auto rewardId = stringutil::toFName(reward->RewardId);
				auto icon = GetIcon(reward->Type, reward->ItemNameId);
				if (reward->State == minecraft::api::RewardState::Unlocked) {
					rewards.Emplace(rewardId, icon, true, false, FText::GetEmpty(), FText::GetEmpty());
				}
				else if (reward->State == minecraft::api::RewardState::Claimed) {
					rewards.Emplace(rewardId, icon, true, true, FText::GetEmpty(), FText::GetEmpty());
				}
				else {
					rewards.Emplace(rewardId, icon, false, false, DescriptionMappings.Contains(challenge.Name) ? DescriptionMappings[challenge.Name] : FText::GetEmpty(), GetFormattedProgress(challenge.CurrentScore, challenge.RequiredScore));
				}
			}
		}
	}
	UpdateRewardInfo(rewards);
}

FText UNewsClaimContainerWidget::GetFormattedProgress(int progress, int requirement) {
	return FText::Format(FTextFormat::FromString(TEXT("{0} / {1}")), progress, requirement);
}

TOptional<online::liveops::EventRewardView> UNewsClaimContainerWidget::GetRewardFromAP(const online::liveops::EventView& eventView, int AP) const {
	auto it = std::find_if(eventView.RewardLevels.begin(), eventView.RewardLevels.end(), [AP](const online::liveops::EventLevelView& levelView) { return levelView.RequiredAP == AP; });
	if (it != eventView.RewardLevels.end()) {
		return it->FreeReward.GetValue();
	}
	return {};
}

UTexture2D* UNewsClaimContainerWidget::GetIcon(minecraft::api::RewardTypes type, FName Id) const {
	auto& registry = GetItemRegistry();
	switch (type) {
	case minecraft::api::RewardTypes::Emeralds:
	case minecraft::api::RewardTypes::Gold:
	case minecraft::api::RewardTypes::Item: {
		auto id = registry.Request(Id);
		return UInventoryItemSlot::GetIconTextureForItemId(id.Get(registry.Request("Sword").GetValue()));
	}
	case minecraft::api::RewardTypes::Cosmetic:
		return Cast<UDungeonsGameInstance>(GameInstance)->GetCosmeticsLibrary()->FindDefinitionByEntitlementName(Id.ToString())->GetCosmeticIconTexture();
	}
	return nullptr;
}

FNewsRewardInfo::FNewsRewardInfo(FName id, UTexture2D* icon, bool unlocked, bool claimed, FText desc, FText progress) 
	: RewardId(std::move(id))
	, Icon(icon)
	, bUnlocked(unlocked)
	, bClaimed(claimed) 
	, ChallengeDescription(std::move(desc))
	, ChallengeProgress(std::move(progress)) {
}
