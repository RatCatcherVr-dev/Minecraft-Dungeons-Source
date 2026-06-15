#include "PCHOnlineDungeonsSubsystem.h"
#include "OSSDungeonsConsoleCommands.h"

#include "OnlineSubsystemDungeons.h"
#include "OnlineIdentityDungeons.h"
#include "OnlineSessionInterfaceDungeons.h"

#include "OnlineSubsystemUtils.h"
#include "P2P/IpAddressDungeonsWebRTC.h"
#include "online/sessions/JoinDungeonsSessionCallbackProxy.h"

namespace DungeonsDebugOnline {

	void JoinSession(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
		IOnlineSubsystem* onlineSystem = Online::GetSubsystem(world, NAME_None);
		if (onlineSystem == nullptr) {
			out.Log(TEXT("No Online Subsystem available"));
			return;
		}

		FOnlineSessionDungeonsPtr CustomSession = StaticCastSharedPtr<FOnlineSessionDungeons>(onlineSystem->GetSessionInterface());
		if (!CustomSession.IsValid())
		{
			out.Log(TEXT("No identity interface available"));
			return;
		}

		FOnlineSessionSearchResult SearchSession;

		FOnlineSession* NewSession = &SearchSession.Session;

		NewSession->NumOpenPrivateConnections = 4;
		NewSession->NumOpenPublicConnections = 4;
		NewSession->OwningUserId = MakeShareable(new FUniqueNetIdDungeons(std::stoull(TCHAR_TO_ANSI(*commands[0]))));

		NewSession->SessionSettings.bAllowInvites = true;
		NewSession->SessionSettings.bAllowJoinInProgress = true;
		NewSession->SessionSettings.bAllowJoinInProgress = true;
		NewSession->SessionSettings.bAllowJoinViaPresenceFriendsOnly = true;
		NewSession->SessionSettings.bAntiCheatProtected = true;
		NewSession->SessionSettings.bIsDedicated = false;
		NewSession->SessionSettings.bIsLANMatch = false;
		NewSession->SessionSettings.bShouldAdvertise = true;
		NewSession->SessionSettings.bUsesPresence = true;
		NewSession->SessionSettings.bUsesStats = true;

		FOnlineSessionSetting setting;
		FSessionSettings settings;

		setting.Data.SetValue(FString(
			"{\"GAME_VERSION\":\"LOCALBUILD\","\
			"\"LEVEL_FILENAME\" : \"Lobby\","\
			"\"LEVEL_NAME\" : \"Invalid\","\
			"\"MISSION_DIFFICULTY\" : \"Invalid\","\
			"\"PLAYER_NAME\" : \"2 Dev 546161756\","\
			"\"RANDOM_SEED\" : \"0\","\
			"\"RECONNECTABLE_GUIDS\" : \"\","\
			"\"THREAT_LEVEL\" : \"Invalid\","\
			"\"UNREAL_MAP_NAME\" : \"lobby\"}"
			));

		settings.Add("CUSTOM", setting);

		NewSession->SessionSettings.Settings = settings;

		FOnlineSessionInfoDungeons* NewSessionInfo = new FOnlineSessionInfoDungeons(FString("1234567"));
		NewSession->SessionInfo = MakeShareable(NewSessionInfo);

		FInternetAddr* Addr = new FInternetAddrDungeonsWebRTC(commands[0], 0);
		NewSessionInfo->HostAddr = MakeShareable(Addr);

		auto JoinSessionProxy = UJoinDungeonsSessionCallbackProxy::JoinDungeonsSession(GWorld->GetWorld(), GWorld->GetWorld()->GetFirstPlayerController(), SearchSession);
		//JoinSessionProxy->Activate();

		JoinSessionProxy->Activate();
		//CustomSession->JoinSession(0, DungeonsGameSessionName, SearchSession);
	}

	void GetAuthToken(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
		if (auto customIdentity = StaticCastSharedPtr<FOnlineIdentityDungeons>(Online::GetIdentityInterface(world))) {
			customIdentity->UpdateServiceToken(customIdentity->GetFirstLocalUserNumber());
			FString token = customIdentity->GetAuthToken(customIdentity->GetFirstLocalUserNumber());
			UE_LOG_ONLINE(Log, TEXT("[Console] Got auth token: %s"), *token);
		}
		else {
			UE_LOG_ONLINE(Log, TEXT("[Console] Failed to cast the identity interface to FOnlineIdentityDungeons"));
		}
	}

	static FAutoConsoleCommand DebugAuthToken(
		TEXT("Dungeons.Online.AuthToken"),
		TEXT("Update and log the auth token") TEXT("\nUsage: Dungeons.Online.AuthToken"),
		FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&GetAuthToken),
		ECVF_Cheat
	);

	static FAutoConsoleCommand DebugTriggerJoinSession(
		TEXT("Dungeons.Online.JoinSession"),
		TEXT("Trigger JoinSession") TEXT("\nUsage: Dungeons.Online.JoinSession"),
		FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&JoinSession),
		ECVF_Cheat
	);
}