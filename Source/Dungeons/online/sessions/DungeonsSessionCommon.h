#pragma once

#include "Dungeons/online/OnlineCommon.h"

UENUM(BlueprintType)
enum class ESessionFailureReason : uint8 {
	CreateSessionFailed,
	StartSessionFailed,
	FindSessionsFailed,
	JoinSessionFailed,
	ConnectionStringResolutionFailed,
	OnlineSystemNotAvailable,
	InvalidPlayerController,
	MapNameNotFound,
	InvalidUniqueNetworkId,
	SessionInterfaceUnavailable,
	SessionNotFound,
	InvalidFriendNetId,
	LoginFailed, //D11.PS
	Unknown,
};
