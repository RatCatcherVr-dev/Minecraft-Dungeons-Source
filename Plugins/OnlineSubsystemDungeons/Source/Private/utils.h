#pragma once

#include "CoreMinimal.h"
#include "UnrealString.h"
#include "xsapiServicesInclude.h"
#include "P2P/WebRTCCommon.h" //@attn: Shipping did not build without this. /@jon

#include <Anticheat.hpp>

#if PLATFORM_SWITCH
#include <nn/os.h>
#endif

namespace OnlineSubsystemConstants {
	/* XBL */
	static const char* DEVELOPMENTSANDBOX = ANTICHEAT_STRINGS("XNQUNP.0");
	static const char* RETAILSANDBOX = ANTICHEAT_STRINGS("RETAIL");
	static const char* CLIENTID = ANTICHEAT_STRINGS("000000004426B87D");
	static const char* SCID = ANTICHEAT_STRINGS("00000000-0000-0000-0000-000067acc3cd");
	static const char* MULTIPLAYERSCID = ANTICHEAT_STRINGS("00000000-0000-0000-0000-000067acc3cd");
	static const uint32_t TITLEID = 1739375565;
	const uint32_t FIRSTPARTYTITLEFLAG = 1u << 31;
	const uint32_t FOCIFLAG = 1u << 24;
	const uint32_t WIN32_USE_FILE_STORAGE = 1u << 28;

	/* CoTURN */
#if UE_BUILD_SHIPPING
	static const char* TURNADDRESS = ANTICHEAT_STRINGS("turn:turn.minecraftservices.com:3478");
	static const char* STUNADDRESS = ANTICHEAT_STRINGS("stun:turn.minecraftservices.com:3478");
#else
	static const char* TURNADDRESS = ANTICHEAT_STRINGS("turn:turn-staging.minecraftservices.com:3478");
	static const char* STUNADDRESS = ANTICHEAT_STRINGS("stun:turn-staging.minecraftservices.com:3478");
#endif
	/* XMPP */
	static const char* XMPPAPPID = ANTICHEAT_STRINGS("Dungeons");
#if UE_BUILD_SHIPPING
	static const char* XMPPMSGADDRESS = ANTICHEAT_STRINGS("signaling.minecraftservices.com");
	static const char* XMPPDOMAIN = ANTICHEAT_STRINGS("signaling.minecraftservices.com");
#if PLATFORM_XBOXONE || PLATFORM_SWITCH || PLATFORM_PS4
	static const char* XMPPADDRESS = ANTICHEAT_STRINGS("wss://signaling.minecraftservices.com:5443/ws");
	static const int XMPPPORT = 0;
#else
	static const char* XMPPADDRESS = ANTICHEAT_STRINGS("signaling.minecraftservices.com");
	static const int XMPPPORT = 5222;
#endif
#else
	static const char* XMPPMSGADDRESS = ANTICHEAT_STRINGS("signaling-staging.minecraftservices.com");
	static const char* XMPPDOMAIN = ANTICHEAT_STRINGS("signaling-staging.minecraftservices.com");
#if PLATFORM_XBOXONE || PLATFORM_SWITCH || PLATFORM_PS4
	static const char* XMPPADDRESS = ANTICHEAT_STRINGS("wss://signaling-staging.minecraftservices.com:5443/ws");
	static const int XMPPPORT = 0;
#else
	static const char* XMPPADDRESS = ANTICHEAT_STRINGS("signaling-staging.minecraftservices.com");
	static const int XMPPPORT = 5222;
#endif
#endif
	
	/* Timeouts */
	static constexpr int WEBRTCISINACTIVE = 60000;
	static constexpr int WEBRTCISDEAD = 15000;
	static constexpr unsigned long long XMPPLOGINTIMEOUT = 10000;
	static constexpr unsigned long long RELAYTOKENVALIDITYTIME = 45ULL * 60ULL * 1000ULL;
	static constexpr unsigned long long TOKENREQUESTTIMEOUT = 10ULL * 1000ULL;
	static constexpr unsigned long long XMPPRETRYTIME = 5000;
	static constexpr int MAX_TIME_TO_JOIN_XBL_SESSION_SECONDS = 30;
	static constexpr int MAX_TIME_TO_CREATE_XBL_SESSION_SECONDS = 30;

	/* ---- */
	static constexpr int32 AuthtokenLocaluserNum = 0;

#if PLATFORM_SWITCH || PLATFORM_PS4
	static const string_t SETTING_XBL_MEMBERS = ANTICHEAT_STRINGS("XBLMEMBERS");
	static const string_t SETTING_PLATFORM = ANTICHEAT_STRINGS("PLATFORM");
	static const string_t SETTING_CROSSPLAY = ANTICHEAT_STRINGS("CROSSPLAY");

	static const string_t SETTING_MAP_NAME = ANTICHEAT_STRINGS("UNREAL_MAP_NAME");
	static const string_t SETTING_PLAYER_NAME = ANTICHEAT_STRINGS("PLAYER_NAME");
	static const string_t SETTING_RANDOM_SEED = ANTICHEAT_STRINGS("RANDOM_SEED");
	static const string_t SETTING_GAME_VERSION = ANTICHEAT_STRINGS("GAME_VERSION");
	static const string_t SETTING_MISSION_DIFFICULTY = ANTICHEAT_STRINGS("MISSION_DIFFICULTY");
	static const string_t SETTING_LEVEL_NAME = ANTICHEAT_STRINGS("LEVEL_NAME");
	static const string_t SETTING_LEVEL_FILENAME = ANTICHEAT_STRINGS("LEVEL_FILENAME");
	static const string_t SETTING_THREAT_LEVEL = ANTICHEAT_STRINGS("THREAT_LEVEL");
	static const string_t SETTINGS_RECONNECTABLE_GUIDS = ANTICHEAT_STRINGS("RECONNECTABLE_GUIDS");
	static const string_t SETTING_PLAYERS_NAMES = ANTICHEAT_STRINGS("PLAYERS_NAMES");
#else
	static const string_t SETTING_XBL_MEMBERS = ANTICHEAT_STRINGS(TEXT("XBLMEMBERS"));
	static const string_t SETTING_PLATFORM = ANTICHEAT_STRINGS(TEXT("PLATFORM"));
	static const string_t SETTING_CROSSPLAY = ANTICHEAT_STRINGS(TEXT("CROSSPLAY"));

	static const string_t SETTING_MAP_NAME = ANTICHEAT_STRINGS(TEXT("UNREAL_MAP_NAME"));
	static const string_t SETTING_PLAYER_NAME = ANTICHEAT_STRINGS(TEXT("PLAYER_NAME"));
	static const string_t SETTING_RANDOM_SEED = ANTICHEAT_STRINGS(TEXT("RANDOM_SEED"));
	static const string_t SETTING_GAME_VERSION = ANTICHEAT_STRINGS(TEXT("GAME_VERSION"));
	static const string_t SETTING_MISSION_DIFFICULTY = ANTICHEAT_STRINGS(TEXT("MISSION_DIFFICULTY"));
	static const string_t SETTING_LEVEL_NAME = ANTICHEAT_STRINGS(TEXT("LEVEL_NAME"));
	static const string_t SETTING_LEVEL_FILENAME = ANTICHEAT_STRINGS(TEXT("LEVEL_FILENAME"));
	static const string_t SETTING_THREAT_LEVEL = ANTICHEAT_STRINGS(TEXT("THREAT_LEVEL"));
	static const string_t SETTINGS_RECONNECTABLE_GUIDS = ANTICHEAT_STRINGS(TEXT("RECONNECTABLE_GUIDS"));
	static const string_t SETTING_PLAYERS_NAMES = ANTICHEAT_STRINGS(TEXT("PLAYERS_NAMES"));
#endif

	static const char* USER_SAVEGAME_ROOT = "save";

	static const FName DungeonsGameSessionName = ANTICHEAT_STRINGS(TEXT("GameSession"));
	static const std::string DungeonsLobbySessionName = ANTICHEAT_STRINGS("DungeonsLobby");	
	static const FString MinecraftServicesUrl = ANTICHEAT_STRINGS("https://api.minecraftservices.com");
}

FString ConvertHRtoString(HRESULT hr);
FString ResultToMessage(HRESULT r);
FString ConvertEventTypeToString(XblSocialManagerEventType eventType);

FString ConvertMultiplayerManagerEventTypeToString(XblMultiplayerEventType eventType);
FString ConvertXboxLiveUserPresenceStateToString(XblPresenceUserState state);

namespace xsapi {
	string_t toString_t(const FString&);
	FString toFString(const string_t&);
	string_t toString_t(const std::string&);
	string_t toString_t(uint64);

	using MultiplayerEvent = XblMultiplayerEvent;
	using MultiplayerEventType = XblMultiplayerEventType;
	using SocialEvent = XblSocialManagerEvent;

	void LogEvent(const MultiplayerEvent&);
	void LogEvent(const SocialEvent&);
}

namespace TimerUtils {
//Always return milliseconds
#if PLATFORM_SWITCH
	static unsigned long long GetCurrentTime() { return  nn::os::ConvertToTimeSpan(nn::os::GetSystemTick()).GetMilliSeconds(); }
#elif PLATFORM_PS4
	static unsigned long long GetCurrentTime() {
		return sceKernelGetProcessTime() / 1000;
}
#else
	static unsigned long long GetCurrentTime() { return GetTickCount64(); }
#endif
	static unsigned long long GetTimeSince(unsigned long long baselineTime) { return GetCurrentTime() - baselineTime; }
}

namespace AsyncTasks {

	struct XBLContext
	{
		std::function<void(XAsyncBlock*)> Callback;
	};

	XAsyncBlock* CreateAsyncBlock(std::function<void(XAsyncBlock*)> callback, XTaskQueueHandle queue);
}

namespace scopedLambda {
	class ScopedLambda
	{
	public:
		explicit ScopedLambda(std::function<void()> onExit);
		ScopedLambda(const ScopedLambda&) = delete;
		ScopedLambda& operator=(const ScopedLambda&) = delete;
		ScopedLambda(ScopedLambda&&) = default;
		~ScopedLambda();

		explicit operator bool() const { return true; }
	private:
		std::function<void()> _onExit;
	};
}

namespace environmentConfig {
	enum class PlatformType
	{
		WINDOWS_PLATFORM,
		XBOXONE_PLATFORM,
		SWITCH_PLATFORM,
		PS4_PLATFORM,
		UNKNOWN
	};

	PlatformType GetPlatform();
	std::string ConvertPlatformToString(PlatformType Platform);
	FString ConvertPlatformToFString(PlatformType Platform);
	PlatformType ConvertToPlatform(const FString& Platform);
	PlatformType ConvertToPlatform(XblPresenceDeviceType DeviceType);
	bool GetConfigBool(const FString& section, const FString& key, bool defaultValue);
	bool IsShippingMode();
	const char* getSandbox();
	bool HasOnlineFlag();
}
