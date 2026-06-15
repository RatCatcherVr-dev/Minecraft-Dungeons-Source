#include "DungeonsLoginFlowSwitch.h"
#include "Themida/Anticheat.hpp"
#include "DungeonsGameInstance.h"
#include "online/sessions/OnlineUtil.h"
#include "online/crossplay/CrossplayOSS.h"

UDungeonsLoginFlowSwitch::UDungeonsLoginFlowSwitch(const FObjectInitializer& ObjectInitializer) : UDungeonsLoginFlowCrossPlay(ObjectInitializer)
{}

const TMap<FString, ELoginResult> ErrorToLoginStatusSwitch = {
	{LoginFlowConstants::SignInCancelled, ELoginResult::UserCanceledLogin},
	{LoginFlowConstants::SpopIssue, ELoginResult::SpopIssue}
};

/* Maps error codes to login statuses */
static ELoginResult ToLoginResultSwitch(const FOnlineError& error)
{
	if (!error.bSucceeded)
		return ErrorToLoginStatusSwitch.Contains(error.GetErrorCode()) ? ErrorToLoginStatusSwitch[error.GetErrorCode()] : ELoginResult::FailedMsaLogin;
	return ELoginResult::Success;
}

void UDungeonsLoginFlowSwitch::ShowMsaLinkUI(const FString& url, const FString& code)
{
	if (OnMsaLinkRequired.IsBound()) {
		/* a. First boot shows the linking screen and player accepts and initiates the linking
			or
		   b. Player opens Configure Online Play -> Activate Cross-Play/Online play and starts linking
		   In both cases the UMG_Account_Link screen is already in the viewport.
		   */
		OnMsaLinkRequired.Broadcast(url, code);
	}
	else /* Player will receive the Account Linking prompt only on initial boot on Nintendo Switch */
		CancelMsaLoginUI();
}

void UDungeonsLoginFlowSwitch::OnLoginCompletedCrossPlay(TSharedPtr<const FUniqueNetId> UniqueId, int LocalPlayerNum, const FOnlineError& error)
{
	ANTICHEAT_VIRT_PROTECT_STRINGS_BEGIN
#ifdef	HAS_MSA_LINKING_DELEGATES
	FCoreDelegates::OnMsaLinkingRequired.Remove(MsaLinkRequiredHandle);
#endif
	ELoginResult LoginUserResult = ToLoginResultSwitch(error);
	if (error.bSucceeded)
		LoginUserResult = SetLoginUserCrossplay(UniqueId, LocalPlayerNum);

	online::getCrossplayOss()->SetXblActive(LoginUserResult == ELoginResult::Success);

	BroadcastLoginComplete(LoginUserResult);
	ANTICHEAT_VIRT_PROTECT_STRINGS_END
}

bool UDungeonsLoginFlowSwitch::IsLocalAccount(APlayerController* player) {
	if (!player)
		return false;

	int32 controllerId = player->GetLocalPlayer()->GetControllerId();
	// Dungeons Identity is not conforming to the Identity Interface - it never returns UsingLocalProfile:
	// have to always skip login for local players with existing save:
	return !GetGameInstance()->IsXblActiveInSaveFile(controllerId);
}
