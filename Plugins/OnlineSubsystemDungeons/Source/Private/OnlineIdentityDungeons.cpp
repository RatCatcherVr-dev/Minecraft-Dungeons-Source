#include "PCHOnlineDungeonsSubsystem.h"
#include "OnlineIdentityDungeons.h"

#include "OnlineSubsystem.h"
#include "OnlineSubsystemDungeons.h"
#include "OnlineFriendsInterfaceDungeons.h"
#include "OnlineSessionSettings.h"
#include "online/OnlineCommon.h"
#include "AsyncTasks/OnlineAsyncTaskDungeonsTriggers.h"
#include "PlayfabServices.h"
#include "OnlineUserCloudCommon.h"

#include <memory>
#include <vector>
#include "utils.h"

#include <Themida/Anticheat.hpp>

#include "OnlineAchievementsInterfaceDungeons.h"
#include "OnlineStatsInterfaceDungeons.h"
#include "Misc/CoreDelegates.h"

#if !PLATFORM_SWITCH && !PLATFORM_PS4
#include "AllowWindowsPlatformAtomics.h"
#include "AllowWindowsPlatformTypes.h"
#endif

#if !PLATFORM_XBOXONE
#include "xal_platform.h"
#include "Xal/xal.h"
#include "xal_user.h"
#include "xal_types.h"
#endif

#if PLATFORM_XBOXONE
#include "xal_xdk_ext.h"
#include "XboxOne/XboxOneInputInterface.h"
#include "XboxOne/XboxOneApplication.h"
#endif

#if !PLATFORM_SWITCH && !PLATFORM_PS4
#include "HideWindowsPlatformTypes.h"
#include "HideWindowsPlatformAtomics.h"
#endif

#include "P2P/Peer2PeerManager.h"
#include "SessionProperties.h"
#include "HrLog.h"
#include "CoreDelegates.h"
#include "IDungeonsAuth.h"

#ifdef EPIC_STORE_BUILD
#include "EpicServices.h"
#include "EpicStoreConstants.h"
#endif

#if PLATFORM_SWITCH
#include "Switch/SwitchUtil.h"
#include "Switch/SwitchPlatformMisc.h"
#endif

const FString ServiceTokenId = "ServiceToken";

namespace dungeonsIdentity {
const float TOKEN_CHECK_INTERVAL_SEC = 30.f;
/* lifetime of the MSA Link codes is 5 minutes. https://microsoft.com/link */
const float MSA_CODE_CHECK_INTERVAL_SEC = 300.0f;

XalPrivilege UnrealToXalPrivilege(const EUserPrivileges::Type Privilege)
{
	switch (Privilege)
	{
	case EUserPrivileges::CanPlayOnline:
		return XalPrivilege::XalPrivilege_Multiplayer;
	case EUserPrivileges::CanCommunicateOnline:
		return XalPrivilege::XalPrivilege_Comms;
	case EUserPrivileges::CanUseUserGeneratedContent:
		return XalPrivilege::XalPrivilege_Ugc;
	case EUserPrivileges::CanUserCrossPlay:
		return XalPrivilege::XalPrivilege_CrossPlay;
	default:
		return XalPrivilege::XalPrivilege_Sessions;
	}
}

uint64_t getUserId(XblUserHandle handle) {
	uint64_t userId = 0;
	HrCheck(XalUserGetId(handle, &userId), "[Identity] Get user id from handle");
	return userId;
}

FString getGamerTag(XblUserHandle handle) {
	const size_t gamerTagSize = XalUserGetGamertagSize(handle, XalGamertagComponent_Classic);
	std::vector<char> gamerTag;
	gamerTag.resize(gamerTagSize);
	size_t used = 0;
	HrCheck(XalUserGetGamertag(handle, XalGamertagComponent_Classic, gamerTagSize, &*gamerTag.begin(), &used), "[Identity] Get gamer tag");
	gamerTag.resize(used);
	return ANSI_TO_TCHAR(gamerTag.data());
}

XblContextHandle createContext(XblUserHandle handle) {
	XblContextHandle context = nullptr;
	HrCheck(XblContextCreateHandle(handle, &context), "[Identity] Creating XblContextHandles");
	return context;
}

XblSocialManagerUserGroup* addLocalUserToSocialManager(XblUserHandle handle, XTaskQueueHandle queueHandle) {
	HrCheck(XblSocialManagerAddLocalUser(handle, XblSocialManagerExtraDetailLevel::NoExtraDetail, queueHandle), "[Identity] Add local user  to social manager");
	XblSocialManagerUserGroup* socialGroup = nullptr;
	HrCheck(XblSocialManagerCreateSocialUserGroupFromFilters(handle, XblPresenceFilter::All, XblRelationshipFilter::Friends, &socialGroup), "[Identity] Creating social group");
	return socialGroup;
}

std::shared_ptr<XBLUserInfo> createUser(FOnlineIdentityDungeons* identity, XblUserHandle handle, uint32 localUserNum) {
	const auto userId = getUserId(handle);
	const auto context = createContext(handle);
	const auto gamerTag = getGamerTag(handle);
	const auto socialGroup = XalUserIsGuest(handle) ? 0 : addLocalUserToSocialManager(handle, identity->GetDungeonsOnlineSubsystem()->GetQueueHandle());

#if PLATFORM_XBOXONE 
	char tempXUID[21];
	snprintf(tempXUID, 21, "%llu", userId);
	auto XboxOneApp = FXboxOneApplication::GetXboxOneApplication();
	const auto systemId = XboxOneApp->GetXboxInputInterface()->GetPlatformUserIdFromXboxUserId(UTF8_TO_TCHAR(tempXUID));
	const XblFunctionContext inviteHandle = { 0 };
	const XblFunctionContext rtaStateHandle = { 0 };
	const XblFunctionContext statChangeHandle = { 0 };

	return std::make_shared<XBLUserInfo>(
		handle,
		localUserNum,
		userId,
		systemId,
		context,
		gamerTag,
		socialGroup,
		inviteHandle,
		rtaStateHandle,
		statChangeHandle
		);
#else

	const auto inviteHandle = XblGameInviteAddNotificationHandler(context,
		[](const XblGameInviteNotificationEventArgs* args, void* context) {
			FOnlineIdentityDungeons* identity = static_cast<FOnlineIdentityDungeons*>(context);
			identity->HandleInviteNotification(args, identity->GetXBLContextHandle(identity->GetFirstLocalUserNumber()));
		}
	, identity);

	const auto statChangeHandle = XblUserStatisticsAddStatisticChangedHandler(context,
		[](XblStatisticChangeEventArgs args, void* context) {
		FOnlineIdentityDungeons* identity = static_cast<FOnlineIdentityDungeons*>(context);
		identity->HandleStatChangeNotification(&args, identity->GetXBLContextHandle(identity->GetFirstLocalUserNumber()));
	}
	, identity);

	const auto rtaStateHandle = XblRealTimeActivityAddConnectionStateChangeHandler(context,
		[] (void* context, XblRealTimeActivityConnectionState connectionState) {
			FOnlineIdentityDungeons* identity = static_cast<FOnlineIdentityDungeons*>(context);
			switch (connectionState)
			{
			case XblRealTimeActivityConnectionState::Connecting:
				UE_LOG_ONLINE(Log, TEXT("[Identity] RTA Connecting"));
				break;
			case XblRealTimeActivityConnectionState::Connected:
				UE_LOG_ONLINE(Log, TEXT("[Identity] RTA Connected"));
				if (identity->GetPreviousRTAEvent() == XblRealTimeActivityConnectionState::Disconnected) {
					UE_LOG_ONLINE(Log, TEXT("[Identity] RTA Connected coming from disconnected state"));
					identity->GetDungeonsOnlineSubsystem()->QueueAsyncTask([identity, connectionState]() {
						HrCheck(XblMultiplayerManagerInitialize(
							OnlineSubsystemConstants::DungeonsLobbySessionName.c_str(), identity->GetDungeonsOnlineSubsystem()->GetQueueHandle())
							, "[Session] Initialize multiplayer manager");

						identity->SetRTAEvent(connectionState);
						identity->RegisterInviteHandlerForUser(identity->GetFirstLocalUserNumber());
						identity->RegisterStatChangeHandlerForUser(identity->GetFirstLocalUserNumber());
					});
				}
			break;
			case XblRealTimeActivityConnectionState::Disconnected:
				UE_LOG_ONLINE(Log, TEXT("[Identity] RTA Disconnected"));
				if (identity->GetPreviousRTAEvent() == XblRealTimeActivityConnectionState::Connected) {
					UE_LOG_ONLINE(Log, TEXT("[Identity] RTA Disconnected coming from connected state"));
					identity->GetDungeonsOnlineSubsystem()->QueueAsyncTask([identity, connectionState]() {
						identity->GetDungeonsOnlineSubsystem()->GetSessionInterface()->TriggerOnSessionFailureDelegates(FUniqueNetIdString("NotConnectedToOnlineService"), ESessionFailure::ServiceConnectionLost);
						identity->SetRTAEvent(connectionState);
						identity->UnregisterInviteHandlerForUser(identity->GetFirstLocalUserNumber());
						identity->UnregisterStatChangeHandlerForUser(identity->GetFirstLocalUserNumber());
					});
				}
				break;
			}
	}, identity);

	const auto user = std::make_shared<XBLUserInfo>(
		handle,
		localUserNum,
		userId,
		localUserNum, // SystemId assumed to be localusernum on other platforms than xbox
		context,
		gamerTag,
		socialGroup,
		inviteHandle,
		rtaStateHandle,
		statChangeHandle
	);

	/* AB#334163 - special case for handling when gamertag is being changed. */
	auto Task = AsyncTasks::CreateAsyncBlock([user](XAsyncBlock* AsyncBlock)
	{
		XblUserProfile Profile;
		bool Success = HrSuccess(XblProfileGetUserProfileResult(AsyncBlock, &Profile), "[Profile] XblProfileGetUserProfileResult");
		if (Success)
		{
			user->SetGamerscore(Profile.gamerscore);
			user->SetDisplayPictureUri(Profile.gameDisplayPictureResizeUri);
		}
		else
		{
			user->SetGamerscore("");
			user->SetDisplayPictureUri("");
		}
		if (Success && user->GetGamerTag() != Profile.gamertag)
		{
			UE_LOG_ONLINE(Log, TEXT("[Identity] Gamertag has changed from '%s' to '%s'"), *user->GetGamerTag(), *FString(Profile.gamertag));
			user->SetGamerTag(Profile.gamertag);
		}
	}, identity->GetDungeonsOnlineSubsystem()->GetQueueHandle());

	HrLogOnlyError(XblProfileGetUserProfileAsync(context, user->UserId, Task), "[Profile] XblProfileGetUserProfileAsync");

	return user;
#endif
}

}

XBLUserInfo::XBLUserInfo(XblUserHandle handle, uint32 localUserNum, uint64_t userId, uint64_t systemId, XblContextHandle context, FString gamerTag,
	XblSocialManagerUserGroup* socialGroup, XblFunctionContext inviteHandle, XblFunctionContext rtaStateHandle, XblFunctionContext statChangeHandle) :
	XBLUser(handle),
	LocalUserNum(localUserNum),
	UserId(userId),
	SystemId(systemId),
	UniqueNetId(MakeShareable(new FUniqueNetIdDungeons(userId))),
	XBLcontext(context),
	SocialGroup(socialGroup),
	InviteHandle(inviteHandle),
	RtaStateHandle(rtaStateHandle),
	StatChangeHandle(statChangeHandle)
{
	SetGamerTag(gamerTag);
}

XBLUserInfo::~XBLUserInfo() {

	if (!XalUserIsGuest(XBLUser))
	{
#if !PLATFORM_XBOXONE
		if (InviteHandle != 0)
		{
			XblGameInviteRemoveNotificationHandler(XBLcontext, InviteHandle);
		}
		if (RtaStateHandle != 0)
		{
			HrLog(XblRealTimeActivityRemoveConnectionStateChangeHandler(XBLcontext, RtaStateHandle), "[Identity] XblRealTimeActivityRemoveConnectionStateChangeHandler");
		}

#endif
		if (StatChangeHandle != 0)
		{
			XblUserStatisticsRemoveStatisticChangedHandler(XBLcontext, StatChangeHandle);
		}

		HrLog(XblSocialManagerDestroySocialUserGroup(SocialGroup), "[Identity] XblSocialManagerDestroySocialUserGroup");
		HrLog(XblSocialManagerRemoveLocalUser(XBLUser), "[Identity] XblSocialManagerRemoveLocalUser");
	}

	XblContextCloseHandle(XBLcontext);
	XalUserCloseHandle(XBLUser);
}

FString XBLUserInfo::GetGamerTag() const
{
	return GamerTag;
}

void XBLUserInfo::SetGamerTag(const FString& InGamerTag)
{
	this->GamerTag = InGamerTag;
}

FString XBLUserInfo::GetGamerscore() const
{
	return Gamerscore;
}

void XBLUserInfo::SetGamerscore(const FString& InGamerscore)
{
	this->Gamerscore = InGamerscore;
}

FString XBLUserInfo::GetDisplayPictureUri() const
{
	return DisplayPictureUri;
}

void XBLUserInfo::SetDisplayPictureUri(const FString& InDisplayPictureUri)
{
	this->DisplayPictureUri = InDisplayPictureUri;
}

std::shared_ptr<const XBLUserInfo> FOnlineIdentityDungeons::GetUser(std::function<bool(const XBLUserInfo&)> predicate) const {
	for (const auto& entry : LocalUsers) {
		if (predicate(*entry.second))
			return entry.second;
	}
	return {};
}

std::shared_ptr<XBLUserInfo> FOnlineIdentityDungeons::GetMutableUser(int32 LocalUserNum) const
{
	for (auto& userpair : LocalUsers) {
		if (userpair.first == LocalUserNum)
			return userpair.second;
	}
	return {};
}

std::shared_ptr<const XBLUserInfo> FOnlineIdentityDungeons::GetUserFromHandle(XblUserHandle handle) const {
	return GetUser([handle](const XBLUserInfo& user) { return user.XBLUser == handle;});
}

FOnlineIdentityDungeons::FOnlineIdentityDungeons(FOnlineSubsystemDungeons* InDungeonsSubsystem)
	: RTAEvent(XblRealTimeActivityConnectionState::Disconnected), DungeonsSubsystem(InDungeonsSubsystem), AsyncInitialized(false), signInPending(false)
{
	HCTraceSetTraceToDebugger(!environmentConfig::IsShippingMode());
	DungeonsAuthToken Token("GET", OnlineSubsystemConstants::MinecraftServicesUrl);
	AuthTokens[ServiceTokenId] = std::move(Token);
}

FOnlineIdentityDungeons::~FOnlineIdentityDungeons()
{
	LocalUsers.clear();

	HrLog(XblCleanupAsync(AsyncTasks::CreateAsyncBlock([](XAsyncBlock*){}, nullptr)), "[Identity] Cleanup xbl async");
	HrLog(XalCleanupAsync(AsyncTasks::CreateAsyncBlock([](XAsyncBlock* asyncBlock) {
		HrLog(XalCleanupResult(asyncBlock), "[Identity] Xal cleanup result");
	}, nullptr)), "[Identity] Cleanup xal async");

	if (Peer2Peer)
	{
		Peer2Peer = {};
	}
#if defined(HAS_MSA_LINKING_DELEGATES)
	if (MsaCodeReceivedHandle.IsValid())
		FCoreDelegates::OnMsaLinkingRequired.Remove(MsaCodeReceivedHandle);
#endif
}

std::shared_ptr<const XBLUserInfo> FOnlineIdentityDungeons::GetUserFromLocalUser(int32 LocalUserNum) const {
	return GetUser([LocalUserNum](const XBLUserInfo& user) { return user.LocalUserNum == LocalUserNum;});
}

std::shared_ptr<const XBLUserInfo> FOnlineIdentityDungeons::GetUserFromNetId(const FUniqueNetId& userNetId) const {
	return GetUser([netId = userNetId.ToString()](const XBLUserInfo& user) { return user.UniqueNetId->ToString() == netId;});
}

std::shared_ptr<const XBLUserInfo> FOnlineIdentityDungeons::GetUserFromXuid(uint64_t xuid) const {
	return GetUser([xuid](const XBLUserInfo& user) { return user.UserId == xuid;});
}

void FOnlineIdentityDungeons::UpdateServiceToken(int32 LocalUserNum) {
	if (const auto user = GetUserFromLocalUser(LocalUserNum)) {
#if PLATFORM_WINDOWS
		GetServiceToken().setDirty(true); //J.E TEMPORARY SOLUTION until we patch xsapi: AB#459912
#endif
		UpdateAuthToken(GetServiceToken(), user->XBLUser, [this, localUserNum = LocalUserNum, netId = *user->UniqueNetId](HRESULT result, const XalUserGetTokenAndSignatureData* tokenAndSignature) {
			if (result == E_XAL_UIREQUIRED)	{
				UE_LOG_ONLINE(Warning, TEXT("[Identity] Token check indicates UI is required to solve an auth issue, logging out"));
				Logout(localUserNum);
			}		
			else if (HrSuccess(result, "[Identity] Updating service token")) {
				SetServiceToken(netId, tokenAndSignature->token);
			}
		});
	}
}

#ifdef EPIC_STORE_BUILD

ANTICHEAT_NO_OPTIMIZATION_BEGIN
void FOnlineIdentityDungeons::UpdateEpicGameServicesToken(int32 LocalUserNum) {
	ANTICHEAT_VIRT_PROTECT_STRINGS_BEGIN;
	if (AuthTokens.count(minecraft::epicstore::EpicGameServicesTokenId) == 0)
		return;
	if (const auto user = GetUserFromLocalUser(LocalUserNum)) {
		UpdateAuthToken(GetEpicGameServicesToken(), user->XBLUser, [this, localUserNum = LocalUserNum, netId = *user->UniqueNetId](HRESULT result, const XalUserGetTokenAndSignatureData* tokenAndSignature) {
			if (result == E_XAL_UIREQUIRED) {
				UE_LOG_ONLINE(Warning, TEXT("[Identity] Token check indicates UI is required to solve an auth issue, logging out"));
				Logout(localUserNum);
			}
			else if (HrSuccess(result, "[Identity] Updating Epic Game Services token (XBL)")) {
				SetEpicGameServicesToken(tokenAndSignature->token);
			}
			else {
				UE_LOG_ONLINE(Warning, TEXT("[Identity] Failed to refresh the XBL token used for the Epic Game Services."));
			}
		});
	}
	ANTICHEAT_VIRT_PROTECT_STRINGS_END;
}

DungeonsAuthToken &FOnlineIdentityDungeons::GetEpicGameServicesToken() {
	return AuthTokens[minecraft::epicstore::EpicGameServicesTokenId];
}

void FOnlineIdentityDungeons::SetEpicGameServicesToken(const char* token) {
	ANTICHEAT_VIRT_PROTECT_STRINGS_BEGIN;
	using namespace minecraft::epicstore;
	auto &EpicGameServicesToken = GetEpicGameServicesToken();
	static FEpicServicesModule &EosModule = FEpicServicesModule::Get();
	EosModule.SignIntoEpicGameServices(EpicGameServicesToken.isCurrentToken(token), token);
	EpicGameServicesToken.set(token);
	ANTICHEAT_VIRT_PROTECT_STRINGS_END;
}
ANTICHEAT_NO_OPTIMIZATION_END

void FOnlineIdentityDungeons::ClearEpicGameServicesToken()
{
	using namespace minecraft::epicstore;
	AuthTokens.erase(EpicGameServicesTokenId);
	FEpicServicesModule &EosModule = FEpicServicesModule::Get();
	EosModule.LogoutOfEpicGameServices();
}
#endif //EPIC_STORE_BUILD

namespace internal
{
	void TriggerTokenCallbacks(FString key, FString token, std::map<FString, std::list<TFunction<void(FString, FString)>>>& tokenQueue)
	{
		auto callbacks = tokenQueue.find(key);
		if (callbacks != tokenQueue.end())
		{
			for(auto& callback : callbacks->second)
			{
				callback(token, "");
			}
			callbacks->second.clear();
		}
	}
	
}

void FOnlineIdentityDungeons::UpdateAuthToken(int32 LocalUserNum, DungeonsAuthToken &token) {
	if (const auto user = GetUserFromLocalUser(LocalUserNum)) {
		UpdateAuthToken(token, user->XBLUser, [this, &token, GamerTag = user->GetGamerTag()](HRESULT result, const XalUserGetTokenAndSignatureData* tokenAndSignature) {
			if (HrSuccess(result, "[Identity] Updating autho token")) {
				token.set(tokenAndSignature->token);
				UE_LOG_ONLINE(Warning, TEXT("[Identity] atho token updated successfully for %s"), *GamerTag);
				if (PlayfabServices::GetInitState() == PlayfabServicesInitState::NotInitialized) {
					UE_LOG_ONLINE(Warning, TEXT("Mojang Services not intialized. Can't update auth token."));
				}
				else {
#if defined(SUBSYSTEM_HAS_AUTH_TOKEN_FUNCTIONS)
					internal::TriggerTokenCallbacks("playfab_key", token.getToken(), TokenQueue);
#endif
				}
			}
			else
			{
				UE_LOG_ONLINE(Warning, TEXT("[Identity] atho token failed to refresh for %s"), *GamerTag);
			}
		});
	}
}

void FOnlineIdentityDungeons::OnMsaCodeReceived(const FString&, const FString&)
{
	MsaLinkCode.reset(new DungeonsMsaLinkCode());
	UE_LOG_ONLINE(Log, TEXT("[Identity] New MSA code received"));
}

void FOnlineIdentityDungeons::RefreshAuthTokens(float DeltaTime)
{
	auto LocalUserNum = GetFirstLocalUserNumber();
	for (auto& tokenPair : AuthTokens)
	{
		DungeonsAuthToken &token = tokenPair.second;
		token.updateTimer(DeltaTime);
		if (token.hasExpired()) {

			//D11.PS Special case where the auth token is the service token.
			if (tokenPair.first == ServiceTokenId)
			{
				UpdateServiceToken(LocalUserNum);
			}
#ifdef EPIC_STORE_BUILD
			else if (tokenPair.first == minecraft::epicstore::EpicGameServicesTokenId)
			{
				UpdateEpicGameServicesToken(LocalUserNum);
			}
#endif
			else
			{
				UpdateAuthToken(LocalUserNum, token);
			}
			token.resetTimer();
		}
	}
}

void FOnlineIdentityDungeons::RefreshMsaCode(float DeltaTime)
{
	bool awaitingMsaLink = MsaLinkCode && signInPending;
	if (awaitingMsaLink)
	{
		MsaLinkCode->UpdateTimer(DeltaTime);
		if (MsaLinkCode->HasExpired() && !MsaLinkCode->SignInRestarted)
		{
			UE_LOG_ONLINE(Log, TEXT("[Identity] MSA Link code has expired. Requesting a new code and restarting the Sign In."));
#if defined(HAS_MSA_LINKING_DELEGATES)
			FCoreDelegates::OnMsaLinkingCancelled.Broadcast();
#endif
			SignInWithUI(0);
			MsaLinkCode->SignInRestarted = true;
		}
	}
}

DungeonsAuthToken &FOnlineIdentityDungeons::GetServiceToken()
{
	return AuthTokens[ServiceTokenId];
}

FOnlineSubsystemDungeons* FOnlineIdentityDungeons::GetDungeonsOnlineSubsystem() const {
	return DungeonsSubsystem;
}

int32 FOnlineIdentityDungeons::GetFirstLocalUserNumber() const
{
	if (!LocalUsers.empty()) {
		return LocalUsers.begin()->first;
	}
	else {
		return -1;
	}
}

XblRealTimeActivityConnectionState FOnlineIdentityDungeons::GetPreviousRTAEvent() const
{
	return RTAEvent;
}

void FOnlineIdentityDungeons::SetRTAEvent(XblRealTimeActivityConnectionState RTAConnectionState)
{
	RTAEvent = RTAConnectionState;
}

bool FOnlineIdentityDungeons::Tick(float DeltaTime)
{
	if (!AsyncInitialized)
	{
		return false;
	}

	PlatformTick(DeltaTime);

	if (Peer2Peer)
	{
		Peer2Peer->Tick(DeltaTime);
	}

	//Get interfaces
	IOnlineFriendsPtr Friends = DungeonsSubsystem->GetFriendsInterface();
	IOnlinePresencePtr Presence = DungeonsSubsystem->GetPresenceInterface();

	const XblSocialManagerEvent* SocialEvents = nullptr;
	size_t SocialEventCount = 0;
	if (FAILED(HrLogOnlyError(XblSocialManagerDoWork(&SocialEvents, &SocialEventCount), "[Identity] Social manager do work")))
	{
		return false;
	}

	for (uint32_t i = 0; i < SocialEventCount; i++)
	{
		const auto& socialEvent = SocialEvents[i];
		xsapi::LogEvent(socialEvent);
		if (FAILED(socialEvent.hr)) {
			GetServiceToken().setDirty(true);
		}

		switch (socialEvent.eventType)
		{
			case XblSocialManagerEventType::LocalUserAdded:
			{
				break;
			}
			case XblSocialManagerEventType::UsersAddedToSocialGraph:
			{
				UE_LOG_ONLINE(Log, TEXT("[Identity] Users added to social graph"));
				Friends->TriggerOnFriendsChangeDelegates(0);
				break;
			}
			case XblSocialManagerEventType::UsersRemovedFromSocialGraph:
			{
				UE_LOG_ONLINE(Log, TEXT("[Identity] Users removed from social graph"));
				Friends->TriggerOnFriendsChangeDelegates(0);
				break;
			}

			case XblSocialManagerEventType::SocialUserGroupLoaded:
			{

#if !PLATFORM_XBOXONE //D11.TODO_XSAPI XblSocialManagerUserGroupGetUsers Crashes on XB1
				const auto user = GetUserFromHandle(socialEvent.user);
				if (!user) {
					UE_LOG_ONLINE(Error, TEXT("[Identity] Could not find user when updating presence."));
					break;
				}

				const XblSocialManagerUser* const* Users = nullptr;
				size_t UsersCount = 0;
				HrLog(XblSocialManagerUserGroupGetUsers(user->SocialGroup, &Users, &UsersCount), "[Identity] Social manager get group users");

				// D11.AH - Not Used ATM -XblUserHandle XboxUser = SocialEvents[i].user;

				for (uint32_t UserIndex = 0; UserIndex < UsersCount; UserIndex++)
				{
					UE_LOG_ONLINE(Log, TEXT("[Identity] '%s', State='%s'")
						, ANSI_TO_TCHAR(Users[UserIndex]->displayName)
						, *ConvertXboxLiveUserPresenceStateToString(Users[UserIndex]->presenceRecord.userState));
				}
#endif
				Friends->ReadFriendsList(0, "", FOnReadFriendsListComplete());
				Friends->TriggerOnFriendsChangeDelegates(0);
				break;
			}
			case XblSocialManagerEventType::PresenceChanged:
			{
				if (Presence.IsValid())
				{
					Presence->TriggerOnPresenceReceivedDelegates(FUniqueNetIdDungeons(0), MakeShared<FOnlineUserPresence>(FOnlineUserPresence()));
					Friends->TriggerOnFriendsChangeDelegates(0);
				}
				break;
			}
			default:
			{
				UE_LOG_ONLINE(Log, TEXT("[Identity] Unkown social_event_type"));
				break;
			}
		}
	}

	RefreshMsaCode(DeltaTime);

	if (GetLoginStatus(GetFirstLocalUserNumber()) != ELoginStatus::LoggedIn) {
		return false;
	}

	RefreshAuthTokens(DeltaTime);
	return true;
}

bool FOnlineIdentityDungeons::AuthenticateLocally(int32 LocalUserNum, XblUserHandle UserHandle)
{
	auto userInfo = dungeonsIdentity::createUser(this, UserHandle, LocalUserNum);
	LocalUsers[LocalUserNum] = userInfo;
	ChangeUserInfo(userInfo);
	FetchAuthTokenAndCompleteLogin(LocalUserNum);

	UE_LOG_ONLINE(Log, TEXT("[Identity] Gamertag='%s'"), *userInfo->GetGamerTag());

	char tempXUID[21];
#if PLATFORM_PS4
	snprintf(tempXUID, 21, "%lu", userInfo->UserId);
#else
	snprintf(tempXUID, 21, "%llu", userInfo->UserId);
#endif
	if (!Peer2Peer)
	{
		Peer2Peer = MakeUnique<Peer2PeerManager>(FString(tempXUID), DungeonsSubsystem);
	}

	UE_LOG_ONLINE(Log, TEXT("[Identity] Gamertag='%s'"), *userInfo->GetGamerTag());
	return true;
}

void FOnlineIdentityDungeons::ChangeUserInfo(std::shared_ptr<XBLUserInfo>& UserToChange)
{
	
}

ANTICHEAT_NO_OPTIMIZATION_BEGIN
void FOnlineIdentityDungeons::SignIn(int32 LocalUserNum)
{
    ANTICHEAT_VIRT_PROTECT_STRINGS_BEGIN;

	if (signInPending) {
		UE_LOG_ONLINE(Warning, TEXT("[Identity] Sign in already in progress"));
		return;
	}
	signInPending = true;
	HRESULT hr = XalTryAddDefaultUserSilentlyAsync(nullptr, AsyncTasks::CreateAsyncBlock([this, LocalUserNum](XAsyncBlock* asyncBlock)
	{
        ANTICHEAT_VIRT_PROTECT_STRINGS_BEGIN;

		XblUserHandle newUser = nullptr;
		HRESULT res = HrLog(XalTryAddDefaultUserSilentlyResult(asyncBlock, &newUser), "[Identity] Attempt to silently signin");
		if (SUCCEEDED(res))
		{
			UE_LOG_ONLINE(Log, TEXT("[Identity] User authenticated silently"));
			bool ok = AuthenticateLocally(LocalUserNum, newUser);
			check(ok && "We might need to handle this");
		}
		else if (res == E_XAL_USERSETFULL || res == E_XAL_USERSETNOTEMPTY)
		{
			//This can happen after failed logout which is only accessed by console command
			SignInWithUI(LocalUserNum);
		}
		else
		{
			SignInWithUI(LocalUserNum);
		}

        ANTICHEAT_VIRT_PROTECT_STRINGS_END;
    }, DungeonsSubsystem->GetQueueHandle()
	));
	HrLog(hr, "[Identity] Add default user silently async");

    ANTICHEAT_VIRT_PROTECT_STRINGS_END;
}

void FOnlineIdentityDungeons::SignInWithUIError(FString LogString, FString ErrorString)
{
	UE_LOG_ONLINE(Log, TEXT("[Identity] %s"), *LogString);
	DungeonsSubsystem->QueueAsyncTask([this, ErrorString]() {
		DungeonsSubsystem->GetExternalUIDungeons()->TriggerUIClosedDelegate(-1, false, FUniqueNetIdDungeons(0), ErrorString);
	});
	signInPending = false;
}

void FOnlineIdentityDungeons::SignInWithUI(int32 LocalUserNum)
{
	ANTICHEAT_VIRT_PROTECT_STRINGS_BEGIN;

	signInPending = true;
	UE_LOG_ONLINE(Log, TEXT("[Identity] Providing UI to user to login"));
	const auto hr = XalAddUserWithUiAsync(nullptr, AsyncTasks::CreateAsyncBlock([this, LocalUserNum](XAsyncBlock* asyncBlock)
	{
		ANTICHEAT_VIRT_PROTECT_STRINGS_BEGIN;

		DungeonsSubsystem->GetExternalUIDungeons()->OnRedirectUrl.Unbind();
		XblUserHandle newUser = nullptr;
		HRESULT res = HrLog(XalAddUserWithUiResult(asyncBlock, &newUser), "Add user with UI result");
		
		bool msaCodeExpired = MsaLinkCode && MsaLinkCode->HasExpired();
		MsaLinkCode.release();
		if (msaCodeExpired) {
			/* silently cancel the sign in to start a new one */
			UE_LOG_ONLINE(Log, TEXT("[Identity] Refreshing the MSA code, silently cancelling the Async Sign In."));
			return;
		}

		if (res == S_OK && GetUserFromHandle(newUser)) {
			SignInWithUIError("User already signed in", LoginFlowConstants::AlreadyLoggedInUserError);
		}
		else if (res == S_OK) {
			UE_LOG_ONLINE(Log, TEXT("[Identity] User authenticated via UI)"));
			AuthenticateLocally(LocalUserNum, newUser);
		}
		else if (res == E_ABORT) {
			SignInWithUIError("User authentication aborted", LoginFlowConstants::SignInCancelled);
		}
		else if (res == E_XAL_NETWORK) {
			SignInWithUIError("Xal reported network connectivity issues", LoginFlowConstants::NetworkIssueError);
		} 
		else if (res == E_XAL_UIREQUIRED) {
			SignInWithUIError("Xal reported player is signed in on another device: SPOP issue", LoginFlowConstants::SpopIssue);
		}
		else {
			SignInWithUIError("Failed to authenticate user", LoginFlowConstants::FailedMSALoginError);
		}

		ANTICHEAT_VIRT_PROTECT_STRINGS_END;
	}, DungeonsSubsystem->GetQueueHandle()));
	HrLog(hr, "[Identity] Add user with UI async");

	ANTICHEAT_VIRT_PROTECT_STRINGS_END;
}

void FOnlineIdentityDungeons::UpdateAuthToken(DungeonsAuthToken& token, XblUserHandle userHandle, OnUpdateAuthTokenComplete onComplete) {
    ANTICHEAT_VIRT_PROTECT_STRINGS_BEGIN;

	std::string method = TCHAR_TO_ANSI(*token.getMethod());
	std::string url = TCHAR_TO_ANSI(*token.getUrl());
	const bool forceRefresh = token.isDirty();

#if PLATFORM_XBOXONE

	//D11.SC This seems to be blocking the main thread on XBOne, lets push it off to a concurrency
	concurrency::create_task([this, method, url, forceRefresh, userHandle, &token, onComplete]
	{
		XalUserGetTokenAndSignatureArgs tokenArgs = {};
		tokenArgs.method = method.c_str();
		tokenArgs.url = url.c_str();
		tokenArgs.forceRefresh = forceRefresh;
		tokenArgs.allUsers = false;


		XAsyncBlock* pAsyncBlock = AsyncTasks::CreateAsyncBlock([&token, forceRefresh, onComplete](XAsyncBlock* asyncBlock)
			{
				ANTICHEAT_VIRT_PROTECT_STRINGS_BEGIN;

				size_t bufferSize = 0;
				HrLog(XalUserGetTokenAndSignatureSilentlyResultSize(asyncBlock, &bufferSize), "[Identity] XalUserGetTokenAndSignatureSilentlyResultSize");
				std::vector<uint8_t> buffer;
				buffer.resize(bufferSize);
				XalUserGetTokenAndSignatureData* tokenAndSig = nullptr;
				const HRESULT result = HrLog(XalUserGetTokenAndSignatureSilentlyResult(asyncBlock, bufferSize, buffer.data(), &tokenAndSig, nullptr), "[Identity] Get token and signature silently");
				onComplete(result, tokenAndSig);
				if (forceRefresh) {
					token.setDirty(false);
				}

				ANTICHEAT_VIRT_PROTECT_STRINGS_END;
			}, DungeonsSubsystem->GetQueueHandle());

		
		const auto hr = XalUserGetTokenAndSignatureSilentlyAsync(userHandle, &tokenArgs, pAsyncBlock);

		HrLog(hr, "[Identity] Update auth token and signature async");
	});
#else
	
	XalUserGetTokenAndSignatureArgs tokenArgs = {};

	tokenArgs.method = method.c_str();
	tokenArgs.url = url.c_str();
	tokenArgs.forceRefresh = forceRefresh;
	tokenArgs.allUsers = false;

	const auto hr = XalUserGetTokenAndSignatureSilentlyAsync(userHandle, &tokenArgs,
		AsyncTasks::CreateAsyncBlock([&token, forceRefresh, onComplete](XAsyncBlock* asyncBlock)
	{
		ANTICHEAT_VIRT_PROTECT_STRINGS_BEGIN;

		size_t bufferSize = 0;
		HrLog(XalUserGetTokenAndSignatureSilentlyResultSize(asyncBlock, &bufferSize), "[Identity] XalUserGetTokenAndSignatureSilentlyResultSize");
		std::vector<uint8_t> buffer;
		buffer.resize(bufferSize);
		XalUserGetTokenAndSignatureData* tokenAndSig = nullptr;
		const HRESULT result = HrLog(XalUserGetTokenAndSignatureSilentlyResult(asyncBlock, bufferSize, buffer.data(), &tokenAndSig, nullptr), "[Identity] Get token and signature silently");
		onComplete(result, tokenAndSig);
		if (forceRefresh) {
			token.setDirty(false);
		}

		ANTICHEAT_VIRT_PROTECT_STRINGS_END;
	}, DungeonsSubsystem->GetQueueHandle()));
	HrLog(hr, "[Identity] Update auth token and signature async");

#endif


    ANTICHEAT_VIRT_PROTECT_STRINGS_END;
}

void FOnlineIdentityDungeons::SetServiceToken(const FUniqueNetId& netId, const char* token) {
	ANTICHEAT_OBFUSCATE_BEGIN
	
	auto &ServicesToken = GetServiceToken();
	if (!ServicesToken.isCurrentToken(token)) {
		if (IDungeonsAuth::IsAvailable()) {
			auto auth = IDungeonsAuth::Get().Auth();

			if (!MinecraftApiAuthRetryDelegateHandle.IsValid()) {				
				MinecraftApiAuthRetryDelegateHandle = auth->OnRetryClientAuthentication.AddLambda([=]() {
					SetDirtyServiceToken(true);
				});
			}
		}
	}
	ServicesToken.set(token);

#if defined(SUBSYSTEM_HAS_AUTH_TOKEN_FUNCTIONS)
	internal::TriggerTokenCallbacks(ServiceTokenId, token, TokenQueue);
#endif
	ANTICHEAT_OBFUSCATE_END
}

void FOnlineIdentityDungeons::FetchAuthTokenAndCompleteLogin(int32 LocalUserNum) {
    ANTICHEAT_VIRT_PROTECT_STRINGS_BEGIN;

	const auto user = GetUserFromLocalUser(LocalUserNum);
	check(user && "The user doesn't exist when trying to set the auth token.");
	
	UpdateAuthToken(GetServiceToken(), user->XBLUser, [this, LocalUserNum = user->LocalUserNum, netId = *user->UniqueNetId](HRESULT result, const XalUserGetTokenAndSignatureData* tokenAndSig) {
        ANTICHEAT_VIRT_PROTECT_STRINGS_BEGIN;

		bool success = true;
		FString errorMessage;
		if (result == E_XAL_UIREQUIRED)	{
			UE_LOG_ONLINE(Warning, TEXT("[Identity] Token check indicates UI is required to solve an auth issue, logging out"));
			Logout(LocalUserNum);
			return;
		}
		if (FAILED(result))	{
			UE_LOG_ONLINE(Warning, TEXT("[Identity] Getting auth token result for minecraft services failed"));
			success = ShouldSuccessCallbackOnAuthTokenFailure();

			if (!success && result == E_XAL_NETWORK) {
				UE_LOG_ONLINE(Warning, TEXT("[Identity] Network issue when fetching auth token"));
				errorMessage = LoginFlowConstants::NetworkIssueError;
			}
		}
		else {
			SetServiceToken(netId, tokenAndSig->token);
		}

		if (signInPending) {
			DungeonsSubsystem->QueueAsyncTask([this, LocalUserNum, success, netId, errorMessage]() {
				TriggerOnLoginCompleteDelegates(LocalUserNum, success, netId, errorMessage);
			});
			signInPending = false;
		}

        ANTICHEAT_VIRT_PROTECT_STRINGS_END;
    });
#ifdef EPIC_STORE_BUILD
	UpdateEpicGameServicesToken(LocalUserNum);
#endif
    ANTICHEAT_VIRT_PROTECT_STRINGS_END;
}

bool FOnlineIdentityDungeons::ShouldSuccessCallbackOnAuthTokenFailure()
{
	return false;
}

//IOnlineIdentity
bool FOnlineIdentityDungeons::Login(int32 LocalUserNum, const FOnlineAccountCredentials& AccountCredentials) {
    ANTICHEAT_VIRT_PROTECT_STRINGS_BEGIN;

	if (GetLoginStatus(LocalUserNum) == ELoginStatus::LoggedIn)
	{
		UE_LOG_ONLINE(Warning, TEXT("[Identity] Failed to login, LocalUserNum '%d' already signed in"), LocalUserNum);
#ifdef WITH_EDITOR
		TriggerOnLoginCompleteDelegates(LocalUserNum, true, *GetUniquePlayerId(LocalUserNum), "");
#endif
		return true;
	}

#if PLATFORM_SWITCH
	FSwitchPlatformMisc::AllowNifm();
	DungeonsSubsystem->QueueAsyncTask(MakeUnique<SwitchUtil::FDungeonsSwitchNSACheck>(LocalUserNum, [this, LocalUserNum](bool success, bool offline)
	{
		if (success)
		{
			SignIn(LocalUserNum);
		}
		else
		{
			DungeonsSubsystem->QueueAsyncTask([this, LocalUserNum, offline]() {
				TriggerOnLoginCompleteDelegates(LocalUserNum, false, FUniqueNetIdDungeons(0), LoginFlowConstants::FailedMSALoginError);
			});
		}
		FSwitchPlatformMisc::DisallowNifm();
	}));
#else
	SignIn(LocalUserNum);
#endif

	return true;

    ANTICHEAT_VIRT_PROTECT_STRINGS_END;
}


void FOnlineIdentityDungeons::RemoveLocalUser(int32 LocalUserNum) {

	LocalUsers.erase(LocalUserNum);
#if CLOUDSAVE_PLASTERFIX_453486
	if (LocalUsers.empty())
	{
		PlayfabServices::PlayfabCleanup();
		AuthTokens.erase("playfab_key");
	}
#endif
#ifdef EPIC_STORE_BUILD
	ClearEpicGameServicesToken();
#endif
}

bool FOnlineIdentityDungeons::Logout(int32 LocalUserNum) {
    ANTICHEAT_VIRT_PROTECT_STRINGS_BEGIN;

	if (GetLoginStatus(LocalUserNum) == ELoginStatus::NotLoggedIn)
	{
		UE_LOG_ONLINE(Warning, TEXT("[Identity] Failed to logout, LocalUserNum '%d' not signed in"), LocalUserNum);
		return false;
	}

	const auto user = GetUserFromLocalUser(LocalUserNum);
	DungeonsSubsystem->GetDungeonsAchievementsInterface()->RemoveAchievementsForPlayer(*user->UniqueNetId);
	DungeonsSubsystem->GetDungeonsStatsInterface()->RemoveStatsForUser(user->UniqueNetId.ToSharedRef());

	return SignOut(LocalUserNum);

    ANTICHEAT_VIRT_PROTECT_STRINGS_END;
}

bool FOnlineIdentityDungeons::SignOut(int32 LocalUserNum)
{
    ANTICHEAT_VIRT_PROTECT_STRINGS_BEGIN;

#if !PLATFORM_XBOXONE
	check(XalSignOutUserAsyncIsPresent() && "XalSignOutUserAsync is not present!");
#endif

		
	const auto user = GetUserFromLocalUser(LocalUserNum);
	if (!user) {
		UE_LOG_ONLINE(Error, TEXT("[Identity] Failed to logout, LocalUserNum '%d' is not found among the local users"), LocalUserNum);
		return false;
	}

	const auto res = XalSignOutUserAsync(user->XBLUser, AsyncTasks::CreateAsyncBlock([this, user] (XAsyncBlock* asyncBlock)
	{
        ANTICHEAT_VIRT_PROTECT_STRINGS_BEGIN;

		if (GetLoginStatus(user->LocalUserNum) == ELoginStatus::NotLoggedIn)
		{
			UE_LOG_ONLINE(Warning, TEXT("[Identity] Failed to logout, LocalUserNum '%d' not signed in"), user->LocalUserNum);
			return;
		}
		HrLog(XAsyncGetStatus(asyncBlock, false), "[Identity] User signed out");

		DungeonsSubsystem->GetSessionInterface()->DestroySession(DungeonsGameSessionName);
		DungeonsSubsystem->QueueAsyncTask([this, user]() {
			TriggerOnLoginStatusChangedDelegates(user->LocalUserNum, ELoginStatus::LoggedIn, ELoginStatus::NotLoggedIn, *user->UniqueNetId);
		});

		TryReleasePeer2Peer(user->LocalUserNum);
		RemoveLocalUser(user->LocalUserNum);


		// Need to reset the multiplayer manager to clear some state, see ADO tickets #294575 and #297850 for more info
		HrCheck(XblMultiplayerManagerInitialize(OnlineSubsystemConstants::DungeonsLobbySessionName.c_str(), DungeonsSubsystem->GetQueueHandle()), "[Identity] Resetting multiplayer manager after successful logout");
		SetRTAEvent(XblRealTimeActivityConnectionState::Disconnected);

        ANTICHEAT_VIRT_PROTECT_STRINGS_END;
    }, DungeonsSubsystem->GetQueueHandle()));

	HrLog(res, "[Identity] XalSignOutUserAsync");

	return true;
    ANTICHEAT_VIRT_PROTECT_STRINGS_END;
}

ANTICHEAT_NO_OPTIMIZATION_END

bool FOnlineIdentityDungeons::AutoLogin(int32 LocalUserNum) {
	ANTICHEAT_PROTECT_STRINGS_BEGIN
	UE_LOG_ONLINE(Log, TEXT("[Identity] Autologin triggered"));
	ANTICHEAT_PROTECT_STRINGS_END
	return true;
}

TSharedPtr<FUserOnlineAccount> FOnlineIdentityDungeons::GetUserAccount(const FUniqueNetId& UserId) const {
	return TSharedPtr<FUserOnlineAccount>();
}

TArray<TSharedPtr<FUserOnlineAccount> > FOnlineIdentityDungeons::GetAllUserAccounts() const {
	return TArray<TSharedPtr<FUserOnlineAccount>>();
}

TSharedPtr<const FUniqueNetId> FOnlineIdentityDungeons::GetUniquePlayerId(int32 LocalUserNum) const {
	const auto user = GetUserFromLocalUser(LocalUserNum);

	//D11.KS - If a guest account is created on Xbox when disconnected from the internet it will never have a valid net id until the account is completely removed from the system, so we create a fake net id for them.
#if PLATFORM_XBOXONE
	if(user && user->UniqueNetId.IsValid() && !user->UniqueNetId->IsValid())
	{
		return MakeShared<FUniqueNetIdDungeons>(FCrc::MemCrc32(*user->GetGamerTag(), user->GetGamerTag().Len() * sizeof(TCHAR)));
	}
#endif

	return user ? user->UniqueNetId : nullptr;
}

TSharedPtr<const FUniqueNetId> FOnlineIdentityDungeons::CreateUniquePlayerId(uint8* Bytes, int32 Size) {
	if (Bytes && Size == sizeof(uint64))
	{
		uint64* RawUniqueId = reinterpret_cast<uint64*>(Bytes);
		return MakeShared<FUniqueNetIdDungeons>(*RawUniqueId);
	}
	return nullptr;
}

TSharedPtr<const FUniqueNetId> FOnlineIdentityDungeons::CreateUniquePlayerId(const FString& Str) {
	return MakeShared<FUniqueNetIdDungeons>(std::stoull(TCHAR_TO_ANSI(*Str)));
}

ELoginStatus::Type FOnlineIdentityDungeons::GetLoginStatus(int32 LocalUserNum) const {
	return GetUserFromLocalUser(LocalUserNum) ? ELoginStatus::LoggedIn : ELoginStatus::NotLoggedIn;
}

ELoginStatus::Type FOnlineIdentityDungeons::GetLoginStatus(const FUniqueNetId& UserId) const {
	return GetUserFromNetId(UserId) ? ELoginStatus::LoggedIn : ELoginStatus::NotLoggedIn;
}

FString FOnlineIdentityDungeons::GetPlayerNickname(int32 LocalUserNum) const {
	const auto& user = GetUserFromLocalUser(LocalUserNum);
#if PLATFORM_SWITCH
	if (!user)
	{
		return FSwitchPlatformMisc::GetPlayerSwitchName(LocalUserNum);
	}

#endif
	return user ? user->GetGamerTag() : "";
}

FString FOnlineIdentityDungeons::GetPlayerNickname(const FUniqueNetId& UserId) const {
	const auto& user = GetUserFromNetId(UserId);
	return user ? user->GetGamerTag() : "";
}

FString FOnlineIdentityDungeons::GetAuthToken(int32 LocalUserNum) const {
	const auto it = AuthTokens.find(ServiceTokenId);
	return it->second.getToken();
}

#ifdef SUBSYSTEM_HAS_AUTH_TOKEN_FUNCTIONS
FString FOnlineIdentityDungeons::GetAuthToken(const FString &key, FString &dummy) const
{
	const auto it = AuthTokens.find(key);
	if (it != AuthTokens.end())
	{
		return it->second.getToken();
	}
	return FString();
}

void FOnlineIdentityDungeons::GetAuthTokenAsync(const FString &key, AuthTokenCallback callback)
{
	FString issuerId;
	FString token = GetAuthToken(key, issuerId);
	if (token.IsEmpty())
	{
		auto it = TokenQueue.find(key);
		if (it != TokenQueue.end())
		{
			it->second.push_back(callback);
		}
		else 
		{
			TokenQueue.insert({key, { callback }});
		}
	}
	else 
	{
		callback(token, issuerId);
	}
}

void FOnlineIdentityDungeons::AddAuthToken(const FString &key, const FString &method, const FString &url)
{
	DungeonsAuthToken token(method, url);
	AuthTokens[key] = std::move(token);

#ifdef EPIC_STORE_BUILD
	if (key == minecraft::epicstore::EpicGameServicesTokenId)
		UpdateEpicGameServicesToken(GetFirstLocalUserNumber());
#else
	UpdateAuthToken(GetFirstLocalUserNumber(), AuthTokens[key]);
#endif // EPIC_STORE_BUILD
}
#endif // SUBSYSTEM_HAS_AUTH_TOKEN_FUNCTIONS

void FOnlineIdentityDungeons::TryReleasePeer2Peer(int32 LocalUserNum)
{
	if (Peer2Peer)
	{
		char tempXUID[21];
#if PLATFORM_PS4
		snprintf(tempXUID, 21, "%lu", LocalUsers[LocalUserNum]->UserId);
#else
		snprintf(tempXUID, 21, "%llu", LocalUsers[LocalUserNum]->UserId);
#endif

		if (Peer2Peer->GetXUID() == tempXUID)
		{
			Peer2Peer = {};
		}
	}
}

void FOnlineIdentityDungeons::GetUserPrivilegeXBL(const FUniqueNetId& UserId, EUserPrivileges::Type Privilege, const FOnGetUserPrivilegeCompleteDelegate& Delegate)
{	
	EPrivilegeResults PrivilegeResult = EPrivilegeResults::GenericFailure;
	XalPrivilege xalPriv = dungeonsIdentity::UnrealToXalPrivilege(Privilege);
	bool hasPrivilege = false;
	XalPrivilegeCheckDenyReasons reasons = XalPrivilegeCheckDenyReasons_None;
	if (const auto& user = GetUserFromNetId(UserId)) {
		const HRESULT res = HrLog(XalUserCheckPrivilege(user->XBLUser, xalPriv, &hasPrivilege, &reasons), "[Identity] GetUserPrivilege for user '" + UserId.ToString() + "'");

		if (reasons == XalPrivilegeCheckDenyReasons::XalPrivilegeCheckDenyReasons_None)
		{
			PrivilegeResult = EPrivilegeResults::NoFailures;
		}
		else if (reasons & static_cast<uint32>(XalPrivilegeCheckDenyReasons::XalPrivilegeCheckDenyReasons_PurchaseRequired))
		{
			PrivilegeResult = EPrivilegeResults::AccountTypeFailure;
		}
		else if ((reasons & static_cast<uint32>(XalPrivilegeCheckDenyReasons::XalPrivilegeCheckDenyReasons_Restricted))
			|| (reasons & static_cast<uint32>(XalPrivilegeCheckDenyReasons::XalPrivilegeCheckDenyReasons_Banned)))
		{
			if (Privilege == EUserPrivileges::CanPlayOnline)
			{
				PrivilegeResult = EPrivilegeResults::OnlinePlayRestricted;
			}
			else if (Privilege == EUserPrivileges::CanCommunicateOnline)
			{
				PrivilegeResult = EPrivilegeResults::ChatRestriction;
			}
			else if (Privilege == EUserPrivileges::CanUseUserGeneratedContent)
			{
				PrivilegeResult = EPrivilegeResults::UGCRestriction;
			}
			else if (Privilege == EUserPrivileges::CanUserCrossPlay)
			{
				PrivilegeResult = EPrivilegeResults::OnlinePlayRestricted;
			}
		}
	}
	else {
		UE_LOG_ONLINE(Log, TEXT("[Identity] No local user found matching %s"), *UserId.ToString());
	}

	if (Delegate.IsBound()) {
		Delegate.Execute(UserId, Privilege, static_cast<uint32>(PrivilegeResult));
	}
}

size_t FOnlineIdentityDungeons::GetLocalUsersNum() const
{
	return LocalUsers.size();
}

void FOnlineIdentityDungeons::GetUserPrivilege(const FUniqueNetId& UserId, EUserPrivileges::Type Privilege, const FOnGetUserPrivilegeCompleteDelegate& Delegate)
{
#if PLATFORM_SWITCH
	FSwitchPlatformMisc::AllowNifm();
	//If we are on switch do the NSA checks first
	DungeonsSubsystem->QueueAsyncTask(MakeUnique<SwitchUtil::FDungeonsSwitchNSOCheck>(0, [this, &UserId, Privilege, Delegate](bool success, bool offline)
	{
		if (success)
		{
			GetUserPrivilegeXBL(UserId, Privilege, Delegate);
		}
		else if (Delegate.IsBound())
		{
			Delegate.Execute(UserId, Privilege, static_cast<uint32>(offline ? EPrivilegeResults::NetworkConnectionUnavailable : EPrivilegeResults::AccountTypeFailure));
		}

		FSwitchPlatformMisc::DisallowNifm();
	}));
#else
	GetUserPrivilegeXBL(UserId, Privilege, Delegate);
#endif
}

FPlatformUserId FOnlineIdentityDungeons::GetPlatformUserIdFromUniqueNetId(const FUniqueNetId& UniqueNetId) const
{
	for(const auto& Entry : LocalUsers)
	{
		auto CurrentUniqueId = GetUniquePlayerId(Entry.second->LocalUserNum);
		if (CurrentUniqueId.IsValid() && (*CurrentUniqueId == UniqueNetId))
		{
			return Entry.second->LocalUserNum;
		}

	}

	return PLATFORMUSERID_NONE;
}

FString FOnlineIdentityDungeons::GetAuthType() const { return ""; }

XblUserHandle FOnlineIdentityDungeons::GetXBLUserHandle(int32 LocalUserNum) const {
	const auto user = GetUserFromLocalUser(LocalUserNum);
	return user ? user->XBLUser : nullptr;
}

XblContextHandle FOnlineIdentityDungeons::GetXBLContextHandle(int32 LocalUserNum) const {
	const auto user = GetUserFromLocalUser(LocalUserNum);
	return user ? user->XBLcontext : nullptr;
}

#ifdef	HAS_GAMERSCORE_AND_PICTURE
FString FOnlineIdentityDungeons::GetPlayerGamerscore(int32 LocalUserNum) const {
	const auto& user = GetUserFromLocalUser(LocalUserNum);
	return user ? user->GetGamerscore() : "";
}

FString FOnlineIdentityDungeons::GetPlayerDisplayPictureUri(int32 LocalUserNum) const {
	const auto& user = GetUserFromLocalUser(LocalUserNum);
	return user ? user->GetDisplayPictureUri() : "";
}
#endif //HAS_GAMERSCORE_AND_PICTURE

#ifdef	HAS_TEXT_TO_SPEECH_ENABLED_FUNC
bool FOnlineIdentityDungeons::GetProfileTextToSpeechEnabled(int32 LocalUserNum) const
{
	if (GetLocalUsersNum() > 0)
	{
		const auto user = GetUserFromLocalUser(LocalUserNum); 
		if (user)
		{
			return user->GetTextToSpeechEnabled();
		}
	}
	return false;
}

#endif

#if !PLATFORM_XBOXONE //D11.PS
void FOnlineIdentityDungeons::HandleInviteNotification(const XblGameInviteNotificationEventArgs* args, XblContextHandle XBLHandle)
{
	FString SenderGamertag = ANSI_TO_TCHAR(args->senderGamertag);
	FString InviteHandleId = ANSI_TO_TCHAR(args->inviteHandleId);
	FUniqueNetIdDungeons SenderUniqueNetId = FUniqueNetIdDungeons(args->senderXboxUserId);

	UE_LOG_ONLINE(Log, TEXT("[Identity] Invite received from '%s"), *SenderGamertag);
	
	const auto hr = XblMultiplayerGetSessionAsync(XBLHandle, &args->sessionReference, AsyncTasks::CreateAsyncBlock([this, SenderGamertag, InviteHandleId, SenderUniqueNetId](XAsyncBlock* asyncBlock)
	{
		XblMultiplayerSessionHandle SessionHandle = nullptr;
		if (HrFailed(XblMultiplayerGetSessionResult(asyncBlock, &SessionHandle), "[Identity] Get session handle for invite")) {
			return;
		}
		scopedLambda::ScopedLambda scopedHandle([SessionHandle]{XblMultiplayerSessionCloseHandle(SessionHandle);});
		const auto properties = sessionProperties::GetProperties(SessionHandle);
		if (!properties) {
			UE_LOG_ONLINE(Error, TEXT("[Identity] Failed to get session properties for invite"));
			return;
		}

		if (!DungeonsSubsystem->GetFriendsInterface()->IsFriend(0, SenderUniqueNetId, SenderGamertag)) {
			UE_LOG_ONLINE(Log, TEXT("[Identity] Filtering out one-way friend invite from %s"), *SenderGamertag);
			return;
		}

		if (DungeonsSubsystem->GetSessionInterface()->IsPlayerInSession(DungeonsGameSessionName, SenderUniqueNetId)) {
			UE_LOG_ONLINE(Log, TEXT("[Identity] Filtering out invite from %s because you are already in the same session"), *SenderGamertag);
			return;
		}

		int32 PublicConnections = 4;
		int32 PublicOpenConnections = PublicConnections - XblMultiplayerSessionMembersAccepted(SessionHandle);
		auto lobby = BuildSessionFromXBLSession(properties.GetValue(), SessionHandle, InviteHandleId, PublicConnections, PublicOpenConnections);

		DungeonsSubsystem->QueueAsyncTask(
			MakeUnique<OnlineAsyncTaskDungeonsTriggerOnSessionInviteReceived>(DungeonsSubsystem->GetSessionInterface(), lobby, SenderGamertag));
	}
	, DungeonsSubsystem->GetQueueHandle()
	));

	HrLog(hr, "[Identity] Xbl multiplayer get session async");
}

void FOnlineIdentityDungeons::RegisterInviteHandlerForUser(int32 userNum)
{
	if (const auto user = GetMutableUser(userNum)) {
		auto userContext = GetXBLContextHandle(userNum);
		HrCheck(XblGameInviteRegisterForEventAsync(userContext, AsyncTasks::CreateAsyncBlock([user](XAsyncBlock* asyncBlock)
		{
			HrLogOnlyError(XblGameInviteRegisterForEventResult(asyncBlock, &user->InviteSubscriptionHandle), "[Identity] Setting InviteSubscriptionHandle to user");
		}, GetDungeonsOnlineSubsystem()->GetQueueHandle())
		), "[Identity] XblGameInviteRegisterForEventAsync");
	}

}

void FOnlineIdentityDungeons::UnregisterInviteHandlerForUser(int32 userNum)
{
	if (auto user = GetUserFromLocalUser(userNum))
	{
		auto userContext = GetXBLContextHandle(userNum);
		HrCheck(XblGameInviteUnregisterForEventAsync(userContext, user->InviteSubscriptionHandle, AsyncTasks::CreateAsyncBlock([](XAsyncBlock* asyncBlock)
		{
			UE_LOG_ONLINE(Log, TEXT("[Identity] XblGameInviteUnregisterForEventAsync complete"));
		}, GetDungeonsOnlineSubsystem()->GetQueueHandle())
		), "[Identity] XblGameInviteUnregisterForEventAsync");
	}
}
#endif

// D11.SSN
void FOnlineIdentityDungeons::HandleStatChangeNotification(const XblStatisticChangeEventArgs* args, XblContextHandle XBLHandle)
{
#ifdef HAS_STATISTICS_WRITTEN_TRACKER_DELEGATE
	UE_LOG_ONLINE(Log, TEXT("Statistic changed callback: stat changed (%s = %s)"), args->latestStatistic.statisticName, args->latestStatistic.value);
	IOnlineStatsPtr StatsInterface = GetDungeonsOnlineSubsystem()->GetStatsInterface();
	const TSharedPtr<const FUniqueNetId> NetID = GetUserFromXuid(args->xboxUserId)->UniqueNetId;
	StatsInterface->OnStatisticsWrittenTrackerDelegate.Broadcast(true, NetID.ToSharedRef());
#endif
}

// This doesn't work right now, never fires
void FOnlineIdentityDungeons::RegisterStatChangeHandlerForUser(int32 userNum)
{
#ifdef HAS_STATISTICS_WRITTEN_TRACKER_DELEGATE
	if (const auto user = GetMutableUser(userNum)) {
		auto userContext = GetXBLContextHandle(userNum);
		const char* name = "1";
		HrCheck(XblUserStatisticsSubscribeToStatisticChange(userContext, user->UserId, OnlineSubsystemConstants::SCID, name, &user->StatChangeSubscriptionHandle), "[Identity] XblGameStatChangeRegisterForEventAsync");
	}
#endif
}

void FOnlineIdentityDungeons::UnregisterStatChangeHandlerForUser(int32 userNum)
{
#ifdef HAS_STATISTICS_WRITTEN_TRACKER_DELEGATE
	if (auto user = GetUserFromLocalUser(userNum))
	{
		auto userContext = GetXBLContextHandle(userNum);
		HrCheck(XblUserStatisticsUnsubscribeFromStatisticChange(userContext, user->StatChangeSubscriptionHandle), "[Identity] XblStatChangeUnregisterForEventAsync");
	}
#endif
}

Peer2PeerManager* FOnlineIdentityDungeons::GetPeer2PeerManager() const {
	return Peer2Peer.Get();
}

void FOnlineIdentityDungeons::SetDirtyServiceToken(bool dirty) {
	GetServiceToken().setDirty(dirty);
}

DungeonsAuthToken::DungeonsAuthToken()
{
}

DungeonsAuthToken::DungeonsAuthToken(const FString &authMethod, const FString &authUrl)
	: url(authUrl)
	, method(authMethod)
{
}

const FString& DungeonsAuthToken::getUrl() const
{
	return url;
}

const FString& DungeonsAuthToken::getMethod() const
{
	return method;
}	


bool DungeonsAuthToken::isDirty() const
{
	return dirty;
}

const FString& DungeonsAuthToken::getToken() const
{
	
	return content;
}

bool DungeonsAuthToken::hasExpired() const
{
	return checkTimer > dungeonsIdentity::TOKEN_CHECK_INTERVAL_SEC;
}

void DungeonsAuthToken::set(const FString& token)
{

	content = token;
	
}

void DungeonsAuthToken::setDirty(bool isDirty)
{
	dirty = isDirty;
}

void DungeonsAuthToken::updateTimer(float deltaTime)
{
	checkTimer += deltaTime;
}

void DungeonsAuthToken::resetTimer()
{
	checkTimer = 0.f;
}

bool DungeonsAuthToken::isCurrentToken(const FString token) const
{
	return content == token;
}

bool DungeonsMsaLinkCode::HasExpired() const { return timer > dungeonsIdentity::MSA_CODE_CHECK_INTERVAL_SEC; }

void DungeonsMsaLinkCode::UpdateTimer(float deltaTime) {  timer += deltaTime; }