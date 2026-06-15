#include "PCHOnlineDungeonsSubsystem.h"

#include "OnlineGameSessionBase.h"
#include "OnlineIdentityDungeons.h"
#include "OnlineSubsystem.h"
#include "HrLog.h"
#include "utils.h"
#include "multiplayer_activity_c.h"
#include "OnlineSubsystemDungeons.h"
#include "OnlineSessionInterfaceDungeons.h"

namespace dungeonsSession {
	namespace global {
		std::vector<XblMultiplayerManagerMember> getLobbySessionMembers() {
			const size_t localMemberCount = XblMultiplayerManagerLobbySessionMembersCount();
			std::vector<XblMultiplayerManagerMember> localMembers(localMemberCount);
			HrLog(XblMultiplayerManagerLobbySessionMembers(localMemberCount, localMembers.data()), "[Session] Getting all members of a lobby");
			return localMembers;
		}
	}

	namespace local {
		std::vector<XblMultiplayerManagerMember> getMembers(const XblMultiplayerEventArgsHandle& EventArgsHandle) {
			size_t MemberCount = 0;
			HrLog(XblMultiplayerEventArgsMembersCount(EventArgsHandle, &MemberCount), "[Session] XblMultiplayerEventArgsMembersCount (MemberJoined)");
			std::vector<XblMultiplayerManagerMember> Members(MemberCount);
			if (MemberCount != 0) {
				HrLog(XblMultiplayerEventArgsMembers(EventArgsHandle, MemberCount, Members.data()), "[Session] XblMultiplayerEventArgsXuid (MemberJoined)");
			}
			return Members;
		}

		void removeAllUsersFromLobbySession(const FOnlineIdentityDungeons* onlineIdentity) {
			for (const auto& member : global::getLobbySessionMembers()) {
				const auto user = onlineIdentity->GetUserFromXuid(member.Xuid);
				if (!user) {
					UE_LOG_ONLINE(Error, TEXT("[Session] Removing a non existing user from lobby"));
					continue;
				}
				UE_LOG_ONLINE(Log, TEXT("[Session] Removing user from lobby (User='%s')"), *user->GetGamerTag());
				HrLog(XblMultiplayerManagerLobbySessionRemoveLocalUser(user->XBLUser), "[Session] Removing local user from lobby session");
			}
		}

		void SetPlatformProperty(XblUserHandle UserHandle)
		{
			auto Platform = environmentConfig::GetPlatform();
			std::string PlatformString("\"" + ConvertPlatformToString(Platform) + "\"");
			HrLogOnlyError(
				XblMultiplayerManagerLobbySessionSetLocalMemberProperties(UserHandle, "PLATFORM", PlatformString.data(), nullptr)
				, "[Session] XblMultiplayerManagerLobbySessionSetLocalMemberProperties");
		}

		enum EAdvertise
		{
			Undefined,
			NotSet,
			Enabled,
			Disabled
		};

		static EAdvertise sLastAdvertise = EAdvertise::Undefined;

		void InvalidateAdvertiseCache()
		{
			sLastAdvertise = EAdvertise::NotSet;
		}

		void SetActivity(FOnlineSubsystemDungeons* DungeonsSubSystem, XblContextHandle xblContext, const XblMultiplayerSessionReference* sessionReference , bool bAdvertise )
		{
			check(sLastAdvertise != EAdvertise::Undefined);
			EAdvertise CurrentAdvertise = bAdvertise ? EAdvertise::Enabled : EAdvertise::Disabled;
			if (CurrentAdvertise == sLastAdvertise)
				return;
			sLastAdvertise = CurrentAdvertise;

			if (XblMultiplayerSessionReferenceIsValid(sessionReference))
			{
				XAsyncBlock* OurAsyncBlock = AsyncTasks::CreateAsyncBlock([bAdvertise](XAsyncBlock* asyncBlock)
				{
					auto SetClear = bAdvertise ? "[Session] XblMultiplayerSetActivityAsyncResult" : "[Session] XblMultiplayerSetActivityAsyncResult";
					HrLog(XAsyncGetStatus(asyncBlock, false), SetClear);
				}, DungeonsSubSystem->GetQueueHandle());
				if (bAdvertise)
				{
					UE_LOG_ONLINE_SESSION(Log, TEXT("[Session] XblMultiplayerSetActivityAsyncResult"));
					HrSuccess(XblMultiplayerSetActivityAsync(xblContext, sessionReference, OurAsyncBlock), "[Session] Setting Activity");
				}
				else
				{
					UE_LOG_ONLINE_SESSION(Log, TEXT("[Session] XblMultiplayerClearActivityAsync"));
					HrLog(XblMultiplayerClearActivityAsync(xblContext, OnlineSubsystemConstants::MULTIPLAYERSCID, OurAsyncBlock), "[Session] Clearing Activity");
				}
			}
			else
			{
				//sLastAdvertise = EAdvertise::Invalid;
				UE_LOG_ONLINE_SESSION(Warning, TEXT("[Session] XblMultiplayerSessionReferenceIsValid Failed"));
			}
		}

		void SetGUIDConnectionAddress(const XblUserHandle UserHandle)
		{
			//#310471
			//XSAPI team suggests us to set a unique identifier as the connection address to make
			//XblMultiplayerManagerLobbySessionHost work properly
			auto UniqueAddress = FGuid::NewGuid();
			HrLogOnlyError(XblMultiplayerManagerLobbySessionSetLocalMemberConnectionAddress(UserHandle, TCHAR_TO_ANSI(*UniqueAddress.ToString()), nullptr), "[Session] XblMultiplayerManagerLobbySessionSetLocalMemberConnectionAddress");
		}

		void GetActivities(XblContextHandle ContextHandle, const char* SCID, const uint64_t* XUIDs, XAsyncBlock* AsyncBlock)
		{
			HrSuccess(XblMultiplayerActivityGetActivityAsync(ContextHandle, XUIDs, 1, AsyncBlock), "[Session] Getting Activity");
		}

		bool IsUserInLobby(uint64_t HostXUID)
		{
			for (const auto& member : global::getLobbySessionMembers())
			{
				if (HostXUID == member.Xuid)
				{
					return true;
				}
			}
			return false;
		}
	}
}

FOnlineGameSessionBase::FOnlineGameSessionBase(FOnlineIdentityDungeons* OnlineIdentity)
	: OnlineIdentity(OnlineIdentity)
{
}

FOnlineGameSessionBase::~FOnlineGameSessionBase() {
	dungeonsSession::local::removeAllUsersFromLobbySession(OnlineIdentity);
}

void FOnlineGameSessionBase::MemberJoined(const XblMultiplayerEvent& mpEvent) const
{
	for (const auto& member : dungeonsSession::local::getMembers(mpEvent.EventArgsHandle)) 
	{
		GetOnlineSession()->RegisterPlayer(OnlineSubsystemConstants::DungeonsGameSessionName, FUniqueNetIdDungeons(member.Xuid), false);
	}
}

void FOnlineGameSessionBase::MemberLeft(const XblMultiplayerEvent& mpEvent) const
{
	for (const auto& member : dungeonsSession::local::getMembers(mpEvent.EventArgsHandle))
	{
		GetOnlineSession()->UnregisterPlayer(OnlineSubsystemConstants::DungeonsGameSessionName, FUniqueNetIdDungeons(member.Xuid));
	}
}

FOnlineSubsystemDungeons* FOnlineGameSessionBase::GetOnlineSubsystem() const
{
	return OnlineIdentity->GetDungeonsOnlineSubsystem();
}

FOnlineSessionDungeons* FOnlineGameSessionBase::GetOnlineSession() const
{
	return GetOnlineSubsystem()->GetDungeonsSessionInterface();
}

FOnlineIdentityDungeons* FOnlineGameSessionBase::GetOnlineIdentity() const
{
	return OnlineIdentity;
}
