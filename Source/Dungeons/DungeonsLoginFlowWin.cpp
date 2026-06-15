#include "DungeonsLoginFlowWin.h"
#include "DungeonsGameInstance.h"
#include "game/actor/character/player/DungeonsLocalPlayer.h"
#include "online/sessions/OnlineUtil.h"
#include "online/crossplay/CrossplayOSS.h"
#include "online/crossplay/ExternalUI.h"


UDungeonsLoginFlowWin::UDungeonsLoginFlowWin(const FObjectInitializer& ObjectInitializer)
	: UDungeonsLoginFlow(ObjectInitializer)
{
#if !WITH_EDITOR
	online::getIdentityInterface()->AutoLogin();
#endif
}

void UDungeonsLoginFlowWin::PlatformLogin(ULocalPlayer* LocalPlayer)
{
	auto externalUI = online::getExternalUIInterface();
	if (externalUI) {
		if (!externalUI->OnLoginFlowUIRequiredDelegates.IsBoundToObject(this)) {
			externalUI->OnLoginFlowUIRequiredDelegates.AddUObject(this, &UDungeonsLoginFlowWin::OnRequireLoginUI);
		}
	}

	UDungeonsLoginFlow::PlatformLogin(LocalPlayer);
}

void UDungeonsLoginFlowWin::LocalPlayerLogin(int ControllerId, APlayerController* PlayerController)
{
	LoggingInPlayerController = PlayerController;
	SetPlayerControllerId(ControllerId);
	if (UDungeonsLocalPlayer* DungeonsLocalPlayer = Cast<UDungeonsLocalPlayer>(PlayerController->GetLocalPlayer()))
	{
		DungeonsLocalPlayer->SetUserSystemId(ControllerId);
	}

	LoadSaveData(ControllerId);
}

//D11.PS could this just be bound via the BP?
void UDungeonsLoginFlowWin::OnRequireLoginUI(const FString& RequestedURL, const FOnLoginRedirectURL& LoginRedirect, const FOnLoginFlowComplete& OnLoginFlowComplete, bool& bOutShouldContinueLogin)
{
	// delegate from Dungeons -> UI to open a URL
	if (OnRequestLogin.IsBound()) {
		OnRequestLogin.Broadcast(RequestedURL);
	}

	OnLoginRedirect = &LoginRedirect;
}

void UDungeonsLoginFlowWin::UpdateURL(const FString& NewURL) {
	if (OnLoginRedirect && OnLoginRedirect->IsBound()) {
		OnLoginRedirect->Execute(NewURL); // if returns empty result, keep loading?
	}
}



