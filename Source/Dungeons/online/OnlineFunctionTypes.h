// © 2020 Mojang Synergies AB. TM Microsoft Corporation.
#pragma once

#include "CoreMinimal.h"
#include "common_header.h"

UENUM(BlueprintType)
enum class ELoginResult : uint8
{
	Success,
	InternalError,
	UserAlreadyInGame,
	UserCanceledLogin,
	FailedMsaLogin,
	NetworkIssue,
	ThisPsnAlreadyLinkedToDifferentMSA,
	LinkingError,
	LinkingGuestAccountError,
	ThisMsaAlreadyLinkedToDifferentPSN,
	PlayfabLoginFailedCrossplayUnavailable,
	SpopIssue /* player already logged in on a different device */
};
ENUM_NAME(ELoginResult);

UENUM(BlueprintType)
enum class EPopupDialogType : uint8
{
	OK,
	ConfirmCancel,

};

ENUM_NAME(EPopupDialogType);
