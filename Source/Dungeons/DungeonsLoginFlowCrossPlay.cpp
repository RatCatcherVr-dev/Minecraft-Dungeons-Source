#include "DungeonsLoginFlowCrossPlay.h"
#include "DungeonsGameInstance.h"
#include "game/actor/character/player/DungeonsLocalPlayer.h"
#include "online/sessions/OnlineUtil.h"
#include "online/crossplay/CrossplayOSS.h"
#include "online/crossplay/ExternalUI.h"
#include "platform/GameVersion.h"


UDungeonsLoginFlowCrossPlay::UDungeonsLoginFlowCrossPlay(const FObjectInitializer& ObjectInitializer)
	: UDungeonsLoginFlow(ObjectInitializer)
{
}

/* loads the save data, then does the login */
void UDungeonsLoginFlowCrossPlay::PlatformLogin(ULocalPlayer* LocalPlayer)
{
	//Set the logged in user before loading save data
	int ControllerId = GetPlayerControllerId();
	if (auto GameInstance = GetGameInstance())
	{
		EnableLoginRefresh();
		GameInstance->OnSaveStateLoaded.AddDynamic(this, &UDungeonsLoginFlowCrossPlay::OnLoadSaveDataComplete);
		GameInstance->LoadSaveData(ControllerId, LoggingInPlayerController.Get());
	}
}

void UDungeonsLoginFlowCrossPlay::LocalPlayerLogin(int ControllerId, APlayerController* PlayerController)
{
	LoggingInPlayerController = PlayerController;
	SetPlayerControllerId(ControllerId);
	if (UDungeonsLocalPlayer* DungeonsLocalPlayer = Cast<UDungeonsLocalPlayer>(PlayerController->GetLocalPlayer()))
	{
		DungeonsLocalPlayer->SetUserSystemId(ControllerId);
		//SetupUniqueNetID();
	}

	if (auto GameInstance = GetGameInstance())
	{
		//Use the base class version of OnLoadSaveDataComplete for local players
		GameInstance->OnSaveStateLoaded.AddDynamic(this, &UDungeonsLoginFlow::OnLoadSaveDataCompleted);
		GameInstance->LoadSaveData(ControllerId, LoggingInPlayerController.Get());
	}
}

void UDungeonsLoginFlowCrossPlay::CrossPlayLogin(APlayerController* PlayerController)
{
	LoggingInPlayerController = PlayerController;
#ifdef	HAS_MSA_LINKING_DELEGATES
	MsaLinkRequiredHandle = FCoreDelegates::OnMsaLinkingRequired.AddUObject(this, &UDungeonsLoginFlowCrossPlay::ShowMsaLinkUI);
#endif
	auto externalUI = online::getExternalUIInterface();
	ULocalPlayer* LocalPlayer = LoggingInPlayerController.Get()->GetLocalPlayer();

#if !UE_BUILD_SHIPPING && (PLATFORM_PS4 || PLATFORM_SWITCH)
	if (DebugLoginResultSet())
	{
		OnLoginComplete.Broadcast(LocalPlayer->GetControllerId(), LocalPlayer->GetPlayerController(nullptr), debugLoginResult);
		ClearDebugLoginResult();
		return;
	}
#endif

	externalUI->ShowLoginUI(LocalPlayer->GetControllerId(), false, false,
		FOnLoginUIClosedDelegate::CreateUObject(this, &UDungeonsLoginFlowCrossPlay::OnLoginCompletedCrossPlay));
}

bool UDungeonsLoginFlowCrossPlay::HasLinkingSucceeded(ELoginResult loginResult, int localPlayerIndex)
{
	return loginResult == ELoginResult::Success && localPlayerIndex == 0;
}

void UDungeonsLoginFlowCrossPlay::OnLoadSaveDataComplete()
{
	//Once save data is loaded do the proper platform login (online).
	if (auto GameInstance = GetGameInstance())
	{
		GetGameInstance()->OnSaveStateLoaded.RemoveDynamic(this, &UDungeonsLoginFlowCrossPlay::OnLoadSaveDataComplete);

		// D11.PS - Initial boot will go through its own flow
		// Do not offer any PSN/NSO sign in to local players, who have already skipped the sign in
		bool isInitialSetup = GetGameInstance()->IsInitialBoot() || IsLocalAccount(LoggingInPlayerController.Get());
		if (isInitialSetup)
		{	
			SetupUniqueNetID();
			BroadcastLoginComplete(ELoginResult::Success);
		}
		else
		{
			CrossPlayLogin(LoggingInPlayerController.Get());
		}
	}
	else
	{
		checkNoEntry();
		BroadcastLoginComplete(ELoginResult::InternalError);
	}
}

void UDungeonsLoginFlowCrossPlay::CancelMsaLoginUI()
{
#ifdef	HAS_MSA_LINKING_DELEGATES
	FCoreDelegates::OnMsaLinkingCancelled.Broadcast();
#endif
}

ELoginResult UDungeonsLoginFlowCrossPlay::SetLoginUserCrossplay(TSharedPtr<const FUniqueNetId> UniqueId, int LocalPlayerNum) {
	ensureMsgf(UniqueId.IsValid(), TEXT("Cannot set the login user: Net id is invalid"));
	if (UniqueId.IsValid())
	{
		return SetLoginUser(online::getIdentityInterface()->GetUniqueNetIdFromType(*UniqueId, { SubsystemType::PS4, SubsystemType::Dungeons }), LocalPlayerNum);		
	}
	return ELoginResult::InternalError;
}

void UDungeonsLoginFlowCrossPlay::SetupUniqueNetID()
{
	int localUserNum = GetPlayerControllerId();
	auto identityIF = online::getIdentityInterface();
	TSharedPtr<const FUniqueNetId> UniqueID = identityIF->GetUniquePlayerId(localUserNum);
	if (!UniqueID)
	{			
		identityIF->TryLocalPlayerLogin(localUserNum, GetGameInstance()->OnLoginCompleteDelegate, GetGameInstance()->OnLoginCompleteDelegateHandle);
		return;				
	}
	SetLoginUser(identityIF->GetUniqueNetIdFromType(*UniqueID, 
		{ SubsystemType::PS4, SubsystemType::Dungeons }), localUserNum);
}