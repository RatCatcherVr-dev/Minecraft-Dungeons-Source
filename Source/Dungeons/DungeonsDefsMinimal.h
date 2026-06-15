#pragma once

#include "common_header.h"
#include "DungeonsDefsMinimal.generated.h"

UENUM(BlueprintType)
enum class ELoadingScreenType : uint8 {
	Menu,
	Level,
	Lobby,
	Count
};
ENUM_NAME(ELoadingScreenType);

UENUM(BlueprintType)
enum class EMapLoadType : uint8 {
	Unset,
	StartLobbySession,
	StartIngameSession,
	OpenMenu,
	OpenIngame,
	OpenLobby, //D11.PS
	TravelIngameServer,
	TravelIngameClient,
	TravelLobbyServer,
	TravelLobbyClient,
	JoinIngameSession,
	JoinLobbySession
};
ENUM_NAME(EMapLoadType);

//D11.KS
UENUM(BlueprintType)
enum class StatusEnum : uint8
{
	Success,
	Fail
};

UENUM(BlueprintType)
enum SwitchOperationMode
{
	Handheld,
	Console
};

UENUM(BlueprintType)
enum class ELowSpecNotifyType : uint8
{
	OnBatteryPower,
	LowMemory,
	LowPerformance
};
ENUM_NAME(ELowSpecNotifyType);

UENUM(BlueprintType)
enum class ENightModeType : uint8 {
	Normal,
	Oceans,
};
ENUM_NAME(ENightModeType);
