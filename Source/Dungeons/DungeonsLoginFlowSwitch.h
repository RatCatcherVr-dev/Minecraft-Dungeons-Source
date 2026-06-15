#pragma once
#include "DungeonsLoginFlowCrossPlay.h"
#include "DungeonsLoginFlowSwitch.generated.h"

UCLASS()
class UDungeonsLoginFlowSwitch : public UDungeonsLoginFlowCrossPlay
{
	GENERATED_BODY()
public:
	UDungeonsLoginFlowSwitch(const FObjectInitializer& ObjectInitializer);
protected:
	virtual void OnLoginCompletedCrossPlay(TSharedPtr<const FUniqueNetId> UniqueId, int LocalPlayerIndex, const FOnlineError& LoginResult) override;
	virtual void ShowMsaLinkUI(const FString& url, const FString& code) override;
	virtual bool IsLocalAccount(APlayerController*) override;
};
