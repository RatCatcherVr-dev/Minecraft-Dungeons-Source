#pragma once

#include "CoreMinimal.h"
#include "BlockingMessage.h"
#include "BlockingMessageBoard.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBlockingMessagePushed, UBlockingMessage*, BlockingMessage);

DECLARE_MULTICAST_DELEGATE(FOnRequestEntitlements)
DECLARE_MULTICAST_DELEGATE(FOnRequestRetryAuthentication)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnRequestLogout, int)

UCLASS(BlueprintType)
class DUNGEONS_API UBlockingMessageBoard final : public UObject {
	
	GENERATED_BODY()

public:
	void PushMessage(EBlockingMessageType);
		
	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnBlockingMessagePushed OnBlockingMessagePushed;

	FOnRequestRetryAuthentication OnRequestRetryAuthentication;
	FOnRequestLogout OnRequestLogout;
private:
	void InvokeRetry(EBlockingMessageType);

	void DismissCurrentMessage();
	
	UPROPERTY()
	UBlockingMessage* CurrentMessage;
	
	FDelegateHandle RetryBlockingMessageHandle;
	FDelegateHandle DismissedBlockingMessageHandle;	
};
