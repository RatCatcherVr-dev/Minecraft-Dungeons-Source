#pragma once
#include "SubOSS.h"

namespace online {
namespace Crossplay {

class Friends : public SubOSS {
public:
	Friends(const SubsystemRepo&);

	bool ReadFriendsList(int32 LocalUserNum, const FString& ListName, const FOnReadFriendsListComplete&);
	bool GetFriendsList(int32 LocalUserNum, const FString& ListName, TArray< TSharedRef<FOnlineFriend> >& OutFriends);
	FDelegateHandle AddOnFriendsChangeDelegate_Handle(int, const FOnFriendsChangeDelegate&);
	void ClearOnFriendsChangeDelegate_Handle(int, FDelegateHandle&);
	bool SendInvite(int32 LocalUserNum, const FUniqueNetId & FriendId, const FString & ListName, const FOnSendInviteComplete& Delegate = FOnSendInviteComplete());
	bool DeleteFriend(int32 LocalUserNum, const FUniqueNetId& FriendId, const FString& ListName);
	TSharedPtr<FOnlineFriend> GetFriend(int32 LocalUserNum, const FUniqueNetId& FriendId, const FString& ListName); // @AA returns nullptr in Dungeons OSS

private:
	TArray<IOnlineSubsystem*> GetSubsystemsWithFriendsInterface() const;
};
}
}
