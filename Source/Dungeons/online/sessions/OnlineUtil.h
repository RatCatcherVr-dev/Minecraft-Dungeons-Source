#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "online/OnlineCommon.h"
#include "online/crossplay/Session.h"
#include "online/crossplay/ExternalUI.h"
#include "OnlineUtil.generated.h"

class UDungeonsGameInstance;
class APlayerControllerBase;
class UGameInstance;

UENUM()
enum class SessionType : uint8 {
	CROSSPLAY,
	PS4,
	UNKNOWN
};

UCLASS(BlueprintType)
class UOnlineUtil : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Online|Session")
	static FName GetSessionType();

	UFUNCTION(BlueprintPure, Category = "Online|Session")
	static SessionType GetSessionEnumType();
	
	UFUNCTION(BlueprintPure, Category = "Online|Session")
	static bool IsOnlineSession();
};

namespace online {
	namespace Crossplay {
		class OSS;
		class Friends;
		class Identity;
	}

	namespace liveops {
		class LiveOps;
		class LeaderboardsHandler;
		class ChallengesHandler;
		class SeasonsHandler;
		class Rewards;
		class ProgressHandler;
		class AdventurePointsHandler;
		class RewardsHandler;
	}

	struct GameVersion
	{
		GameVersion(int major, int minor) : mMajor(major), mMinor(minor) {}
		int mMajor;
		int mMinor;
		bool operator==(const GameVersion& rhs) const { return mMajor == rhs.mMajor && mMinor == rhs.mMinor; }
		bool operator!=(const GameVersion& rhs) const { return !operator==(rhs); }
		FString Get() const;
	};

	Crossplay::OSS* getCrossplayOss();
	TSharedPtr<Crossplay::Session> getSessionInterface();
	TSharedPtr<Crossplay::Identity> getIdentityInterface();
	TSharedPtr<Crossplay::Friends> getFriendsInterface();
	TSharedPtr<Crossplay::ExternalUI> getExternalUIInterface();

	liveops::LiveOps* getLiveOps(UGameInstance*);
	void createLiveOps(UGameInstance*);
	void removeLiveOps(UGameInstance*, int32 localUserNum);
	void removeLiveOps(UDungeonsGameInstance*);
	liveops::LeaderboardsHandler* getLeaderboardsInterface(UGameInstance*);
	liveops::ChallengesHandler* getChallengesHandlerInterface(UGameInstance*);
	liveops::SeasonsHandler* getSeasonsHandlerInterface(UGameInstance*);
	liveops::ProgressHandler* getProgressHandlerInterface(UGameInstance*);
	liveops::AdventurePointsHandler* getAdventurePointsHandlerInterface(UGameInstance*);
	liveops::RewardsHandler* getRewardsHandlerInterface(UGameInstance*);
	std::string getSelectedSeason(UGameInstance*);
	
	FNamedOnlineSession* getCurrentSession();
	TOptional<FString> getCurrentSessionId();
	FString getLocalUserName(const UWorld*);
	void updateOnlineSession(const UWorld*, bool forceUpdate = false);
	bool usingNullSubsystem(const UWorld*);
	bool usingDungeonsSubsystem(const UWorld*);
	bool matchOnlineServiceName(const UWorld*, const FString&);
	FText getDefaultOnlineServiceName();
	bool isOnlineSession();
	bool isLoggedInOnline(const UWorld*);
	TOptional<GameVersion> getVersion(const FString& versionString);
	bool isRunningMyGameVersion(const FOnlineSessionSearchResult& res);
	bool shouldShowSession(const FOnlineSessionSearchResult& res, const UWorld* world);
	bool shouldShowSecondaryName(FString secondaryName);
	bool shouldShowInvite(const FOnlineSessionSearchResult& res, const UWorld* world);
	bool IsCrossplayEnabled();
	bool SetXblActive(bool newValue, APlayerControllerBase* PlayerControllerBase);
	const FString EmptyPresence = "None";
	
	bool IsUsingOnlineFeatures();
	void SetDynamicNamespace(FString dynamicNamespace);
	bool IsUsingOnlineFeaturesInPIE();
	FString GetMinecraftAPINamespace();
	bool IsUsingCachedEntitlements();
}

namespace sessionSettings {
	static const FName SETTING_PLATFORM("PLATFORM");
	static const FName SETTING_CROSSPLAY("CROSSPLAY");
	static const char* SETTING_PSNNAME("PSNName");
	static const char* SETTING_XBLGAMERTAG("XBLGamerTag");

	enum class PlatformType
	{
		WINDOWS_PLATFORM,
		XBOXONE_PLATFORM,
		SWITCH_PLATFORM,
		PS4_PLATFORM,
		UNKNOWN
	};

	struct FPlayerNames
	{
		FString XBLGamerTag;
		FString PSNName;
	};

	PlatformType GetPlatform();
	std::string ConvertPlatformToString(PlatformType Platform);
	PlatformType ConvertToPlatform(const FString& Platform);
	bool IsSamePlatform(PlatformType Platform);
	bool CanPlatformRespondToOutOfTitleInvites(PlatformType Platform);
	TArray<FPlayerNames> GetPlayerNames();
	FString GetPlayerNamesAsString();
	FString GetInviteSenderName(const TArray<FPlayerNames>& PlayerNames, const FString& PlayerName);
}
