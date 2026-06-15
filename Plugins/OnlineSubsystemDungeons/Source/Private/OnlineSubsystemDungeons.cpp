#include "PCHOnlineDungeonsSubsystem.h"
#include "OnlineSubsystemDungeons.h"
#include "RunnableThread.h"

#include "OnlineSubsystemDungeonsTypes.h"
#include "OnlineSessionInterfaceDungeons.h"
#include "OnlineFriendsInterfaceDungeons.h"
#include "OnlineIdentityDungeons.h"
#include "OnlinePresenceInterfaceDungeons.h"
#include "OnlineExternalUIInterfaceDungeons.h"
#include "P2P/SocketSubsystemDungeonsWebRTC.h"
#include "SocketSubsystemModule.h"
#include "AsyncTasks/OnlineAsyncTaskDungeonsTriggers.h"
#include "OnlineAchievementsInterfaceDungeons.h"
#include "OnlineSubsystem.h"
#include "OnlineStatsInterfaceDungeons.h"
#include "OnlineUserCloudinterfaceXbl.h"
#include "Engine/Engine.h"

FOnlineSubsystemDungeons::~FOnlineSubsystemDungeons()
{
}

IOnlineSessionPtr FOnlineSubsystemDungeons::GetSessionInterface() const { return SessionInterface; }
IOnlineFriendsPtr FOnlineSubsystemDungeons::GetFriendsInterface() const { return FriendsInterface; }
IOnlinePartyPtr FOnlineSubsystemDungeons::GetPartyInterface() const { return nullptr; }
IOnlineGroupsPtr FOnlineSubsystemDungeons::GetGroupsInterface() const { return nullptr; }
IOnlineSharedCloudPtr FOnlineSubsystemDungeons::GetSharedCloudInterface() const { return nullptr; }
#if CLOUDSAVE_ENABLED
IOnlineUserCloudPtr FOnlineSubsystemDungeons::GetUserCloudInterface() const { return CloudLoadSaveInterface; }
#else
IOnlineUserCloudPtr FOnlineSubsystemDungeons::GetUserCloudInterface() const { return nullptr; }
#endif
IOnlineEntitlementsPtr FOnlineSubsystemDungeons::GetEntitlementsInterface() const { return nullptr; }
IOnlineLeaderboardsPtr FOnlineSubsystemDungeons::GetLeaderboardsInterface() const { return nullptr; }
IOnlineVoicePtr FOnlineSubsystemDungeons::GetVoiceInterface() const { return nullptr; }
IOnlineExternalUIPtr FOnlineSubsystemDungeons::GetExternalUIInterface() const { return ExternalUIInterface; }
IOnlineTimePtr FOnlineSubsystemDungeons::GetTimeInterface() const { return nullptr; }
IOnlineIdentityPtr FOnlineSubsystemDungeons::GetIdentityInterface() const { return IdentityInterface; }
IOnlineTitleFilePtr FOnlineSubsystemDungeons::GetTitleFileInterface() const { return nullptr; }
IOnlineStorePtr FOnlineSubsystemDungeons::GetStoreInterface() const { return nullptr; }
IOnlineEventsPtr FOnlineSubsystemDungeons::GetEventsInterface() const { return nullptr; }
IOnlineAchievementsPtr FOnlineSubsystemDungeons::GetAchievementsInterface() const { return AchievementsInterface; }
IOnlineSharingPtr FOnlineSubsystemDungeons::GetSharingInterface() const { return nullptr; }
IOnlineUserPtr FOnlineSubsystemDungeons::GetUserInterface() const { return nullptr; }
IOnlineMessagePtr FOnlineSubsystemDungeons::GetMessageInterface() const { return nullptr; }
IOnlinePresencePtr FOnlineSubsystemDungeons::GetPresenceInterface() const { return PresenceInterface; }
IOnlineChatPtr FOnlineSubsystemDungeons::GetChatInterface() const { return nullptr; }
IOnlineStatsPtr FOnlineSubsystemDungeons::GetStatsInterface() const { return StatsInterface; }
IOnlineTurnBasedPtr FOnlineSubsystemDungeons::GetTurnBasedInterface() const { return nullptr; }
IOnlineTournamentPtr FOnlineSubsystemDungeons::GetTournamentInterface() const { return nullptr; }

class FConfigCacheIni;

void RedirectHCTraceLogs(_In_z_ const char* areaName,
	_In_ HCTraceLevel level,
	_In_ uint64_t threadId,
	_In_ uint64_t timestamp,
	_In_z_ const char* message)
{
	UE_LOG_ONLINE(Log, TEXT("[SubsystemDungeons] %s"), ANSI_TO_TCHAR(message));
}

void FOnlineSubsystemDungeons::InitSocketSubsystem()
{
	CreateDungeonsSocketWebRTCSubsystem(IdentityInterface);
}

bool FOnlineSubsystemDungeons::Init()
{
	StartTicker();

	/* Init thread */
	QueueHandler = std::make_unique<ManualCompletionQueue>();
	OnlineAsyncTaskThreadRunnable = std::make_unique<FOnlineAsyncTaskManagerDungeons>(this);
	OnlineAsyncTaskThread = std::unique_ptr<FRunnableThread>(FRunnableThread::Create(OnlineAsyncTaskThreadRunnable.get(),
		*FString::Printf(TEXT("OnlineAsyncTaskThreadDungeons %s"), *InstanceName.ToString()), 128 * 1024, TPri_Normal));

	/* Init interfaces */
	IdentityInterface = dungeonsIdentity::platform::Create(this);
	SessionInterface = dungeonsSession::platform::Create(this);
	FriendsInterface = MakeShareable(new FOnlineFriendsDungeons(this));
	PresenceInterface = MakeShareable(new FOnlinePresenceDungeons(this));
	ExternalUIInterface = MakeShareable(new FOnlineExternalUIDungeons(this));
	AchievementsInterface = MakeShareable(new FOnlineAchievementsDungeons(this));
	StatsInterface = MakeShareable(new FOnlineStatsDungeons(this));
#if CLOUDSAVE_ENABLED
	CloudLoadSaveInterface = MakeShareable(new FOnlineUserCloudXbl());
#endif
	/* Init socketsubsystem */
	InitSocketSubsystem();

	if (!environmentConfig::IsShippingMode() && environmentConfig::GetConfigBool("OnlineSubsystemDungeons", "LogXsapi", false))
	{
		UE_LOG_ONLINE(Log, TEXT("[SubsystemDungeons] Enabling XSAPI logs"));
		HCTraceSetClientCallback(RedirectHCTraceLogs);
	}
	return true;
}

bool FOnlineSubsystemDungeons::Shutdown()
{
	UE_LOG_ONLINE(Display, TEXT("[SubsystemDungeons] OnlineSubsystemDungeons::Shutdown()"));
	if (GEngine->IsEditor() && environmentConfig::HasOnlineFlag())
	{	// Don't shutdown the subsystem for the editor
		return true;
	}

	FOnlineSubsystemImpl::Shutdown();
	OnlineAsyncTaskThread = {};
	OnlineAsyncTaskThreadRunnable = {};

	FSocketSubsystemDungeonsWebRTC::Destroy();

	//Delete interfaces
#define DESTRUCT_INTERFACE(Interface) \
	if (Interface.IsValid()) \
	{ \
		ensure(Interface.IsUnique()); \
		Interface = nullptr; \
	}

	// Destruct the interfaces


	DESTRUCT_INTERFACE(CloudLoadSaveInterface);
	DESTRUCT_INTERFACE(FriendsInterface);
	DESTRUCT_INTERFACE(SessionInterface);
	DESTRUCT_INTERFACE(PresenceInterface);
	DESTRUCT_INTERFACE(ExternalUIInterface);
	DESTRUCT_INTERFACE(AchievementsInterface);
	DESTRUCT_INTERFACE(StatsInterface);
	DESTRUCT_INTERFACE(IdentityInterface)



#undef DESTRUCT_INTERFACE

		FModuleManager& ModuleManager = FModuleManager::Get();

	if (ModuleManager.IsModuleLoaded("Sockets"))
	{
		FSocketSubsystemModule& SSS = FModuleManager::GetModuleChecked<FSocketSubsystemModule>("Sockets");
		SSS.UnregisterSocketSubsystem(DUNGEONSPLATFORM);
	}

	return true;
}


FString FOnlineSubsystemDungeons::GetAppId() const { return "Dungeons"; }
bool FOnlineSubsystemDungeons::Exec(class UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar) { return false; }
FText FOnlineSubsystemDungeons::GetOnlineServiceName() const { return NSLOCTEXT("OnlineSubsystemDungeons", "OnlineServiceName", "Dungeons"); }

FOnlineExternalUIDungeonsPtr FOnlineSubsystemDungeons::GetExternalUIDungeons() const {
	return ExternalUIInterface;
}

void FOnlineSubsystemDungeons::QueueAsyncTask(TUniquePtr<FOnlineAsyncTask> AsyncTask)
{
	check(OnlineAsyncTaskThreadRunnable);
	OnlineAsyncTaskThreadRunnable->AddToInQueue(AsyncTask.Release());
}

void FOnlineSubsystemDungeons::QueueAsyncTask(std::function<void()> AsyncTask) {
	QueueAsyncTask(MakeUnique<GenericAsyncTask>(std::move(AsyncTask)));
}

bool FOnlineSubsystemDungeons::Tick(float DeltaTime)
{
	if (!FOnlineSubsystemImpl::Tick(DeltaTime))
	{
		return false;
	}

	QueueHandler->Completion();

	if (OnlineAsyncTaskThreadRunnable)
	{
		OnlineAsyncTaskThreadRunnable->GameTick();
	}

	if (SessionInterface.IsValid())
	{
		SessionInterface->Tick(DeltaTime);
	}

	if (IdentityInterface.IsValid())
	{
		IdentityInterface->Tick(DeltaTime);
	}

	if (StatsInterface.IsValid())
	{
		StatsInterface->Tick(DeltaTime);
	}

	return true;
}

bool FOnlineSubsystemDungeons::IsEnabled() const
{
	return true;
}

FOnlineIdentityDungeons* FOnlineSubsystemDungeons::GetDungeonsIdentityInterface() const {
	return IdentityInterface.Get();
}


FOnlineSessionDungeons* FOnlineSubsystemDungeons::GetDungeonsSessionInterface() const {
	return SessionInterface.Get();
}

FOnlineStatsDungeons* FOnlineSubsystemDungeons::GetDungeonsStatsInterface() const
{
	return StatsInterface.Get();
}

FOnlineAchievementsDungeons* FOnlineSubsystemDungeons::GetDungeonsAchievementsInterface() const
{
	return AchievementsInterface.Get();
}




XTaskQueueHandle FOnlineSubsystemDungeons::GetQueueHandle() const {
	return QueueHandler->GetQueue();
}

FOnlineSubsystemDungeons::FOnlineSubsystemDungeons(FName InInstanceName)
	: FOnlineSubsystemImpl(DUNGEONSPLATFORM, InInstanceName)
{
	StopTicker();
}