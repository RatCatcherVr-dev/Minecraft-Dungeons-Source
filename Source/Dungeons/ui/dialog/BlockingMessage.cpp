#include "Dungeons.h"
#include "BlockingMessage.h"

EBlockingMessageType UBlockingMessage::GetType() const {
	return Type;
}

void UBlockingMessage::SetType(const EBlockingMessageType type) {
	Type = type;
}

void UBlockingMessage::Retry() const {
	OnRetry.Broadcast();
}

void UBlockingMessage::Dismiss() const {
	OnDismissed.Broadcast();
}
