#include "PCHOnlineDungeonsSubsystem.h"
#include "utils.h"

#include "OnlineSubsystem.h"
#include "EngineMinimal.h"
#include "game/util/Log.h"

#if PLATFORM_SWITCH
#include "Switch/SwitchSocketIncludes.h"
#endif


#define RETURN_STRING_FROM_HR(e) case (e): return #e

FString ConvertHRtoString(HRESULT hr)
{
	switch (hr)
	{
		RETURN_STRING_FROM_HR(S_OK);
		RETURN_STRING_FROM_HR(E_FAIL);
		RETURN_STRING_FROM_HR(E_POINTER);
		RETURN_STRING_FROM_HR(E_INVALIDARG);
		RETURN_STRING_FROM_HR(E_OUTOFMEMORY);
		RETURN_STRING_FROM_HR(E_NOT_SUFFICIENT_BUFFER);
		RETURN_STRING_FROM_HR(E_NOT_SUPPORTED);
		RETURN_STRING_FROM_HR(E_ABORT);
		RETURN_STRING_FROM_HR(E_NOTIMPL);
		RETURN_STRING_FROM_HR(E_ACCESSDENIED);
		RETURN_STRING_FROM_HR(E_PENDING);
		RETURN_STRING_FROM_HR(E_UNEXPECTED);
		RETURN_STRING_FROM_HR(E_TIME_CRITICAL_THREAD);
		RETURN_STRING_FROM_HR(E_NOINTERFACE);
		RETURN_STRING_FROM_HR(E_BOUNDS);
		RETURN_STRING_FROM_HR(E_NO_TASK_QUEUE);

		RETURN_STRING_FROM_HR(HTTP_E_STATUS_AMBIGUOUS);
		RETURN_STRING_FROM_HR(HTTP_E_STATUS_BAD_GATEWAY);
		RETURN_STRING_FROM_HR(HTTP_E_STATUS_BAD_METHOD);
		RETURN_STRING_FROM_HR(HTTP_E_STATUS_BAD_REQUEST);
		RETURN_STRING_FROM_HR(HTTP_E_STATUS_CONFLICT);
		RETURN_STRING_FROM_HR(HTTP_E_STATUS_DENIED);
		RETURN_STRING_FROM_HR(HTTP_E_STATUS_EXPECTATION_FAILED);
		RETURN_STRING_FROM_HR(HTTP_E_STATUS_FORBIDDEN);
		RETURN_STRING_FROM_HR(HTTP_E_STATUS_GATEWAY_TIMEOUT);
		RETURN_STRING_FROM_HR(HTTP_E_STATUS_GONE);
		RETURN_STRING_FROM_HR(HTTP_E_STATUS_LENGTH_REQUIRED);
		RETURN_STRING_FROM_HR(HTTP_E_STATUS_MOVED);
		RETURN_STRING_FROM_HR(HTTP_E_STATUS_NONE_ACCEPTABLE);
		RETURN_STRING_FROM_HR(HTTP_E_STATUS_NOT_FOUND);
		RETURN_STRING_FROM_HR(HTTP_E_STATUS_NOT_MODIFIED);
		RETURN_STRING_FROM_HR(HTTP_E_STATUS_NOT_SUPPORTED);
		RETURN_STRING_FROM_HR(HTTP_E_STATUS_PAYMENT_REQ);
		RETURN_STRING_FROM_HR(HTTP_E_STATUS_PRECOND_FAILED);
		RETURN_STRING_FROM_HR(HTTP_E_STATUS_PROXY_AUTH_REQ);
		RETURN_STRING_FROM_HR(HTTP_E_STATUS_RANGE_NOT_SATISFIABLE);
		RETURN_STRING_FROM_HR(HTTP_E_STATUS_REDIRECT);
		RETURN_STRING_FROM_HR(HTTP_E_STATUS_REDIRECT_KEEP_VERB);
		RETURN_STRING_FROM_HR(HTTP_E_STATUS_REDIRECT_METHOD);
		RETURN_STRING_FROM_HR(HTTP_E_STATUS_REQUEST_TIMEOUT);
		RETURN_STRING_FROM_HR(HTTP_E_STATUS_REQUEST_TOO_LARGE);
		RETURN_STRING_FROM_HR(HTTP_E_STATUS_SERVER_ERROR);
		RETURN_STRING_FROM_HR(HTTP_E_STATUS_SERVICE_UNAVAIL);
		RETURN_STRING_FROM_HR(HTTP_E_STATUS_UNEXPECTED);
		RETURN_STRING_FROM_HR(HTTP_E_STATUS_UNEXPECTED_SERVER_ERROR);
		RETURN_STRING_FROM_HR(HTTP_E_STATUS_UNSUPPORTED_MEDIA);
		RETURN_STRING_FROM_HR(HTTP_E_STATUS_URI_TOO_LONG);
		RETURN_STRING_FROM_HR(HTTP_E_STATUS_USE_PROXY);
		RETURN_STRING_FROM_HR(HTTP_E_STATUS_VERSION_NOT_SUP);
		RETURN_STRING_FROM_HR(ONL_E_ACTION_REQUIRED);
		RETURN_STRING_FROM_HR(WEB_E_INVALID_JSON_STRING);
		RETURN_STRING_FROM_HR(WEB_E_UNEXPECTED_CONTENT);

		RETURN_STRING_FROM_HR(E_XAL_NOTINITIALIZED);
		RETURN_STRING_FROM_HR(E_XAL_ALREADYINITIALIZED);
		RETURN_STRING_FROM_HR(E_XAL_USERSETNOTEMPTY);
		RETURN_STRING_FROM_HR(E_XAL_USERSETFULL);
		RETURN_STRING_FROM_HR(E_XAL_USERSIGNEDOUT);
		RETURN_STRING_FROM_HR(E_XAL_DUPLICATEDUSER);
		RETURN_STRING_FROM_HR(E_XAL_NETWORK);
		RETURN_STRING_FROM_HR(E_XAL_CLIENTERROR);
		RETURN_STRING_FROM_HR(E_XAL_UIREQUIRED);
		RETURN_STRING_FROM_HR(E_XAL_HANDLERALREADYREGISTERED);
		RETURN_STRING_FROM_HR(E_XAL_UNEXPECTEDUSERSIGNEDIN);
		RETURN_STRING_FROM_HR(E_XAL_NOTATTACHEDTOJVM);
		RETURN_STRING_FROM_HR(E_XAL_DEVICEUSER);
		RETURN_STRING_FROM_HR(E_XAL_DEFERRALNOTAVAILABLE);
		RETURN_STRING_FROM_HR(E_XAL_MISSINGPLATFORMEVENTHANDLER);
		RETURN_STRING_FROM_HR(E_XAL_USERNOTFOUND);
		RETURN_STRING_FROM_HR(E_XAL_NOTOKENREQUIRED);
		RETURN_STRING_FROM_HR(E_XAL_NODEFAULTUSER);
		RETURN_STRING_FROM_HR(E_XAL_FAILEDTORESOLVE);
		RETURN_STRING_FROM_HR(E_XAL_NOACCOUNTPROVIDER);

		RETURN_STRING_FROM_HR(E_XBL_RUNTIME_ERROR);
		RETURN_STRING_FROM_HR(E_XBL_RTA_GENERIC_ERROR);
		RETURN_STRING_FROM_HR(E_XBL_RTA_SUBSCRIPTION_LIMIT_REACHED);
		RETURN_STRING_FROM_HR(E_XBL_RTA_ACCESS_DENIED);
		RETURN_STRING_FROM_HR(E_XBL_RTA_NOT_ACTIVATED);
		RETURN_STRING_FROM_HR(E_XBL_AUTH_UNKNOWN_ERROR);
		RETURN_STRING_FROM_HR(E_XBL_AUTH_RUNTIME_ERROR);
		RETURN_STRING_FROM_HR(E_XBL_AUTH_NO_TOKEN);
		RETURN_STRING_FROM_HR(E_XBL_ALREADY_INITIALIZED);
		RETURN_STRING_FROM_HR(E_XBL_NOT_INITIALIZED);

		RETURN_STRING_FROM_HR(__HRESULT_FROM_WIN32(ERROR_ARITHMETIC_OVERFLOW));
		RETURN_STRING_FROM_HR(__HRESULT_FROM_WIN32(ERROR_BAD_CONFIGURATION));
		RETURN_STRING_FROM_HR(__HRESULT_FROM_WIN32(ERROR_BAD_LENGTH));
		RETURN_STRING_FROM_HR(__HRESULT_FROM_WIN32(ERROR_CANCELLED));
		RETURN_STRING_FROM_HR(__HRESULT_FROM_WIN32(ERROR_NO_SUCH_USER));
		RETURN_STRING_FROM_HR(__HRESULT_FROM_WIN32(ERROR_RESOURCE_DATA_NOT_FOUND));

		RETURN_STRING_FROM_HR(E_HC_PERFORM_ALREADY_CALLED);
		RETURN_STRING_FROM_HR(E_HC_ALREADY_INITIALISED);
		RETURN_STRING_FROM_HR(E_HC_CONNECT_ALREADY_CALLED);
		RETURN_STRING_FROM_HR(E_HC_NO_NETWORK);

	default: return "Unknown error";
	}
}

FString ResultToMessage(HRESULT r)
{
	switch (r)
	{
	case S_OK: return "Ok";
	case E_POINTER: return "Invalid pointer";
	case E_ABORT: return "Operation was canceled";
	case E_FAIL: return "Generic failure";
	case E_OUTOFMEMORY: return "Allocation failed";
	case E_INVALIDARG: return "Invalid argument";
	case E_UNEXPECTED: return "Unexpected";
	case E_NOT_SUFFICIENT_BUFFER: return "The supplied buffer is too small";
	case E_NOT_SUPPORTED: return "Feature is not present";

	case E_XAL_NOTINITIALIZED: return "Library was not initialized";
	case E_XAL_ALREADYINITIALIZED: return "Library was already initialized";
	case E_XAL_USERSETNOTEMPTY: return "The user set is not currently empty";
	case E_XAL_USERSETFULL: return "Concurrent local user limit has been reached";
	case E_XAL_USERSIGNEDOUT: return "User has been signed out";
	case E_XAL_DUPLICATEDUSER: return "The user attempted to sign in an account that is already in the user set";
	case E_XAL_NETWORK: return "Network error";
	case E_XAL_CLIENTERROR: return "Client operation failed";
	case E_XAL_UIREQUIRED: return "UI is required to complete the request";
	case E_XAL_HANDLERALREADYREGISTERED: return "This handler already has a callback registered";
	case E_XAL_UNEXPECTEDUSERSIGNEDIN: return "User signed into the an unexpected account";
	case E_XAL_NOTATTACHEDTOJVM: return "The current thread is not attached to the Java VM";
	case E_XAL_DEVICEUSER: return "An API was called with a device user which does not support being called with device users";
	case E_XAL_DEFERRALNOTAVAILABLE: return "Cannot acquire a deferral handle";
	case E_XAL_MISSINGPLATFORMEVENTHANDLER: return "A required platform event handler was not set";
	case E_XAL_USERNOTFOUND: return "No user was found for the given LocalId";
	case E_XAL_NOTOKENREQUIRED: return "No token is required for the given Url";
	case E_XAL_NODEFAULTUSER: return "No default user could be found";
	case E_XAL_FAILEDTORESOLVE: return "Could not resolve user issue";

	case E_XAL_INTERNAL_SWITCHUSER: return "The user decided to switch accounts during sign in";
	case E_XAL_INTERNAL_NOUSERFOUND: return "No user found";
	case E_XAL_INTERNAL_TOOMANYCACHEDUSERS: return "More than one user has credentials cached, a default user could not be chosen";
	case E_XAL_INTERNAL_BADUSERTOKEN: return "A user token was rejected by Xbox services";
	case E_XAL_INTERNAL_BADDEVICEIDENTITY: return "The device identity was rejected by Xbox services";
	case E_XAL_INTERNAL_UNAUTHORIZED: return "An unauthorized response was returned from Xbox services";
	case E_XAL_INTERNAL_NODISPLAYCLAIMSFOUND: return "No display claims found in token cache";
	default: return "<UNKNOWN>";
	}
}

FString
ConvertEventTypeToString(XblSocialManagerEventType eventType)
{
	switch (eventType)
	{
	case XblSocialManagerEventType::UsersAddedToSocialGraph: return "users_added";
	case XblSocialManagerEventType::UsersRemovedFromSocialGraph: return "users_removed";
	case XblSocialManagerEventType::PresenceChanged: return "presence_changed";
	case XblSocialManagerEventType::ProfilesChanged: return "profiles_changed";
	case XblSocialManagerEventType::SocialRelationshipsChanged: return "social_relationships_changed";
	case XblSocialManagerEventType::LocalUserAdded: return "local_user_added";
	case XblSocialManagerEventType::SocialUserGroupLoaded: return "social_user_group_loaded";
	case XblSocialManagerEventType::SocialUserGroupUpdated: return "social_user_group_updated";
	default: return "unknown";
	}
}
FString
ConvertXboxLiveUserPresenceStateToString(XblPresenceUserState state)
{
	switch (state)
	{
	case XblPresenceUserState::Unknown: return "Unknown";
	case XblPresenceUserState::Online: return "Online";
	case XblPresenceUserState::Away: return "Away";
	case XblPresenceUserState::Offline: return "Offline";
	default: return "Unkown";
	}
}

FString
ConvertMultiplayerManagerEventTypeToString(XblMultiplayerEventType eventType)
{
	switch (eventType)
	{
	case XblMultiplayerEventType::ArbitrationComplete: return "ArbitrationComplete";
	case XblMultiplayerEventType::ClientDisconnectedFromMultiplayerService: return "ClientDisconnectedFromMultiplayerService";
	case XblMultiplayerEventType::FindMatchCompleted: return "FindMatchCompleted";
	case XblMultiplayerEventType::HostChanged: return "HostChanged";
	case XblMultiplayerEventType::InviteSent: return "InviteSent";
	case XblMultiplayerEventType::JoinabilityStateChanged: return "JoinabilityStateChanged";
	case XblMultiplayerEventType::JoinGameCompleted: return "JoinGameCompleted";
	case XblMultiplayerEventType::JoinLobbyCompleted: return "JoinLobbyCompleted";
	case XblMultiplayerEventType::LeaveGameCompleted: return "LeaveGameCompleted";
	case XblMultiplayerEventType::LocalMemberConnectionAddressWriteCompleted: return "LocalMemberConnectionAddressWriteCompleted";
	case XblMultiplayerEventType::LocalMemberPropertyWriteCompleted: return "LocalMemberPropertyWriteCompleted";
	case XblMultiplayerEventType::MemberJoined: return "MemberJoined";
	case XblMultiplayerEventType::MemberLeft: return "MemberLeft";
	case XblMultiplayerEventType::MemberPropertyChanged: return "MemberPropertyChanged";
	case XblMultiplayerEventType::PerformQosMeasurements: return "PerformQosMeasurements";
	case XblMultiplayerEventType::SessionPropertyChanged: return "SessionPropertyChanged";
	case XblMultiplayerEventType::SessionPropertyWriteCompleted: return "SessionPropertyWriteCompleted";
	case XblMultiplayerEventType::SessionSynchronizedPropertyWriteCompleted: return "SessionSynchronizedPropertyWriteCompleted";
	case XblMultiplayerEventType::SynchronizedHostWriteCompleted: return "SynchronizedHostWriteCompleted";
	case XblMultiplayerEventType::TournamentGameSessionReady: return "TournamentGameSessionReady";
	case XblMultiplayerEventType::TournamentRegistrationStateChanged: return "TournamentRegistrationStateChanged";
	case XblMultiplayerEventType::UserAdded: return "UserAdded";
	case XblMultiplayerEventType::UserRemoved: return "UserRemoved";
	default: return "unknown";
	}
}

string_t xsapi::toString_t(const FString& str)
{
	// What a mess!!!
	// TCHAR is char16_t on switch
	// TCHAR is wchar_t on PS4
	// string_t is a std::string on switch
	// string_t is a std::string on PS4
#if PLATFORM_PS4
	return utility::conversions::to_string_t(TCHAR_TO_ANSI(*str));
#else
	return utility::conversions::to_string_t(*str);

#endif
}


FString xsapi::toFString(const string_t& str)
{
#if !PLATFORM_SWITCH && !PLATFORM_PS4
	return TCHAR_TO_ANSI(str.c_str());
#else
	return str.c_str();
#endif
}

string_t xsapi::toString_t(const std::string& str)
{
	return utility::conversions::to_string_t(str);
}

string_t xsapi::toString_t(uint64 value)
{
	return toString_t(std::to_string(value));
}

void xsapi::LogEvent(const MultiplayerEvent& e)
{
	if (SUCCEEDED(e.Result))
	{
		UE_LOG_ONLINE(Log, TEXT("[LogEvent] Received multiplayer event (type='%s')"), *ConvertMultiplayerManagerEventTypeToString(e.EventType));
	}
	else
	{
		UE_LOG_ONLINE(Error, TEXT("[LogEvent] Received multiplayer event with error: (type='%s', err='%s (%u)', msg='%s')"),
			*ConvertMultiplayerManagerEventTypeToString(e.EventType),
			*ConvertHRtoString(e.Result),
			e.Result,
			ANSI_TO_TCHAR(e.ErrorMessage)
		);
	}
}

void xsapi::LogEvent(const SocialEvent& e)
{
	if (SUCCEEDED(e.hr))
	{
		UE_LOG_ONLINE(Log, TEXT("[LogEvent] Received Social Manager event (type='%s')"), *ConvertEventTypeToString(e.eventType));
	}
	else
	{
		UE_LOG_ONLINE(Error, TEXT("[LogEvent] Received Social Manager event with error: (type='%s', err='%s (%u)')"),
			*ConvertEventTypeToString(e.eventType),
			*ConvertHRtoString(e.hr),
			e.hr
		);
	}
}

XAsyncBlock* AsyncTasks::CreateAsyncBlock(std::function<void(XAsyncBlock*)> callback, XTaskQueueHandle queue)
{
	return new XAsyncBlock{
		queue,
		new XBLContext{std::move(callback)},
		[](XAsyncBlock* asyncBlock) {
			ensureMsgf(asyncBlock->context, TEXT("The asyncBlock context is null, this should never happen."));
			XBLContext* ctx = static_cast<XBLContext*>(asyncBlock->context);
			ctx->Callback(asyncBlock);
			delete ctx;
			delete asyncBlock;
		}
	};
}

namespace scopedLambda {
	ScopedLambda::ScopedLambda(std::function<void()> onExit)
		: _onExit(std::move(onExit)) {
	}

	ScopedLambda::~ScopedLambda() {
		_onExit();
	}
}

namespace environmentConfig {
	bool IsDevelopmentMode()
	{
		return GetConfigBool("Environment", "Development", false);
	}

	bool IsShippingMode()
	{
#if UE_BUILD_SHIPPING
		return true;
#else
		return !IsDevelopmentMode();
#endif
	}
	
	environmentConfig::PlatformType GetPlatform()
	{
#if PLATFORM_WINDOWS
		return PlatformType::WINDOWS_PLATFORM;
#elif PLATFORM_XBOXONE
		return PlatformType::XBOXONE_PLATFORM;
#elif PLATFORM_SWITCH
		return PlatformType::SWITCH_PLATFORM;
#elif PLATFORM_PS4
		return PlatformType::PS4_PLATFORM;
#else
		static_assert(false, "No platform specified");
#endif
	}

	std::string ConvertPlatformToString(PlatformType Platform)
	{
		switch (Platform)
		{
		case environmentConfig::PlatformType::WINDOWS_PLATFORM:
			return "WINDOWS";
		case environmentConfig::PlatformType::XBOXONE_PLATFORM:
			return "XBOXONE";
		case environmentConfig::PlatformType::SWITCH_PLATFORM:
			return "SWITCH";
		case environmentConfig::PlatformType::PS4_PLATFORM:
			return "PS4";
		default:
			return "UNKNOWN";
		}
	}

	FString ConvertPlatformToFString(PlatformType Platform)
	{
		switch (Platform)
		{
		case environmentConfig::PlatformType::WINDOWS_PLATFORM:
			return "WINDOWS";
		case environmentConfig::PlatformType::XBOXONE_PLATFORM:
			return "XBOXONE";
		case environmentConfig::PlatformType::SWITCH_PLATFORM:
			return "SWITCH";
		case environmentConfig::PlatformType::PS4_PLATFORM:
			return "PS4";
		default:
			return "UNKNOWN";
		}
	}

	environmentConfig::PlatformType ConvertToPlatform(const FString& Platform)
	{
		if (!Platform.Compare("WINDOWS"))
		{
			return PlatformType::WINDOWS_PLATFORM;
		}
		if (!Platform.Compare("XBOXONE"))
		{
			return PlatformType::XBOXONE_PLATFORM;
		}
		if (!Platform.Compare("SWITCH"))
		{
			return PlatformType::SWITCH_PLATFORM;
		}
		if (!Platform.Compare("PS4"))
		{
			return PlatformType::PS4_PLATFORM;
		}
		return PlatformType::UNKNOWN;
	}

	environmentConfig::PlatformType ConvertToPlatform(XblPresenceDeviceType DeviceType)
	{
		switch (DeviceType) {			
		case XblPresenceDeviceType::XboxOne:
			return PlatformType::XBOXONE_PLATFORM;
		case XblPresenceDeviceType::Nintendo:
			return PlatformType::SWITCH_PLATFORM;
		case XblPresenceDeviceType::PlayStation:
			return PlatformType::PS4_PLATFORM;
		case XblPresenceDeviceType::Win32:
			return PlatformType::WINDOWS_PLATFORM;
		case XblPresenceDeviceType::Unknown:
		default:
			return PlatformType::UNKNOWN;
		}
	}

	const char* getSandbox()
	{
#if !UE_BUILD_SHIPPING //D11.PS we want to be on the retail sandbox in shipping.
		if (IsDevelopmentMode()) {
			return OnlineSubsystemConstants::DEVELOPMENTSANDBOX;
		}
#endif
		return OnlineSubsystemConstants::RETAILSANDBOX;
	}

	void ConfigBoolLog(const FString& msg, const FString& section, const FString& key, bool value)
	{
		const FString valueStr(value ? "true" : "false");
		UE_LOG_ONLINE(Log, TEXT("[Config] %s '%s' from '%s:%s' in config file '%s'"), *msg, *valueStr, *section, *key, *GEngineIni);
	}

	bool GetConfigBool(const FString& section, const FString& key, bool defaultValue)
	{
		if (!GConfig)
		{
			ConfigBoolLog("No GConfig falling back on default value", section, key, defaultValue);
			return defaultValue;
		}
		bool configValue = false;
		if (!GConfig->GetBool(*section, *key, configValue, GEngineIni))
		{
			ConfigBoolLog("Could not find section and/or key falling back on default value", section, key, defaultValue);
			return defaultValue;
		}
		ConfigBoolLog("Successfully read", section, key, configValue);
		return configValue;
	}

	bool HasOnlineFlag()
	{
		return FParse::Param(FCommandLine::Get(), TEXT("online"));
	}
}
