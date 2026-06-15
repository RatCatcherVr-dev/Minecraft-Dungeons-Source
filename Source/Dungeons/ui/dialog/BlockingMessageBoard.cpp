#include "Dungeons.h"
#include "BlockingMessageBoard.h"
#include "util/EnumUtil.h"

void UBlockingMessageBoard::PushMessage(const EBlockingMessageType messageType) {
	if (!CurrentMessage) {
		CurrentMessage = NewObject<UBlockingMessage>();
		CurrentMessage->SetType(messageType);
		DismissedBlockingMessageHandle = CurrentMessage->OnDismissed.AddUObject(this, &UBlockingMessageBoard::DismissCurrentMessage);
		RetryBlockingMessageHandle = CurrentMessage->OnRetry.AddUObject(this, &UBlockingMessageBoard::InvokeRetry, messageType);
#if UE_BUILD_SHIPPING		
		OnBlockingMessagePushed.Broadcast(CurrentMessage);
#endif		
	}
}

void UBlockingMessageBoard::InvokeRetry(const EBlockingMessageType messageType) {
	constexpr int32 LocalUserNum = 0;

	DismissCurrentMessage();

	switch (messageType) {	
	case EBlockingMessageType::SourceAuthenticationFailed:
		OnRequestRetryAuthentication.Broadcast();
		break;
	case EBlockingMessageType::MissingBaseGameEntitlement:
	case EBlockingMessageType::EntitlementsRequestFailed:
		OnRequestLogout.Broadcast(LocalUserNum);
		break;
	case EBlockingMessageType::Unset:
	default:
		UE_LOG(LogDungeons, Log, TEXT("Attempting to retry on unmapped EBlockingMessageType %s"), *GetEnumValueToStringStripped(messageType));
	}
	
}

void UBlockingMessageBoard::DismissCurrentMessage() {
	if (CurrentMessage) {
		CurrentMessage->OnRetry.Remove(RetryBlockingMessageHandle);
		CurrentMessage->OnDismissed.Remove(DismissedBlockingMessageHandle);
		CurrentMessage = nullptr;	
	}	
}
