#pragma once

#include "CoreMinimal.h"
#include "DungeonsSessionCommon.h"
#include "Dungeons/online/friends/DungeonsFriendsCommon.h"
#include "Net/OnlineBlueprintCallProxyBase.h"
#include "InviteDungeonsSessionCallbackProxy.generated.h"

class APlayerController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInviteSessionFailed, ESessionFailureReason, Reason);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInviteSessionSucceeded);

UCLASS()
class DUNGEONS_API UInviteDungeonsSessionCallbackProxy : public UOnlineBlueprintCallProxyBase {
	GENERATED_UCLASS_BODY()
public:

	UPROPERTY(BlueprintAssignable)
	FInviteSessionSucceeded InviteSessionSucceeded;

	UPROPERTY(BlueprintAssignable)
	FInviteSessionFailed InviteSessionFailed;

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Dungeons|Online")
	static UInviteDungeonsSessionCallbackProxy* InviteDungeonsFriend(const UObject* WorldContextObject, ABasePlayerController* playerController, const FBlueprintFriend& Friend);

	virtual void Activate() override;

private:
	void ClearKicked(FUniqueNetIdWrapper friendId);

	const UObject* WorldContextObject;
	TWeakObjectPtr<class ABasePlayerController> PlayerControllerWeakPtr;
	FUniqueNetIdWrapper FriendUniqueId;
};
