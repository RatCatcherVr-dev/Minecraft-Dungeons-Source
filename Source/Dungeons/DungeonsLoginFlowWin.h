#pragma once
#include "DungeonsLoginFlow.h"
#include "DungeonsLoginFlowWin.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRequestLoginWin, const FString&, Url);

UCLASS(BlueprintType)
class UDungeonsLoginFlowWin : public UDungeonsLoginFlow
{
	GENERATED_BODY()
public:
	UDungeonsLoginFlowWin(const FObjectInitializer& ObjectInitializer);
	virtual void LocalPlayerLogin(int ControllerId, APlayerController* PlayerController) override;
private:
	virtual void PlatformLogin(ULocalPlayer* LocalPlayer) override;

	UFUNCTION(BlueprintCallable, Category = "Dungeons|LoginFlow")
	void UpdateURL(const FString& NewURL);

	void OnRequireLoginUI(const FString& RequestedURL, const FOnLoginRedirectURL& OnLoginRedirect, const FOnLoginFlowComplete& OnLoginFlowComplete, bool& bOutShouldContinueLogin);

	UPROPERTY(BlueprintAssignable, Category = "Dungeons|LoginFlow")
	FRequestLoginWin OnRequestLogin;

	const FOnLoginRedirectURL* OnLoginRedirect = nullptr;
};



