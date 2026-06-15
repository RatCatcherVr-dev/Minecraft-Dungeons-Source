#pragma once
#include "SubOSS.h"

namespace online {
namespace Crossplay {

class Identity : public SubOSS {
public:
	Identity(const SubsystemRepo&);
#if defined(SUBSYSTEM_HAS_AUTH_TOKEN_FUNCTIONS)
	void AddAuthToken();
	FString GetAuthToken(const FString& key, FString& issuerId) const;
#endif
	TSharedPtr<const FUniqueNetId> GetSponsorUniquePlayerId(int32 LocalUserNum) const;
	FString GetAuthToken(int32 LocalUserNum) const;
	bool GetAuthTokenAsync(const FString &key, TFunction<void(FString, FString)> callback);
	FString GetPlayerNickname(int32 LocalUserNum) const;
	FString GetPlayerNickname(const FUniqueNetId& UserId) const;
	FString GetSecondaryPlayerNickname(int32 LocalUserNum) const;
	FString GetPlayerGamerscore(int32 LocalUserNum) const;
	FString GetPlayerDisplayPicUri(int32 LocalUserNum) const;
	bool GetProfileTextToSpeechEnabled(int32 LocalUserNum) const;

	void AutoLogin();

	void TryLocalPlayerLogin(int32 LocalUserNum, const FOnLoginCompleteDelegate& LoginCompleteDelegate, FDelegateHandle& LoginCompleteDelegateHandle);
	void ClearLocalPlayerLoginDelegate(int32 LocalUserNum, FDelegateHandle& LoginCompleteDelegateHandle);

	TSharedPtr<const FUniqueNetId> GetUniquePlayerId(int32 LocalUserNum) const;
	TSharedPtr<const FUniqueNetId> GetUniquePlayerId(int32 LocalUserNum, FName SubsystemType) const;
	TSharedPtr<const FUniqueNetId> GetUniquePlayerIdForSessionSubsystem(int32 localUserNum) const;
	bool Logout(int32 LocalUserNum, SubsystemType subsystemType);
	bool Logout(int32 LocalUserNum);
	bool Logout(const FUniqueNetId& UniqueNetId);
	ELoginStatus::Type GetLoginStatus(int32 LocalUserNum) const;
	ELoginStatus::Type GetLoginStatus(int32 LocalUserNum, SubsystemType) const;
	void GetUserPrivilege(const FUniqueNetId& LocalUserId, EUserPrivileges::Type Privilege, const IOnlineIdentity::FOnGetUserPrivilegeCompleteDelegate& Delegate);
	FPlatformUserId GetPlatformUserIdFromUniqueNetId(const FUniqueNetId& UniqueNetId) const;

	void AddOnLoginStatusChangedDelegate_Handle(int num, const FOnLoginStatusChangedDelegate& Delegate);
	void AddOnControllerPairingChangedDelegate_Handle(const FOnControllerPairingChangedDelegate& Delegate);

private:
	IOnlineIdentityPtr GetIdentityInterface() const;
	IOnlineIdentityPtr GetIdentityInterfacePrioritizePS4() const;
};
}
}
