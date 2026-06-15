#include "PCHOnlineDungeonsSubsystem.h"

#include "OnlineFriendsInterfaceDungeons.h"

#include "OnlineSubsystemTypes.h"
#include "OnlineSubsystemDungeonsTypes.h"
#include "OnlineIdentityDungeons.h"
#include "OnlineSubsystemDungeons.h"

#include "GenericPlatform/GenericPlatformHttp.h"

#include "utils.h"

#include "xsapiServicesInclude.h"
#include "HrLog.h"

namespace dungeonsFriends {
	FString getFriendPath(const FString& myID, const FString& FriendId, const FString& ListName) {

		const FString FriendIDPath = FGenericPlatformHttp::UrlEncode(FriendId);
		const FString FriendGamerTag = FGenericPlatformHttp::UrlEncode(ListName);

		FString Path = "https://social.xboxlive.com";

		Path += "/users/xuid";
		Path += "(" + myID + ")";
		if (!FriendGamerTag.IsEmpty()) {
			Path += "/people/gt(";
			Path += FriendGamerTag;
			Path += ")";
		}
		else if (!FriendIDPath.IsEmpty()) {
			Path += "/people/xuid(";
			Path += FriendIDPath;
			Path += ")";
		}
		return Path;
	}

	XblHttpCallHandle makeFriendHTTPCall(XblContextHandle XBLHandle, const FString& method, const FString& path) {
		XblHttpCallHandle Call = nullptr;
		if (HrFailed(XblHttpCallCreate(XBLHandle, TCHAR_TO_ANSI(*method), TCHAR_TO_ANSI(*path), &Call), "[Friends] XblHttpCallCreate"))
		{
			return {};
		}

		XblHttpCallRequestSetRetryAllowed(Call, true);
		XblHttpCallRequestSetHeader(Call, "x-xbl-contract-version", "2", true);
		// D11.AH - AB_#488345 Please note Switch Curl lib does not like 0 length 
		//		    strings for the body, passing {} is harmless to solve the bug.
		XblHttpCallRequestSetRequestBodyString(Call, "{}");

		return Call;
	}
}
FOnlineFriendDungeons::FOnlineFriendDungeons(uint64 UniqueNetId, FOnlineUserPresence InPresence) 
	: UserId(new FUniqueNetIdDungeons(UniqueNetId)), Presence(InPresence)
{}

FOnlineFriendDungeons::~FOnlineFriendDungeons()
{}

TSharedRef<const FUniqueNetId> FOnlineFriendDungeons::GetUserId() const
{
	return UserId;
}

FString FOnlineFriendDungeons::GetRealName() const
{
	return RealName;
}

FString FOnlineFriendDungeons::GetDisplayName(const FString& Platform) const
{
	return DisplayName;
}

bool FOnlineFriendDungeons::GetUserAttribute(const FString& AttrName, FString& OutAttrValue) const
{
	if (Attributes.Contains(AttrName)) {
		OutAttrValue = Attributes[AttrName];
		return true;
	}
	return false;
}

EInviteStatus::Type FOnlineFriendDungeons::GetInviteStatus() const
{
	return EInviteStatus::Accepted;
}

const FOnlineUserPresence& FOnlineFriendDungeons::GetPresence() const
{
	return Presence;
}

bool FOnlineFriendDungeons::SetUserLocalAttribute(const FString& AttrName, const FString& AttrValue) {
	Attributes.Emplace(AttrName, AttrValue);
	return true;
}

void FOnlineFriendDungeons::SetUserAttributeBool(const FString& AttrName, bool AttrValue) {
	SetUserLocalAttribute(AttrName, AttrValue ? FString("true") : FString("false"));
}

FOnlineFriendsDungeons::FOnlineFriendsDungeons(class FOnlineSubsystemDungeons* InDungeonsSubsystem) : DungeonsSubsystem(InDungeonsSubsystem)
{
}

FOnlineFriendsDungeons::~FOnlineFriendsDungeons() 
{
}

bool FOnlineFriendsDungeons::ReadFriendsList(int32 LocalUserNum, const FString& ListName, const FOnReadFriendsListComplete& Delegate)
{
	FOnlineIdentityDungeons* identityPtr = DungeonsSubsystem->GetDungeonsIdentityInterface();
	if (!identityPtr)
	{
		UE_LOG_ONLINE(Warning, TEXT("[Friends] Identity interface not initilized (nullptr), aborting retrieve friendslist"));
		return false;
	}
	const auto Me = identityPtr->GetUserFromLocalUser(LocalUserNum);
	if (!Me) {
		UE_LOG_ONLINE(Error, TEXT("[Friends] Could not find the user when reading the friends list"));
		return false;
	}
	if (!Me->SocialGroup) {
		UE_LOG_ONLINE(Error, TEXT("[Friends] A user has no social group when reading the friends list"));
		return false;
	}
	const XblSocialManagerUser* const* Users = nullptr;
	size_t UsersCount = 0;
	if (HrFailed(XblSocialManagerUserGroupGetUsers(Me->SocialGroup, &Users, &UsersCount), "[Friends] Getting users for a user group")) {
		return false;
	}

	Friends.Empty();

	for (int i = 0; i < UsersCount; i++) {
		bool IsPlayingThisGame = false;
		bool IsOnline = false;
		bool IsTwowayFriend = false;
		FString InvitableStatus = "";
		FString statusString = "";
		environmentConfig::PlatformType platformType = environmentConfig::PlatformType::UNKNOWN;

		const XblSocialManagerUser* const User = Users[i];

		switch (User->presenceRecord.userState) {
		case XblPresenceUserState::Online:
		case XblPresenceUserState::Away:
			IsOnline = true;			
			break;		
		default:
			break;
		}

		if (User->isFollowedByCaller && User->isFollowingUser) {
			IsTwowayFriend = true;
		}

		for (uint32_t Record = 0; Record < User->presenceRecord.presenceTitleRecordCount; Record++)
		{
			const auto& PresenceRecord = User->presenceRecord.presenceTitleRecords[Record];
			if (PresenceRecord.isTitleActive &&
				PresenceRecord.titleId == OnlineSubsystemConstants::TITLEID ) {
				statusString = PresenceRecord.presenceText;
				platformType = environmentConfig::ConvertToPlatform(PresenceRecord.deviceType);
				IsPlayingThisGame = true;
				if (platformType == environmentConfig::PlatformType::WINDOWS_PLATFORM) {
					// Fixes AB#404816 where Win32 can end up before WindowsOneCore in users presence record
					break;
				}
			}
		}
		
		FOnlineUserPresence Presence;
		Presence.bIsOnline = IsOnline;
		Presence.bIsPlaying = IsOnline; //Not knowable - but assumed. 
		Presence.bIsPlayingThisGame = IsPlayingThisGame;
		Presence.Status.StatusStr = statusString;

		TSharedRef<FOnlineFriendDungeons> Buddy = MakeShared<FOnlineFriendDungeons>(User->xboxUserId, Presence);
		Buddy->DisplayName = User->gamertag;
		Buddy->RealName = User->realName;
		Buddy->SetUserLocalAttribute(TEXT("OnlineService"), TEXT("XBL"));
		Buddy->SetUserAttributeBool(TEXT("IsTwoWayFriend"), IsTwowayFriend);

		FVariantData data;
		data.SetValue(environmentConfig::ConvertPlatformToFString(platformType));
		Buddy->Presence.Status.Properties.Add(DefaultPlatformKey, data);

		FUniqueNetIdDungeons id(User->xboxUserId);
		Friends.Add(id, Buddy);
	}
	
	Delegate.ExecuteIfBound(LocalUserNum, true, ListName, FString());
	return true;
}

bool FOnlineFriendsDungeons::DeleteFriendsList(int32 LocalUserNum, const FString& ListName, const FOnDeleteFriendsListComplete& Delegate) { return false; }

bool FOnlineFriendsDungeons::SendInvite(int32 LocalUserNum, const FUniqueNetId& FriendId, const FString& ListName, const FOnSendInviteComplete& Delegate) {

	if (ListName.IsEmpty() && !FriendId.IsValid()) {
		UE_LOG_ONLINE(Log, TEXT("[Friends] Add Friend triggered without a valid friend gamer tag (listname) or xuid (FriendId)!"));
		return false;
	}

	auto httpCall = MakeUpdateFriendCall(LocalUserNum, FriendId, ListName, "PUT");

	auto result = XblHttpCallPerformAsync(httpCall, 
		XblHttpCallResponseBodyType::String, 
		AsyncTasks::CreateAsyncBlock([httpCall, LocalUserNum, FriendId = FUniqueNetIdString(FriendId), ListName, Delegate](XAsyncBlock* asyncBlock)
	{
		uint32_t status = 0;
		const char* ErrMsg = nullptr;

		HrLog(XblHttpCallGetStatusCode(httpCall, &status), "[Friends] Get status code");
		HrLog(XblHttpCallGetPlatformNetworkErrorMessage(httpCall, &ErrMsg), "[Friends] Get platform network error message");

		if (Delegate.IsBound()) {
			bool success = status >= 200 && status < 300;
			Delegate.Execute(LocalUserNum, success, FriendId, ListName, ErrMsg);
		}
		XblHttpCallCloseHandle(httpCall);
	}, DungeonsSubsystem->GetQueueHandle()));

	return HrSuccess(result, "[Friends] Send invite perform async");
}

bool FOnlineFriendsDungeons::AcceptInvite(int32 LocalUserNum, const FUniqueNetId& FriendId, const FString& ListName, const FOnAcceptInviteComplete& Delegate) { return false; }
bool FOnlineFriendsDungeons::RejectInvite(int32 LocalUserNum, const FUniqueNetId& FriendId, const FString& ListName) { return false; }


bool FOnlineFriendsDungeons::DeleteFriend(int32 LocalUserNum, const FUniqueNetId& FriendId, const FString& ListName) {
	if (ListName.IsEmpty() && !FriendId.IsValid()) {
		UE_LOG_ONLINE(Log, TEXT("[Friends] Delete Friend triggered without a valid friend gamer tag (listname) or xuid (FriendId)!"));
		return false;
	}
	auto httpCall = MakeUpdateFriendCall(LocalUserNum, FriendId, ListName, "DELETE");

	XblHttpCallPerformAsync(httpCall,
		XblHttpCallResponseBodyType::String,
		AsyncTasks::CreateAsyncBlock([httpCall, LocalUserNum, DeleteFriendId = FriendId.ToString()](XAsyncBlock* asyncBlock)
	{
		uint32_t status = 0;

		XblHttpCallGetStatusCode(httpCall, &status);

		bool success = status >= 200 && status < 300;
		if (success)
		{
			UE_LOG_ONLINE(Log, TEXT("[Friends] Delete Friend %s success! Http Status: %d"), *DeleteFriendId, status);
		}
		else
		{
			UE_LOG_ONLINE(Warning, TEXT("[Friends] Delete Friend %s failed! Http Status: %d"), *DeleteFriendId, status);
		}
		XblHttpCallCloseHandle(httpCall);
	}, DungeonsSubsystem->GetQueueHandle()));

	return true;
}

bool FOnlineFriendsDungeons::GetFriendsList(int32 LocalUserNum, const FString& ListName, TArray< TSharedRef<FOnlineFriend> >& OutFriends) 
{ 
	for (const auto& buddy : Friends)
	{
		OutFriends.Add(buddy.Value);
	}
	return true; 
}

TSharedPtr<FOnlineFriend> FOnlineFriendsDungeons::GetFriend(int32 LocalUserNum, const FUniqueNetId& FriendId, const FString& ListName) { return nullptr; }
bool FOnlineFriendsDungeons::IsFriend(int32 LocalUserNum, const FUniqueNetId& FriendId, const FString& ListName) 
{
	return Friends.Contains(static_cast<const FUniqueNetIdDungeons>(FriendId));
}
bool FOnlineFriendsDungeons::QueryRecentPlayers(const FUniqueNetId& UserId, const FString& Namespace) { return false; }
bool FOnlineFriendsDungeons::GetRecentPlayers(const FUniqueNetId& UserId, const FString& Namespace, TArray< TSharedRef<FOnlineRecentPlayer> >& OutRecentPlayers) { return false; }
bool FOnlineFriendsDungeons::BlockPlayer(int32 LocalUserNum, const FUniqueNetId& PlayerId) { return false; }
bool FOnlineFriendsDungeons::UnblockPlayer(int32 LocalUserNum, const FUniqueNetId& PlayerId) { return false; }
bool FOnlineFriendsDungeons::QueryBlockedPlayers(const FUniqueNetId& UserId) { return false; }
bool FOnlineFriendsDungeons::GetBlockedPlayers(const FUniqueNetId& UserId, TArray< TSharedRef<FOnlineBlockedPlayer> >& OutBlockedPlayers) { return false; }
void FOnlineFriendsDungeons::DumpBlockedPlayers() const { return; }
void FOnlineFriendsDungeons::DumpRecentPlayers() const { return; }

XblHttpCallHandle FOnlineFriendsDungeons::MakeUpdateFriendCall(int32 LocalUserNum, const FUniqueNetId& FriendId, const FString& ListName, FString Method) const {
	XblContextHandle contextHandle = GetXBLContextHandle(LocalUserNum);

	FString path = dungeonsFriends::getFriendPath(
		*DungeonsSubsystem->GetDungeonsIdentityInterface()->GetUserFromLocalUser(LocalUserNum)->UniqueNetId->ToString(), 
		FriendId.ToString(), 
		ListName);

	return dungeonsFriends::makeFriendHTTPCall(contextHandle, Method, path);
}

XblUserHandle FOnlineFriendsDungeons::GetXBLUserHandle(int32 LocalUserNum) const {
	const auto IdentityInterface = DungeonsSubsystem->GetDungeonsIdentityInterface();
	return IdentityInterface->GetXBLUserHandle(LocalUserNum);
}

XblContextHandle FOnlineFriendsDungeons::GetXBLContextHandle(int32 LocalUserNum) const {
	const auto IdentityInterface = DungeonsSubsystem->GetDungeonsIdentityInterface();
	return IdentityInterface->GetXBLContextHandle(LocalUserNum);
}