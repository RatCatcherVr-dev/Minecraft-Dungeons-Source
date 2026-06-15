#pragma once
#include "DungeonsLoginFlow.h"
#include "DungeonsLoginFlowCrossPlay.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMsaLinkRequired, const FString&, url, const FString&, code);


UCLASS(BlueprintType)
class UDungeonsLoginFlowCrossPlay : public UDungeonsLoginFlow
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintAssignable, Category = "Dungeons|LoginFlow")
	FOnMsaLinkRequired OnMsaLinkRequired;

	UFUNCTION(BlueprintCallable, Category = "Dungeons|LoginFlow")
	void CrossPlayLogin(APlayerController* PlayerController);

	UFUNCTION(BlueprintCallable, Category = "Dungeons|LoginFlow")
	bool HasLinkingSucceeded(ELoginResult result, int localPlayerIndex);

	virtual void LocalPlayerLogin(int ControllerId, APlayerController* PlayerController) override;

	UFUNCTION(BlueprintCallable, Category = "Dungeons|LoginFlow")
	void CancelMsaLoginUI();

protected:
	UDungeonsLoginFlowCrossPlay(const FObjectInitializer& ObjectInitializer);
	void PlatformLogin(ULocalPlayer* LocalPlayer) override;

	UFUNCTION()
	void OnLoadSaveDataComplete();
	virtual void ShowMsaLinkUI(const FString& url, const FString& code) {}
	virtual void OnLoginCompletedCrossPlay(TSharedPtr<const FUniqueNetId> UniqueId, int LocalPlayerIndex, const FOnlineError& LoginResult) {}
	virtual bool IsLocalAccount(APlayerController*) { return false; }
	ELoginResult SetLoginUserCrossplay(TSharedPtr<const FUniqueNetId> UniqueId, int LocalPlayerNum);

	void SetupUniqueNetID();	

	FDelegateHandle MsaLinkRequiredHandle;
};
