#include "Rewards.h"
#include "DungeonsGameInstance.h"
#include "LiveOpsClient.h"
#include "RewardsHandler.h"
#include "game/component/WalletComponent.h"
#include "game/item/generator/ItemGenerator.h"
#include "Util/SeasonsCommon.h"
#include "modules/LiveOps/RewardResponse.h"
#include "util/SeasonsCommon.h"
#include "Util/StringUtil.h"

namespace online
{
namespace liveops
{
namespace internal
{

bool lowerEquals(const std::string& a, const std::string& b) {
    return std::equal(a.begin(), a.end(),
                      b.begin(), b.end(),
                      [](char a, char b) {
                          return tolower(a) == tolower(b);
                      });
}

}

std::unique_ptr<RewardType> RewardType::MakeReward(const minecraft::api::Reward& reward, UGameInstance* InGameInstance) {
	switch (reward.type) {
		case minecraft::api::RewardTypes::Emeralds:
			return std::make_unique<EmeraldsReward>(reward, InGameInstance);
		case minecraft::api::RewardTypes::Gold:
			return std::make_unique<GoldReward>(reward, InGameInstance);
		case minecraft::api::RewardTypes::Item:
			return std::make_unique<ItemReward>(reward, InGameInstance);
		case minecraft::api::RewardTypes::Cosmetic:
			return std::make_unique<CosmeticReward>(reward, InGameInstance);
		case minecraft::api::RewardTypes::AdventurePoints:
			return std::make_unique<AdventurePointsReward>(reward, InGameInstance);
		default:
			return {};
	}
}

RewardType::RewardType(const minecraft::api::Reward& reward, UGameInstance* GameInstance)
	: GameInstance(GameInstance)
	, reward(reward) {
}

const minecraft::api::Reward& RewardType::GetData() const {
	return reward;
}

void RewardType::Claim(const ClaimConfig& config) {
	ClaimInternal(config);
}

void RewardType::SetClaimed() {
	reward.state = minecraft::api::RewardState::Claimed;
}


EmeraldsReward::EmeraldsReward(const minecraft::api::Reward& reward, UGameInstance* GameInstance)
	: RewardType(reward, GameInstance) { }

void EmeraldsReward::ClaimInternal(const ClaimConfig& config) {
	if (auto apc = Cast<APlayerCharacter>(config.player)) {
		auto* wallet = apc->GetWalletComponent();
		wallet->ClientAdd(game::item::type::Emerald.getId(), reward.value.Get(0));
		config.callback(ClaimResult::Succeeded);
	} else {
		config.callback(ClaimResult::InternalError);
	}
	SetClaimed();
}

GoldReward::GoldReward(const minecraft::api::Reward& reward, UGameInstance* GameInstance)
	: RewardType(reward, GameInstance) { }

void GoldReward::ClaimInternal(const ClaimConfig& config) {
	if (auto apc = Cast<APlayerCharacter>(config.player)) {
		auto* wallet = apc->GetWalletComponent();
		wallet->ClientAdd(game::item::type::Gold.getId(), reward.value.Get(0));
		config.callback(ClaimResult::Succeeded);
	} else {
		config.callback(ClaimResult::InternalError);
	}
	SetClaimed();
}

ItemReward::ItemReward(const minecraft::api::Reward& reward, UGameInstance* GameInstance)
	: RewardType(reward, GameInstance) { }

void ItemReward::ClaimInternal(const ClaimConfig& config) {
	if (auto apc = Cast<APlayerCharacter>(config.player)) {
		auto* stash = apc->GetItemStashComponent();
		FString name(reward.itemNameId.Get("").c_str());
		auto type = GetItemRegistry().Request(*name);
		stash->ClientAddItem(itemgen::generate(type.GetValue()));
		config.callback(ClaimResult::Succeeded);
	} else {
		config.callback(ClaimResult::InternalError);
	}
	SetClaimed();
}

CosmeticReward::CosmeticReward(const minecraft::api::Reward& reward, UGameInstance* GameInstance)
	: RewardType(reward, GameInstance)
	, TimerHandle(FTimerHandle()){ }

CosmeticReward::~CosmeticReward() {
	UnbindHandles();
}

void CosmeticReward::UnbindHandles() {
	if (auto dungeonsGameInstance = Cast<UDungeonsGameInstance>(GameInstance)) {
		if (TimerHandle.IsValid()) {
			dungeonsGameInstance->GetTimerManager().ClearTimer(TimerHandle);
		}
		if (auto repo = dungeonsGameInstance->GetEntitlementsRepository()) {
			repo->OnEntitlementsProvided.Remove(EntitlementsProvided);
			repo->OnEntitlementsRequestFailed.Remove(FailedRequest);
			EntitlementsProvided.Reset();
			FailedRequest.Reset();
		}
	}
}

void CosmeticReward::ClaimInternal(const ClaimConfig& config) {
	auto dungeonsGameInstance = Cast<UDungeonsGameInstance>(GameInstance);
	auto repo = dungeonsGameInstance->GetEntitlementsRepository();
	Retries = 1;

	auto timerRequest = [&, repo]() {
		Retries++;
		repo->RequestEntitlements();
		UE_LOG(LogLiveOps, Log, TEXT("[LiveOps] Retrying to fetch entitlement: %s"), UTF8_TO_TCHAR(reward.rewardId.c_str()));
	};

	auto retryRequest = [&, dungeonsGameInstance, timerRequest]() {
		dungeonsGameInstance->GetTimerManager().ClearTimer(TimerHandle);
		if (Retries <= 10) {
			dungeonsGameInstance->GetTimerManager().SetTimer(TimerHandle, timerRequest, Retries, false);
		} else {
			config.callback(ClaimResult::Failed);
			UE_LOG(LogLiveOps, Log, TEXT("[LiveOps] Failed to fetch entitlement: %s"), UTF8_TO_TCHAR(reward.rewardId.c_str()));
			UnbindHandles();
		}
	};

	EntitlementsProvided = repo->OnEntitlementsProvided.AddLambda([&, repo, retryRequest, dungeonsGameInstance](const TArray<FEntitlement>& entitlements) {
		auto isThisCosmetic = [name = stringutil::toFString(reward.itemNameId.GetValue())](const FEntitlement& ent) { return name == ent.GetName(); };

		if (entitlements.ContainsByPredicate(isThisCosmetic)) {
			SetClaimed();
			UnbindHandles();
			config.callback(ClaimResult::Succeeded);
		} else {
			retryRequest();
		}
	});

	FailedRequest = repo->OnEntitlementsRequestFailed.AddLambda(retryRequest);

	repo->RequestEntitlements();
}

AdventurePointsReward::AdventurePointsReward(const minecraft::api::Reward& reward, UGameInstance* GameInstance)
	: RewardType(reward, GameInstance) {}

void AdventurePointsReward::ClaimInternal(const ClaimConfig& config) {
	SetClaimed();
	config.callback(ClaimResult::Succeeded);
}

}
}

