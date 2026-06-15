#include "DungeonsLoginFlowPS4.h"
#include "Themida/Anticheat.hpp"
#include "online/sessions/OnlineUtil.h"
#include "online/crossplay/CrossplayOSS.h"
#include "PlayfabServices/Public/PlayfabServices.h"

const TMap<FString, ELoginResult> ErrorToLoginResultPS4 = {
	{LoginFlowConstants::NetworkIssueError, ELoginResult::NetworkIssue},
	{LoginFlowConstants::FailedMSALoginError, ELoginResult::FailedMsaLogin},
	{LoginFlowConstants::FailedPlayfabLoginError, ELoginResult::PlayfabLoginFailedCrossplayUnavailable},
	{LoginFlowConstants::GenericLinkingError, ELoginResult::LinkingError},
	{LoginFlowConstants::MSALinkedToDifferentPSNError, ELoginResult::ThisMsaAlreadyLinkedToDifferentPSN},
	{LoginFlowConstants::PSNLinkedToDifferentMSAError, ELoginResult::ThisPsnAlreadyLinkedToDifferentMSA},
	{LoginFlowConstants::GuestAccountError, ELoginResult::LinkingGuestAccountError},
	{LoginFlowConstants::AlreadyLoggedInUserError, ELoginResult::UserAlreadyInGame},
	{LoginFlowConstants::SignInCancelled, ELoginResult::UserCanceledLogin},
	{LoginFlowConstants::SpopIssue, ELoginResult::SpopIssue}
};

/* Maps error codes to login statuses */
static ELoginResult ToLoginResultPS4(const FOnlineError& error)
{
	if (!error.bSucceeded)
		return ErrorToLoginResultPS4.Contains(error.GetErrorCode()) ? ErrorToLoginResultPS4[error.GetErrorCode()] : ELoginResult::InternalError;
	return ELoginResult::Success;
}

UDungeonsLoginFlowPS4::UDungeonsLoginFlowPS4(const FObjectInitializer& ObjectInitializer) : UDungeonsLoginFlowCrossPlay(ObjectInitializer)
{}

void UDungeonsLoginFlowPS4::ShowMsaLinkUI(const FString& url, const FString& code)
{
	if (OnMsaLinkRequired.IsBound()) {
		/* a. First boot shows the linking screen and player accepts and initiates the linking
			or
		   b. Player opens Configure Online Play -> Activate Cross-Play/Online play and starts linking
		   In both cases the UMG_Account_Link screen is already in the viewport.
		   */
		OnMsaLinkRequired.Broadcast(url, code);
	}
	else {
		ensureMsgf(OnShowAccountLinkScreen.IsBound(), TEXT("OnShowAccountLinkScreen delegate should be bound in the UMG_TitleScreen"));
		/* XSAPI requested account linking during the normal boot flow.
		  This might happen when the same account is used in MineCraft Bedrock and Dungeons at the same time or if the player has just patched to the xplay version.
		  UMG_TitleScreen is in the viewport.
		  To link: create the UMG_Account_Link screen and add it to the viewport, then pass the url and code directly */
		OnShowAccountLinkScreen.Broadcast(url, code);
	}
	isLinkingUIShown = true;
}

auto UDungeonsLoginFlowPS4::CreateLogoutAndDeactivateDungeonsLambda(TSharedPtr<const FUniqueNetId> UniqueId)
{
	return [this, UniqueId]() {
		LogoutOfOss(UniqueId);
		online::getCrossplayOss()->SetXblActive(false);
		online::getCrossplayOss()->DeactivateDungeonsOSS();
	};
}

auto CreateDeactivateDungeonsLambda()
{
	return []() {
		online::getCrossplayOss()->SetXblActive(false);
		online::getCrossplayOss()->DeactivateDungeonsOSS();
	};
}

/* Action to call right after all login/UI actions are complete,
 ex. the error message popup is closed by the player */
void UDungeonsLoginFlowPS4::SetPostLoginAction(TSharedPtr<const FUniqueNetId> UniqueId, ELoginResult loginResult)
{
	switch (loginResult)
	{
	case ELoginResult::Success:
		return;
	case ELoginResult::UserCanceledLogin:
		PostLoginAction.BindLambda(CreateDeactivateDungeonsLambda());
		break;
	case ELoginResult::InternalError:
	case ELoginResult::UserAlreadyInGame:
		PostLoginAction.BindLambda(
			[this, UniqueId]() {
				LogoutOfOss(online::getIdentityInterface()->GetUniqueNetIdFromType(*UniqueId, SubsystemType::PS4)); 
			}
		);
		break;
	case ELoginResult::NetworkIssue:
		PostLoginAction.BindLambda(CreateLogoutAndDeactivateDungeonsLambda(UniqueId));
		break;
	case ELoginResult::LinkingError:
	case ELoginResult::ThisMsaAlreadyLinkedToDifferentPSN:
	case ELoginResult::ThisPsnAlreadyLinkedToDifferentMSA:
	case ELoginResult::SpopIssue:
	default: /* other error */
		PostLoginAction.BindLambda(CreateLogoutAndDeactivateDungeonsLambda(
			online::getIdentityInterface()->GetUniqueNetIdFromType(*UniqueId, SubsystemType::Dungeons)
		));
		break;
	}
}

ELoginResult UDungeonsLoginFlowPS4::SetLoginUserPS4(TSharedPtr<const FUniqueNetId> UniqueId, int LocalPlayerNum, const FOnlineError& error)
{	
	ELoginResult loginResult = ToLoginResultPS4(error);
	bool noLoginErrors = loginResult == ELoginResult::Success || loginResult == ELoginResult::UserCanceledLogin;
	if (SetLoginUserCrossplay(UniqueId, LocalPlayerNum) != ELoginResult::Success && noLoginErrors)
		return ELoginResult::InternalError;
	return loginResult;
}

void UDungeonsLoginFlowPS4::OnLoginCompletedCrossPlay(TSharedPtr<const FUniqueNetId> UniqueId, int LocalPlayerNum, const FOnlineError& error)
{	
#ifdef	HAS_MSA_LINKING_DELEGATES
	FCoreDelegates::OnMsaLinkingRequired.Remove(MsaLinkRequiredHandle);
#endif
	ELoginResult loginResult = SetLoginUserPS4(UniqueId, LocalPlayerNum, error);
	SetPostLoginAction(UniqueId, loginResult);
	BroadcastLoginComplete(loginResult);
}

bool UDungeonsLoginFlowPS4::IsLinkingShown() const {
	return isLinkingUIShown;
}

bool UDungeonsLoginFlowPS4::IsLocalAccount(APlayerController* player) {
	if (!player)
		return false;

	int32 controllerId = player->GetLocalPlayer()->GetControllerId();
	return online::getIdentityInterface()->GetLoginStatus(controllerId) == ELoginStatus::UsingLocalProfile;
}
