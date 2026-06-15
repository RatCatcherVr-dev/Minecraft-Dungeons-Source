#pragma once

#include "Interfaces/OnlineStatsInterface.h"
#include "OnlineAsyncTaskManagerDungeons.h"
#include "OnlineSubsystemDungeonsTypes.h"
#include "xsapiServicesInclude.h"

struct DungeonsStatistic
{
	std::string name;
	std::string type;
	std::string value;
};

/**
 *	FOnlineStatsDungeons - Interface class for stats
 */
class FOnlineStatsDungeons : public IOnlineStats {
public:
	FOnlineStatsDungeons(class FOnlineSubsystemDungeons* InSubsystem);
	virtual ~FOnlineStatsDungeons() = default;

	//~ Begin IOnlineStats Interface
	virtual void QueryStats(const TSharedRef<const FUniqueNetId> LocalUserId, const TSharedRef<const FUniqueNetId> StatsUser, const FOnlineStatsQueryUserStatsComplete& Delegate = FOnlineStatsQueryUserStatsComplete()) override;
	virtual void QueryStats(const TSharedRef<const FUniqueNetId> LocalUserId, const TArray<TSharedRef<const FUniqueNetId>>& StatUsers, const TArray<FString>& StatNames, const FOnlineStatsQueryUsersStatsComplete& Delegate = FOnlineStatsQueryUsersStatsComplete()) override;
	virtual TSharedPtr<const FOnlineStatsUserStats> GetStats(const TSharedRef<const FUniqueNetId> StatsUserId) const override;
	virtual void UpdateStats(const TSharedRef<const FUniqueNetId> LocalUserId, const TArray<FOnlineStatsUserUpdatedStats>& UpdatedUserStats, const FOnlineStatsUpdateStatsComplete& Delegate = FOnlineStatsUpdateStatsComplete()) override;

#if !UE_BUILD_SHIPPING
	virtual void ResetStats(const TSharedRef<const FUniqueNetId> StatsUserId) override;
#endif // !UE_BUILD_SHIPPING
	//~ End IOnlineStats Interface

	virtual void Tick(float DeltaTime);
	FOnlineSubsystemDungeons* GetSubsystem();

	void RemoveStatsForUser(TSharedRef<const FUniqueNetId> PlayerId);
	TUniqueNetIdMap<FOnlineStatsUserStats> CachedStats;

private:
	bool WriteStats(const FOnlineStatsUserUpdatedStats& UpdatedUserStat, XblContextHandle& XBLcontext, uint64_t XUID);
	void FlushStats();

	FOnlineSubsystemDungeons* DungeonsSubsystem;
	TUniqueNetIdMap<FOnlineStatsUserUpdatedStats> QueuedStats;
	float TimeSinceLastFlush = 0;

	bool IsAllSetUp(const TSharedRef<const FUniqueNetId> NetID) const;
	FOnlineStatsUserStats ConvertStats(const TSharedRef<const FUniqueNetId> StatsUserId, TArray<DungeonsStatistic>& DungeonsStats);
	void AddStatsForPlayer(TSharedRef<const FUniqueNetId> PlayerId, TArray<DungeonsStatistic>& Stats);
	void OnStatsQueried(bool WasSuccessful, uint64_t XUID, TArray<DungeonsStatistic>& StatsQueryResults, const FOnlineStatsQueryUserStatsComplete& Delegate);
};

typedef TSharedPtr<FOnlineStatsDungeons, ESPMode::ThreadSafe> FFOnlineStatsDungeonsPtr;