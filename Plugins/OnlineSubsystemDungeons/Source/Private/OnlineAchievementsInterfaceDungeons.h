#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystemDungeons.h"
#include "OnlineSubsystemDungeonsTypes.h"
#include "OnlineJsonSerializer.h"
#include "OnlineAchievementsInterface.h"
#include "OnlineStats.h"
#include "OnlineIdentityDungeons.h"
#include "OnlineAsyncTaskManager.h"
#include "utils.h"

/// Represents the association between a title and achievements.
struct FDungeonsTitleAssociation {
	/// The UTF-8 encoded localized name of the title.
	FString name;
	/// The title ID.
	uint32_t titleId;
};

/// Represents requirements for unlocking the achievement.
struct FDungeonsAchievementRequirement {
	/// The UTF-8 encoded achievement requirement ID.
	FString id;
	/// A UTF-8 encoded value that indicates the current progress of the player towards meeting 
	/// the requirement.
	FString currentProgressValue;
	/// The UTF-8 encoded target progress value that the player must reach in order to meet 
	/// the requirement.
	FString targetProgressValue;
};

/// Represents progress details about the achievement, including requirements.
struct FDungeonsAchievementProgression {
	/// The actions and conditions that are required to unlock the achievement.
	TArray<FDungeonsAchievementRequirement> requirements;
	/// The timestamp when the achievement was first unlocked.
	time_t timeUnlocked;
};

/// Represents a media asset for an achievement.
struct FDungeonsAchievementMediaAsset {
	/// The UTF-8 encoded name of the media asset, such as "tile01".
	FString name;
	/// The type of media asset.
	XblAchievementMediaAssetType mediaAssetType;
	/// The UTF-8 encoded URL of the media asset.
	FString url;
};

/// Represents a reward that is associated with the achievement.
struct FDungeonsAchievementReward {
	/// The UTF-8 encoded localized reward name.
	FString name;
	/// The UTF-8 encoded description of the reward.
	FString description;
	/// The UTF-8 encoded title-defined reward value (data type and content varies by reward type).
	FString value;
	/// The reward type.
	XblAchievementRewardType rewardType;
	/// The UTF-8 encoded property type of the reward value string.
	FString valueType;
	/// The media asset associated with the reward. 
	/// If the reward type is gamerscore, this will be nullptr. 
	/// If the reward type is in_app, this will be a media asset. 
	/// If the reward type is art, this may be a media asset or nullptr.
	FDungeonsAchievementMediaAsset* mediaAsset;
};

/// Represents an achievement, a system-wide mechanism for directing and 
/// rewarding users' in-game actions consistently across all games.
struct FDungeonsAchievement {
	/// The UTF-8 encoded achievement ID. Can be a uint or a guid.
	FString id;
	/// The UTF-8 encoded ID of the service configuration set associated with the achievement.
	FString serviceConfigurationId;
	/// The UTF-8 encoded localized achievement name.
	FString name;
	/// The game/app titles associated with the achievement.
	TArray<FDungeonsTitleAssociation> titleAssociations;
	/// The state of a user's progress towards the earning of the achievement.
	XblAchievementProgressState progressState;
	/// The progression object containing progress details about the achievement, 
	/// including requirements.
	FDungeonsAchievementProgression progression;
	/// The media assets associated with the achievement, such as image IDs.
	TArray<FDungeonsAchievementMediaAsset> mediaAssets;
	/// The UTF-8 encoded collection of platforms that the achievement is available on.
	TArray<FString> platformsAvailableOn;
	/// Whether or not the achievement is secret.
	bool isSecret;
	/// The UTF-8 encoded description of the unlocked achievement.
	FString unlockedDescription;
	/// The UTF-8 encoded description of the locked achievement.
	FString lockedDescription;
	/// The UTF-8 encoded product_id the achievement was released with. This is a globally unique identifier that 
	/// may correspond to an application, downloadable content, etc.
	FString productId;
	/// The type of achievement, such as a challenge achievement.
	XblAchievementType type;
	/// The participation type for the achievement, such as group or individual.
	XblAchievementParticipationType participationType;
	/// The time window during which the achievement is available. Applies to Challenges.
	XblAchievementTimeWindow available;
	/// The collection of rewards that the player earns when the achievement is unlocked.
	TArray<FDungeonsAchievementReward> rewards;
	/// The estimated time that the achievement takes to be earned.
	uint64_t estimatedUnlockTime;
	/// A UTF-8 encoded deep link for clients that enables the title to launch at a desired 
	/// starting point for the achievement.
	FString deepLink;
	/// A value that indicates whether or not the achievement is revoked by enforcement.
	bool isRevoked;
};

class FAchievementsConfig : public FOnlineJsonSerializable
{
public:
	FJsonSerializableKeyValueMapInt		AchievementMap;

	BEGIN_ONLINE_JSON_SERIALIZER

		ONLINE_JSON_SERIALIZE_MAP("AchievementMap", AchievementMap);

	END_ONLINE_JSON_SERIALIZER
};

class FOnlineAchievementsDungeons : public IOnlineAchievements
{

public:
	FOnlineAchievementsDungeons(FOnlineSubsystemDungeons* InDungeonsSubsystem);

	virtual ~FOnlineAchievementsDungeons() = default;

private:
	FOnlineSubsystemDungeons* DungeonsSubsystem;

	/** Cached achievement descriptions for an Id */
	TMap<FString, FOnlineAchievementDesc> AchievementDescriptions;

	/** Our game Title's ID */
	int32 TitleId;

	FAchievementsConfig AchievementsMap;

public:
	//~ Begin IOnlineAchievements Interface
	virtual void WriteAchievements(const FUniqueNetId& PlayerId, FOnlineAchievementsWriteRef& WriteObject, const FOnAchievementsWrittenDelegate& Delegate = FOnAchievementsWrittenDelegate()) override;
	virtual void QueryAchievements(const FUniqueNetId& PlayerId, const FOnQueryAchievementsCompleteDelegate& Delegate = FOnQueryAchievementsCompleteDelegate()) override;
	virtual void QueryAchievementDescriptions(const FUniqueNetId& PlayerId, const FOnQueryAchievementsCompleteDelegate& Delegate = FOnQueryAchievementsCompleteDelegate()) override;
	virtual EOnlineCachedResult::Type GetCachedAchievement(const FUniqueNetId& PlayerId, const FString& AchievementId, FOnlineAchievement& OutAchievement) override;
	virtual EOnlineCachedResult::Type GetCachedAchievements(const FUniqueNetId& PlayerId, TArray<FOnlineAchievement>& OutAchievements) override;
	virtual EOnlineCachedResult::Type GetCachedAchievementDescription(const FString& AchievementId, FOnlineAchievementDesc& OutAchievementDesc) override;

#if !UE_BUILD_SHIPPING
	virtual bool ResetAchievements(const FUniqueNetId& PlayerId) override;
#endif // !UE_BUILD_SHIPPING
	//~ End IOnlineAchievements Interface

	FOnlineSubsystemDungeons* GetSubsystem();

	void RemoveAchievementsForPlayer(const FUniqueNetId& PlayerId);

	void GetAchievementsNextPage(XblAchievementsResultHandle previousResultHandle, TSharedPtr<TArray<FDungeonsAchievement>> AchievementsForPlayer, FOnlineSubsystemDungeons* DungeonsSubSystem, uint64_t xboxUserId, const FOnQueryAchievementsCompleteDelegate& Delegate);

private:
	TUniqueNetIdMap<TArray<FDungeonsAchievement>> CachedAchievements;

	bool IsAllSetUp(const FUniqueNetId &PlayerId) const;
	void UpdateAchievement(const FUniqueNetId& PlayerId, FString AchievementIndex, float Percent, const FOnAchievementsWrittenDelegate& Delegate = FOnAchievementsWrittenDelegate());

	bool LoadAchievementsFromJson();
	FString GetAchievementIndex(FString achievementName) const;
	FString GetAchievementImageURL(const FUniqueNetId& PlayerId, int AchievementIndex) const;
	FOnlineAchievementDesc GetAchievementDescription(const FUniqueNetId& PlayerId, int AchievementIndex);
	void DungeonsAchievementToOnlineAchievement(FDungeonsAchievement& DungeonsAchievement, FOnlineAchievement& OnlineAchievement);
	void AddAchievementsForPlayer(const FUniqueNetId& PlayerId, TSharedPtr<TArray<FDungeonsAchievement>> achievements);

	void OnAchievementWritten(bool Success, uint64_t XUID, const FString& AchievementId, float Percentage, const FOnAchievementsWrittenDelegate& Delegate = FOnAchievementsWrittenDelegate());
	void OnAchievementsQueried(bool Success, uint64_t XUID, TSharedPtr<TArray<FDungeonsAchievement>> AchievementsForPlayer, const FOnQueryAchievementsCompleteDelegate& Delegate = FOnQueryAchievementsCompleteDelegate());

	int32 GetTitleId();
};

typedef TSharedPtr<FOnlineAchievementsDungeons, ESPMode::ThreadSafe> FOnlineAchievementsDungeonsPtr;