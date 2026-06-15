#include "Dungeons.h"
#include "OpenChatDungeonsSessionCallbackProxy.h"
#include <Online.h>
#include "OnlineUtil.h"
#include "online/crossplay/CrossplayOSS.h"
#include "online/crossplay/ExternalUI.h"

UOpenChatDungeonsSessionCallbackProxy::UOpenChatDungeonsSessionCallbackProxy(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {

}

UOpenChatDungeonsSessionCallbackProxy* UOpenChatDungeonsSessionCallbackProxy::OpenChatDungeonsFriend(const UObject* WorldContextObject, APlayerController* playerController, const FBlueprintFriend& Friend) {
	UOpenChatDungeonsSessionCallbackProxy* node = NewObject<UOpenChatDungeonsSessionCallbackProxy>();
	node->WorldContextObject = WorldContextObject;
	node->PlayerControllerWeakPtr = playerController;	
	node->FriendUniqueId.SetUniqueNetId(Friend.UniqueNetId.GetUniqueNetId());

	TSharedPtr<const FUniqueNetId> pid = Friend.UniqueNetId.GetUniqueNetId();

	auto ExternalUI = online::getExternalUIInterface();
	check(ExternalUI.IsValid());

	if (pid->IsValid()) {
		bool bShowingUI = ExternalUI->ShowProfileUI(
			*pid.Get(),
			*pid.Get());

		UE_LOG(LogOnline, Log, TEXT("TestProfileUI bShowingUI: %d"), bShowingUI);
	}
	return node;
}

void UOpenChatDungeonsSessionCallbackProxy::Activate() {

}
