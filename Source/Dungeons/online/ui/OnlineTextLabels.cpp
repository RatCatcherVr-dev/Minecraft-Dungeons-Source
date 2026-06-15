

#include "OnlineTextLabels.h"
#include "DungeonsLoginFlow.h"
#include "Public/Interfaces/OnlineIdentityInterface.h"
#include "online/sessions/OnlineUtil.h"
#include "platform/GameVersion.h"
#include "StringTableRegistry.h"

#define LOCTEXT_NAMESPACE "Privileges"

FText UOnlineTextLabels::GetPrivilegeError(int32 PrivilegeError)
{
	uint32 PrivilegeErrorInt = static_cast<uint32>(PrivilegeError);
	if ((PrivilegeErrorInt & static_cast<uint32>(IOnlineIdentity::EPrivilegeResults::RequiredSystemUpdate)) != 0)
	{
		return LOCTEXT("RequiredSystemUpdate", "A required system update is available.  Please upgrade to access online features.");
	}
	else if ((PrivilegeErrorInt & static_cast<uint32>(IOnlineIdentity::EPrivilegeResults::RequiredPatchAvailable)) != 0)
	{
		return LOCTEXT("RequiredPatchAvailable", "A required game patch is available.  Please upgrade to access online features.");
	}
	else if ((PrivilegeErrorInt& static_cast<uint32>(IOnlineIdentity::EPrivilegeResults::AgeRestrictionFailure)) != 0)
	{
		return LOCTEXT("AgeRestrictionFailure", "Cannot play due to age restrictions!");
	}
	else if ((PrivilegeErrorInt & static_cast<uint32>(IOnlineIdentity::EPrivilegeResults::UserNotFound)) != 0)
	{
		return LOCTEXT("UserNotFound", "Cannot play due to invalid user!");
	}
	else if ((PrivilegeErrorInt & static_cast<uint32>(IOnlineIdentity::EPrivilegeResults::GenericFailure)) != 0)
	{
		return LOCTEXT("GenericFailure", "Cannot play online. Check your network connection.");
	}
	else if ((PrivilegeErrorInt & static_cast<uint32>(IOnlineIdentity::EPrivilegeResults::NetworkConnectionUnavailable)) != 0)
	{
		return LOCTEXT("NetworkConnectionUnavailable", "Check your network connection.");
	}
	else if ((PrivilegeErrorInt & static_cast<uint32>(IOnlineIdentity::EPrivilegeResults::UserNotLoggedIn)) != 0)
	{
		return LOCTEXT("UserNotLoggedIn", "You are not logged in. Please log in to access online features.");
	}
	else if ((PrivilegeErrorInt & static_cast<uint32>(IOnlineIdentity::EPrivilegeResults::UGCRestriction)) != 0)
	{
		return LOCTEXT("UGCRestriction", "Cannot play due to being restricted by the User Generated Content.");
	}
	else if ((PrivilegeErrorInt & static_cast<uint32>(IOnlineIdentity::EPrivilegeResults::OnlinePlayRestricted)) != 0)
	{
		return LOCTEXT("OnlinePlayRestricted", "Cannot play due to being Online Play restricted.");
	}
	else if ((PrivilegeErrorInt & static_cast<uint32>(IOnlineIdentity::EPrivilegeResults::ChatRestriction)) != 0)
	{
		return LOCTEXT("ChatRestriction", "Cannot play due to being chat restricted.");
	}
	return FText();
}

#undef LOCTEXT_NAMESPACE

/// Popup messages
/// Loctext for popups for online lib purposes, cross-play and more.
///
namespace online { namespace ui {

static const FName platformLabels = "/Game/Decor/Text/PlatformSpecificTerminology.PlatformSpecificTerminology";
static const FString XBL = "XBL";
static const FString yourMSA = "MSA_your";
static const FString MSA = "MSA";
static const FString PSN = "PSN";
static const FString PSN_accountFor = "PSN_accountFor";
static const FString NSO = "NSO";
static const FString NSO_membership = "NSO_membership";

}}

#define LOCTEXT_NAMESPACE "PopupMessageLabels"
FText UOnlineTextLabels::GetSignInError(ELoginResult result) {
	switch(result) 
	{
	case ELoginResult::UserAlreadyInGame:
	case ELoginResult::InternalError:
		return LOCTEXT("Popup_Internal_Error", "An internal error occurred. Some online features might not be available.");
				
	case ELoginResult::LinkingError:
	case ELoginResult::FailedMsaLogin:
	{	
		FFormatNamedArguments Arguments;
		Arguments.Add(TEXT("serviceprovider"), FText::FromStringTable(online::ui::platformLabels, online::ui::yourMSA));
		return FText::Format(LOCTEXT("Popup_ConnectError", "Sorry, we are currently unable to sign you in to {serviceprovider}. Please make sure that you have a working internet connection and try again."), Arguments);
	}
	case ELoginResult::NetworkIssue:
		return LOCTEXT("Popup_CheckInternet", "Check your Internet connection and try again.");

	case ELoginResult::LinkingGuestAccountError:
		return LOCTEXT("Popup_GuestAccount", "For cross-platform play, you cannot use a guest account.");

	case ELoginResult::ThisPsnAlreadyLinkedToDifferentMSA:
	{	
		FFormatNamedArguments Arguments;
		Arguments.Add(TEXT("PSNUserId"), FText::FromString(online::getExternalUIInterface()->GetLinkErrorGamertagHint()));
		Arguments.Add(TEXT("PSN"), FText::FromStringTable(online::ui::platformLabels, online::ui::PSN));
		return FText::Format(LOCTEXT("Popup_LinkErrorMessage", "This account for {PSN} has already been linked to a different Microsoft Account \n{PSNUserId}"), Arguments);
	}
	case ELoginResult::ThisMsaAlreadyLinkedToDifferentPSN:
	{	
		FFormatNamedArguments Arguments;
		Arguments.Add(TEXT("PSN"), FText::FromStringTable(online::ui::platformLabels, online::ui::PSN));
		return FText::Format(LOCTEXT("Popup_already_linked_PSN", "This Microsoft Account has already been linked to a different account for {PSN}."), Arguments);
	}
	case ELoginResult::PlayfabLoginFailedCrossplayUnavailable:
		return LOCTEXT("Popup_PlayfabLoginFailed", "Unfortunately, cross-platform play sessions are currently unavailable. Please try again later.\n\nOnline and Offline games can be played as normal.");
	case ELoginResult::SpopIssue:
		return LOCTEXT("Popup_LoggedInOnAnotherDevice", "You are signed in to this game on another device.\n\nTo use this Microsoft Account, sign out on the other device first.");
	case ELoginResult::UserCanceledLogin:
	case ELoginResult::Success:
		default:
			return FText::FromString("");

	}
};

FText UOnlineTextLabels::GetLinkAccountError(ELoginResult result) {
	if (result == ELoginResult::PlayfabLoginFailedCrossplayUnavailable)
		return GetSignInError(result);
	
	FFormatNamedArguments Arguments;
	if (UGameVersion::GetPlatformEnum() == EPlatformType::E_PLATFORM_SWITCH) {
		Arguments.Add(TEXT("PSN"), FText::FromStringTable(online::ui::platformLabels, online::ui::NSO));
		Arguments.Add(TEXT("retry-hint"), FText::FromString(""));
	}
	else {
		Arguments.Add(TEXT("PSN"), FText::FromStringTable(online::ui::platformLabels, online::ui::PSN));
		Arguments.Add(TEXT("retry-hint"), LOCTEXT("Retry_Signin_Hint", "You can attempt to sign in again at any time from the Settings > Game > Cross-Platform Play"));
	}
	Arguments.Add(TEXT("ErrorText"), GetSignInError(result));
	return FText::Format(LOCTEXT("PopupLinkErrorMessage", "We could not link your account for {PSN} to a Microsoft Account at this time.\n\n{ErrorText}\n\n{retry-hint}"), Arguments);
}

static FText accountFor(EPlatformType platform) {
	switch (platform) {
	case EPlatformType::E_PLATFORM_PS4:
		return FText::FromStringTable(online::ui::platformLabels, online::ui::PSN_accountFor);
	case EPlatformType::E_PLATFORM_SWITCH:
		return FText::FromStringTable(online::ui::platformLabels, online::ui::NSO_membership);
	default:
		break;
	}
	return FText::FromStringTable(online::ui::platformLabels, online::ui::MSA);
}

FText UOnlineTextLabels::GetAuthenticationErrorTitle(EBlockingMessageType blockingMessageType) {
	switch (blockingMessageType)
	{
	case EBlockingMessageType::SourceAuthenticationFailed:
	case EBlockingMessageType::EntitlementsRequestFailed:
		return LOCTEXT("Title_EntitlementVerifificationFailed", "Unable to verify game ownership");
	case EBlockingMessageType::MissingBaseGameEntitlement:
		return LOCTEXT("Title_MissingBaseGameEntitlement", "Game ownership not established");
	}

	return FText::FromString("");
}

FText UOnlineTextLabels::GetAuthenticationErrorMessage(EBlockingMessageType blockingMessageType) {
	FFormatNamedArguments Args;
	Args.Add(TEXT("account"), accountFor(UGameVersion::GetPlatformEnum()));

	switch (blockingMessageType)
	{
	case EBlockingMessageType::SourceAuthenticationFailed:
	case EBlockingMessageType::EntitlementsRequestFailed:
		return LOCTEXT("Message_EntitlementVerifificationFailed", "Sorry, we're currently unable to verify that you own Minecraft Dungeons. Please make sure that you have a working Internet connection and try again.");
	case EBlockingMessageType::MissingBaseGameEntitlement:
		return FText::Format(LOCTEXT("Message_MissingBaseGameEntitlement", "Sorry, you don't appear to own Minecraft Dungeons with this {account}. Make sure that you are correctly signed in and restart the game."), Args);
	}

	return FText::FromString("");
}


#undef LOCTEXT_NAMESPACE

