#include "AdventurePointsHandler.h"


#include "DungeonsGameInstance.h"
#include "online/seasons/LiveOps.h"
#include "IDungeonsAPIClient.h"
#include "core/HttpServiceResponse.h"
#include "game/Conversion.h"
#include "modules/LiveOps/ChallengeResponse.h"
#include "online/seasons/Util/Filter.h"


namespace online
{
namespace liveops
{
namespace internal
{
std::vector<FilterPart> BuildFilter(const std::vector<minecraft::api::FilterPartResponse>& filterParts) {
	std::vector<FilterPart> filter;
	for (const auto& part : filterParts) {
		filter.push_back({part.type, part.value});
	}
	return filter;
}

int64 CalculateScore(const AdventurePointsHandler::FilterConfig& config) {
	return (config.currentScore / config.perCompleted)*config.scoreGranted;
}

int64 GetRemainingScore(const AdventurePointsHandler::FilterConfig& config) {
	return config.currentScore % config.perCompleted;
}

void TryUpdateScore(AdventurePointsHandler::FilterConfig& config) {
	if (config.currentScore >= config.perCompleted) {
		config.triggerUpdate(CalculateScore(config));
		config.currentScore = GetRemainingScore(config);
	}
}

template <typename T>
void RemoveHandles(AdventurePointsHandler::Handles& handles, T& delegate) {
	for (auto& handle : handles) {
		delegate.Remove(handle);
	}
	handles.clear();
}

minecraft::api::AdventurePointsConfig GenerateTestData() {
	minecraft::api::AdventurePointsConfig config;
	config.rewardSources.push_back({
		minecraft::api::ObjectiveType::MissionCompleted,
		{
			{ minecraft::api::FilterPartType::Mission, "creeperwoods" }
		},
		1,
		1
	});
	config.rewardSources.push_back({
		minecraft::api::ObjectiveType::MobDeath,
		{
			{minecraft::api::FilterPartType::Operator, "AND"},
		},
		1,
		1
	});
	config.rewardSources.push_back({
		minecraft::api::ObjectiveType::XpGained,
		{
			{minecraft::api::FilterPartType::Operator, "AND"},
		},
		1,
		1
	});
	config.rewardSources.push_back({
		minecraft::api::ObjectiveType::ReviveFriend,
		{
			{minecraft::api::FilterPartType::Operator, "AND"},
		},
		1,
		1
	});
	config.rewardSources.push_back({
		minecraft::api::ObjectiveType::PlayerHealed,
		{
			{minecraft::api::FilterPartType::Operator, "AND"},
		},
		1,
		1
	});
	config.rewardSources.push_back({
		minecraft::api::ObjectiveType::CurrencyCollected,
		{
			{ minecraft::api::FilterPartType::Currency, "Gold"}
		},
		1,
		100
	});
	config.rewardSources.push_back({
		minecraft::api::ObjectiveType::CurrencyCollected,
		{
			{ minecraft::api::FilterPartType::Currency, "Emerald"}
		},
		1,
		100
	});
	config.rewardSources.push_back({
		minecraft::api::ObjectiveType::TrialComplete,
		{
			{minecraft::api::FilterPartType::Operator, "AND"},
		},
		1,
		1
	});
	config.rewardSources.push_back({
		minecraft::api::ObjectiveType::ChestOpened,
		{},
		1,
		1
	});
	config.rewardSources.push_back({
		minecraft::api::ObjectiveType::BlocksTravelled,
		{},
		1,
		100,
	});
	return config;
}

}

AdventurePointsHandler::AdventurePointsHandler(std::shared_ptr<LiveOpsClient> apiClient, UGameInstance* gameInstance)
	: LiveIF(std::move(apiClient), gameInstance){
	
}

void AdventurePointsHandler::Init() {
}

void AdventurePointsHandler::Teardown() {
	ClearHandles();
}

void AdventurePointsHandler::Request() {
	MultiRequestPartial(std::shared_ptr<MultiRequest>());
}

void AdventurePointsHandler::MultiRequestPartial(std::shared_ptr<MultiRequest> multiRequest) {
	LiveOpsClient::Callback<minecraft::api::GameConfigResponse> callback = [&, multiRequest](minecraft::api::HttpServiceResponse<minecraft::api::GameConfigResponse> response) {
		if (response.successful()) {
			SetConfig(response.getBody()->data.adventurePointsConfig);
		} else {
			UE_LOG(LogLiveOps, Error, TEXT("Unsuccessful http response for AdventurePointsHandler"));
		}

		if (multiRequest) {
			multiRequest->CompleteSingleRequest(response);
		}
	};
	mApiClient->Request<DungeonsEndpoint::CONFIG_GAME>(callback);
}

void AdventurePointsHandler::SetConfig(const minecraft::api::AdventurePointsConfig& inData) {
	if (data != inData) {
		data = inData;
		ClearHandles();
		SetupConfig();
	}
}

AdventurePointsHandler::TriggerUpdate AdventurePointsHandler::GetTriggerUpdateFunction() const {
	return [&] (int64 score) {
		if (auto progress = getProgressHandlerInterface(GameInstance)) {
			progress->AddToQueue(MakeProgressRequest(getSelectedSeason(GameInstance), score));
		}
	};
}

void AdventurePointsHandler::SetupConfig() {
	for (auto source : data.rewardSources) {
		FilterConfig config = {
			GetTriggerUpdateFunction(),
			internal::BuildFilter(source.filterParts),
			source.scoreGranted,
			source.perCompleted,
			0
		};
		switch (source.type) {
			case minecraft::api::ObjectiveType::TrialComplete:
			case minecraft::api::ObjectiveType::MissionCompleted: {
				MissionCompletedHandles.push_back(online::getLiveOps(GameInstance)->OnMissionFinished.AddLambda(
				[&, config](ELevelNames name, 
				            EGameDifficulty difficulty, 
				            FEndlessStruggle endlessStruggle,
				            EExtraChallenge extraChallenge,
				            bool hyperMission,
				            ELevelVariationType levelVariation,
				            int collectedTokens,
				            TOptional<FString> trialId) mutable {
					if (filter::missioncompleted::isMatch(config.filter, name, difficulty, endlessStruggle, {trialId, Cast<UDungeonsGameInstance>(GameInstance)->GetTrialType(trialId.Get(""))})) {
						config.currentScore++;
						internal::TryUpdateScore(config);
					}
				}));
				break;
			}
			case minecraft::api::ObjectiveType::XpGained: {
				XpGainedHandles.push_back(online::getLiveOps(GameInstance)->OnXPChanged.AddLambda(
				[config](int32 amount) mutable {
					config.currentScore += amount;
					internal::TryUpdateScore(config);
				}));
				break;
			}
			case minecraft::api::ObjectiveType::ReviveFriend: {
				ReviveFriendHandles.push_back(online::getLiveOps(GameInstance)->OnRevive.AddLambda(
				[config]() mutable {
					if (filter::revivefriend::isMatch(config.filter)) {
						config.currentScore++;
						internal::TryUpdateScore(config);
					}
				}));
				break;
			}
			case minecraft::api::ObjectiveType::PlayerHealed: {
				PlayerHealedHandles.push_back(online::getLiveOps(GameInstance)->OnHealed.AddLambda(
				[config](float, const FOnAttributeChangeData& data) mutable {
					if (filter::healingdone::isMatch(config.filter, data)) {
						config.currentScore += data.NewValue - data.OldValue;
						internal::TryUpdateScore(config);
					}
				}));
				break;
			}
			case minecraft::api::ObjectiveType::CurrencyCollected: {
				CurrencyCollectedHandles.push_back(online::getLiveOps(GameInstance)->OnCurrencyChanged.AddLambda(
				[config, GameInstance = GameInstance](const FName& type, const int32& amount) mutable {
					if (filter::currencycollected::isMatch(config.filter, GameInstance, type, amount)) {
						config.currentScore += amount;
						internal::TryUpdateScore(config);
					}
				}));
				break;
			}
			case minecraft::api::ObjectiveType::MobDeath: {
				MobDeathHandles.push_back(online::getLiveOps(GameInstance)->OnKilledOther.AddLambda(
				[config](EntityType mobType) mutable {
					if (filter::killobjective::isMatch(config.filter, mobType)) {
						config.currentScore++;
						internal::TryUpdateScore(config);
					}
				}));
				break;
			}
			case minecraft::api::ObjectiveType::ChestOpened: {
				ChestOpenedHandles.push_back(online::getLiveOps(GameInstance)->OnOpenChest.AddLambda(
				[config]() mutable {
					config.currentScore++;
					internal::TryUpdateScore(config);
				}));
				break;
			}
			case minecraft::api::ObjectiveType::BlocksTravelled: {
				MovementHandles.push_back(online::getLiveOps(GameInstance)->OnMovement.AddLambda(
				[config, previousBlock = TOptional<BlockPos>{}](float deltaSeconds, const FVector& oldLocation, const FVector& oldVelocity) mutable {
					const FVector newLocation = oldLocation + oldVelocity * deltaSeconds;
					const BlockPos newBlock = conversion::ueToBlock(newLocation);

					if (!previousBlock.IsSet()) {
						previousBlock = newBlock;
					}

					if (newBlock != previousBlock) {
						previousBlock = newBlock;
						config.currentScore++;
						internal::TryUpdateScore(config);
					}
				}));
				break;
			}
			default:
				checkNoEntry();
		}
	}
}

void AdventurePointsHandler::ClearHandles() {
	internal::RemoveHandles(MissionCompletedHandles, online::getLiveOps(GameInstance)->OnMissionFinished);
	internal::RemoveHandles(MobDeathHandles, online::getLiveOps(GameInstance)->OnKilledOther);
	internal::RemoveHandles(XpGainedHandles, online::getLiveOps(GameInstance)->OnXPChanged);
	internal::RemoveHandles(PlayerHealedHandles, online::getLiveOps(GameInstance)->OnHealed);
	internal::RemoveHandles(ReviveFriendHandles, online::getLiveOps(GameInstance)->OnRevive);
	internal::RemoveHandles(CurrencyCollectedHandles, online::getLiveOps(GameInstance)->OnCurrencyChanged);
	internal::RemoveHandles(TrialCompletedHandles, online::getLiveOps(GameInstance)->OnMissionFinished);
	internal::RemoveHandles(ChestOpenedHandles, online::getLiveOps(GameInstance)->OnOpenChest);
	internal::RemoveHandles(MovementHandles, online::getLiveOps(GameInstance)->OnMovement);
}
}
}
