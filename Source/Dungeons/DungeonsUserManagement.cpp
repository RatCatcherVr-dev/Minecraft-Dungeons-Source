#include "DungeonsUserManagement.h"
#include "Engine/LocalPlayer.h"
#include "online/sessions/OnlineUtil.h"
#include "DungeonsGameInstance.h"
#include "online/crossplay/CrossplayOSS.h"
#include "online/crossplay/ExternalUI.h"
#include "DungeonsLoginFlow.h"

#if PLATFORM_SWITCH
#include "Switch/SwitchPlatformMisc.h"
#include "CoreDelegates.h"
#endif
#include "platform/GameVersion.h"
#include "game/actor/character/player/DungeonsLocalPlayer.h"
#include "SharedConstants.h"
#include <Themida/Anticheat.hpp>

#include "IDungeonsAuth.h"

UDungeonsUserManager* UDungeonsUserManager::spInstance;

UDungeonsUserManager* UDungeonsUserManager::Instance()
{
	return spInstance;
}

UDungeonsUserManager::UDungeonsUserManager(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{
	spInstance = this;
}

UDungeonsGameInstance *UDungeonsUserManager::GetGameInstance()
{
	if (auto World = GetWorld())
	{
		return World->GetGameInstance<UDungeonsGameInstance>();
	}
	return nullptr;
}


void UDungeonsUserManager::Logout(int32 localUserNum) {
	auto customIdentity = online::getIdentityInterface();
	if (!customIdentity.IsValid()) {
		FFrame::KismetExecutionMessage(TEXT("No identity interface available"), ELogVerbosity::Warning);
		return;
	}

	online::removeLiveOps(GetGameInstance(), localUserNum);
	customIdentity->Logout(localUserNum);
}

int32 UDungeonsUserManager::GetInitialUser()
{
	//D11.PS - When coming back from standby if this is called too early it can cause a crash because the world is null.
	if (auto GameInstance = GetGameInstance())
	{
		if (GameInstance->GetNumLocalPlayers() > 0)
		{
			if (auto LocalPlayer = GetGameInstance()->GetLocalPlayerByIndex(0))
			{
				return LocalPlayer->GetControllerId();
			}
		}
	}

	UE_LOG(LogPlayerManagement, Error, TEXT("No initial player, this shouldn't happen."));
	return -1;
}

int32 UDungeonsUserManager::GetInitialUserSystemId()
{
	if (GetGameInstance()) {
		if (GetGameInstance()->GetNumLocalPlayers() > 0)
		{
			if (auto LocalPlayer = GetGameInstance()->GetLocalPlayerByIndex(0))
			{
				return Cast<UDungeonsLocalPlayer>(LocalPlayer)->GetSystemUserId();
			}
		}
	}

	UE_LOG(LogPlayerManagement, Error, TEXT("No initial player, this shouldn't happen."));
	return -1;
}

APlayerController* UDungeonsUserManager::GetPlayerControllerFromControllerID(int ControllerId)
{
	if (auto LocalPlayer = GetGameInstance()->FindLocalPlayerFromControllerId(ControllerId))
	{
		return LocalPlayer->GetPlayerController(GetWorld());
	}
	return nullptr;
}


void UDungeonsUserManager::Print(FString str)
{
	UE_LOG(LogPlayerManagement, Log, TEXT("PrintLog: %s"), *str);
}

void UDungeonsUserManager::ResetLocalPlayers()
{
	uint8 ControllerIndex = -1;
	for (auto localPlayer : GetLocalPlayers())
	{
		Logout(localPlayer->GetControllerId());

		localPlayer->SetCachedUniqueNetId(nullptr);
#if !PLATFORM_XBOXONE
		ControllerIndex++;
#endif
		localPlayer->SetControllerId(ControllerIndex);
	}
}

void UDungeonsUserManager::AddUser(int ControllerId, APlayerController* PlayerController)
{
	OnUserAddedDelegate.Broadcast(ControllerId, PlayerController);
}

//D11.KS - We will handle creating local players here.
APlayerController* UDungeonsUserManager::CreatePlayer(int32 ControllerID, bool bSpawnPawn)
{
	FString Error;

	ULocalPlayer* LocalPlayer = GetGameInstance()->CreateLocalPlayer(ControllerID, Error, bSpawnPawn);
	if (Error.Len() > 0)
	{
		UE_LOG(LogPlayerManagement, Error, TEXT("Failed to Create Player: %s"), *Error);
	}

	if (LocalPlayer)
	{
		OnAddedLocalPlayer.Broadcast();
		GetGameInstance()->localPlayersThatCanLogin.Add(LocalPlayer);

		return LocalPlayer->GetPlayerController(GetWorld());
	}

	return nullptr;
}

//D11.KS - We will handle removing local players here.
bool UDungeonsUserManager::RemovePlayer(int32 ControllerID)
{
	ULocalPlayer* const ExistingPlayer = GetGameInstance()->FindLocalPlayerFromControllerId(ControllerID);

	if (ExistingPlayer != NULL)
	{
		APlayerController* pc = ExistingPlayer->GetPlayerController(GetWorld());

		Logout(ExistingPlayer->GetControllerId());

		if (pc->GetPawn() != NULL)
		{
			pc->GetPawn()->Destroy();
		}

		if (pc != NULL)
		{
			pc->Destroy();
		}

		GetGameInstance()->localPlayersThatCanLogin.Remove(ExistingPlayer);

		if (GetGameInstance()->RemoveLocalPlayer(ExistingPlayer)) {
			OnRemovedLocalPlayer.Broadcast();
			return true;
		}
	}

	return false;
}

void UDungeonsUserManager::RemoveExtraLocalPlayers()
{
	auto& localPlayers = GetWorld()->GetGameInstance()->GetLocalPlayers();

	for (int i = 1; i < localPlayers.Num(); i++)
	{
		GetWorld()->GetGameInstance()->RemoveLocalPlayer(localPlayers[i]);
		OnRemovedLocalPlayer.Broadcast();
	}
}

int UDungeonsUserManager::GetLocalPlayerIndexFromPlayerController(APlayerController* controller)
{
	if (controller != nullptr && controller->GetLocalPlayer() != nullptr) {
		return GetLocalPlayerIndex(controller->GetLocalPlayer());
	}
	else {
		return PLATFORMUSERID_NONE;
	}
}

int UDungeonsUserManager::GetLocalPlayerIndex(ULocalPlayer* localPlayer)
{
	return GetGameInstance()->GetLocalPlayers().Find(localPlayer);
}

TArray<ULocalPlayer*> UDungeonsUserManager::GetLocalPlayers()
{
	TArray<ULocalPlayer*> players;

	for (auto * player : GetGameInstance()->GetLocalPlayers())
	{
		if (player->GetPlayerController(GetWorld()) && player->GetPlayerController(GetWorld())->IsLocalPlayerController()) {
			players.Add(player);
		}
	}


	return  players;
}

//D11.KS - Really we should have had this one originally, we can cast after getting, I can't replace the other two functions as they are used in way too many blueprints.
TArray<APlayerControllerBase*> UDungeonsUserManager::GetAllLocalPlayerControllers()
{
	TArray<APlayerControllerBase*> players;
	players.Reserve(4);

	for (auto* player : GetLocalPlayers()) {
		if (auto* controller = player->GetPlayerController(GetWorld())) {
			players.Add(Cast<APlayerControllerBase>(controller));
		}
	}

	return players;
}

//D11.KS - Marked as deprecated, would like to merge both this and GetLocalMenuPlayerControllers into one. Can we use GetAllLocalPlayerControllers instead please.
TArray<ABasePlayerController*> UDungeonsUserManager::GetLocalPlayerControllers()
{
	TArray<ABasePlayerController*> players;

	for (auto* player : GetLocalPlayers()) {
		if (auto* controller = player->GetPlayerController(GetWorld())) {
			players.Add(Cast<ABasePlayerController>(controller));
		}
	}

	return players;
}

TArray<ABaseMenuPlayerController*> UDungeonsUserManager::GetLocalMenuPlayerControllers()
{
	TArray<ABaseMenuPlayerController*> players;

	for (auto* player : GetLocalPlayers()) {
		if (auto* controller = Cast<ABaseMenuPlayerController>(player->GetPlayerController(GetWorld()))) {
			players.Add(controller);
		}
	}

	return players;
}


ULocalPlayer* UDungeonsUserManager::GetInitialLocalPlayer() const
{
	if (auto world = GetWorld())
	{		
		if (auto instance = world->GetGameInstance())
		{
			return instance->GetNumLocalPlayers() ? instance->GetLocalPlayerByIndex(0) : nullptr;
		}
	}
	return nullptr;
}

APlayerController* UDungeonsUserManager::GetInitialPlayerController() const
{
	/* The first controller in the controller iterator can be a defunct player controller who is awaiting server destruction
	   on clients when traveling. This means we cannot trust that the first controller which IsLocallyControlled() is actually
	   the controller of the first local player. It will be LocallyControlled, but lacking a local player. To the best of our
	   knowledge, only *one* playercontroller can point to a UPlayer. Thus we find ourselves at this fix: We should check for a
	   LocalPlayer, to ensure we actually get the first local player controller.
	*/
	if(auto* world = GetWorld()) {
		for (auto pcIterator = world->GetPlayerControllerIterator(); pcIterator; ++pcIterator) {
			auto playerController = pcIterator->Get();
			if (playerController && playerController->GetLocalPlayer()) {
				return playerController;
			}
		}
	}
	return nullptr;
}

void UDungeonsUserManager::InitialUserSignedOut()
{
	ResetLocalPlayers();
	OnUserSignedOutDelegate.Broadcast(GetInitialUser());
}

FString UDungeonsUserManager::GetLocalUserName(int ControllerId)
{
	const auto identity = online::getIdentityInterface();
	if (!identity.IsValid()) {
		UE_LOG(LogOnline, Log, TEXT("(OSS) Identity interface is not available"));
		return UKismetSystemLibrary::GetPlatformUserName();
	}

	if (online::usingNullSubsystem(GetWorld())) {
		return UKismetSystemLibrary::GetPlatformUserName();
	}

	FString identityUsername = identity->GetPlayerNickname(ControllerId);

	if (
#if !PLATFORM_SWITCH && !PLATFORM_PS4
		!identity->GetUniquePlayerId(ControllerId).IsValid() || 
#endif
		identityUsername.IsEmpty())
	{
		int32 initialControllerId = GetGameInstance()->GetLocalPlayerByIndex(0)->GetControllerId();

		//Should be impossible for P1 to not be signed in when playing, but just incase.
		if (ControllerId == initialControllerId)
		{
			identityUsername = "Default";
		}
		else
		{
			int LocalSystemUserID = GetLocalUserSystemIdFromControllerID(ControllerId);
			identityUsername = FString::Printf(TEXT("%s(%i)"), *GetLocalUserName(initialControllerId).Left(12), LocalSystemUserID);
		}
	}

	return identityUsername;
}

FString UDungeonsUserManager::GetSwitchOfflineUserName(int ControllerId)
{
#if PLATFORM_SWITCH
	return FSwitchPlatformMisc::GetPlayerSwitchName(ControllerId);
#endif
	return FString();
}

// D11.SSN
int UDungeonsUserManager::GetLocalUserNumFromControllerID(int ControllerId) {
	for (auto player : GetLocalPlayers()) {
		if (player->GetControllerId() == ControllerId) {
			return GetLocalPlayerIndexFromPlayerController(player->GetPlayerController(GetWorld()));
		}
	}
	return -1;
}

int UDungeonsUserManager::GetLocalUserSystemIdFromControllerID(int ControllerId)
{
	for (auto player : GetLocalPlayers()) {
		if (player->GetControllerId() == ControllerId) {
			return Cast<UDungeonsLocalPlayer>(player)->GetSystemUserId();
		}
	}
	return -1;
}

// D11.DJB
int32 UDungeonsUserManager::GetServiceInitialUserLocalIndex()
{
	int32 userId = GetGameInstance()->GetFirstGamePlayer()->GetControllerId();
#if PLATFORM_PS4
#ifdef REQUEST_PS4_FIRST_CONTROLLER
	const auto OnlineIdentityInterface = online::getIdentityInterface();
#endif // REQUEST_PS4_FIRST_CONTROLLER
#endif // PLATFORM_PS4
	return userId;
}




//D11.PC
void UDungeonsUserManager::OnLoginRequestEnded(int LocalPlayerIndex, APlayerController* PlayerController, ELoginResult Result)
{
	ELocalUserLoginResult LoginRes = ELocalUserLoginResult::Success;

#if PLATFORM_XBOXONE // Only xbox will have problems with users D11.PS - is this really needed?
	switch (Result)
	{
	case ELoginResult::UserAlreadyInGame:
		LoginRes = ELocalUserLoginResult::UserAlreadyInGame;
		break;
	case ELoginResult::Success:
		LoginRes = LocalPlayerIndex != -1 ? ELocalUserLoginResult::Success : ELocalUserLoginResult::Error;
		break;
	default:
		LoginRes = ELocalUserLoginResult::Error;
		break;
	}
#endif

	OnLoginRequestComplete.Broadcast(UGameplayStatics::GetPlayerControllerID(PlayerController), LoginRes);
	GetGameInstance()->GetLoginFlow()->OnLoginComplete.RemoveDynamic(this, &UDungeonsUserManager::OnLoginRequestEnded);
}


void UDungeonsUserManager::TryPlayerControllerPlatformLogin(APlayerController* PlayerController)
{
	if (PlayerController)
	{
		int ControllerId = PlayerController->GetLocalPlayer()->GetControllerId();
		OnLoginRequest.Broadcast(ControllerId);

 		GetGameInstance()->GetLoginFlow()->OnLoginComplete.AddUniqueDynamic(this, &UDungeonsUserManager::OnLoginRequestEnded);
		GetGameInstance()->GetLoginFlow()->LocalPlayerLogin(ControllerId, PlayerController);
	}
}