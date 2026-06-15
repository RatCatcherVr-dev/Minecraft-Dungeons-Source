#include "SeasonalEventTrackerWidget.h"

#include "DungeonsGameInstance.h"
#include "DungeonsGameState.h"
#include "Components/TextBlock.h"
#include "online/seasons/LiveOps.h"
#include "online/seasons/ProgressHandler.h"
#include "online/sessions/OnlineUtil.h"
#include "online/ui/OnlineTextLabels.h"

namespace internal
{

FText GetNameFromReward(online::liveops::ChallengeRewardView reward, UGameInstance* gameInstance) {
	const auto Id = reward.ItemNameId;
	switch (reward.Type) {
		case minecraft::api::RewardTypes::Cosmetic:
			return Cast<UDungeonsGameInstance>(gameInstance)->GetCosmeticsLibrary()->FindDefinitionByEntitlementName(Id.ToString())->GetCosmeticDisplayName();
		case minecraft::api::RewardTypes::Item:
			if (auto itemId = GetItemRegistry().Request(Id)) {
				return FText::FromString(GetItemRegistry().Get(itemId.GetValue()).getName());
			}
			break;
		case minecraft::api::RewardTypes::AdventurePoints:
		case minecraft::api::RewardTypes::Gold:
		case minecraft::api::RewardTypes::Emeralds:
		default:
			break;
	}
	return FText::GetEmpty();
}

FText ConnectionStatusDisplayText(EMinecraftAPIConnectionStatus connectionStatus) {
	FString output;
	switch (connectionStatus) {
		case EMinecraftAPIConnectionStatus::Connected:
			output = "Connected";
			break;
		case EMinecraftAPIConnectionStatus::TimingOut:
			output = "Timing out";
			break;
		case EMinecraftAPIConnectionStatus::GameClientTooOld:
			output = "Game client too old";
			break;
		case EMinecraftAPIConnectionStatus::NoConnection:
		default:
			output = "No connection";
			break;
	}
	return FText::Format(online::ui::eventTrackerConnection(), FText::FromString(output));
}

FFormatOrderedArguments GetEventViewArgs(const online::liveops::EventView& eventView, UGameInstance* gameInstance) {
	FFormatOrderedArguments args;
	for (auto challenge : eventView.ChallengeView.Challenges) {
		if (challenge.Reward.State == minecraft::api::RewardState::Unlocked) {
			args.Add(FText::Format(online::ui::eventTrackerClaimable(),
				GetNameFromReward(challenge.Reward, gameInstance)));
		} else if (challenge.Reward.State != minecraft::api::RewardState::Claimed) {
			args.Add(FText::Format(online::ui::eventTrackerInProgress(),
					FText::AsNumber(std::min(challenge.CurrentScore, challenge.RequiredScore)),
					FText::AsNumber(challenge.RequiredScore),
					challenge.Description));
		} else {
			args.Add(FText::Format(online::ui::eventTrackerComplete(),
				GetNameFromReward(challenge.Reward, gameInstance)));
		}
	}
	return args;
}

FText EventViewDisplayText(FFormatOrderedArguments args) {
	FString tokens = "";
	bool first = true;
	for (int i = 0; i < args.Num(); i++) {
		if (!first) {
			tokens += "\n";
		}
		first = false;
		tokens += "{"+ FString::FromInt(i) +"}";
	}
	return FText::Format(FText::FromString(tokens), args);
}

}

void USeasonalEventTrackerWidget::NativePreConstruct() {
	Super::NativePreConstruct();
	SetState(EMinecraftAPIConnectionStatus::NoConnection, {});
}

void USeasonalEventTrackerWidget::NativeConstruct() {
	Super::NativeConstruct();
	GameInstance = GetGameInstance();

	if (auto liveOps = online::getLiveOps(GameInstance)) {
		// Initial state
		SetState(liveOps->GetConnectionStatus(), liveOps->GetEventView());

		// Listen to updates
		EventViewHandle = liveOps->OnEventViewUpdate.AddLambda([&] (online::liveops::UpdateRequestStatus status, const TOptional<online::liveops::EventView>& eventView) {
			const auto connectionStatus = liveOps->GetConnectionStatus();
			if (status == online::liveops::UpdateRequestStatus::Success) {
				SetState(connectionStatus, eventView);
			}
			else if (connectionStatus != EMinecraftAPIConnectionStatus::Connected) {
				SetState(connectionStatus, {});
			}
		});
	}
}

void USeasonalEventTrackerWidget::BeginDestroy() {
	Super::BeginDestroy();
	if (GameInstance) {
		if (auto liveOps = online::getLiveOps(GameInstance)) {
			liveOps->OnEventViewUpdate.Remove(EventViewHandle);
			EventViewHandle.Reset();
		}
	}
}

void USeasonalEventTrackerWidget::OnRequestUpdate() {
	if (const auto liveOps = online::getLiveOps(GameInstance)) {
		liveOps->RequestEventViewUpdateAsync();
	}
	if (auto minecraftAPI = GetGameInstance<UDungeonsGameInstance>()->GetMinecraftAPI()) {
		minecraftAPI->GetTrialsProvider()->LoadTrials();
	}
}

void USeasonalEventTrackerWidget::SetState(EMinecraftAPIConnectionStatus connectionStatus, const TOptional<online::liveops::EventView>& eventView) {
	if (connectionStatus == EMinecraftAPIConnectionStatus::Connected && eventView.IsSet()) {
		state.trialProgress = internal::EventViewDisplayText(internal::GetEventViewArgs(eventView.GetValue(), GameInstance));
	}
	else {
		state.trialProgress = internal::ConnectionStatusDisplayText(connectionStatus);
	}

	UpdateWidgets();
}

void USeasonalEventTrackerWidget::UpdateWidgets() {
	if (!ProgressTextBlock) return;

	if (state.trialProgress.IsEmptyOrWhitespace()) {
		SetVisibility(ESlateVisibility::Collapsed);
	} else {
		SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		ProgressTextBlock->SetText(state.trialProgress);
	}
}
