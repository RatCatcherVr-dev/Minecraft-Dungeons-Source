#include "PCHOnlineDungeonsSubsystem.h"
#include "OnlineStatsInterfaceDungeons.h"
#include "OnlineSubsystemDungeons.h"
#include "OnlineIdentityDungeons.h"
#include "OnlineError.h"

#include "HrLog.h"
#include "utils.h"

#define LOCTEXT_NAMESPACE "OnlineSubsystemDungeons"
#define ONLINE_ERROR_NAMESPACE "errors.com.epicgames.oss.stats"

//const std::string TIME_STAT = "1"; no longer used
const std::string KILLS_STAT = "2";
const std::string DEATHS_STAT = "3";
const std::string DISTANCE_STAT = "4";
const std::string CHESTS_STAT = "5";
const std::string HEAL_STAT = "6";
const std::string EMERALD_STAT = "7";
const std::string ARTIFACTS_STAT = "8";
const std::string REVIVES_STAT = "9";
const std::string LEVEL_STAT = "10";
const int NoStats = 9;

static const TArray<std::string> statNamesArray = {
	KILLS_STAT,
	DEATHS_STAT,
	DISTANCE_STAT,
	CHESTS_STAT,
	HEAL_STAT,
	EMERALD_STAT,
	ARTIFACTS_STAT,
	REVIVES_STAT,
	LEVEL_STAT
};

// arrays used for easier stat type conversion - all number stats returned as
// doubles initially, then converted for easier writing/reading
static const TArray<std::string> integerStats = {
	KILLS_STAT,
	DEATHS_STAT,
	CHESTS_STAT,
	HEAL_STAT,
	EMERALD_STAT,
	ARTIFACTS_STAT,
	REVIVES_STAT,
	LEVEL_STAT
};

static const TArray<std::string> floatStats = {
	DISTANCE_STAT
};

static const TArray<std::string> stringStats = {
	// none right now
};

namespace OnlineStatsLiveHelpers
{
	//helpers for setting a stat value
	void SetStatValue(XblTitleManagedStatistic& UpdateStat, int64 Value)
	{
		UpdateStat.numberValue = Value;
	}

	void SetStatValue(XblTitleManagedStatistic& UpdateStat, double Value)
	{
		UpdateStat.numberValue = Value;
	}

	void SetStatValue(XblTitleManagedStatistic& UpdateStat, const FString& Value)
	{
		UpdateStat.stringValue = TCHAR_TO_UTF8(*Value);
	}

	//template helpers for getting the value of an online stat update
	template<typename T>
	T GetStatValue(const FVariantData& StatUpdate);

	template<>
	double GetStatValue<double>(const FVariantData& StatUpdate)
	{
		switch (StatUpdate.GetType())
		{
		case EOnlineKeyValuePairDataType::Float:
		{
			float Result;
			StatUpdate.GetValue(Result);
			return Result;
		}
		break;

		case EOnlineKeyValuePairDataType::Double:
		{
			double Result;
			StatUpdate.GetValue(Result);
			return Result;
		}
		break;
		}

		return 0.0f;
	}

	template<>
	int64 GetStatValue<int64>(const FVariantData& StatUpdate)
	{
		switch (StatUpdate.GetType())
		{
		case EOnlineKeyValuePairDataType::Int32:
		{
			int32 Result;
			StatUpdate.GetValue(Result);
			return Result;
		}
		break;

		case EOnlineKeyValuePairDataType::Int64:
		{
			int64 Result;
			StatUpdate.GetValue(Result);
			return Result;
		}
		break;

		case EOnlineKeyValuePairDataType::UInt32:
		{
			uint32 Result;
			StatUpdate.GetValue(Result);
			return Result;
		}
		break;

		case EOnlineKeyValuePairDataType::UInt64:
		{
			uint64 Result;
			StatUpdate.GetValue(Result);
			return Result;
		}
		break;
		}

		return 0;
	}

	template<>
	FString GetStatValue<FString>(const FVariantData& StatUpdate)
	{
		FString Result;
		if (StatUpdate.GetType() == EOnlineKeyValuePairDataType::String)
		{
			StatUpdate.GetValue(Result);
		}
		return Result;
	}

	// helper template to apply a stat modification
	template<typename T>
	bool ApplyStatModification(XblTitleManagedStatistic& UpdateStat, const FOnlineStatUpdate& NewValue, FVariantData& OldValue)
	{
		bool bNeedsUpdate = true;
		bool bUpdated = false;

		//FVariantData stat = NewValue.GetValue();
		T UpdateValue = OnlineStatsLiveHelpers::GetStatValue<T>(NewValue.GetValue());

		switch (NewValue.GetModificationType())
		{
		case FOnlineStatUpdate::EOnlineStatModificationType::Set:
		break;

		case FOnlineStatUpdate::EOnlineStatModificationType::Sum:
		{
			if (UpdateStat.statisticType == XblTitleManagedStatType::Number)
			{
				if (OldValue != nullptr) {
					T CurrentValue = OnlineStatsLiveHelpers::GetStatValue<T>(OldValue);
					UpdateValue += CurrentValue;
				}
			}
		}
		break;

		case FOnlineStatUpdate::EOnlineStatModificationType::Largest:
		{
			if (UpdateStat.statisticType == XblTitleManagedStatType::Number)
			{
				if (OldValue != nullptr) {
					T CurrentValue = OnlineStatsLiveHelpers::GetStatValue<T>(OldValue);
					if (UpdateValue < CurrentValue) {
						UpdateValue = CurrentValue;
					}
				}
			}
		}
		break;

		case FOnlineStatUpdate::EOnlineStatModificationType::Smallest:
		{
			if (UpdateStat.statisticType == XblTitleManagedStatType::Number)
			{
				if (OldValue != nullptr) {
					T CurrentValue = OnlineStatsLiveHelpers::GetStatValue<T>(OldValue);
					if (UpdateValue > CurrentValue) {
						UpdateValue = CurrentValue;
					}
				}
			}
		}
		break;

		default:
		{
			UE_LOG_ONLINE_STATS(Warning, TEXT("Unsupported stat modification type: %d for stat %s"), (int)NewValue.GetModificationType(), *UpdateStat.statisticName);
			bNeedsUpdate = false;
		}
		break;
		}

		// should now update all stats unless there is a problem with the value
		if (bNeedsUpdate)
		{
			OnlineStatsLiveHelpers::SetStatValue(UpdateStat, UpdateValue);
			OldValue.SetValue(UpdateValue);
			bUpdated = true;
		}
		return bUpdated;
	}

	bool ApplyStatUpdate(XblTitleManagedStatistic& UpdateStat, const FOnlineStatUpdate& NewValue, FVariantData& OldValue)
	{
		bool bUpdated = false;
		try
		{
			// try to apply the value update
			switch (NewValue.GetType())
			{
			case EOnlineKeyValuePairDataType::Int32:
			case EOnlineKeyValuePairDataType::Int64:
			case EOnlineKeyValuePairDataType::UInt32:
			case EOnlineKeyValuePairDataType::UInt64:
			{
				UpdateStat.statisticType = XblTitleManagedStatType::Number;
				bUpdated = OnlineStatsLiveHelpers::ApplyStatModification<int64>(UpdateStat, NewValue, OldValue);
			}
			break;

			case EOnlineKeyValuePairDataType::Float:
			case EOnlineKeyValuePairDataType::Double:
			{
				UpdateStat.statisticType = XblTitleManagedStatType::Number;
				bUpdated = OnlineStatsLiveHelpers::ApplyStatModification<double>(UpdateStat, NewValue, OldValue);
			}
			break;

			case EOnlineKeyValuePairDataType::String:
			{
				UpdateStat.statisticType = XblTitleManagedStatType::String;
				bUpdated = OnlineStatsLiveHelpers::ApplyStatModification<FString>(UpdateStat, NewValue, OldValue);
			}
			break;

			default:
			{
				bUpdated = false;
				UE_LOG_ONLINE_STATS(Warning, TEXT("Unsupported stat update type: %s"), EOnlineKeyValuePairDataType::ToString(NewValue.GetType()));
			}
			break;
			}

		}
		catch (const std::exception& e)
		{
			UE_LOG_ONLINE_STATS(Warning, TEXT("Exception setting stat value: Name:%s, Type:%s, Exception: %ls"), *UpdateStat.statisticName, EOnlineKeyValuePairDataType::ToString(NewValue.GetType()), e.what());
			bUpdated = false;
		}

		return bUpdated;
	}

	void ModifyQueuedStats(FOnlineStatUpdate& QueuedStat, const FOnlineStatUpdate& NewStat)
	{
		FVariantData QueuedValue = QueuedStat.GetValue();
		XblTitleManagedStatistic UpdatedStat = {};
		ApplyStatUpdate(UpdatedStat, NewStat, QueuedValue);

		FOnlineStatValue NewValue;

		switch (NewStat.GetType())
		{
		case EOnlineKeyValuePairDataType::Int32: NewValue.SetValue(static_cast<int32>(UpdatedStat.numberValue)); break;
		case EOnlineKeyValuePairDataType::Int64: NewValue.SetValue(static_cast<int64>(UpdatedStat.numberValue)); break;
		case EOnlineKeyValuePairDataType::UInt32: NewValue.SetValue(static_cast<uint32>(UpdatedStat.numberValue)); break;
		case EOnlineKeyValuePairDataType::UInt64: NewValue.SetValue(static_cast<uint64>(UpdatedStat.numberValue)); break;
		case EOnlineKeyValuePairDataType::Float: NewValue.SetValue(static_cast<float>(UpdatedStat.numberValue)); break;
		case EOnlineKeyValuePairDataType::Double: NewValue.SetValue(static_cast<double>(UpdatedStat.numberValue)); break;
		case EOnlineKeyValuePairDataType::String: NewValue.SetValue(FString(UpdatedStat.stringValue)); break;
		default:
		{
			UE_LOG_ONLINE_STATS(Warning, TEXT("Unsupported stat update type: %s"), EOnlineKeyValuePairDataType::ToString(NewStat.GetType()));
		}
		break;
		}

		QueuedStat.Set(NewValue, QueuedStat.GetModificationType());
	}
}


FOnlineStatsDungeons::FOnlineStatsDungeons(FOnlineSubsystemDungeons* InSubsystem) : DungeonsSubsystem(InSubsystem)
{
	
}

void FOnlineStatsDungeons::QueryStats(const TSharedRef<const FUniqueNetId> LocalUserId, const TSharedRef<const FUniqueNetId> StatsUser, const FOnlineStatsQueryUserStatsComplete& Delegate)
{
	if (!IsAllSetUp(LocalUserId))
	{
		const TSharedPtr<const FOnlineStatsUserStats> Empty;
		Delegate.ExecuteIfBound(ONLINE_ERROR(EOnlineErrorResult::InvalidParams), Empty);
	}

	FOnlineSubsystemDungeons* DungeonsSubSystem = static_cast<FOnlineSubsystemDungeons*>(GetSubsystem());
	FOnlineIdentityDungeons* IdentityInterface = DungeonsSubSystem->GetDungeonsIdentityInterface();
	FUniqueNetIdDungeonsRef DungeonsNetId = StaticCastSharedRef<const FUniqueNetIdDungeons>(LocalUserId);
	std::shared_ptr<const XBLUserInfo> User = IdentityInterface->GetUserFromXuid(DungeonsNetId->XUID);

	if (User != nullptr)
	{
		try
		{
			// stat names taken from static array
			static const char* statisticNames[NoStats] = {};
			for (int i = 0; i < NoStats; i++)
			{
				statisticNames[i] = statNamesArray[i].c_str();
			}

			XblContextHandle XBLcontext = User->XBLcontext;
			uint64_t xboxUserId = DungeonsNetId->XUID;

			HrLog(XblUserStatisticsGetSingleUserStatisticsAsync(XBLcontext, xboxUserId, OnlineSubsystemConstants::SCID, statisticNames, NoStats, AsyncTasks::CreateAsyncBlock([this, xboxUserId, Delegate](XAsyncBlock* asyncBlock)
			{
				FOnlineStatsDungeons* StatsInterface = static_cast<FOnlineStatsDungeons*>(this);
				size_t resultSize;

				if (HrSuccess(XblUserStatisticsGetSingleUserStatisticsResultSize(asyncBlock, &resultSize), "[Feature Stats] XblUserStatisticsGetSingleUserStatisticsResultSize"))
				{
					std::vector<char> buffer(resultSize, 0);
					XblUserStatisticsResult* results{};
					TArray<DungeonsStatistic> StatsQueryResults;

					if (HrSuccess(XblUserStatisticsGetSingleUserStatisticsResult(asyncBlock, resultSize, buffer.data(), &results, nullptr), "[Feature Stats] XblUserStatisticsGetSingleUserStatisticsResult"))
					{
						for (unsigned int i = 0; i < results->serviceConfigStatistics->statisticsCount; i++)
						{
							DungeonsStatistic stat;
							stat.name = results->serviceConfigStatistics->statistics[i].statisticName;
							stat.type = results->serviceConfigStatistics->statistics[i].statisticType;
							stat.value = results->serviceConfigStatistics->statistics[i].value;

							StatsQueryResults.Emplace(stat);
						}
					}
					bool WasSuccessful = results != nullptr;
					StatsInterface->OnStatsQueried(WasSuccessful, xboxUserId, StatsQueryResults, Delegate);
				}
			}
			, DungeonsSubSystem->GetQueueHandle()))
			, "[Feature Stats] XblUserStatisticsGetSingleUserStatisticsAsync");
		}
		catch (const std::exception& e)
		{
			UE_LOG_ONLINE_STATS(Warning, TEXT("XblUserStatisticsGetSingleUserStatisticsAsync Exception: %s."), e.what());
		}
	}
	else
	{
		UE_LOG_ONLINE_STATS(Warning, TEXT("%s is not a local valid user"), *LocalUserId->ToDebugString());
	}
}

void FOnlineStatsDungeons::QueryStats(const TSharedRef<const FUniqueNetId> LocalUserId, const TArray<TSharedRef<const FUniqueNetId>>& StatUsers, const TArray<FString>& StatNames, const FOnlineStatsQueryUsersStatsComplete& Delegate) {
	assert(0);
}

TSharedPtr<const FOnlineStatsUserStats> FOnlineStatsDungeons::GetStats(const TSharedRef<const FUniqueNetId> StatsUserId) const
{
	TSharedPtr<FOnlineStatsUserStats> UserStats = MakeShared<FOnlineStatsUserStats>(StatsUserId);
	if (CachedStats.Num() <= 0 || !CachedStats.Contains(StatsUserId))
	{
		return UserStats;
	}

	const FOnlineStatsUserStats* FoundStats = CachedStats.Find(StatsUserId);
	if (FoundStats && FoundStats->Stats.Num() > 0)
	{
		UserStats = MakeShared<FOnlineStatsUserStats>(*FoundStats);
	}
	
	return UserStats;
}

void FOnlineStatsDungeons::UpdateStats(const TSharedRef<const FUniqueNetId> LocalUserId, const TArray<FOnlineStatsUserUpdatedStats>& UpdatedUserStats, const FOnlineStatsUpdateStatsComplete& Delegate)
{
	if (!IsAllSetUp(LocalUserId))
	{
		return;
	}

	FOnlineSubsystemDungeons* DungeonsSubSystem = static_cast<FOnlineSubsystemDungeons*>(GetSubsystem());
	FOnlineIdentityDungeons* IdentityInterface = DungeonsSubSystem->GetDungeonsIdentityInterface();

	for (const FOnlineStatsUserUpdatedStats& UpdatedUserStat : UpdatedUserStats)
	{
		if (FOnlineStatsUserUpdatedStats* QueuedUsersStats = QueuedStats.Find(UpdatedUserStat.Account))
		{
			for (const TPair<FString, FOnlineStatUpdate>& UpdatePair : UpdatedUserStat.Stats)
			{
				if (FOnlineStatUpdate* QueuedStat = QueuedUsersStats->Stats.Find(UpdatePair.Key))
				{
					const FOnlineStatValue& QueuedValue = QueuedStat->GetValue();
					OnlineStatsLiveHelpers::ModifyQueuedStats(*QueuedStat, UpdatePair.Value);
				}
				else
				{
					QueuedUsersStats->Stats.Add(UpdatePair.Key, UpdatePair.Value);
				}
			}
		}
		else
		{
			QueuedStats.Emplace(UpdatedUserStat.Account, UpdatedUserStat);
		}
	}
	Delegate.ExecuteIfBound(ONLINE_ERROR(EOnlineErrorResult::Success));
}

#if !UE_BUILD_SHIPPING
void FOnlineStatsDungeons::ResetStats(const TSharedRef<const FUniqueNetId> StatsUserId)
{
	if (!IsAllSetUp(StatsUserId))
	{
		return;
	}

	FOnlineStatsUserUpdatedStats BlankUserStats = FOnlineStatsUserUpdatedStats(StatsUserId);
	FOnlineSubsystemDungeons* DungeonsSubSystem = static_cast<FOnlineSubsystemDungeons*>(GetSubsystem());
	FOnlineIdentityDungeons* IdentityInterface = DungeonsSubSystem->GetDungeonsIdentityInterface();

	try
	{
		bool bSuccess = false;
		FUniqueNetIdDungeonsRef DungeonsNetId = StaticCastSharedRef<const FUniqueNetIdDungeons>(StatsUserId);
		std::shared_ptr<const XBLUserInfo> User = IdentityInterface->GetUserFromXuid(DungeonsNetId->XUID);
		if (User != nullptr)
		{
			XblContextHandle XBLcontext = User->XBLcontext;
			uint64_t xboxUserId = DungeonsNetId->XUID;
			WriteStats(BlankUserStats, XBLcontext, xboxUserId);
		}
		else
		{
			UE_LOG_ONLINE_STATS(Warning, TEXT("%s is not a local valid user"), *StatsUserId->ToDebugString());
		}
	}
	catch (const std::exception& e)
	{
		UE_LOG_ONLINE_STATS(Warning, TEXT("FOnlineAsyncTaskLiveStatsUpdate Exception: %s."), e.what());
	}
}
#endif // !UE_BUILD_SHIPPING

bool FOnlineStatsDungeons::WriteStats(const FOnlineStatsUserUpdatedStats& UpdatedUserStat, XblContextHandle& XBLcontext, uint64_t XUID)
{
	try
	{
		// stat names taken from static array
		const char* statisticNames[NoStats] = {};
		for (int i = 0; i < NoStats; i++)
		{
			statisticNames[i] = statNamesArray[i].c_str();
		}

		FOnlineSubsystemDungeons* DungeonsSubSystem = static_cast<FOnlineSubsystemDungeons*>(GetSubsystem());
		FOnlineStatsUserStats* CurrentStats = CachedStats.Find(UpdatedUserStat.Account);;
		if (!CurrentStats)
		{
			FOnlineStatsUserStats UserStats(UpdatedUserStat.Account);
			CurrentStats = &CachedStats.Emplace(UpdatedUserStat.Account, UserStats);
		}
		TArray<XblTitleManagedStatistic> XBLStatsToUpdate;

		int count = 0;
		for (const TPair<FString, FOnlineStatUpdate>& UpdatePair : UpdatedUserStat.Stats)
		{
			FVariantData& oldValue = CurrentStats->Stats.FindOrAdd(UpdatePair.Key);
			XblTitleManagedStatistic UpdatedStat = {};
			int nameIndex = 0;
			if (statNamesArray.Find(TCHAR_TO_UTF8(*UpdatePair.Key), nameIndex))
			{
				UpdatedStat.statisticName = statisticNames[nameIndex];
				if (OnlineStatsLiveHelpers::ApplyStatUpdate(UpdatedStat, UpdatePair.Value, oldValue))
				{
					XBLStatsToUpdate.Emplace(UpdatedStat);
					count++;
				}
			}
		}

		HRESULT hr = XblTitleManagedStatsWriteAsync(XBLcontext, XUID, &XBLStatsToUpdate[0], count, AsyncTasks::CreateAsyncBlock([](XAsyncBlock* asyncBlock) {}
		, DungeonsSubSystem->GetQueueHandle()));
		HrLog(hr, "[Feature Stats] XblUserStatisticsGetSingleUserStatisticsAsync");
		if (SUCCEEDED(hr))
		{
			return true;
		}
	}
	catch (const std::exception& e)
	{
		UE_LOG_ONLINE_STATS(Warning, TEXT("XblTitleManagedStatsWriteAsync Exception: %s."), e.what());
	}
	return false;
}

void FOnlineStatsDungeons::FlushStats()
{
	FOnlineSubsystemDungeons* DungeonsSubSystem = static_cast<FOnlineSubsystemDungeons*>(GetSubsystem());
	FOnlineIdentityDungeons* IdentityInterface = DungeonsSubSystem->GetDungeonsIdentityInterface();

	TArray<TSharedRef<const FUniqueNetId>> StatsToRemove;

	for (const auto& UpdatedUserStats : QueuedStats)
	{
		bool bSuccess = false;
		FUniqueNetIdDungeonsRef DungeonsNetId = StaticCastSharedRef<const FUniqueNetIdDungeons>(UpdatedUserStats.Value.Account);
		std::shared_ptr<const XBLUserInfo> User = IdentityInterface->GetUserFromXuid(DungeonsNetId->XUID);
		if (User != nullptr)
		{
			XblContextHandle XBLcontext = User->XBLcontext;
			uint64_t xboxUserId = DungeonsNetId->XUID;
			if (WriteStats(UpdatedUserStats.Value, XBLcontext, xboxUserId))
			{
				StatsToRemove.Emplace(UpdatedUserStats.Key);
			}
		}
		else
		{
			UE_LOG_ONLINE_STATS(Warning, TEXT("%s is not a local valid user"), *UpdatedUserStats.Value.Account->ToDebugString());
		}
	}

	for (TSharedRef<const FUniqueNetId> key : StatsToRemove)
	{
		QueuedStats.Remove(key);
	}
}

void FOnlineStatsDungeons::OnStatsQueried(bool WasSuccessful, uint64_t XUID, TArray<DungeonsStatistic>& StatsQueryResults, const FOnlineStatsQueryUserStatsComplete& Delegate)
{
	FOnlineSubsystemDungeons* DungeonsSubSystem = static_cast<FOnlineSubsystemDungeons*>(GetSubsystem());
	FOnlineIdentityDungeons* IdentityInterface = DungeonsSubSystem->GetDungeonsIdentityInterface();
	auto user = IdentityInterface->GetUserFromXuid(XUID);
	if (user)
	{
		TSharedPtr<const FUniqueNetId> PlayerId = user->UniqueNetId;
		TSharedPtr<const FOnlineStatsUserStats> stats = nullptr;

		if (WasSuccessful)
		{
			UE_LOG_ONLINE(Log, TEXT("[Feature Stats] Stats queried successfully"));
			if (StatsQueryResults.Num() > 0)
			{
				AddStatsForPlayer(PlayerId.ToSharedRef(), StatsQueryResults);
			}
			stats = GetStats(PlayerId.ToSharedRef());
			Delegate.ExecuteIfBound(ONLINE_ERROR(EOnlineErrorResult::Success), stats);
		}
		else
		{
			UE_LOG_ONLINE(Log, TEXT("[Feature Stats] Stats query failed"));
			Delegate.ExecuteIfBound(ONLINE_ERROR(EOnlineErrorResult::Unknown), stats);
		}
	}
}

void FOnlineStatsDungeons::AddStatsForPlayer(TSharedRef<const FUniqueNetId> PlayerId, TArray<DungeonsStatistic>& Stats)
{
	CachedStats.Add(PlayerId, ConvertStats(PlayerId, Stats));
}

void FOnlineStatsDungeons::RemoveStatsForUser(TSharedRef<const FUniqueNetId> PlayerId)
{
	if (CachedStats.Contains(PlayerId))
	{
		CachedStats.Remove(PlayerId);
	}
	if (QueuedStats.Contains(PlayerId))
	{
		QueuedStats.Remove(PlayerId);
	}
}

FOnlineStatsUserStats FOnlineStatsDungeons::ConvertStats(const TSharedRef<const FUniqueNetId> StatsUserId, TArray<DungeonsStatistic>& DungeonsStats)
{
	FOnlineStatsUserStats UserStats(StatsUserId);

	for (DungeonsStatistic& Stat : DungeonsStats)
	{
		// Stat.type for every number stat returns as "double" for some reason, converting here
		FOnlineStatValue& statValue = UserStats.Stats.Emplace(FString(Stat.name.data()));
		if (integerStats.Contains(Stat.name.data()))
		{
			int64 val = std::stoi(Stat.value);
			statValue.SetValue(val);
		}
		if (floatStats.Contains(Stat.name.data()))
		{
			double val = std::stod(Stat.value);
			statValue.SetValue(val);
		}
		if (stringStats.Contains(Stat.name.data()))
		{
			FString val = Stat.value.data();
			statValue.SetValue(val);
		}
	}

	return UserStats;
}

FOnlineSubsystemDungeons* FOnlineStatsDungeons::GetSubsystem()
{
	return DungeonsSubsystem;
}

bool FOnlineStatsDungeons::IsAllSetUp(const TSharedRef<const FUniqueNetId> NetID) const
{
	if (!DungeonsSubsystem)
	{
		UE_LOG_ONLINE(Warning, TEXT("[Feature Stats] Unable to write stats, DungeonsSubsystem is NULL"));
		return false;
	}

	if (!NetID.Get().IsValid())
	{
		return false;
	}

	FOnlineIdentityDungeons* IdentityInterface = DungeonsSubsystem->GetDungeonsIdentityInterface();

	if (!IdentityInterface)
	{
		UE_LOG_ONLINE(Warning, TEXT("[Feature Stats] Unable to write stats, IdentityInterface is NULL"));
		return false;
	}

	return true;
}

void FOnlineStatsDungeons::Tick(float DeltaTime)
{
	TimeSinceLastFlush += DeltaTime;
	if (TimeSinceLastFlush >= 300)
	{
		FlushStats();
		TimeSinceLastFlush = 0;
	}
}

#undef LOCTEXT_NAMESPACE
