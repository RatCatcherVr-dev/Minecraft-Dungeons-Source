#pragma once
#if PLATFORM_XBOXONE

#include "OnlineSubsystemDungeons.h"
#include "OnlineIdentityDungeons.h"
#include "OnlineError.h"

#define USE_OSSLIVE_PRIVILEGECHECK 1

#define TEMP_XPRIVILEGE_CROSSPLAY_ALLOWED 185

#if USE_OSSLIVE_PRIVILEGECHECK
enum class ECheckForPackageUpdateResult : uint8
{
	NoUpdateAvailable,
	OptionalUpdateAvailable,
	MandatoryUpdateAvailable
};
#endif

class FOnlineIdentityDungeonsXboxOne : public FOnlineIdentityDungeons
{
public:
	FOnlineIdentityDungeonsXboxOne(class FOnlineSubsystemDungeons* InSubsystem);
	virtual ~FOnlineIdentityDungeonsXboxOne();

	void PlatformTick(float DeltaTime) override;

	TSharedPtr<class FXboxOneInputInterface, ESPMode::ThreadSafe> GetXboxInputInterface();

	bool Login(int32 LocalUserNum, const FOnlineAccountCredentials& AccountCredentials) override;

	bool Logout(int32 LocalUserNum) override;

	virtual TSharedPtr<const FUniqueNetId> GetSponsorUniquePlayerId(int32 LocalUserNum) const override;
	int GetUserIDFromUser(Windows::Xbox::System::User^ user);
	void GetUserIdString(Windows::Xbox::System::User^ user, std::string& stdString) const;

	virtual FPlatformUserId GetPlatformUserIdFromUniqueNetId(const FUniqueNetId& UniqueNetId) const override;

#if USE_OSSLIVE_PRIVILEGECHECK
	virtual void GetUserPrivilege(const FUniqueNetId& UserId, EUserPrivileges::Type Privilege, const FOnGetUserPrivilegeCompleteDelegate& Delegate) override;
#endif

private:
	FUniqueNetIdDungeonsRef GetNetIdFromUser(Windows::Xbox::System::User^ user) const;
	void OnUserAdded(Windows::Xbox::System::User^ InUserAdded);
	void OnUserRemoved(Windows::Xbox::System::User^ InUserRemoved);
	void OnEngineInitComplete();
	void OnControllerPairingChange(int32 InControllerIndex, FPlatformUserId InNewUserId, FPlatformUserId InOldUserId);

	void HookEvents();
	void UnHookEvents();
	static void CALLBACK ConnectionStateChanged(_In_opt_ void* context, _In_ XblRealTimeActivityConnectionState state);

	void AddConnectionStateChangeHandler();
	void RemoveConnectionStateChangeHandler();
	bool IsConnectionStateHandlerInitialized() const { return ConnectionChangeContext != 0; }

	void InitializePlatform();
	void SetAllUsersActiveInTitle();
private:
	FDelegateHandle ControllerPairingChanged;
	XblFunctionContext ConnectionChangeContext = 0;
	XblRealTimeActivityConnectionState CurrentConnectionState{ XblRealTimeActivityConnectionState::Disconnected };
	Windows::Foundation::EventRegistrationToken SignOutStartedToken;
protected:
	virtual bool ShouldSuccessCallbackOnAuthTokenFailure() override;

	virtual void ChangeUserInfo(std::shared_ptr<XBLUserInfo>& UserToChange) override;

	void UpdateUserSpeechAccessiblity(std::shared_ptr<XBLUserInfo>& UserInfo);

};

typedef TSharedPtr<FOnlineIdentityDungeonsXboxOne, ESPMode::ThreadSafe> FOnlineIdentityDungeonsXboxPtr;

#endif