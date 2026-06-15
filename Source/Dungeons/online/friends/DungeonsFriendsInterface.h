#pragma once

#include "CoreMinimal.h"
#include "OnlineFriendsInterface.h"
#include "DungeonsFriendsCommon.h"
#include <DelegateCombinations.h>
#include "DungeonsFriendsInterface.generated.h"

UENUM(BlueprintType)
enum class EFriendsAddResponse : uint8 {
	Unset,
	MissingParameter,
	Searching,
	NoMatch,
	FriendAdded,
	FriendAlreadyAdded,
	GeneralError
};
ENUM_NAME(EFriendsAddResponse);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFriendsUpdateCompleted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFriendsUpdateFailed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FAddFriendCompleted, EFriendsAddResponse, Response, const FString&, ListName, const FString&, ErrorString);

UCLASS()
class UDungeonsFriendsInterface : public UObject {
	GENERATED_BODY()
public:
	UDungeonsFriendsInterface(const FObjectInitializer& ObjectInitializer);

	int32 GetInitialControllerID() const;

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "Dungeons|Online|Friends")
	FBlueprintPlayerInfoList GetAllFriends();

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "Dungeons|Online|Friends")
	FBlueprintPlayerInfoList GetOnlineFriends();

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "Dungeons|Online|Friends")
	FBlueprintPlayerInfoList GetFriendsInDungeons();

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "Dungeons|Online|Friends")
	bool JoinSessionOfFriend(const FBlueprintFriend& player);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "Dungeons|Online|Friends")
	bool ShouldDisplayFriend(const FBlueprintFriend& player);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "Dungeons|Online|Friends")
	EFriendInvitableStatus GetFriendInvitableStatus(const FBlueprintFriend& player);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "Dungeons|Online|Friends")
	bool IsSessionOpenForInvites();

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Online|Friends")
	bool IsPlayingMultiplayer();

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "Dungeons|Online|Friends")
	bool UpdateFriendsList();

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "Dungeons|Online|Friends")
	void SetPresenceStatus(const FString& levelName);

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Online|Friends")
	EFriendsAddResponse AddPlayerAsFriend(const FBlueprintFriend& player);

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Online|Friends")
	EFriendsAddResponse AddFriendByListName(const FString& listName);

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Online|Friends")
	bool SupportsAddFriend() const;

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "Dungeons|Online|Friends")
	void RemoveFriend(const FBlueprintFriend& player);

	UPROPERTY(BlueprintAssignable, Category = "Dungeons|Online|Friends")
	FFriendsUpdateCompleted FriendsUpdateCompleted;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons|Online|Friends")
	FFriendsUpdateFailed FriendsUpdateFailed;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons|Online|Friends")
	FAddFriendCompleted AddFriendCompleted;


	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "Dungeons|Online")
	bool ShowSessionPlayerProfile(int localPlayerIndex, const ABasePlayerState* targetPlayer);

	// #D11.CM
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "Dungeons|Online")
	bool ShowPlayerProfile(int localPlayerIndex, const FBlueprintFriend& targetPlayer);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "Dungeons|Online")
	bool CanDisplayProfileOfName(const FString& DisplayName);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "Dungeons|Online")
	bool ShowProfileOfName(const FString& DisplayName);

	// #D11.CM
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "Dungeons|Online")
	bool ShowLocalPlayerProfile(int localPlayerIndex, int targetPlayerIndex);

	// #D11.CM
	TSharedPtr<const FUniqueNetId> GetNetIDFromPlayerIndex(int localPlayerIndex);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "Dungeons|Online")
	bool DoesPlayerHaveValidNetID(int localPlayerIndex) 
	{ 
		return GetNetIDFromPlayerIndex(localPlayerIndex).IsValid(); 
	};

	// #D11.CM
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "Dungeons|Online|Friends")
	bool IsValidFriend(FBlueprintFriend player);
	
	bool ConfigureDelegates();
	bool DeleteDelegates();
	UFUNCTION(BlueprintCallable, Category = "Dungeons|Online|Friends")
	bool SetPrivateGame(bool PrivateGame);
	UFUNCTION(BlueprintCallable, Category = "Dungeons|Online|Friends")
	bool IsPrivateGame();

private:
	/** Delegates */
	FOnPresenceReceivedDelegate OnPresenceReceivedDelegate;
	FOnFriendsChangeDelegate OnFriendsChangedDelegate;
	FOnReadFriendsListComplete OnReadFriendsListComplete;
	FOnFindFriendSessionCompleteDelegate OnFindFriendSessionCompleteDelegate;
	FOnSendInviteComplete OnFriendAddedComplete;

	/** Delegate handles */
	FDelegateHandle OnPresenceReceivedDelegateHandle;
	FDelegateHandle OnFriendsChangedDelegateHandle;
	FDelegateHandle OnFindFriendSessionCompleteDelegateHandle;

	void OnFindSessionCompleted(int32 LocalUserNum, bool bWasSuccessful, const TArray<FOnlineSessionSearchResult>& result);
	void OnReadFriendsCompleted(int32 LocalUserNum, bool bWasSuccessful, const FString& ListName, const FString& ErrorString);
	void OnAddFriendCompleted(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& FriendId, const FString& ListName, const FString& ErrorString);
	void OnFriendsChanged();
	void OnPresenceReceived(const FUniqueNetId& UserId, const TSharedRef<FOnlineUserPresence>& PresenceData);

	int32 GetFriendIndex(const FUniqueNetId& UserId);
	TSharedPtr<const FUniqueNetId> GetLocalPlayerId();

	FString PresenceStatusString;

	FBlueprintPlayerInfoList AllFriends;

	TArray<FUniqueNetIdWrapper> FriendSessionRequests;
	void AddRequest(const FUniqueNetId& UserId);
	void NextRequest();
	bool PrivateGame = false;
};