#pragma once

#include "game/item/ItemTypeDefs.h"

#define USE_MOJANG_SERVICES_ANALYTICS 1

UENUM(BlueprintType)
enum class ETelemetryEventType : uint8
{
	GameSessionStartup	UMETA(DisplayName = "game_session_start"),
	OnDeath				UMETA(DisplayName = "on_death"),
	OnRateMission		UMETA(DisplayName = "on_rate_mission"),
	OnCheckpoint		UMETA(DisplayName = "on_checkpoint"),
	RoundAwards			UMETA(DisplayName = "round_awards"),

	OnLevelStart		UMETA(DisplayName = "level_start"),
	OnJoinLobby			UMETA(DisplayName = "join_lobby"),
	OnLevelJoin			UMETA(DisplayName = "join_level"),
	OnChatWheel			UMETA(DisplayName = "on_chat_wheel"),
	OnTextChatUse		UMETA(DisplayName = "on_text_chat"),
	OnObjectiveDone		UMETA(DisplayName = "on_objective_done"),
	GameSessionEnd		UMETA(DisplayName = "game_session_end"),
	Logout				UMETA(DisplayName = "logout"),
	SettingsChange		UMETA(DisplayName = "settings_change"),
	OnMobKilled			UMETA(DisplayName = "mob_kill"),

	Assert				UMETA(DisplayName = "assert"),
	CrashDetected		UMETA(DisplayName = "crashed"),
	Count
};

ENUM_NAME(ETelemetryEventType);

namespace analytics
{
	FString GetTelemetryEventTypeAsString(ETelemetryEventType EnumValue, bool StripClassName = false);
}
