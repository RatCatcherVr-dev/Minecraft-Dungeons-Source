#include "Dungeons.h"
#include "DungeonsFriendUtil.h"

EFriendOnlineStatus UDungeonsFriendUtil::GetFriendOnlineStatus(const FBlueprintFriend& Friend) {

	if (Friend.IsOnline) {
		return EFriendOnlineStatus::Online;
	}
	else {
		return EFriendOnlineStatus::Offline;
	}
}
