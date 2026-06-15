#pragma once

#include "OnlineIdentityDungeons.h"

class FOnlineIdentityDungeons;
class FOnlineSessionDungeons;

namespace dungeonsSession {
	namespace global {
		std::vector<XblMultiplayerManagerMember> getLobbySessionMembers();
	}
	namespace local {
		std::vector<XblMultiplayerManagerMember> getMembers(const XblMultiplayerEventArgsHandle&);
		void removeAllUsersFromLobbySession(const FOnlineIdentityDungeons*);
		void SetPlatformProperty(XblUserHandle);
		void InvalidateAdvertiseCache();
		void SetActivity(FOnlineSubsystemDungeons* DungeonsSubSystem, XblContextHandle xblContext, const XblMultiplayerSessionReference* sessionReference , bool bAdvertise);
		void SetGUIDConnectionAddress(const XblUserHandle);
		void GetActivities(XblContextHandle ContextHandle, const char* SCID, const uint64_t* XUIDs, XAsyncBlock* AsyncBlock);
		bool IsUserInLobby(uint64_t HostXUID);
	}
}

class FOnlineGameSessionBase {
public:
	FOnlineGameSessionBase(FOnlineIdentityDungeons*);
	~FOnlineGameSessionBase();

	FOnlineIdentityDungeons* GetOnlineIdentity() const;
	FOnlineSubsystemDungeons* GetOnlineSubsystem() const;
	FOnlineSessionDungeons* GetOnlineSession() const;
	void MemberJoined(const XblMultiplayerEvent&) const;
	void MemberLeft(const XblMultiplayerEvent&) const;

private:
	FOnlineIdentityDungeons* OnlineIdentity;
};
