#pragma once

#include "CoreMinimal.h"
#include "OnlineAsyncTaskManagerDungeons.h"
#include "OnlineSubsystemImpl.h"
#include "OnlineSubsystemDungeonsTypes.h"
#include <functional>
#include <memory>
#include "ManualCompletionQueue.h"

class FOnlineFriendsDungeons;
class FOnlinePresenceDungeons;
class FOnlineIdentityDungeons;
class FOnlineSessionDungeons;
class FOnlineExternalUIDungeons;
class FOnlineAchievementsDungeons;
class FOnlineStatsDungeons;
class FOnlineUserCloudXbl;

typedef TSharedPtr<FOnlineFriendsDungeons, ESPMode::ThreadSafe> FOnlineFriendsDungeonsPtr;
typedef TSharedPtr<FOnlinePresenceDungeons, ESPMode::ThreadSafe> FOnlinePresenceDungeonsPtr;
typedef TSharedPtr<FOnlineIdentityDungeons, ESPMode::ThreadSafe> FOnlineIdentityDungeonsPtr;
typedef TSharedPtr<FOnlineSessionDungeons, ESPMode::ThreadSafe> FOnlineSessionDungeonsPtr;
typedef TSharedPtr<FOnlineExternalUIDungeons, ESPMode::ThreadSafe> FOnlineExternalUIDungeonsPtr;
typedef TSharedPtr<FOnlineAchievementsDungeons, ESPMode::ThreadSafe> FOnlineAchievementsDungeonsPtr;
typedef TSharedPtr<FOnlineStatsDungeons, ESPMode::ThreadSafe> FOnlineStatsDungeonsPtr;
typedef TSharedPtr<FOnlineUserCloudXbl, ESPMode::ThreadSafe> FOnlineUserCloudXblPtr;

class FOnlineAsyncTask;
class FOnlineAsyncItem;

/**
 * OnlineSubsystemDungeons - Implementation of xbox live service
 */
class ONLINESUBSYSTEMDUNGEONS_API FOnlineSubsystemDungeons :
	public FOnlineSubsystemImpl
{

public:
	virtual ~FOnlineSubsystemDungeons();

	//IOnlineSubsystem
	virtual IOnlineSessionPtr GetSessionInterface() const override;
	virtual IOnlineFriendsPtr GetFriendsInterface() const override;
	virtual IOnlinePartyPtr GetPartyInterface() const override;
	virtual IOnlineGroupsPtr GetGroupsInterface() const override;
	virtual IOnlineSharedCloudPtr GetSharedCloudInterface() const override;
	virtual IOnlineUserCloudPtr GetUserCloudInterface() const override;
	virtual IOnlineEntitlementsPtr GetEntitlementsInterface() const override;
	virtual IOnlineLeaderboardsPtr GetLeaderboardsInterface() const override;
	virtual IOnlineVoicePtr GetVoiceInterface() const override;
	virtual IOnlineExternalUIPtr GetExternalUIInterface() const override;
	virtual IOnlineTimePtr GetTimeInterface() const override;
	virtual IOnlineIdentityPtr GetIdentityInterface() const override;
	virtual IOnlineTitleFilePtr GetTitleFileInterface() const override;
	virtual IOnlineStorePtr GetStoreInterface() const override;
	virtual IOnlineStoreV2Ptr GetStoreV2Interface() const override { return nullptr; }
	virtual IOnlinePurchasePtr GetPurchaseInterface() const override { return nullptr; }
	virtual IOnlineEventsPtr GetEventsInterface() const override;
	virtual IOnlineAchievementsPtr GetAchievementsInterface() const override;
	virtual IOnlineSharingPtr GetSharingInterface() const override;
	virtual IOnlineUserPtr GetUserInterface() const override;
	virtual IOnlineMessagePtr GetMessageInterface() const override;
	virtual IOnlinePresencePtr GetPresenceInterface() const override;
	virtual IOnlineChatPtr GetChatInterface() const override;
	virtual IOnlineStatsPtr GetStatsInterface() const override;
	virtual IOnlineTurnBasedPtr GetTurnBasedInterface() const override;
	virtual IOnlineTournamentPtr GetTournamentInterface() const override;

	FOnlineExternalUIDungeonsPtr GetExternalUIDungeons() const;

	void QueueAsyncTask(TUniquePtr<FOnlineAsyncTask> AsyncTask);
	void QueueAsyncTask(std::function<void()> AsyncTask);

	void InitSocketSubsystem();
	virtual bool Init() override;
	virtual bool Shutdown() override;
	virtual FString GetAppId() const override;
	virtual bool Exec(class UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar) override;
	virtual FText GetOnlineServiceName() const override;

	// FTickerObjectBase

	virtual bool Tick(float DeltaTime) override;



	// FOnlineSubsystemLive

	/**
	 * Is the Live API available for use
	 * @return true if Live functionality is available, false otherwise
	 */
	virtual bool IsEnabled() const override;

	FOnlineIdentityDungeons* GetDungeonsIdentityInterface() const;
	FOnlineSessionDungeons* GetDungeonsSessionInterface() const;
	FOnlineStatsDungeons* GetDungeonsStatsInterface() const;
	FOnlineAchievementsDungeons* GetDungeonsAchievementsInterface() const;
	XTaskQueueHandle GetQueueHandle() const;

PACKAGE_SCOPE:

	/** Only the factory makes instances */

	FOnlineSubsystemDungeons(FName InInstanceName);

private:
	std::unique_ptr<class FOnlineAsyncTaskManagerDungeons>  OnlineAsyncTaskThreadRunnable;
	std::unique_ptr<class FRunnableThread>  OnlineAsyncTaskThread;

	FOnlineSessionDungeonsPtr SessionInterface;
	FOnlineFriendsDungeonsPtr FriendsInterface;
	FOnlineIdentityDungeonsPtr IdentityInterface;
	FOnlinePresenceDungeonsPtr PresenceInterface;
	FOnlineExternalUIDungeonsPtr ExternalUIInterface;
	FOnlineAchievementsDungeonsPtr AchievementsInterface;
	FOnlineStatsDungeonsPtr StatsInterface;
	FOnlineUserCloudXblPtr CloudLoadSaveInterface;
	std::unique_ptr<ManualCompletionQueue> QueueHandler;
};

typedef TSharedPtr<FOnlineSubsystemDungeons, ESPMode::ThreadSafe> FOnlineSubsystemDungeonsPtr;