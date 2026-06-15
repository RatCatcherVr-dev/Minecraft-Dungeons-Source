#include "Dungeons.h"
#include "OnlineConsoleCommands.h"

#include "Classes/Engine/World.h"
#include "Classes/GameFramework/Actor.h"
#include "Classes/GameFramework/PlayerController.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "util/SharedRandom.h"
#include "DungeonsGameInstance.h"

#include "game/Game.h"

#include "sessions/SessionSettings.h"
#include "sessions/OnlineUtil.h"

#include "crossplay/CrossplayOSS.h"
#include "crossplay/Friends.h"
#include "crossplay/Presence.h"

#ifdef STEAM_BUILD
#include "steam/steam_api.h"
#endif


namespace DungeonsDebugOnline {

	void findNetRelevantActors(const TSubclassOf<AActor>& actorClass, const APlayerController* playerController, const UWorld* world, TArray<const AActor*>& relevantActors, int& relevantCount, int& totalCount) {
		if (const APlayerCharacter* playerCharacter = Cast<APlayerCharacter>(playerController->GetCharacter())) {
			TArray<AActor*> actors;
			UGameplayStatics::GetAllActorsOfClass(world, actorClass, actors);

			for (const AActor* actor : actors) {
				if (!actor)
					continue;

				if (relevantActors.Contains(actor))
					continue;

				if (actor->IsNetRelevantFor(playerController, playerCharacter, playerCharacter->GetActorLocation())) {
					relevantActors.Add(actor);
					++relevantCount;
				}

				++totalCount;
			}
		}
	}

	void markNetRelevantActors(const UWorld* world, const APlayerController* playerController) {
		TArray<const AActor*> relevantActors;
		int relevantCount = 0;
		int totalCount = 0;

		findNetRelevantActors(AActor::StaticClass(), playerController, world, relevantActors, relevantCount, totalCount);

		if (const APlayerCharacter* playerCharacter = Cast<APlayerCharacter>(playerController->GetCharacter())) {
			float percentActors = float(relevantCount) / float(totalCount) * 100.0f;
			DrawDebugString(world, playerCharacter->GetActorLocation(), FString::Printf(TEXT("Replicated Actors: %u/%u (%u%%)"), relevantCount, totalCount, int(percentActors)), nullptr, FColor::Blue, 0.0f, false);
		}

		for (const AActor* actor : relevantActors) {
			DrawDebugString(world, actor->GetActorLocation(), TEXT("X"), nullptr, FColor::Blue, 0.0f, false);
		}
	}

};

void OpenReceiveInvite(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	if (!world) {
		return;
	}

	UDungeonsGameInstance* gameInstance = Cast<UDungeonsGameInstance>(world->GetGameInstance());
	if (!gameInstance) {
		return;
	}


	Random& rnd = Util::sharedRandom();


	// Construct fake session:
	FBlueprintGameSession gameSession;
	gameSession.HostDisplayName = FString("FakeUser") + FString::FromInt(rnd.nextInt());
	gameSession.HostUniqueNetId = FUniqueNetIdWrapper::Invalid();
	gameSession.IsPublic = true;
	gameSession.LevelSettings = levelsettingsutil::generateDummyMissionSettings();
	gameSession.ReceivedStartTime = world->GetTimeSeconds();

	// add host
	FBlueprintFriend hostFriend;
	hostFriend.DisplayName = gameSession.HostDisplayName;
	hostFriend.RealName = "HostRealName";
	hostFriend.UniqueNetId = gameSession.HostUniqueNetId;
	gameSession.PlayerInfoList.Players.Add(hostFriend);

	// add friends
	uint32_t numFriends = rnd.nextInt(3);
	for (uint32_t i = 0; i < numFriends; ++i) {
		FBlueprintFriend buddy;
		buddy.DisplayName = "Displayname";
		buddy.RealName = "RealName";
		buddy.UniqueNetId = FUniqueNetIdWrapper::Invalid();
		gameSession.PlayerInfoList.Players.Add(buddy);
	}

	gameInstance->OnFakeSessionInviteReceived(gameSession);
}

void PrivateGame(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	if (commands.Num() != 1) {
		out.Log(TEXT("Please pass 1 for private or 0 for public game"));
		return;
	}

	if (!commands[0].IsNumeric()) {
		out.Log(TEXT("Argument must be a number"));
		return;
	}

	auto sessionIface = online::getSessionInterface();
	if (!sessionIface.IsValid()) {
		return;
	}

	FNamedOnlineSession* session = sessionIface->GetNamedSession(DungeonsGameSessionName);
	if (session == nullptr) {
		out.Log(TEXT("Could not find game session"));
		return;
	}

	int isPrivate = FCString::Atoi(*commands[0]);
	auto friends = world->GetGameInstance<UDungeonsGameInstance>()->GetFriendsInterface();

	if (isPrivate == 0) {
		friends->SetPrivateGame(false);
		out.Log(TEXT("Session is now public"));
	}
	else if (isPrivate == 1) {
		friends->SetPrivateGame(true);
		out.Log(TEXT("Session is now private"));
	}

	if (auto gi = Cast<UDungeonsGameInstance>(world->GetGameInstance())) {
		gi->GetFriendsInterface()->SetPresenceStatus(gi->Configuration.GetLevelDisplayName());
	}
}

static void DoSetClientEnabledCommands(const TArray<FString>& args, UWorld* world, FOutputDevice& out) {
	if (!args.Num())
	{
		out.Log(TEXT("Command takes single boolean!"));
		return;
	}
	auto game = actorquery::getGame(world);
	game->setAllowClientConsoleCommands(args[0].ToBool());
}

void FindSessions(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	auto customSession = online::getSessionInterface();
	if (!customSession.IsValid())
	{
		out.Log(TEXT("No session interface available"));
		return;
	}

	TSharedRef<FOnlineSessionSearch> SessionSearch = MakeShareable(new FOnlineSessionSearch);
	customSession->FindSessions(0, SessionSearch);
}

void InviteFriends(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	auto customSession = online::getSessionInterface();
	if (!customSession.IsValid())
	{
		out.Log(TEXT("No session interface available"));
		return;
	}

	customSession->SendSessionInviteToFriend(0, "", FUniqueNetIdString(""));
}

void CreateSession(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {

	auto customSession = online::getSessionInterface();
	if (!customSession.IsValid())
	{
		out.Log(TEXT("No session interface available"));
		return;
	}


	auto customIdentity = online::getIdentityInterface();
	if (!customIdentity.IsValid())
	{
		out.Log(TEXT("No identity interface available"));
		return;
	}

	auto levelsettings = levelsettingsutil::generateLobbySettings(world);

	auto settings = SessionSettings(customIdentity->GetPlayerNickname(0), levelsettings, false, online::IsCrossplayEnabled());

	customSession->CreateSession(*customIdentity->GetUniquePlayerId(0).Get(), DungeonsGameSessionName, settings.Get());
}

void SetPresence(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	const auto customPresence = online::getCrossplayOss()->GetPresenceIF();
	if (!customPresence.IsValid())
	{
		out.Log(TEXT("No presence interface available"));
		return;
	}


	auto customIdentity = online::getIdentityInterface();
	if (!customIdentity.IsValid())
	{
		out.Log(TEXT("No identity interface available"));
		return;
	}

	customPresence->SetPresence(*customIdentity->GetUniquePlayerIdForSessionSubsystem(0).Get(), FOnlineUserPresenceStatus());
}

void Logout(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	online::getIdentityInterface()->Logout(0);
}

void AddFriend(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	TSharedPtr<online::Crossplay::Friends> customFriends = online::getFriendsInterface();
	if (commands.Num() == 1) {
		if (commands[0].IsNumeric()) {
			customFriends->SendInvite(0, FUniqueNetIdString(commands[0]), "");
		}
		else {
			customFriends->SendInvite(0, FUniqueNetIdString(""), commands[0]);
		}
	}
	else {
		out.Log(TEXT("Please pass one and only one gamertag or xuid of a friend to add"));
	}
}

void DeleteFriend(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	TSharedPtr<online::Crossplay::Friends> customFriends = online::getFriendsInterface();
	if (commands.Num() == 1) {
		if (commands[0].IsNumeric()) {
			customFriends->DeleteFriend(0, FUniqueNetIdString(commands[0]), "");
		}
		else {
			customFriends->DeleteFriend(0, FUniqueNetIdString(""), commands[0]);
		}
	}
	else {
		out.Log(TEXT("Please pass one and only one gamertag or xuid of a friend to delete"));
	}
}

void SetNetLimit(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	if (commands.Num() == 1 && commands[0].IsNumeric()) {
		int netLimit = FCString::Atoi(*commands[0]);
		if (auto player = Cast<ABasePlayerController>(world->GetGameInstance()->GetFirstLocalPlayerController(world))) {
			player->ServerSetNetConnectionSpeed(netLimit);
		}
	}
	else {
		out.Log(TEXT("Please pass a positive integer (bytes per second)"));
	}
}

void CheckPrivileges(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	auto gi = Cast<UDungeonsGameInstance>(world->GetGameInstance());
	gi->StartOnlinePrivilegeTask(IOnlineIdentity::FOnGetUserPrivilegeCompleteDelegate::CreateUObject(gi, &UDungeonsGameInstance::OnUserCanPlayInvite), EUserPrivileges::CanPlayOnline, online::getIdentityInterface()->GetUniquePlayerId(0));
}

void SetCrossplay(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {

	if (commands.Num() == 1 && commands[0].IsNumeric()) {
		bool enableCrossplay = commands[0].ToBool();
		online::Crossplay::OSS* oss = online::getCrossplayOss();
		out.Log(TEXT("Setting Crossplay"));
		if (oss->SetCrossplaySetting(enableCrossplay)) {
			out.Log(TEXT("Success"));
		} else {
			out.Log(TEXT("Failed"));
		}
	} else {
		out.Log(TEXT("Please pass one parameter: 1/0 (On/Off)"));
	}
}

TAutoConsoleVariable<int32> CVarAllowFriendsSidebar(
	TEXT("Dungeons.Online.AllowFriendsSidebarToggle"),
	0,
	TEXT("Enables opening the friends sidebar the 'F'riends key.\n")
	TEXT("<=0: off.\n")
	TEXT(">0: on.\n"),
	ECVF_Cheat);

TAutoConsoleVariable<int32> CVarDebugDrawNetCullDistance(
	TEXT("Dungeons.Online.DrawNetCullDistance"),
	0,
	TEXT("Draw a circle around the player to indicate NetCullDistance.\n")
	TEXT("0: off.\n")
	TEXT("1: on.\n"),
	ECVF_Cheat);

static FAutoConsoleCommand DebugOpenReceiveInvite(
	TEXT("Dungeons.Online.OpenReceiveInvite"),
	TEXT("Open the receive invite popup") TEXT("\nUsage: Dungeons.Online.OpenReceiveInvite"),
	FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&OpenReceiveInvite),
	ECVF_Cheat);

static FAutoConsoleCommand DebugPrivateGame(
	TEXT("Dungeons.Online.PrivateGame"),
	TEXT("Set the current game session to private (1) or public (0)") TEXT("\nUsage: Dungeons.Online.PrivateGame 1"),
	FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&PrivateGame),
	ECVF_Cheat);

static const FAutoConsoleCommand Suicide(TEXT("Dungeons.Online.ClientCallsAllowed")
	, TEXT("Enable or disable clients calling server console commands")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoSetClientEnabledCommands)
	, ECVF_Cheat);

static FAutoConsoleCommand DebugTriggerFindSessions(
	TEXT("Dungeons.Online.FindSessions"),
	TEXT("Trigger FindSessions flow") TEXT("\nUsage: Dungeons.Online.FindSessions"),
	FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&FindSessions),
	ECVF_Cheat
);

static FAutoConsoleCommand DebugTriggerInviteFriends(
	TEXT("Dungeons.Online.InviteFriends"),
	TEXT("Trigger InviteFriends flow") TEXT("\nUsage: Dungeons.Online.InviteFriends"),
	FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&InviteFriends),
	ECVF_Cheat
);

static FAutoConsoleCommand DebugTriggerCreateSession(
	TEXT("Dungeons.Online.CreateSession"),
	TEXT("Trigger create session flow") TEXT("\nUsage: Dungeons.Online.CreateSession"),
	FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&CreateSession),
	ECVF_Cheat
);

static FAutoConsoleCommand DebugTriggerSetPresence(
	TEXT("Dungeons.Online.SetPresence"),
	TEXT("Trigger create session flow") TEXT("\nUsage: Dungeons.Online.SetPresence"),
	FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&SetPresence),
	ECVF_Cheat
);

static FAutoConsoleCommand DebugLogout(
	TEXT("Dungeons.Online.Logout"),
	TEXT("Log out local player with id 0") TEXT("\nUsage: Dungeons.Online.Logout"),
	FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&Logout),
	ECVF_Cheat
);

static FAutoConsoleCommand DebugTriggerAddFriend(
	TEXT("Dungeons.Online.Friend.Add"),
	TEXT("Trigger add friend flow") TEXT("\nUsage: Dungeons.Online.Friend.Add <GamerTag/XUID>"),
	FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&AddFriend),
	ECVF_Cheat
);

static FAutoConsoleCommand DebugTriggerDeleteFriend(
	TEXT("Dungeons.Online.Friend.Delete"),
	TEXT("Trigger delete friend flow") TEXT("\nUsage: Dungeons.Online.Friend.Delete <GamerTag/XUID>"),
	FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DeleteFriend),
	ECVF_Cheat
);

static FAutoConsoleCommand DebugTriggerSetNetLimit(
	TEXT("Dungeons.Online.SetNetLimit"),
	TEXT("Set how many bytes per second to recieve from the server") TEXT("\nUsage: Dungeons.Online.SetNetLimit <Bytes/Second>"),
	FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&SetNetLimit),
	ECVF_Cheat
);

static FAutoConsoleCommand DebugTriggerCheckPrivileges(
	TEXT("Dungeons.Online.Privileges"),
	TEXT("Trigger check privileges") TEXT("\nUsage: Dungeons.Online.Privileges"),
	FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&CheckPrivileges),
	ECVF_Cheat
);

static FAutoConsoleCommand DebugTriggerCreateOssInterface(
	TEXT("Dungeons.Online.SetCrossplaySetting"),
	TEXT("Set crossplay to on (1) or off (0) on PS4.") TEXT("\nUsage: Dungeons.Online.SetCrossplaySetting"),
	FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&SetCrossplay),
	ECVF_Cheat
);

#ifdef STEAM_BUILD
 void ClearSteamAchievements(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	 		SteamUserStats()->ResetAllStats(true);
	}
	
 static FAutoConsoleCommand DebugClearSteamAchivements(
		TEXT("Dungeons.Online.Steam.ClearAchievements"),
		TEXT("Clear achievements") TEXT("\nUsage: Dungeons.Online.Steam.ClearAchievements"),
		FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&ClearSteamAchievements),
		ECVF_Cheat
	);
#endif


