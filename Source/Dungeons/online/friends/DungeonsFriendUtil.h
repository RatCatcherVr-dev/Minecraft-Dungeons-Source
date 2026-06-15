#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "online/friends/DungeonsFriendsCommon.h"
#include "DungeonsFriendUtil.generated.h"

UCLASS()
class DUNGEONS_API UDungeonsFriendUtil : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintPure, Category = "Dungeons|Online|Session")
	static EFriendOnlineStatus GetFriendOnlineStatus(const FBlueprintFriend& Friend);
};
