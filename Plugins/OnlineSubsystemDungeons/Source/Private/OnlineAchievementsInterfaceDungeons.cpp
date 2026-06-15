#include "PCHOnlineDungeonsSubsystem.h"
#include "OnlineAchievementsInterfaceDungeons.h"
#include "OnlineSubsystemDungeonsTypes.h"
#include "OnlineIdentityDungeons.h"
#include "xsapiServicesInclude.h"
#include "utils.h"
#include "OnlineSubsystemDungeons.h"
#include "SharedPointer.h"
#include <string>
#include "Paths.h"
#include "FileHelper.h"
#include "Kismet/GameplayStatics.h"
#include "HrLog.h"

namespace internal
{

void XblAchievementToDungeonsAchievement(const XblAchievement& XBLAchievement, FDungeonsAchievement& DungeonsAchievement)
{
	DungeonsAchievement.id = FString(UTF8_TO_TCHAR(XBLAchievement.id));
	DungeonsAchievement.serviceConfigurationId = FString(UTF8_TO_TCHAR(XBLAchievement.serviceConfigurationId));
	DungeonsAchievement.name = FString(UTF8_TO_TCHAR(XBLAchievement.name));
	for (int i = 0; i < XBLAchievement.titleAssociationsCount; i++)
	{
		FDungeonsTitleAssociation dungeonsTitleAssociation;
		dungeonsTitleAssociation.name = FString(UTF8_TO_TCHAR(XBLAchievement.titleAssociations[i].name));
		dungeonsTitleAssociation.titleId = XBLAchievement.titleAssociations[i].titleId;
		DungeonsAchievement.titleAssociations.Emplace(dungeonsTitleAssociation);
	}
	DungeonsAchievement.progressState = XBLAchievement.progressState;
	for (int i = 0; i < XBLAchievement.progression.requirementsCount; i++)
	{
		FDungeonsAchievementRequirement dungeonsRequirement;
		dungeonsRequirement.id = FString(UTF8_TO_TCHAR(XBLAchievement.progression.requirements[i].id));
		dungeonsRequirement.currentProgressValue = FString(UTF8_TO_TCHAR(XBLAchievement.progression.requirements[i].currentProgressValue));
		dungeonsRequirement.targetProgressValue = FString(UTF8_TO_TCHAR(XBLAchievement.progression.requirements[i].targetProgressValue));
		DungeonsAchievement.progression.requirements.Emplace(dungeonsRequirement);
	}
	DungeonsAchievement.progression.timeUnlocked = XBLAchievement.progression.timeUnlocked;
	for (int i = 0; i < XBLAchievement.mediaAssetsCount; i++)
	{
		FDungeonsAchievementMediaAsset dungeonsMediaAsset;
		dungeonsMediaAsset.name = FString(UTF8_TO_TCHAR(XBLAchievement.mediaAssets[i].name));
		dungeonsMediaAsset.mediaAssetType = XBLAchievement.mediaAssets[i].mediaAssetType;
		dungeonsMediaAsset.url = FString(UTF8_TO_TCHAR(XBLAchievement.mediaAssets[i].url));
		DungeonsAchievement.mediaAssets.Emplace(dungeonsMediaAsset);
	}
	for (int i = 0; i < XBLAchievement.platformsAvailableOnCount; i++)
	{
		DungeonsAchievement.platformsAvailableOn.Emplace(XBLAchievement.platformsAvailableOn[i]);
	}
	DungeonsAchievement.isSecret = XBLAchievement.isSecret;
	DungeonsAchievement.unlockedDescription = FString(UTF8_TO_TCHAR(XBLAchievement.unlockedDescription));
	DungeonsAchievement.lockedDescription = FString(UTF8_TO_TCHAR(XBLAchievement.lockedDescription));
	DungeonsAchievement.productId = FString(UTF8_TO_TCHAR(XBLAchievement.productId));
	DungeonsAchievement.type = XBLAchievement.type;
	DungeonsAchievement.participationType = XBLAchievement.participationType;
	DungeonsAchievement.available.startDate = XBLAchievement.available.startDate;
	DungeonsAchievement.available.endDate = XBLAchievement.available.endDate;
	for (int i = 0; i < XBLAchievement.rewardsCount; i++)
	{
		FDungeonsAchievementReward dungeonsReward;
		dungeonsReward.name = FString(UTF8_TO_TCHAR(XBLAchievement.rewards[i].name));
		dungeonsReward.description = FString(UTF8_TO_TCHAR(XBLAchievement.rewards[i].description));
		dungeonsReward.value = FString(UTF8_TO_TCHAR(XBLAchievement.rewards[i].value));
		dungeonsReward.rewardType = XBLAchievement.rewards[i].rewardType;
		dungeonsReward.valueType = FString(UTF8_TO_TCHAR(XBLAchievement.rewards[i].valueType));
		FString mediaAssetName = FString(UTF8_TO_TCHAR(XBLAchievement.rewards[i].mediaAsset->name));
		dungeonsReward.mediaAsset = DungeonsAchievement.mediaAssets.FindByPredicate([mediaAssetName](const FDungeonsAchievementMediaAsset& mediaAsset) { return mediaAssetName.Equals(mediaAsset.name); });
		DungeonsAchievement.rewards.Emplace(dungeonsReward);
	}
	DungeonsAchievement.estimatedUnlockTime = XBLAchievement.estimatedUnlockTime;
	DungeonsAchievement.deepLink = FString(UTF8_TO_TCHAR(XBLAchievement.deepLink));
	DungeonsAchievement.isRevoked = XBLAchievement.isRevoked;
}

void AppendXblAchievements(TArray<FDungeonsAchievement>& Achievements, const XblAchievement* NewAchievements, size_t NewAchievementsCount)
{
	for (size_t i = 0; i < NewAchievementsCount; ++i)
	{
		FDungeonsAchievement dungeonsAchievement;
		XblAchievementToDungeonsAchievement(NewAchievements[i], dungeonsAchievement);
		Achievements.Emplace(dungeonsAchievement);
	}
}

}

FOnlineSubsystemDungeons* FOnlineAchievementsDungeons::GetSubsystem()
{
	return DungeonsSubsystem;
}

FString FOnlineAchievementsDungeons::GetAchievementImageURL(const FUniqueNetId& PlayerId, int AchievementIndex) const
{
	const TArray<FDungeonsAchievement>* achievementsPtr = CachedAchievements.Find(PlayerId.AsShared());
	if (achievementsPtr && achievementsPtr->Num() > 0)
	{
		const TArray<FDungeonsAchievement>& achievements = *achievementsPtr;
		FDungeonsAchievement achievement = achievements[AchievementIndex];
		for (FDungeonsAchievementMediaAsset& mediaAsset : achievement.mediaAssets)
		{
			if (mediaAsset.mediaAssetType == XblAchievementMediaAssetType::Icon)
			{
				return mediaAsset.url;
			}
		}
	}
	return "";
}

FOnlineAchievementDesc FOnlineAchievementsDungeons::GetAchievementDescription(const FUniqueNetId& PlayerId, int AchievementIndex)
{
	FOnlineAchievementDesc achievementDescription;
	const TArray<FDungeonsAchievement>* achievementsPtr = CachedAchievements.Find(PlayerId.AsShared());
	if (achievementsPtr && achievementsPtr->Num() > 0)
	{
		const TArray<FDungeonsAchievement>& achievements = *achievementsPtr;
		achievementDescription.Title = FText::FromString(achievements[AchievementIndex].name);
		achievementDescription.UnlockedDesc = FText::FromString(achievements[AchievementIndex].unlockedDescription);
		achievementDescription.LockedDesc = FText::FromString(achievements[AchievementIndex].lockedDescription);
		achievementDescription.bIsHidden = achievements[AchievementIndex].isSecret;
		achievementDescription.UnlockTime = achievements[AchievementIndex].progression.timeUnlocked;
	}
	return achievementDescription;
}

void FOnlineAchievementsDungeons::OnAchievementWritten(bool Success, uint64_t XUID, const FString& AchievementId, float Percentage, const FOnAchievementsWrittenDelegate& Delegate)
{
	FOnlineSubsystemDungeons* DungeonsSubSystem = static_cast<FOnlineSubsystemDungeons*>(GetSubsystem());
	FOnlineIdentityDungeons* IdentityInterface = DungeonsSubSystem->GetDungeonsIdentityInterface();

	auto user = IdentityInterface->GetUserFromXuid(XUID);
	if (Success && user != nullptr)
	{
		TSharedPtr<const FUniqueNetId> PlayerId = user->UniqueNetId;

		Delegate.ExecuteIfBound(*PlayerId.Get(), Success);
		UE_LOG_ONLINE(Log, TEXT("[Achievements] Achievement %s written successfully"), *AchievementId);

		if (CachedAchievements.Num() <= 0)
		{
			return;
		}

		TArray<FDungeonsAchievement>* AchievementsPtr = CachedAchievements.Find(PlayerId.ToSharedRef());
		if (AchievementsPtr && AchievementsPtr->Num() > 0)
		{
			const TArray<FDungeonsAchievement>& Achievements = *AchievementsPtr;
			int ArrayIndex = Achievements.IndexOfByPredicate([AchievementId](const FDungeonsAchievement& achievement) { return AchievementId.Equals(achievement.id); });

			// D11.SSN
#ifdef HAS_ACHIEVEMENT_WRITTEN_TRACKER_DELEGATE
			if (ArrayIndex >= 0 && ArrayIndex <= Achievements.Num())
			{
				FOnlineAchievementDesc achievementDesc = GetAchievementDescription(*PlayerId.Get(), ArrayIndex);
				OnAchievementWrittenTrackerDelegates.Broadcast(Success, PlayerId, ArrayIndex + 1, Achievements[ArrayIndex].name, Percentage, GetAchievementImageURL(*PlayerId.Get(), ArrayIndex), achievementDesc.Title.ToString(), achievementDesc.UnlockedDesc.ToString());
			}
#endif
		}
	}
}

FOnlineAchievementsDungeons::FOnlineAchievementsDungeons(FOnlineSubsystemDungeons* InDungeonsSubsystem)
	: DungeonsSubsystem(InDungeonsSubsystem)
{
	LoadAchievementsFromJson();
	TitleId = GetTitleId();
}

void FOnlineAchievementsDungeons::WriteAchievements(const FUniqueNetId& PlayerId, FOnlineAchievementsWriteRef& WriteObject, const FOnAchievementsWrittenDelegate& Delegate)
{
	if (!IsAllSetUp(PlayerId))
	{
		Delegate.ExecuteIfBound(PlayerId, false);
		return;
	}

	for (FStatPropertyArray::TConstIterator It(WriteObject->Properties); It; ++It)
	{
		float Percent = 0.0f;
		It.Value().GetValue(Percent);
		Percent = FMath::Clamp(Percent, 0.0f, 100.0f);

		FString achievementName = It.Key().ToString();
		FString AchievementIndexStr = GetAchievementIndex(achievementName);

		UpdateAchievement(PlayerId, AchievementIndexStr, Percent, Delegate);
	}
}

void FOnlineAchievementsDungeons::UpdateAchievement(const FUniqueNetId& PlayerId, FString AchievementIndex, float Percent, const FOnAchievementsWrittenDelegate& Delegate)
{
	FOnlineSubsystemDungeons* DungeonsSubSystem = static_cast<FOnlineSubsystemDungeons*>(GetSubsystem());
	FOnlineIdentityDungeons* IdentityInterface = DungeonsSubSystem->GetDungeonsIdentityInterface();
	FUniqueNetIdDungeonsRef DungeonsNetId = StaticCastSharedRef<const FUniqueNetIdDungeons>(PlayerId.AsShared());
	std::shared_ptr<const XBLUserInfo> User = IdentityInterface->GetUserFromXuid(DungeonsNetId->XUID);

	if (User != nullptr)
	{
		const XblContextHandle& XBLcontext = User->XBLcontext;
		uint64_t xboxUserId = DungeonsNetId->IsValid() ? DungeonsNetId->XUID : 0;

		HrLog(XblAchievementsUpdateAchievementAsync(XBLcontext, xboxUserId, TCHAR_TO_ANSI(*AchievementIndex), Percent, AsyncTasks::CreateAsyncBlock([this, xboxUserId, AchievementIndex, Percent, Delegate](XAsyncBlock* asyncBlock)
		{
			auto HR = XAsyncGetStatus(asyncBlock, true);
			bool WasSuccessful = SUCCEEDED(HR);
			FOnlineAchievementsDungeons* AchievementsInterface = static_cast<FOnlineAchievementsDungeons*>(this);
			AchievementsInterface->OnAchievementWritten(WasSuccessful, xboxUserId, AchievementIndex, Percent, Delegate);
		},
			DungeonsSubSystem->GetQueueHandle())), "[Achievements] XblAchievementsUpdateAchievementAsync");
	}
}

FString FOnlineAchievementsDungeons::GetAchievementIndex(FString achievementName) const
{
	const int32* AchievementIndex = AchievementsMap.AchievementMap.Find(achievementName);
	if (!AchievementIndex)
	{
		UE_LOG_ONLINE(Warning, TEXT("FOnlineAchievementsLive::WriteAchievements: No mapping for achievement %s"), *achievementName);
		return "";
	}

	return FString::FromInt(*AchievementIndex);
}

bool FOnlineAchievementsDungeons::IsAllSetUp(const FUniqueNetId &PlayerId) const
{
	if (!DungeonsSubsystem)
	{
		UE_LOG_ONLINE(Warning, TEXT("[Achievements] Unable to WriteAchievements, DungeonsSubsystem is NULL"));
		return false;
	}

	if (!PlayerId.IsValid())
	{
		return false;
	}

	FOnlineIdentityDungeons* IdentityInterface = DungeonsSubsystem->GetDungeonsIdentityInterface();

	if (!IdentityInterface)
	{
		UE_LOG_ONLINE(Warning, TEXT("[Achievements] Unable to WriteAchievements, IdentityInterface is NULL"));
		return false;
	}

	return true;
}

void FOnlineAchievementsDungeons::QueryAchievements(const FUniqueNetId& PlayerId, const FOnQueryAchievementsCompleteDelegate& Delegate /*= FOnQueryAchievementsCompleteDelegate()*/)
{
	if (!IsAllSetUp(PlayerId))
	{
		Delegate.ExecuteIfBound(PlayerId, false);
		return;
	}

	FOnlineSubsystemDungeons* DungeonsSubSystem = static_cast<FOnlineSubsystemDungeons*>(GetSubsystem());
	FOnlineIdentityDungeons* IdentityInterface = DungeonsSubsystem->GetDungeonsIdentityInterface();
	FUniqueNetIdDungeonsRef DungeonsNetId = StaticCastSharedRef<const FUniqueNetIdDungeons>(PlayerId.AsShared());

	std::shared_ptr<const XBLUserInfo> User = IdentityInterface->GetUserFromXuid(DungeonsNetId->XUID);
	if (User != nullptr)
	{
		auto XBLcontext = User->XBLcontext;
		uint64_t xboxUserId = DungeonsNetId->XUID;

		HRESULT hr = XblAchievementsGetAchievementsForTitleIdAsync(XBLcontext, xboxUserId, TitleId, XblAchievementType::All, false, XblAchievementOrderBy::TitleId, 0, 0, AsyncTasks::CreateAsyncBlock([this, xboxUserId, Delegate, DungeonsSubSystem](XAsyncBlock* asyncBlock)
		{
			FOnlineAchievementsDungeons* AchievementsInterface = static_cast<FOnlineAchievementsDungeons*>(this);
			XblAchievementsResultHandle resultHandle = nullptr;
			TSharedPtr<TArray<FDungeonsAchievement>> AchievementsForPlayer(new TArray<FDungeonsAchievement>());

			if (HrSuccess(XblAchievementsGetAchievementsForTitleIdResult(asyncBlock, &resultHandle), "[Achievements] XblAchievementsGetAchievementsForTitleIdResult"))
			{
				UE_LOG_ONLINE(Log, TEXT("[Achievements] Successfully found achievements"));
				const XblAchievement* achievements = nullptr;
				size_t achievementsCount = 0;

				if (HrSuccess(XblAchievementsResultGetAchievements(resultHandle, &achievements, &achievementsCount), "[Achievements] XblAchievementsResultGetAchievements"))
				{
					internal::AppendXblAchievements(*AchievementsForPlayer.Get(), achievements, achievementsCount);
					achievements = nullptr;
					bool WasSuccessful = AchievementsForPlayer->Num() > 0;

					bool hasNext = false;
					if (HrSuccess(XblAchievementsResultHasNext(resultHandle, &hasNext), "[Achievements] XblAchievementsResultHasNext"))
					{
						if (hasNext)
						{
							GetAchievementsNextPage(resultHandle, AchievementsForPlayer, DungeonsSubSystem, xboxUserId, Delegate);
						}
						else 
						{
							AchievementsInterface->OnAchievementsQueried(WasSuccessful, xboxUserId, AchievementsForPlayer, Delegate);
						}
					}
					else
					{
						AchievementsInterface->OnAchievementsQueried(WasSuccessful, xboxUserId, AchievementsForPlayer, Delegate);
					}
				}
				else
				{
					AchievementsInterface->OnAchievementsQueried(false, xboxUserId, AchievementsForPlayer, Delegate);
				}
				XblAchievementsResultCloseHandle(resultHandle);
			}
			else
			{
				AchievementsInterface->OnAchievementsQueried(false, xboxUserId, AchievementsForPlayer, Delegate);
			}
		}
		, DungeonsSubSystem->GetQueueHandle()));
		HrLog(hr, "[Achievement] XblAchievementsGetAchievementsForTitleIdAsync");
	}
}

// D11.SSN - function is recursive to continually find next pages of achievements,
//           as next page queries requires the previous result's handle
void FOnlineAchievementsDungeons::GetAchievementsNextPage(XblAchievementsResultHandle previousResultHandle, TSharedPtr<TArray<FDungeonsAchievement>> AchievementsForPlayer, FOnlineSubsystemDungeons* DungeonsSubSystem, uint64_t xboxUserId, const FOnQueryAchievementsCompleteDelegate& Delegate)
{
	HRESULT hr = XblAchievementsResultGetNextAsync(previousResultHandle, 0, AsyncTasks::CreateAsyncBlock([this, AchievementsForPlayer, DungeonsSubSystem, xboxUserId, Delegate](XAsyncBlock* asyncBlock)
	{
		FOnlineAchievementsDungeons* AchievementsInterface = static_cast<FOnlineAchievementsDungeons*>(this);
		XblAchievementsResultHandle nextResultHandle = nullptr;
		bool WasSuccessful = AchievementsForPlayer->Num() > 0;

		if (HrSuccess(XblAchievementsResultGetNextResult(asyncBlock, &nextResultHandle), "[Achievements] XblAchievementsResultGetNextResult"))
		{
			UE_LOG_ONLINE(Log, TEXT("[Achievements] Successfully found achievements next page"));
			const XblAchievement* achievements = nullptr;
			size_t achievementsCount = 0;

			if (HrSuccess(XblAchievementsResultGetAchievements(nextResultHandle, &achievements, &achievementsCount), "[Achievements] XblAchievementsResultGetAchievements"))
			{
				internal::AppendXblAchievements(*AchievementsForPlayer.Get(), achievements, achievementsCount);
				achievements = nullptr;
				WasSuccessful = AchievementsForPlayer->Num() > 0;
				bool hasNext = false;

				if (HrSuccess(XblAchievementsResultHasNext(nextResultHandle, &hasNext), "[Achievements] XblAchievementsResultHasNext"))
				{
					if (hasNext)
					{
						GetAchievementsNextPage(nextResultHandle, AchievementsForPlayer, DungeonsSubSystem, xboxUserId, Delegate);
					}
					else
					{
						AchievementsInterface->OnAchievementsQueried(WasSuccessful, xboxUserId, AchievementsForPlayer, Delegate);
					}
				}
				else
				{
					AchievementsInterface->OnAchievementsQueried(WasSuccessful, xboxUserId, AchievementsForPlayer, Delegate);
				}
			}
			else
			{
				AchievementsInterface->OnAchievementsQueried(false, xboxUserId, AchievementsForPlayer, Delegate);
			}

			XblAchievementsResultCloseHandle(nextResultHandle);
		}
		else
		{
			AchievementsInterface->OnAchievementsQueried(false, xboxUserId, AchievementsForPlayer, Delegate);
		}
	}
	, DungeonsSubsystem->GetQueueHandle()));
	HrLog(hr, "[Achievement] XblAchievementsResultGetNextAsync");
}

void FOnlineAchievementsDungeons::OnAchievementsQueried(bool Success, uint64_t XUID, TSharedPtr<TArray<FDungeonsAchievement>> AchievementsForPlayer, const FOnQueryAchievementsCompleteDelegate& Delegate)
{
	FOnlineSubsystemDungeons* DungeonsSubSystem = static_cast<FOnlineSubsystemDungeons*>(GetSubsystem());
	FOnlineIdentityDungeons* IdentityInterface = DungeonsSubSystem->GetDungeonsIdentityInterface();
	auto user = IdentityInterface->GetUserFromXuid(XUID);
	if (user)
	{
		TSharedPtr<const FUniqueNetId> PlayerId = user->UniqueNetId;

		if (Success)
		{
			UE_LOG_ONLINE(Log, TEXT("[Achievements] Achievements queried successfully"));
			if (AchievementsForPlayer->Num() > 0)
			{
				AddAchievementsForPlayer(*PlayerId.Get(), AchievementsForPlayer);
			}
			else
			{
				Success = false;
			}
		}
		else
		{
			UE_LOG_ONLINE(Log, TEXT("[Achievements] Achievements query failed"));
		}
		Delegate.ExecuteIfBound(*PlayerId.Get(), Success);
	}
}

void FOnlineAchievementsDungeons::QueryAchievementDescriptions(const FUniqueNetId& PlayerId, const FOnQueryAchievementsCompleteDelegate& Delegate /*= FOnQueryAchievementsCompleteDelegate()*/)
{
	UE_LOG_ONLINE(Warning, TEXT("[Achievements] This is not necessary as with QueryAchievements we have all the achievements information saved on 'CachedAchievements'"));
}

EOnlineCachedResult::Type FOnlineAchievementsDungeons::GetCachedAchievement(const FUniqueNetId& PlayerId, const FString& AchievementId, FOnlineAchievement& OutAchievement)
{
	if (CachedAchievements.Num() <= 0)
	{
		return EOnlineCachedResult::NotFound;;
	}

	TArray<FDungeonsAchievement>* achievements = CachedAchievements.Find(PlayerId.AsShared());

	if (achievements && achievements->Num() > 0)
	{
		FDungeonsAchievement* requestedAchievement = achievements->FindByPredicate([AchievementId](const FDungeonsAchievement& achievement) { return AchievementId.Equals(achievement.id); });
		if (requestedAchievement && FCString::Atoi(*requestedAchievement->id) != -1)
		{
			DungeonsAchievementToOnlineAchievement(*requestedAchievement, OutAchievement);
			return EOnlineCachedResult::Success;
		}
	}

	return EOnlineCachedResult::NotFound;
}

EOnlineCachedResult::Type FOnlineAchievementsDungeons::GetCachedAchievements(const FUniqueNetId& PlayerId, TArray<FOnlineAchievement>& OutAchievements)
{
	if (CachedAchievements.Num() <= 0)
	{
		return EOnlineCachedResult::NotFound;
	}

	TArray<FDungeonsAchievement>* achievementsPtr = CachedAchievements.Find(PlayerId.AsShared());
	if (achievementsPtr && achievementsPtr->Num() > 0)
	{
		TArray<FDungeonsAchievement>& achievements = *achievementsPtr;
		for (FDungeonsAchievement& dungeonsAchievement : achievements)
		{
			FOnlineAchievement OnlineAchievement;
			DungeonsAchievementToOnlineAchievement(dungeonsAchievement, OnlineAchievement);
			OutAchievements.Add(OnlineAchievement);
		}
	}
	return OutAchievements.Num() > 0 ? EOnlineCachedResult::Success : EOnlineCachedResult::NotFound;
}

void FOnlineAchievementsDungeons::DungeonsAchievementToOnlineAchievement(FDungeonsAchievement& DungeonsAchievement, FOnlineAchievement& OnlineAchievement)
{
	OnlineAchievement.Id = DungeonsAchievement.id;
	if (DungeonsAchievement.progressState == XblAchievementProgressState::Achieved)
	{
		OnlineAchievement.Progress = 100;
	}
	else if (DungeonsAchievement.progressState == XblAchievementProgressState::NotStarted)
	{
		OnlineAchievement.Progress = 0;
	}
	else
	{
		OnlineAchievement.Progress = FCString::Atof(*DungeonsAchievement.progression.requirements[0].currentProgressValue);
	}
}

EOnlineCachedResult::Type FOnlineAchievementsDungeons::GetCachedAchievementDescription(const FString& AchievementId, FOnlineAchievementDesc& OutAchievementDesc)
{
	return EOnlineCachedResult::NotFound;
}
#if !UE_BUILD_SHIPPING
bool FOnlineAchievementsDungeons::ResetAchievements(const FUniqueNetId& PlayerId)
{
	return false;
}
#endif
int32 FOnlineAchievementsDungeons::GetTitleId()
{
	uint32_t titleId = 0;
	HrLog(XalGetTitleId(&titleId), "[Achievements] Get Title ID");
	return titleId;
}

void FOnlineAchievementsDungeons::AddAchievementsForPlayer(const FUniqueNetId& PlayerId, TSharedPtr<TArray<FDungeonsAchievement>> achievements)
{
	if (TArray<FDungeonsAchievement>* playerAchievements = CachedAchievements.Find(PlayerId.AsShared()))
	{
		playerAchievements->Empty();
		playerAchievements->Append(*achievements.Get());
	}
	else
	{
		CachedAchievements.Emplace(PlayerId.AsShared(), *achievements.Get());
	}
}

void FOnlineAchievementsDungeons::RemoveAchievementsForPlayer(const FUniqueNetId& PlayerId)
{
	if (CachedAchievements.Contains(PlayerId.AsShared()))
	{
		CachedAchievements.Remove(PlayerId.AsShared());
	}
}

bool FOnlineAchievementsDungeons::LoadAchievementsFromJson()
{
	bool bResult = false;
	FString BaseDir = FPaths::ProjectDir() + TEXT("Config/OSS/");
	BaseDir += UGameplayStatics::GetPlatformName();

	const FString JsonConfigFilename = BaseDir + "/Achievements.json";
	FString JsonText;

	if (FFileHelper::LoadFileToString(JsonText, *JsonConfigFilename))
	{
		if (AchievementsMap.FromJson(JsonText))
		{
			bResult = true;
		}
		else
		{
			UE_LOG_ONLINE(Warning, TEXT("FOnlineAchievementsDungeons: Failed to parse json filename=[%s] jsontext=[%s]"), *JsonConfigFilename, *JsonText);
		}
	}
	else
	{
		UE_LOG_ONLINE(Warning, TEXT("FOnlineAchievementsDungeons: Failed to load config filename=[%s]"), *JsonConfigFilename);
	}

	return bResult;
}