// © 2020 Mojang Synergies AB. TM Microsoft Corporation.
#include "LoginFlowBpFunctionsLib.h"
#include "online/sessions/OnlineUtil.h"
#include "online/crossplay/CrossplayOSS.h"
#include "online/crossplay/ExternalUI.h"
#include "online/crossplay/Identity.h"
#include "Dungeons/DungeonsGameInstance.h"
#include "Dungeons/DungeonsUserManagement.h"
#include "game/actor/character/player/DungeonsLocalPlayer.h"
#include "online/crossplay/UserCloud.h"
#include "OnlineUserCloudCommon.h"

FText ULoginFlowBpFunctionsLib::GetLinkErrorGamertagHint() {
	FString gamerTag = online::getExternalUIInterface()->GetLinkErrorGamertagHint();
	if (!gamerTag.IsEmpty())
		return FText::FromString(FString::Printf(TEXT("(%s)"), *gamerTag));
	return FText();
}

void ULoginFlowBpFunctionsLib::SignOutXBL(APlayerControllerBase* playerController) {
	online::SetXblActive(false, playerController);
	online::removeLiveOps(playerController->GetGameInstance<UDungeonsGameInstance>(), 0);
	online::getIdentityInterface()->Logout(0, SubsystemType::Dungeons);
	if (online::getCrossplayOss()->IsPS4Active() && online::getCrossplayOss()->IsDungeonsActive()) {
		online::getCrossplayOss()->DeactivateDungeonsOSS();
	}
}

/* unlinks PSN from XBL via Playfab */
void ULoginFlowBpFunctionsLib::UnlinkXBLAccount(APlayerControllerBase* playerController) {
#if PLATFORM_PS4
	if (!online::getCrossplayOss()->IsPS4Active())
		return;

	FString issuerIdPSN = "";
	auto ps4AuthToken = online::getIdentityInterface()->GetAuthToken("playfab_key", issuerIdPSN);
	auto gameInstance = playerController->GetGameInstance<UDungeonsGameInstance>();

	PlayfabServices::UnlinkXBLAccount({ ps4AuthToken, issuerIdPSN, "",
		[gameInstance, playerController](bool success) {
			ULoginFlowBpFunctionsLib::SignOutXBL(playerController);
		}
	});
#endif
}

void ULoginFlowBpFunctionsLib::SetCrossplayState(APlayerControllerBase* playerController, bool EnableCrossplay)
{
	if (!playerController)
		return;
	int LocalUserNum = Cast<UDungeonsLocalPlayer>(playerController->GetLocalPlayer())->GetSystemUserId();
	auto gameInstance = playerController->GetGameInstance<UDungeonsGameInstance>();
	if (auto* save = gameInstance->EditGlobalSaveState(LocalUserNum))
	{
		save->mRecordedData.mCrossplay = EnableCrossplay;
		save->Save(LocalUserNum);
		online::getCrossplayOss()->SetCrossplaySetting(save->mRecordedData.mCrossplay);
	}
}

void ULoginFlowBpFunctionsLib::ActivateDungeonsOSS(APlayerControllerBase* playerController) {
	online::getCrossplayOss()->ActivateDungeonsOSS();
	online::SetXblActive(true, playerController);
}

bool ULoginFlowBpFunctionsLib::IsCrossplayEnabled() { return online::IsCrossplayEnabled(); }



bool ULoginFlowBpFunctionsLib::IsInitialUserAccountLinked()
{
	auto Id = UDungeonsUserManager::Instance()->GetInitialUserSystemId();
	auto LocalPlayerControllers = UDungeonsUserManager::Instance()->GetLocalMenuPlayerControllers();
	for (auto pLocalPlayerController : LocalPlayerControllers)
	{
		auto pLocalPlayer = pLocalPlayerController->GetLocalPlayer();
		auto pDungeonsPlayer = Cast<UDungeonsLocalPlayer>(pLocalPlayer);
		if (pDungeonsPlayer->GetSystemUserId() == Id)
		{
			return pLocalPlayer->GetUniqueNetIdFromCachedControllerId().IsValid();
		}
	}
	return false;
}

bool ULoginFlowBpFunctionsLib::AreCloudServicesEnabled() 
{ 
	auto pUserCloudInterface = online::getCrossplayOss()->GetUserCloudIF().Get();
	check(pUserCloudInterface);
	return IsInitialUserAccountLinked() && pUserCloudInterface->AreCloudServicesEnabled();
}


bool ULoginFlowBpFunctionsLib::ShouldDisplayCloudServicesFeature()
{
#if CLOUDSAVE_ENABLED
	extern bool ShouldDisplayCloudServicesFeature();
	return ShouldDisplayCloudServicesFeature();
#else
	return false;
#endif
}


bool ULoginFlowBpFunctionsLib::IsSignedInToPSN() {
	return online::getCrossplayOss()->IsPS4Active() && online::getIdentityInterface()->GetLoginStatus(0, SubsystemType::PS4) == ELoginStatus::LoggedIn;
}

FString ULoginFlowBpFunctionsLib::GetXBLNameTag(int localPlayerNum) {
	return online::getIdentityInterface()->GetSecondaryPlayerNickname(localPlayerNum);
}

FString ULoginFlowBpFunctionsLib::GetXBLGamerscore(int localPlayerNum) {
	return online::getIdentityInterface()->GetPlayerGamerscore(localPlayerNum);
}

FString ULoginFlowBpFunctionsLib::GetXBLPictureUri(int localPlayerNum) {
	return online::getIdentityInterface()->GetPlayerDisplayPicUri(localPlayerNum);
}

namespace loginflowcommands {
	static bool DebugConsoleOpen;
}

bool ULoginFlowBpFunctionsLib::IsDebugConsoleOpen() { return loginflowcommands::DebugConsoleOpen; }

void ULoginFlowBpFunctionsLib::SetDebugConsoleOpen(bool value) { loginflowcommands::DebugConsoleOpen = value; }


bool ULoginFlowBpFunctionsLib::IsProfileTextToSpeechEnabled(int localPlayerNum)
{
	return online::getIdentityInterface()->GetProfileTextToSpeechEnabled(localPlayerNum);
}