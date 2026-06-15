#include "Identity.h"

#include "online/sessions/OnlineUtil.h"

#ifdef EPIC_STORE_BUILD
#include "EpicStoreConstants.h"
#endif

namespace online {
namespace Crossplay {

namespace internal {
	constexpr auto PrioritizePs4Order = { SubsystemType::PS4, SubsystemType::Dungeons, SubsystemType::Null };
}
	
Identity::Identity(const SubsystemRepo& subsystems) : SubOSS(subsystems)
{
}

#if defined(SUBSYSTEM_HAS_AUTH_TOKEN_FUNCTIONS)
void Identity::AddAuthToken()
{
	/* always use the PS4 playfab token on PS4 */
	if (auto* ps4Subsystem = Subsystems.Get(SubsystemType::PS4))
		ps4Subsystem->GetIdentityInterface()->AddAuthToken("playfab_key", "6c5e28ec-6639-4073-a692-106184ef6705", "");
	if (auto* dungeonsSubsystem = Subsystems.Get(SubsystemType::Dungeons))
	{
		dungeonsSubsystem->GetIdentityInterface()->AddAuthToken("playfab_key", "POST", "https://playfabapi.com/");
#ifdef EPIC_STORE_BUILD
		dungeonsSubsystem->GetIdentityInterface()->AddAuthToken(minecraft::epicstore::EpicGameServicesTokenId, "GET", minecraft::epicstore::EpicGameServicesTokenUrl);
#endif //EPIC_STORE_BUILD
	}
}

FString Identity::GetAuthToken(const FString& key, FString& issuerId) const {
	/* always use the PS4 playfab token on PS4 */
	return GetIdentityInterfacePrioritizePS4()->GetAuthToken(key, issuerId);
}
#endif

TSharedPtr<const FUniqueNetId> Identity::GetSponsorUniquePlayerId(int32 LocalUserNum) const
{
	return GetIdentityInterfacePrioritizePS4()->GetSponsorUniquePlayerId(LocalUserNum);
}

FString Identity::GetAuthToken(int32 LocalUserNum) const {
	return GetIdentityInterfacePrioritizePS4()->GetAuthToken(LocalUserNum);
}

bool Identity::GetAuthTokenAsync(const FString &key, TFunction<void(FString, FString)> callback) {
#if defined(HAS_ON_AUTH_TOKEN_UPDATE) && defined(SUBSYSTEM_HAS_AUTH_TOKEN_FUNCTIONS)
	GetIdentityInterfacePrioritizePS4()->GetAuthTokenAsync(key, callback);
	return true;
#endif
	return false;
}

FString Identity::GetPlayerNickname(int32 LocalUserNum) const
{
	/* On PlayStation - prioritize the PSN Nickname.
	 From Cross Platform Policy https://ps4.siedev.net/resources/documents/SDK/7.500/Cross_Platform_Policy/0002.html#__document_toc_00000008:
	 The PlayStation Network Online ID must be the primary identifier displayed for players playing from a PlayStation device. */
	return GetIdentityInterfacePrioritizePS4()->GetPlayerNickname(LocalUserNum);
}

FString Identity::GetPlayerNickname(const FUniqueNetId& UserId) const {
	return GetIdentityInterfacePrioritizePS4()->GetPlayerNickname(UserId);
}

FString Identity::GetSecondaryPlayerNickname(int32 LocalUserNum) const
{
	// To extract XBL gamertag on PS4
	auto subsystem = Subsystems.GetActive(SubsystemType::Dungeons);
	if (subsystem && sessionSettings::GetPlatform() == sessionSettings::PlatformType::PS4_PLATFORM) {
		return subsystem->GetIdentityInterface()->GetPlayerNickname(LocalUserNum);
	}
	else {
		return "";
	}
}

FString Identity::GetPlayerGamerscore(int32 LocalUserNum) const
{
#ifdef	HAS_GAMERSCORE_AND_PICTURE
	// To extract XBL gamerscore on PS4
	auto subsystem = Subsystems.GetActive(SubsystemType::Dungeons);
	if (subsystem) {
		return subsystem->GetIdentityInterface()->GetPlayerGamerscore(LocalUserNum);
	}
	else {
		return "";
	}
#endif	
	return "";
}

FString Identity::GetPlayerDisplayPicUri(int32 LocalUserNum) const
{
#ifdef	HAS_GAMERSCORE_AND_PICTURE
	// To extract XBL gamerscore on PS4
	auto subsystem = Subsystems.GetActive(SubsystemType::Dungeons);
	if (subsystem) {
		return subsystem->GetIdentityInterface()->GetPlayerDisplayPictureUri(LocalUserNum);
	}
	else {
		return "";
	}
#endif	
	return "";
}

bool Identity::GetProfileTextToSpeechEnabled(int32 LocalUserNum) const
{
#ifdef	HAS_TEXT_TO_SPEECH_ENABLED_FUNC
	auto subsystem = Subsystems.GetActive(SubsystemType::Dungeons);
	if (subsystem) {
		return subsystem->GetIdentityInterface()->GetProfileTextToSpeechEnabled(LocalUserNum);
	}
	else {
		return false;
	}
#endif
	return false;
}

void Identity::AutoLogin()
{
	auto subsystem = Subsystems.GetActive(SubsystemType::Steam);
	if (subsystem)
	{
		subsystem->GetIdentityInterface()->AutoLogin(0);
	}
}

void Identity::TryLocalPlayerLogin(int32 LocalUserNum, const FOnLoginCompleteDelegate& LoginCompleteDelegate, FDelegateHandle& LoginCompleteDelegateHandle)
{
	LoginCompleteDelegateHandle = GetIdentityInterface()->AddOnLoginCompleteDelegate_Handle(LocalUserNum, LoginCompleteDelegate);
	GetIdentityInterface()->Login(LocalUserNum, FOnlineAccountCredentials());
}

void Identity::ClearLocalPlayerLoginDelegate(int32 LocalUserNum, FDelegateHandle& LoginCompleteDelegateHandle)
{
	GetIdentityInterface()->ClearOnLoginCompleteDelegate_Handle(LocalUserNum, LoginCompleteDelegateHandle);
}

TSharedPtr<const FUniqueNetId> Identity::GetUniquePlayerId(int32 LocalUserNum) const {
	return GetIdentityInterface()->GetUniquePlayerId(LocalUserNum);
}

TSharedPtr<const FUniqueNetId> Identity::GetUniquePlayerId(int32 LocalUserNum, FName SubsystemType) const
{
	const auto* subsystem = GetSubsystemFromName(SubsystemType);
	check(subsystem && "GetUniquePlayerId() - subsystem is null!");
	return subsystem->GetIdentityInterface()->GetUniquePlayerId(LocalUserNum);
}

TSharedPtr<const FUniqueNetId> Identity::GetUniquePlayerIdForSessionSubsystem(int32 localUserNum) const {
	return GetSessionSubsystem()->GetIdentityInterface()->GetUniquePlayerId(localUserNum);
}

/* Log out of all subsystems */
bool Identity::Logout(int32 LocalUserNum)
{
	bool result = true;
	for (const auto* subsystem : Subsystems.GetAllActive()) {
		result = result && subsystem->GetIdentityInterface()->Logout(LocalUserNum);
	}
	return result;
}

/* Logs out of the specified subsystem */
bool Identity::Logout(int32 LocalUserNum, SubsystemType subsystemType)
{	
	if (const auto* subsystem = Subsystems.GetActive(subsystemType))
		return subsystem->GetIdentityInterface()->Logout(LocalUserNum);
	return false;
}

bool Identity::Logout(const FUniqueNetId& UniqueNetId)
{
	if (auto subsystem = GetSubsystemFromUniqueNetId(UniqueNetId))
	{
		int32 id = subsystem->GetIdentityInterface()->GetPlatformUserIdFromUniqueNetId(UniqueNetId);
		return subsystem->GetIdentityInterface()->Logout(id);
	}
	else {
		checkNoEntry();
	}
	return false;
}

ELoginStatus::Type Identity::GetLoginStatus(int32 LocalUserNum) const
{	
	/* On PS4 consult the PS4 subsystem first */
	if (auto* ps4Subsystem = Subsystems.GetActive(SubsystemType::PS4))
	{
		ELoginStatus::Type ps4Status = ps4Subsystem->GetIdentityInterface()->GetLoginStatus(LocalUserNum);
		/* if the Dungeons OSS is also active - merge the results */
		if (auto* dungeonsSubsystem = Subsystems.GetActive(SubsystemType::Dungeons))
		{
			ELoginStatus::Type dungeonsStatus = dungeonsSubsystem->GetIdentityInterface()->GetLoginStatus(LocalUserNum);
			ps4Status = ps4Status < dungeonsStatus ? ps4Status : dungeonsStatus;
		}
		return ps4Status;
	}
	return GetIdentityInterface()->GetLoginStatus(LocalUserNum);
}

ELoginStatus::Type Identity::GetLoginStatus(int32 LocalUserNum, SubsystemType subsystemType) const
{
	if (const auto* subsystem = Subsystems.GetActive(subsystemType))
		return subsystem->GetIdentityInterface()->GetLoginStatus(LocalUserNum);
	return ELoginStatus::NotLoggedIn;
}

void Identity::GetUserPrivilege(const FUniqueNetId& LocalUserId, EUserPrivileges::Type Privilege, const IOnlineIdentity::FOnGetUserPrivilegeCompleteDelegate& Delegate) {
	if (Subsystems.IsActive(SubsystemType::PS4) && Subsystems.IsActive(SubsystemType::Dungeons)) {
		/* If both subsystems are active on PS4 - make sure the privilege is present in both */
		const auto* subsystem = GetPS4Subsystem();
		IOnlineIdentity::FOnGetUserPrivilegeCompleteDelegate child;
		child.BindLambda(
			[subsystem = GetDungeonsSubsystem(), LocalUserId = GetUniqueNetIdFromType(LocalUserId, SubsystemType::Dungeons), Privilege, Delegate]
			(const FUniqueNetId& UserId, EUserPrivileges::Type Ps4Privilege, uint32 PrivilegeResults) 
			{
				if (PrivilegeResults == (uint32)IOnlineIdentity::EPrivilegeResults::NoFailures && subsystem) {
					subsystem->GetIdentityInterface()->GetUserPrivilege(*LocalUserId, Privilege, Delegate);
				} else {
					Delegate.ExecuteIfBound(UserId, Ps4Privilege, PrivilegeResults);
				}
			}
		);

		subsystem->GetIdentityInterface()->GetUserPrivilege(*GetUniqueNetIdFromType(LocalUserId, SubsystemType::PS4), Privilege, child);
	}
	else {
		SubsystemType type = GetFirstActiveType(internal::PrioritizePs4Order);
		Subsystems.GetActive(type)->GetIdentityInterface()->GetUserPrivilege(*GetUniqueNetIdFromType(LocalUserId, type), Privilege, Delegate);
	}
}

FPlatformUserId Identity::GetPlatformUserIdFromUniqueNetId(const FUniqueNetId & UniqueNetId) const
{
	auto subsystem = GetSubsystemFromUniqueNetId(UniqueNetId);
	return subsystem->GetIdentityInterface()->GetPlatformUserIdFromUniqueNetId(UniqueNetId);
}

void Identity::AddOnLoginStatusChangedDelegate_Handle(int num, const FOnLoginStatusChangedDelegate& Delegate)
{
	/* add to all subsystems even the disabled ones, because crossplay can be disabled/enabled in-game */
	for (SubsystemType sType : { SubsystemType::Dungeons, SubsystemType::PS4 }) {
		if (auto* subsystem = Subsystems.Get(sType))
			subsystem->GetIdentityInterface()->AddOnLoginStatusChangedDelegate_Handle(num, Delegate);
	}
}

void Identity::AddOnControllerPairingChangedDelegate_Handle(const FOnControllerPairingChangedDelegate& Delegate)
{
	/* add to all subsystems even the disabled ones, because crossplay can be disabled/enabled in-game */
	for (SubsystemType sType : { SubsystemType::Dungeons, SubsystemType::PS4 }) {
		if (auto* subsystem = Subsystems.Get(sType))
			subsystem->GetIdentityInterface()->AddOnControllerPairingChangedDelegate_Handle(Delegate);
	}
}

IOnlineIdentityPtr Identity::GetIdentityInterface() const {
	return Subsystems.GetFirstActiveSubsystem()->GetIdentityInterface();
}

IOnlineIdentityPtr Identity::GetIdentityInterfacePrioritizePS4() const {
	return Subsystems.GetFirstActiveSubsystem(internal::PrioritizePs4Order)->GetIdentityInterface();
}

}
}
