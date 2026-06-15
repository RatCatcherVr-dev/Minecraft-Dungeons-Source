#pragma once

#include "CoreMinimal.h"
#include "DungeonsSessionCommon.h"
#include "Dungeons/online/friends/DungeonsFriendsCommon.h"
#include "Net/OnlineBlueprintCallProxyBase.h"
#include "OpenChatDungeonsSessionCallbackProxy.generated.h"

class APlayerController;

UCLASS()
class DUNGEONS_API UOpenChatDungeonsSessionCallbackProxy : public UOnlineBlueprintCallProxyBase {
	GENERATED_UCLASS_BODY()
public:

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Dungeons|Online")
	static UOpenChatDungeonsSessionCallbackProxy* OpenChatDungeonsFriend(const UObject* WorldContextObject, APlayerController* playerController, const FBlueprintFriend& Friend);

	virtual void Activate() override;

private:

	const UObject* WorldContextObject;
	TWeakObjectPtr<APlayerController> PlayerControllerWeakPtr;
	FUniqueNetIdWrapper FriendUniqueId;
};
