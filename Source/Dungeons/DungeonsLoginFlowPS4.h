#pragma once
#include "DungeonsLoginFlowCrossPlay.h"
#include "DungeonsLoginFlowPS4.generated.h"

UCLASS()
class UDungeonsLoginFlowPS4 : public UDungeonsLoginFlowCrossPlay
{
	GENERATED_BODY()
public:
	UDungeonsLoginFlowPS4(const FObjectInitializer& ObjectInitializer);
	bool IsLinkingShown() const;
protected: 
	virtual void OnLoginCompletedCrossPlay(TSharedPtr<const FUniqueNetId> UniqueId, int LocalPlayerIndex, const FOnlineError& LoginResult) override;
	virtual void ShowMsaLinkUI(const FString& url, const FString& code) override;
	virtual bool IsLocalAccount(APlayerController*) override;
private:
	auto CreateLogoutAndDeactivateDungeonsLambda(TSharedPtr<const FUniqueNetId>);
	void SetPostLoginAction(TSharedPtr<const FUniqueNetId>, ELoginResult);
	ELoginResult SetLoginUserPS4(TSharedPtr<const FUniqueNetId> UniqueId, int LocalPlayerNum, const FOnlineError& error);
	bool isLinkingUIShown = false;
};
