#pragma once


UENUM()
enum EMessageType
{
	OneButton,
	TwoButton
};

UENUM(BlueprintType)
enum class EGlobalMessageTypes : uint8
{
	Unset,
	ClientKicked,
	InviteTimedOut,
	ConnectionTimeout,
	NoInternetConnection,
	NotConnectedToOnlineService,
	DisconnectedFromHost,
	FailedToCreate,
	CrossPlayDisabled,
	PS4CrossplayDisabled,
	PS4CrossplayEnabled,
	JoinPermissionError,
};
