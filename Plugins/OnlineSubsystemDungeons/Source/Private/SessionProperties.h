#pragma once
#include "xsapiServicesInclude.h"
#include <Optional.h>
#include "OnlineSessionSettings.h"

class FNamedOnlineSession;
class FOnlineSessionSearchResult;
class FOnlineSession;

namespace sessionProperties {

struct SessionProperties {
	static constexpr int32 CURRENT_VERSION = 5;
	
	int32 Version = CURRENT_VERSION;
	string_t OwningUserName;
	string_t OwningXUID;
	string_t Platform;
	bool CrossplayEnabled;
	bool IsPrivateGame;
	bool IsSessionReadyToJoin;

	string_t MapName;
	string_t LevelName;
	string_t LevelFilename;
	string_t PlayerName;
	string_t MissionDifficulty;
	string_t ThreatLevel;
	string_t GameVersion;
	string_t ReconnectableGUIDS;
	string_t PlayerNames;
	uint32_t RandomSeed;
};

struct XBLMemberInfo
{
	string_t GamerTag;
	string_t Platform;
};

TOptional<SessionProperties> GetProperties(XblMultiplayerSessionHandle);

void SetProperties(const FNamedOnlineSession*);
void SetXblMembers(XblMultiplayerSessionHandle SessionHandle, FOnlineSession& session);
string_t ConvertMemberInfoToJson(const std::vector<XBLMemberInfo>& MemberInfo);
void SetIsReady(bool IsReady);

FOnlineSessionSearchResult BuildSessionFromXBLSession(
	const SessionProperties& properties,
	XblMultiplayerSessionHandle SessionHandle,
	const FString& HandleId,
	int32 PublicConnections, 
	int32 PublicOpenConnections);

}
