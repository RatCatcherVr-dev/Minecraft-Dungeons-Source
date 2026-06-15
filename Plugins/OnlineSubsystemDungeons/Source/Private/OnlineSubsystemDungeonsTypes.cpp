#include "PCHOnlineDungeonsSubsystem.h"
#include "OnlineSubsystemDungeonsTypes.h"
#include "utils.h"
#include "P2P/IpAddressDungeonsWebRTC.h"


namespace internalDungeonsTypes
{
	static FUniqueNetIdString TemporarySessionId("DEAD1111-8D77-461D-BB49-48192CC54434");
}

FOnlineSessionInfoDungeons::FOnlineSessionInfoDungeons() : FOnlineSessionInfoDungeons(internalDungeonsTypes::TemporarySessionId)
{
}

FOnlineSessionInfoDungeons::FOnlineSessionInfoDungeons(const string_t& sessionId) :
	FOnlineSessionInfoDungeons(xsapi::toFString(sessionId))
{
}

FOnlineSessionInfoDungeons::FOnlineSessionInfoDungeons(const FString& sessionId) :
	FOnlineSessionInfoDungeons(FUniqueNetIdString(sessionId, TEXT("DUNGEONS")))
{
}

FOnlineSessionInfoDungeons::FOnlineSessionInfoDungeons(FUniqueNetIdString sessionId) :
	SessionId(sessionId)
{
	ensure(SessionId.IsValid() && "Creating session info with invalid sessionId");
}

const FUniqueNetIdString& FOnlineSessionInfoDungeons::GetSessionIdAsUniqueNetIdString() const
{
	ensure(SessionId != internalDungeonsTypes::TemporarySessionId && "Trying to get a temporary session id.");
	return SessionId;
}

void FOnlineSessionInfoDungeons::SetSessionId(const FUniqueNetIdString& sessionId)
{
	SessionId = sessionId;
	ensure(SessionId.IsValid() && "Setting session info with invalid sessionId");
}

const FUniqueNetId& FOnlineSessionInfoDungeons::GetSessionId() const
{
	return GetSessionIdAsUniqueNetIdString();
}

const uint8* FOnlineSessionInfoDungeons::GetBytes() const
{
	return nullptr;
}

int32 FOnlineSessionInfoDungeons::GetSize() const
{
	return sizeof(FUniqueNetIdString);
}

bool FOnlineSessionInfoDungeons::IsValid() const
{
	return true;
}

FString FOnlineSessionInfoDungeons::ToString() const
{
	return GetSessionId().ToString();
}

FString FOnlineSessionInfoDungeons::ToDebugString() const
{
	return GetSessionId().ToDebugString();
}
