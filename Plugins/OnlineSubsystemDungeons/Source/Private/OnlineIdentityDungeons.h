#pragma once

#include "CoreMinimal.h"
#include <map>
#include <memory>
#include <functional>
#include "Interfaces/OnlineIdentityInterface.h"
#include "OnlineSubsystemTypes.h"
#include "xsapiServicesInclude.h"

namespace WebSocket
{
	class Context;
}

class FUniqueNetIdDungeons;
class Peer2PeerManager;


struct XBLUserInfo
{
	XBLUserInfo(XblUserHandle handle, uint32 localUserNum, uint64_t userId, uint64_t systemId, XblContextHandle context, FString gamerTag,
	            XblSocialManagerUserGroup* socialGroup, XblFunctionContext inviteHandle, XblFunctionContext rtaStateHandle, XblFunctionContext statChangeHandle);

	~XBLUserInfo();

	FString GetGamerTag() const;
	void SetGamerTag(const FString&);
	FString GetGamerscore() const;
	void SetGamerscore(const FString&);
	FString GetDisplayPictureUri() const;
	void SetDisplayPictureUri(const FString&);

	void SetTextToSpeechEnabled(bool enabled) { TextToSpeechEnabled = enabled; }
	bool GetTextToSpeechEnabled() const { return TextToSpeechEnabled; }

	XBLUserInfo(const XBLUserInfo&) = delete;
	XBLUserInfo& operator=(const XBLUserInfo&) = delete;

	XblUserHandle XBLUser;
	uint32 LocalUserNum;
	uint64_t UserId;
	uint64_t SystemId;
	TSharedPtr<FUniqueNetIdDungeons> UniqueNetId;
	XblContextHandle XBLcontext;
	XblSocialManagerUserGroup* SocialGroup;
	XblFunctionContext InviteHandle;
	XblFunctionContext RtaStateHandle;
	XblFunctionContext StatChangeHandle;
	XblRealTimeActivitySubscriptionHandle InviteSubscriptionHandle;
	XblRealTimeActivitySubscriptionHandle StatChangeSubscriptionHandle;
private:
	FString GamerTag;
	FString Gamerscore;
	FString DisplayPictureUri;
	bool TextToSpeechEnabled = false;
};

struct DungeonsAuthToken
{
	DungeonsAuthToken();
	DungeonsAuthToken(const FString &authMethod, const FString &authUrl);
	bool isDirty() const;
	const FString& getToken() const;
	bool hasExpired() const;
	void set(const FString&);
	void setDirty(bool);
	void updateTimer(float deltaTime);
	void resetTimer();
	bool isCurrentToken(FString) const;
	const FString& getUrl() const;
	const FString& getMethod() const;

private:
	FString content;
	FString url;
	FString method;
	float checkTimer = 0.f;
	bool dirty = false;
};

struct DungeonsMsaLinkCode
{
	bool HasExpired() const;
	void UpdateTimer(float);
	bool SignInRestarted = false;
private:	
	float timer = 0.f;
};

class FOnlineIdentityDungeons : public IOnlineIdentity
{
	TUniquePtr<Peer2PeerManager> Peer2Peer;
	std::map<FString, DungeonsAuthToken> AuthTokens;
	XblRealTimeActivityConnectionState RTAEvent;
	FDelegateHandle MinecraftApiAuthRetryDelegateHandle;

	std::shared_ptr<const XBLUserInfo> GetUser(std::function<bool(const XBLUserInfo&)>) const;
	std::shared_ptr<XBLUserInfo> GetMutableUser(int32 LocalUserNum) const;

	using OnUpdateAuthTokenComplete = std::function<void(HRESULT, const XalUserGetTokenAndSignatureData*)>;
	void UpdateAuthToken(DungeonsAuthToken&, XblUserHandle, OnUpdateAuthTokenComplete);
	void SetServiceToken(const FUniqueNetId&, const char*);
	DungeonsAuthToken &GetServiceToken();
	void GetUserPrivilegeXBL(const FUniqueNetId& UserId, EUserPrivileges::Type Privilege, const FOnGetUserPrivilegeCompleteDelegate& Delegate);

	void SignInWithUIError(FString LogString, FString ErrorString);

protected:
	void RemoveLocalUser(int32 LocalUserNum);
	void TryReleasePeer2Peer(int32 LocalUserNum);

	class FOnlineSubsystemDungeons* DungeonsSubsystem;
	bool AsyncInitialized;
	std::map<int32, std::shared_ptr<XBLUserInfo>> LocalUsers;

	bool AuthenticateLocally(int32 LocalUserNum, XblUserHandle);

	virtual void ChangeUserInfo(std::shared_ptr<XBLUserInfo>& UserToChange);

	void SignIn(int32 LocalUserNum);
	void SignInWithUI(int32 LocalUserNum);
	void FetchAuthTokenAndCompleteLogin(int32 LocalUserNum);
	virtual bool ShouldSuccessCallbackOnAuthTokenFailure();
	void UpdateAuthToken(int32 LocalUserNum, DungeonsAuthToken &token);
	void OnMsaCodeReceived(const FString&, const FString&);
	bool SignOut(int32 LocalUserNum);
	bool signInPending;
#ifdef SUBSYSTEM_HAS_AUTH_TOKEN_FUNCTIONS
	std::map<FString, std::list<AuthTokenCallback>> TokenQueue;
#endif
	std::unique_ptr<DungeonsMsaLinkCode> MsaLinkCode;
	FDelegateHandle MsaCodeReceivedHandle;
public:
	FOnlineIdentityDungeons(FOnlineSubsystemDungeons* InDungeonsSubsystem);
	virtual ~FOnlineIdentityDungeons();

	FOnlineSubsystemDungeons* GetDungeonsOnlineSubsystem() const;

	std::shared_ptr<const XBLUserInfo> GetUserFromNetId(const FUniqueNetId&) const;
	std::shared_ptr<const XBLUserInfo> GetUserFromLocalUser(int32 LocalUserNum) const;
	std::shared_ptr<const XBLUserInfo> GetUserFromXuid(uint64_t xuid) const;
	std::shared_ptr<const XBLUserInfo> GetUserFromHandle(XblUserHandle) const;

	XblRealTimeActivityConnectionState GetPreviousRTAEvent() const;
	void SetRTAEvent(XblRealTimeActivityConnectionState);

	virtual bool Tick(float DeltaTime);

	//IOnlineIdentity
	bool Login(int32 LocalUserNum, const FOnlineAccountCredentials& AccountCredentials) override;
	bool Logout(int32 LocalUserNum) override;
	bool AutoLogin(int32 LocalUserNum) override;
	TSharedPtr<FUserOnlineAccount> GetUserAccount(const FUniqueNetId& UserId) const override;
	TArray<TSharedPtr<FUserOnlineAccount> > GetAllUserAccounts() const override;
	TSharedPtr<const FUniqueNetId> GetUniquePlayerId(int32 LocalUserNum) const override;
	TSharedPtr<const FUniqueNetId> CreateUniquePlayerId(uint8* Bytes, int32 Size) override;
	TSharedPtr<const FUniqueNetId> CreateUniquePlayerId(const FString& Str) override;
	ELoginStatus::Type GetLoginStatus(int32 LocalUserNum) const override;
	ELoginStatus::Type GetLoginStatus(const FUniqueNetId& UserId) const override;
	FString GetPlayerNickname(int32 LocalUserNum) const override;
	FString GetPlayerNickname(const FUniqueNetId& UserId) const override;
	FString GetAuthToken(int32 LocalUserNum) const override;

#ifdef	HAS_GAMERSCORE_AND_PICTURE
	FString GetPlayerGamerscore(int32 LocalUserNum) const override;
	FString GetPlayerDisplayPictureUri(int32 LocalUserNum) const override;
#endif

#ifdef	HAS_TEXT_TO_SPEECH_ENABLED_FUNC
	bool GetProfileTextToSpeechEnabled(int32 LocalUserNum) const override;
#endif

	//D11.PS
#ifdef SUBSYSTEM_HAS_AUTH_TOKEN_FUNCTIONS
	FString GetAuthToken(const FString &key, FString &issuerId) const override;
	void GetAuthTokenAsync(const FString &key, AuthTokenCallback callback) override;
	void AddAuthToken(const FString &key, const FString &method, const FString &url) override;
#endif

	void RevokeAuthToken(const FUniqueNetId& UserId, const FOnRevokeAuthTokenCompleteDelegate& Delegate) override { return; };
	void GetUserPrivilege(const FUniqueNetId& UserId, EUserPrivileges::Type Privilege, const FOnGetUserPrivilegeCompleteDelegate& Delegate) override;
	FPlatformUserId GetPlatformUserIdFromUniqueNetId(const FUniqueNetId& UniqueNetId) const override;
	FString GetAuthType() const override;

	XblUserHandle GetXBLUserHandle(int32 LocalUserNum) const;
	XblContextHandle GetXBLContextHandle(int32 LocalUserNum) const;

#if !PLATFORM_XBOXONE //D11.PS
	void HandleInviteNotification(const XblGameInviteNotificationEventArgs* args, XblContextHandle XBLHandle);
	void RegisterInviteHandlerForUser(int32 userNum);
	void UnregisterInviteHandlerForUser(int32 userNum);
#endif

	//D11.SSN
	void HandleStatChangeNotification(const XblStatisticChangeEventArgs* args, XblContextHandle XBLHandle);
	void RegisterStatChangeHandlerForUser(int32 userNum);
	void UnregisterStatChangeHandlerForUser(int32 userNum);

	void SetDirtyServiceToken(bool dirty);
	Peer2PeerManager* GetPeer2PeerManager() const;

	int32 GetFirstLocalUserNumber() const;
	void UpdateServiceToken(int32 LocalUserNum);

#ifdef EPIC_STORE_BUILD
	DungeonsAuthToken &GetEpicGameServicesToken();
	void SetEpicGameServicesToken(const char*);
	void UpdateEpicGameServicesToken(int32 LocalUserNum);
	void ClearEpicGameServicesToken();
#endif
	
	// D11.PC Virtual functions
	virtual void PlatformTick(float DeltaTime) {};
	size_t GetLocalUsersNum() const;

private:
	void RefreshAuthTokens(float DeltaTime);
	void RefreshMsaCode(float DeltaTime);
};

typedef TSharedPtr<FOnlineIdentityDungeons, ESPMode::ThreadSafe> FOnlineIdentityDungeonsPtr;

namespace dungeonsIdentity
{
	namespace platform
	{
		FOnlineIdentityDungeonsPtr Create(class FOnlineSubsystemDungeons* InDungeonsSubsystem);
	}
};
