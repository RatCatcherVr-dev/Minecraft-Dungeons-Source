#pragma once

#include "CoreMinimal.h"
#include "BlockingMessage.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnRetry);
DECLARE_MULTICAST_DELEGATE(FOnDismissed);

UENUM(BlueprintType)
enum class EBlockingMessageType : uint8 {
	Unset,
	SourceAuthenticationFailed,
	MissingBaseGameEntitlement,
	EntitlementsRequestFailed
};
ENUM_NAME(EBlockingMessageType);

UCLASS(BlueprintType)
class DUNGEONS_API UBlockingMessage final : public UObject {
	
	GENERATED_BODY()

public:	
	UFUNCTION(BlueprintCallable)
	EBlockingMessageType GetType() const;

	void SetType(EBlockingMessageType);

	UFUNCTION(BlueprintCallable)
	void Retry() const;

	UFUNCTION(BlueprintCallable)
	void Dismiss() const;

	FOnRetry OnRetry;
	FOnDismissed OnDismissed;
private:	
	EBlockingMessageType Type = EBlockingMessageType::Unset;
};
