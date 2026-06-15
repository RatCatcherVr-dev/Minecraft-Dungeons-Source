

#include "UserFeedbackTextLabels.h"
#include "platform/GameVersion.h"
#include "StringTableRegistry.h"


/// Popup messages
/// Loctext for popups for online lib purposes, cross-play and more.
///
namespace textlabels { 
static const FName platformLabels = "/Game/Decor/Text/PlatformSpecificTerminology.PlatformSpecificTerminology";
static const FString XBL = "XBL";
static const FString yourMSA = "MSA_your";
static const FString MSA = "MSA";
static const FString PSN = "PSN";
static const FString PSN_accountFor = "PSN_accountFor";
static const FString NSO = "NSO";
static const FString NSO_membership = "NSO_membership";

static const FString controller = "XBOX_controllerName";
static const FString DUALSHOCK = "PS4_controllerCapitalized";
static const FString PS4_wireless = "PS4_controllerName_short";

}

#define LOCTEXT_NAMESPACE "PopupMessageLabels"
FDialogData UUserFeedbackTextLabels::GetControllerDisconnectedMessage() {
	FFormatNamedArguments Args;
	if (UGameVersion::GetPlatformEnum() == EPlatformType::E_PLATFORM_PS4) {
		Args.Add(TEXT("controller"), FText::FromStringTable(textlabels::platformLabels, textlabels::DUALSHOCK));
		Args.Add(TEXT("controller_short"), FText::FromStringTable(textlabels::platformLabels, textlabels::PS4_wireless));
	} else {
		Args.Add(TEXT("controller"), FText::FromStringTable(textlabels::platformLabels, textlabels::controller));
		Args.Add(TEXT("controller_short"), FText::FromStringTable(textlabels::platformLabels, textlabels::controller));
	}

	return { FText::Format(LOCTEXT("popup_controllerDisconnect_title", "{controller_short} Disconnected"), Args)
		, FText::Format(LOCTEXT("popup_controllerDisconnect_template", "Reconnect the {controller} to continue"), Args)
	};
}


#undef LOCTEXT_NAMESPACE

