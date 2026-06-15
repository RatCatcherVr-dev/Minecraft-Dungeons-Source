#include "LiveOpsConsoleCommands.h"

#include "DungeonsGameInstance.h"
#include "PlayfabPlayer.h"

#include "online/seasons/LiveOps.h"

#include "online/seasons/ChallengesHandler.h"
#include "online/seasons/LeaderboardsHandler.h"
#include "online/seasons/ProgressHandler.h"
#include "online/sessions/OnlineUtil.h"
#include "DungeonsLoginFlow.h"

#if !UE_BUILD_SHIPPING

void LiveOpsStatus(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	auto gameInstance = world->GetGameInstance<UDungeonsGameInstance>();
	auto challengeHandler = online::getChallengesHandlerInterface(world->GetGameInstance());
	PlayFabPlayer* PfPlayer = nullptr;
	PfPlayer = PlayfabServices::GetPlayfabPlayer();

	out.Log(TEXT("============ LiveOps Status ============"));
	out.Logf(TEXT(" MinecraftAPI module loaded - %s"), (IDungeonsAuth::IsAvailable() && IDungeonsAuth::Get().Auth() ? *FString("true") : *FString("false")));
	out.Logf(TEXT(" MinecraftAPI signed in - %s"), (IDungeonsAuth::IsAvailable() && IDungeonsAuth::Get().Auth() && IDungeonsAuth::Get().Auth()->GetClient() ? *FString("true") : *FString("false")));
	out.Logf(TEXT(" MinecraftAPI Namespace - %s"), *online::GetMinecraftAPINamespace());
	switch (online::getLiveOps(gameInstance)->GetConnectionStatus()) {
		case EMinecraftAPIConnectionStatus::Connected:
			out.Log(TEXT(" MinecraftAPI Connection status - Connected"));
			break;
		case EMinecraftAPIConnectionStatus::TimingOut:
			out.Log(TEXT(" MinecraftAPI Connection status - TimingOut"));
			break;
		case EMinecraftAPIConnectionStatus::GameClientTooOld:
			out.Log(TEXT(" MinecraftAPI Connection status - GameClientTooOld"));
			break;
		case EMinecraftAPIConnectionStatus::NoConnection:
		default:
			out.Log(TEXT(" MinecraftAPI Connection status - NoConnection"));
			break;
	}
	if (IDungeonsAuth::IsAvailable() && IDungeonsAuth::Get().Auth() && IDungeonsAuth::Get().Auth()->GetClient()) {
		auto liveOps = online::getLiveOps(gameInstance);
		auto& authData = liveOps->APIAuthDataCache;
		out.Logf(TEXT(" MinecraftAPI Jwt token - %s"), *authData.JwtToken.Get(""));
		out.Logf(TEXT(" MinecraftAPI UserID - %s"), *authData.UserId.Get(""));
	}
	out.Log(TEXT(""));
	if (PfPlayer) {
		out.Logf(TEXT(" Playfab signed in - %s"), (!PfPlayer->GetSessionTicket().IsEmpty() ? *FString("true") : *FString("false")));
		out.Logf(TEXT(" PlayfabId - %s"), *PfPlayer->GetPlayFabId());
		out.Logf(TEXT(" PlayerId - %s"), *PfPlayer->GetPlayerId());
		auto key = FString("playfab_key");
		out.Logf(TEXT(" PlayfabToken (platform specific) - %s"), *PfPlayer->GetAuthToken(key));
		out.Logf(TEXT(" SessionTicket - %s"), *PfPlayer->GetSessionTicket());
		out.Logf(TEXT(" EntityToken - %s"), *PfPlayer->GetEntityToken());
		out.Logf(TEXT(" LinkedXblAccount - %s"), *PfPlayer->GetLinkedXblAccount());
	}
	else {
		out.Log(TEXT(" Playfab signed in - false"));
	}
	out.Log(TEXT(""));
	out.Logf(TEXT(" LiveOps module loaded - %s"), (online::getLiveOps(gameInstance) ? *FString("true") : *FString("false")));
	out.Logf(TEXT(" Challenges downloaded - %d"), (challengeHandler ? challengeHandler->GetChallenges().challenges.size() : 0));
	int completedChallenges = 0;
	int totalRewards = 0;
	if (challengeHandler) {
		for (const auto& challenge : challengeHandler->GetChallenges().challenges) {
			if (challenge->IsCompleted()) {
				completedChallenges++;
			}
			for (auto&& rewardType : challenge->GetData().rewards) {
				totalRewards++;
			}
		}
	}
	out.Logf(TEXT(" Completed challenges - %d"), completedChallenges);
	out.Logf(TEXT(" Total rewards - %d"), totalRewards);
	if (auto progressionHandler = online::getProgressHandlerInterface(gameInstance)) {
		out.Log(TEXT(" ProgressionsHandler started - true"));
		out.Logf(TEXT(" Total progression keys - %d"), progressionHandler->GetAllStats().size());

	}
	else {
		out.Log(TEXT(" ProgressionsHandler started - false"));
	}
	out.Log(TEXT("============================================"));
}

void PrintChallenges(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	out.Log(TEXT("============ Challenges ============"));

	if (const auto challengesHandler = online::getChallengesHandlerInterface(world->GetGameInstance())) {
		auto stats = online::getProgressHandlerInterface(world->GetGameInstance());
		for (const auto& challenge : challengesHandler->GetChallenges().challenges) {
			out.Logf(TEXT("--- %s ---"), *FString(challenge->GetName().c_str()));
			out.Logf(TEXT("Is active: %s"), *FString(challenge->IsActive() ? "True" : "False"));
			out.Log(TEXT("-- Rewards"));
			out.Log(TEXT("Reward name - Finished"));
			for (const auto& reward : challenge->GetData().rewards) {
				out.Logf(TEXT("%s - %d"),
					*FString(reward.rewardId.c_str()),
					challenge->IsCompleted() ? 1 : 0
				);
			}
			out.Log(TEXT("-- Objectives"));
			out.Log(TEXT("Progress key - (current / goal)"));
			for (const auto& objective : challenge->objectives) {
				const int64 currentValue = stats->GetStatValue(objective->GetProgressName()).Get(0);
				out.Logf(TEXT("%s - (%lld / %lld)"), 
						*FString(objective->GetProgressName().c_str()),
						currentValue,
						objective->GetData().requiredScore);
			}
			out.Log(TEXT("-----------------"));
		}
	}
	out.Log(TEXT("============================================"));
}

void PrintAdventurePoints(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	out.Log(TEXT("============ Adventure Points ============"));

	auto progressHandler = online::getProgressHandlerInterface(world->GetGameInstance());
	auto adventurePointsHandler = online::getAdventurePointsHandlerInterface(world->GetGameInstance());

	auto name = online::getSelectedSeason(world->GetGameInstance());
	auto value = progressHandler->GetStatValue(name).Get(0);
	out.Logf(TEXT("%s - %lld"), *FString(name.c_str()), value);
	out.Log(TEXT("============================================"));
}

void PrintLeaderboards(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	out.Log(TEXT("============ Leaderboards ============"));
	auto leaderboardsHandler = online::getLeaderboardsInterface(world->GetGameInstance());

	if (leaderboardsHandler) {
		auto map = leaderboardsHandler->GetLeaderboards().GetLeaderboardMap();
		for (const auto& pair : map) {
			out.Logf(TEXT("Leaderboard: %s"), *FString(pair.first.c_str()));
			for (const auto& elem : pair.second) {
				out.Logf(TEXT("		%d, %d, %s"), elem.mRank, elem.mValue, *FString(elem.mName.c_str()));
			}
		}
	}
}

void AddProgression(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	if (commands.Num() == 2) {
		if (commands[1].IsNumeric()) {
			std::string name = TCHAR_TO_UTF8(*commands[0]);
			int score = FCString::Atoi(*commands[1]);

			auto progress = online::getProgressHandlerInterface(world->GetGameInstance());
			progress->AddToQueue(online::liveops::MakeProgressRequest(name, score));
			out.Log(TEXT("Added the number to the Progression"));
			auto liveOps = online::getLiveOps(world->GetGameInstance());
			liveOps->RequestDataUpdateAsync();
		}
		else {
			out.Log(TEXT("Second argument needs to be a number!"));
		}
	}
	else {
		out.Log(TEXT("Incorrect input arguments, example: Dungeons.Online.Seasons.AddProgression 1yearchallengetrials 5"));
	}
}

void ReadProgression(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	if (commands.Num() == 1) {
		std::string name = TCHAR_TO_UTF8(*commands[0]);
		auto progress = online::getProgressHandlerInterface(world->GetGameInstance());
		auto val = progress->GetStatValue(name);
		if (val) {
			out.Logf(TEXT("Progress: %s - %lld"), *FString(name.c_str()), val.GetValue());
		}
		else {
			out.Log(TEXT("Progress not tracked!"));
		}
	}
	else {
		out.Log(TEXT("Incorrect input arguments, example: Dungeons.Online.Seasons.ReadProgression 1yearchallengetrials"));
	}
}

void PrintAllProgression(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	if (const auto progressHandler = online::getProgressHandlerInterface(world->GetGameInstance())) {
		out.Log(TEXT("============ Progressions ============"));

		for (const auto& progress : progressHandler->GetAllStats()) {
			out.Logf(TEXT("Progress: %s - %lld"), *FString(progress.second.name.c_str()), progress.second.value);
		}

		out.Log(TEXT("======================================"));
	}
}

void ResetAllProgression(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	auto progress = online::getProgressHandlerInterface(world->GetGameInstance());
	progress->ResetAllProgression([=] (const auto& response) {
		auto liveOps = online::getLiveOps(world->GetGameInstance());
		liveOps->RequestDataUpdateAsync();
		auto* entitlementsRepository = world->GetGameInstance<UDungeonsGameInstance>()->GetEntitlementsRepository();
		entitlementsRepository->RequestEntitlements();
		
	});
	out.Log(TEXT("Progression and cosmetic entitlement reset request sent"));
}

void MockChallenges(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	auto gameInstance = world->GetGameInstance<UDungeonsGameInstance>();
	auto challengeHandler = online::getChallengesHandlerInterface(world->GetGameInstance());

	if (challengeHandler) {
		challengeHandler->UseMockedChallenges();
		out.Log(TEXT("Mock challenges set up"));
	}
	else {
		out.Log(TEXT("Failed to set up mock challenges"));
	}
}

void ClaimReward(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	auto gameInstance = world->GetGameInstance<UDungeonsGameInstance>();
	auto rewardsHandler = online::getRewardsHandlerInterface(world->GetGameInstance());

	if (rewardsHandler) {
		if (commands.Num() == 2) {
			online::liveops::RewardKey rewardId = TCHAR_TO_UTF8(*commands[0]);
			std::string seasonId = TCHAR_TO_UTF8(*commands[1]);
			rewardsHandler->Claim({ {rewardId}, seasonId, nullptr,
				[] (online::liveops::ClaimResult claimResult) {
					switch (claimResult) {
						case online::liveops::ClaimResult::Succeeded:
							UE_LOG(LogLiveOps, Log, TEXT("[LiveOps] ClaimResults::Succeeded"));
							break;
						case online::liveops::ClaimResult::Failed:
							UE_LOG(LogLiveOps, Log, TEXT("[LiveOps] ClaimResults::Failed"));
							break;
						case online::liveops::ClaimResult::NotClaimable:
							UE_LOG(LogLiveOps, Log, TEXT("[LiveOps] ClaimResults::NotClaimable"));
							break;
						case online::liveops::ClaimResult::InternalError:
							UE_LOG(LogLiveOps, Log, TEXT("[LiveOps] ClaimResults::InternalError"));
							break;
						case online::liveops::ClaimResult::Unknown:
						default:
							UE_LOG(LogLiveOps, Log, TEXT("[LiveOps] ClaimResults::Unknown"));
							break;
					}
				}
			});
			out.Log(TEXT("Claim request sent, see results in log."));
		} else {
			out.Log(TEXT("Incorrect input arguments, example: Dungeons.Online.Seasons.ClaimReward rewardid seasonX"));
		}
	}
	else {
		out.Log(TEXT("Failed to fetch RewardsHandler, claim failed."));
	}
}

void SetNamespace(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	if (commands.Num() == 1) {
		online::SetDynamicNamespace(commands[0]);
		auto gameInstance = world->GetGameInstance<UDungeonsGameInstance>();
		auto loginFlow = gameInstance->GetLoginFlow();
		loginFlow->RefreshLogin();
	}
	else {
		out.Log(TEXT("Please pass one parameter, example: SpookyFall2021"));
	}
}

static FAutoConsoleCommand DebugTriggerSeasonsPrintChallenges(
	TEXT("Dungeons.Online.Seasons.PrintChallenges"),
	TEXT("Prints the current status of all challenges.") TEXT("\nUsage: Dungeons.Online.Seasons.PrintChallenges"),
	FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&PrintChallenges),
	ECVF_Cheat
);

static FAutoConsoleCommand DebugTriggerSeasonsPrintLeaderboards(
	TEXT("Dungeons.Online.Seasons.PrintLeaderboards"),
	TEXT("Prints the current names of all active leaderboards") TEXT("\nUsage: Dungeons.Online.Seasons.PrintLeaderboards"),
	FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&PrintLeaderboards),
	ECVF_Cheat
);

static FAutoConsoleCommand DebugTriggerSeasonsPrintAdventurePoints(
	TEXT("Dungeons.Online.Seasons.PrintAdventurePoints"),
	TEXT("Prints the currently tracked adventure points and its current score") TEXT("\nUsage: Dungeons.Online.Seasons.PrintAdventurePoints"),
	FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&PrintAdventurePoints),
	ECVF_Cheat
);

static FAutoConsoleCommand DebugTriggerSeasonsLiveOpsStatus(
	TEXT("Dungeons.Online.Seasons.LiveOpsStatus"),
	TEXT("Prints out an overview of the LiveOps status") TEXT("\nUsage: Dungeons.Online.Seasons.LiveOpsStatus"),
	FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&LiveOpsStatus),
	ECVF_Cheat
);

static FAutoConsoleCommand DebugTriggerSeasonsLiveOpsAddProgression(
	TEXT("Dungeons.Online.Seasons.AddProgression"),
	TEXT("Adds progression to a specific progress value") TEXT("\nUsage: Dungeons.Online.Seasons.AddProgression 1yaniversary 5"),
	FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&AddProgression),
	ECVF_Cheat
);

static FAutoConsoleCommand DebugTriggerSeasonsLiveOpsReadProgression(
	TEXT("Dungeons.Online.Seasons.ReadProgression"),
	TEXT("Reads a local progress value") TEXT("\nUsage: Dungeons.Online.Seasons.ReadProgression 1yaniversary"),
	FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&ReadProgression),
	ECVF_Cheat
);

static FAutoConsoleCommand DebugTriggerSeasonsLiveOpsPrintAllProgression(
	TEXT("Dungeons.Online.Seasons.PrintAllProgression"),
	TEXT("Prints all progress values") TEXT("\nUsage: Dungeons.Online.Seasons.PrintAllProgression"),
	FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&PrintAllProgression),
	ECVF_Cheat
);

static FAutoConsoleCommand DebugTriggerSeasonsLiveOpsResetAllProgression(
	TEXT("Dungeons.Online.Seasons.ResetAllProgression"),
	TEXT("Reset progression for user") TEXT("\nUsage: Dungeons.Online.Seasons.ResetAllProgression"),
	FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&ResetAllProgression),
	ECVF_Cheat
);

static FAutoConsoleCommand DebugTriggerSeasonsLiveOpsMockChallenges(
	TEXT("Dungeons.Online.Seasons.MockChallenges"),
	TEXT("Sets up mocked challenges") TEXT("\nUsage: Dungeons.Online.Seasons.MockChallenges"),
	FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&MockChallenges),
	ECVF_Cheat
);

static FAutoConsoleCommand DebugTriggerSeasonsLiveOpsClaimReward(
	TEXT("Dungeons.Online.Seasons.ClaimReward"),
	TEXT("Tries to claim a reward for a season") TEXT("\nUsage: Dungeons.Online.Seasons.ClaimReward rewardid seasonX"),
	FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&ClaimReward),
	ECVF_Cheat
);

static FAutoConsoleCommand DebugTriggerSetNamespace(
	TEXT("Dungeons.Online.Seasons.SetNamespace"),
	TEXT("Set current MinecraftAPI namespace") TEXT("\nUsage: Dungeons.Online.Seasons.SetNamespace <namespace>"),
	FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&SetNamespace),
	ECVF_Cheat
);

#endif
