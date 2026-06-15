#pragma once

#include "UObject/ObjectMacros.h"
#include "Dungeons/online/OnlineCommon.h"
#include "game/levels.h"
#include "game/difficulty/Difficulty.h"
#include "game/difficulty/ThreatLevel.h"
#include "game/LevelSettings.h"
#include "DungeonsFriendsCommon.generated.h"

UENUM(BlueprintType)
enum class EFriendsFailureReason : uint8 {
	OnlineSystemNotAvailable,
	InvalidPlayerController,
	FriendsSubSystemNotAvailable,
	InvalidUniqueNetworkId,
	Unknown
};

UENUM(BlueprintType)
enum class EFriendOnlineStatus : uint8 {
	InGameSession,
	Online,
	Offline,
};

UENUM(BlueprintType)
enum class EFriendOnlineService : uint8 {
	PSN,
	XBL
};

UENUM(BlueprintType)
enum class EFriendInvitableStatus : uint8 {
	Invitable,
	BlockedDifferentPlatform,
	BlockedInactive,
	BlockedOnewayFriend,
	BlockedSessionClosed,
	BlockedInvalidPlayerId,
	BlockedUnknownReason,
	BlockedCrossplayEnabled,
	BlockedCrossplayDisabled
};
ENUM_NAME(EFriendInvitableStatus);

USTRUCT(BlueprintType)
struct FBlueprintFriend {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString DisplayName;
	UPROPERTY(BlueprintReadOnly)
	FString RealName;
	UPROPERTY(BlueprintReadOnly)
	bool IsOnline;
	UPROPERTY(BlueprintReadOnly)
	bool IsPlayingThisGame;
	UPROPERTY(BlueprintReadOnly)
	bool IsInJoinableSession;	
	UPROPERTY(BlueprintReadOnly)
	FString InGameStatus;
	UPROPERTY(BlueprintReadOnly)
	bool isPlayingOnPs4;

	EFriendInvitableStatus InvitableStatus;
	UPROPERTY(BlueprintReadOnly)
	EFriendOnlineService OnlineService;
	FOnlineSessionSearchResult SessionSearchResult;
	FUniqueNetIdWrapper UniqueNetId;

	bool operator <(const FBlueprintFriend& rhs) const {
		if (OnlineService != rhs.OnlineService) {
			return OnlineService < rhs.OnlineService;
		}
		return DisplayName < rhs.DisplayName;
	}
};

USTRUCT(BlueprintType)
struct FBlueprintPlayerInfoList {
	GENERATED_BODY()

	FBlueprintPlayerInfoList() {}
	FBlueprintPlayerInfoList(TArray<FBlueprintFriend> players) : Players(players) {}

	UPROPERTY(BlueprintReadOnly)
	TArray<FBlueprintFriend> Players;
};

USTRUCT(BlueprintType)
struct FBlueprintGameSession {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FLevelSettings LevelSettings;
	
	UPROPERTY(BlueprintReadOnly) 
	bool IsPublic;

	FUniqueNetIdWrapper HostUniqueNetId;
	FOnlineSessionSearchResult SearchResult;

	UPROPERTY(BlueprintReadOnly)
	float ReceivedStartTime;

	UPROPERTY(BlueprintReadOnly)
	FString HostDisplayName;

	UPROPERTY(BlueprintReadOnly)
	FString InviteSenderName;

	UPROPERTY(BlueprintReadOnly)
	FBlueprintPlayerInfoList PlayerInfoList;
};
