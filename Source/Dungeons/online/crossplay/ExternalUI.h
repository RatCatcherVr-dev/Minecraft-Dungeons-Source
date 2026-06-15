#pragma once
#include "SubOSS.h"
#include <functional>
#include "PlayfabServices.h"

namespace online {
namespace Crossplay {




class ExternalUI : public SubOSS {
public:
	ExternalUI(const SubsystemRepo&);
	bool ShowLoginUI(int ControllerIndex, bool bShowOnlineOnly, bool bShowSkipButton, const FOnLoginUIClosedDelegate& Delegate);
	bool ShowAccountUpgradeUI(const FUniqueNetId& UniqueId);
	bool ShowProfileUI(const FUniqueNetId& Requestor, const FUniqueNetId& Requestee, const FOnProfileUIClosedDelegate& Delegate = FOnProfileUIClosedDelegate());
	bool ShowStoreUI(int32 LocalUserNum, const FShowStoreParams& ShowParams, const FOnShowStoreUIClosedDelegate& Delegate = FOnShowStoreUIClosedDelegate());
	void ShowStoreUI(UWorld* World, int32 LocalUserNum, const FString category, FString productID);

	FOnLoginFlowUIRequired OnLoginFlowUIRequiredDelegates;

	FString GetLinkErrorGamertagHint() const;

private:
	FDelegateHandle OnAuthTokenUpdateHandle;
	TArray<IOnlineSubsystem*> GetSubsystemsWithExternalUI() const;
	void DoAccountLinking(TSharedPtr<const FUniqueNetId> UniqueId, const int ControllerIndex, const FOnLoginUIClosedDelegate& Delegate, const IOnlineSubsystem* subsystem, AccountLinkStatus currentStatus);
	bool CrossplayLoginFlow(const int ControllerIndex, bool bShowOnlineOnly, bool bShowSkipButton, const FOnLoginUIClosedDelegate& Delegate);
	void TriggerXblLoginError(TSharedPtr<const FUniqueNetId> UniqueId, const int Controllerindex, const FOnLoginUIClosedDelegate& Delegate, FOnlineError Error);

	auto CreatePSNLoginLambda(const FOnLoginUIClosedDelegate& Delegate, const IOnlineSubsystem* subsystem, bool bShowOnlineOnly, bool bShowSkipButton);
	auto DoPlayfabLogin(const FOnLoginUIClosedDelegate& Delegate, const IOnlineSubsystem* subsystem, bool bShowOnlineOnly, bool bShowSkipButton, TSharedPtr<const FUniqueNetId> UniquePS4Id, const int ControllerIndex, const FOnlineError& Error);
	auto CreatePlayfabLoginLambda(const FOnLoginUIClosedDelegate& Delegate, TSharedPtr<const FUniqueNetId> UniqueId, const IOnlineSubsystem* subsystem, const int ControllerIndex, bool bShowOnlineOnly, bool bShowSkipButton);
	auto CreateXblLoginLambda(const FOnLoginUIClosedDelegate& Delegate, const IOnlineSubsystem* subsystem, TSharedPtr<const FUniqueNetId> UniquePS4Id);
	auto LinkAccounts(const FOnLoginUIClosedDelegate& Delegate, const int ControllerIndex, TSharedPtr<const FUniqueNetId> UniqueId, const IOnlineSubsystem* subsystem);
	void SetGamertagHint(const int ControllerIndex);
	auto CreateAccountLinkingLambda(const FOnLoginUIClosedDelegate& Delegate, const int ControllerIndex, TSharedPtr<const FUniqueNetId> UniqueId, const IOnlineSubsystem* subsystem);

	std::function<void()> TokenRecievedLambda;
	FString LinkErrorGamertagHint;
	FTimerHandle TimeOutHandle;
	bool TimedOut = false;
};
}
}
