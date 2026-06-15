#include "DungeonsLoginFlow.h"
#include "DungeonsGameInstance.h"
#include "DungeonsUserManagement.h"
#include "IDungeonsAuth.h"
#include "PlayfabPlayer.h"
#include "game/actor/character/player/DungeonsLocalPlayer.h"
#include "online/sessions/OnlineUtil.h"
#include "online/crossplay/CrossplayOSS.h"
#include "online/crossplay/ExternalUI.h"
#include "PlayfabServices/Public/PlayfabServices.h"
#include "online/seasons/LiveOps.h"
#include "TimerManager.h"

UDungeonsLoginFlow::UDungeonsLoginFlow(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{

}

void UDungeonsLoginFlow::BeginDestroy() {
	DisableLoginRefresh();
	Super::BeginDestroy();
}

void UDungeonsLoginFlow::PlatformLogin(ULocalPlayer* LocalPlayer)
{
	auto externalUI = online::getExternalUIInterface();
	externalUI->ShowLoginUI(LocalPlayer->GetControllerId(), false, false,
		FOnLoginUIClosedDelegate::CreateUObject(this, &UDungeonsLoginFlow::OnLoginCompleted));
}

void UDungeonsLoginFlow::OnLoginCompleted(TSharedPtr<const FUniqueNetId> UniqueId, int LocalPlayerNum, const FOnlineError& LoginResult)
{
	//Attempt to sign the user in - if the login failed then we assume the login was cancelled for now.
	ELoginResult LoginUserResult = LoginResult.bSucceeded ? SetLoginUser(UniqueId, LocalPlayerNum) : ELoginResult::UserCanceledLogin;

	if (online::IsUsingOnlineFeaturesInPIE()) {
		LoginUserResult = LoginUserResult == ELoginResult::UserAlreadyInGame ? ELoginResult::Success : LoginUserResult;
	}

	if (LoginUserResult != ELoginResult::Success)
	{
		//If the login has been unsuccessful then logout of the OSS
		LogoutOfOss(UniqueId);

		if (LoginResult.GetErrorCode().Equals(LoginFlowConstants::NetworkIssueError))
		{
			LoginUserResult = ELoginResult::NetworkIssue;
		}
		else if (LoginResult.GetErrorCode().Equals(LoginFlowConstants::AlreadyLoggedInUserError))
		{
			//Force the result to cancelled whilst we don't have the string setup. This will result in no dialog box.
			LoginUserResult = ELoginResult::UserCanceledLogin;
		}
	}

#if !PLATFORM_SWITCH // if user cancels login on switch they should be treated as a local player 
	if (LoginUserResult == ELoginResult::UserCanceledLogin)
	{
		SetPlayerControllerId(PLATFORMUSERID_NONE);
	}
#endif
	if (LoginUserResult == ELoginResult::Success)
	{
		if (GetGameInstance()->GetUserManager()->GetInitialPlayerController() == LoggingInPlayerController.Get()) {
			EnableLoginRefresh();
		}
		LoadSaveData(LocalPlayerNum);
	}
	else
	{
		BroadcastLoginComplete(LoginUserResult);
	}
}

UDungeonsGameInstance *UDungeonsLoginFlow::GetGameInstance() const
{
	return Cast<UDungeonsGameInstance>(GetOuter());
}

void UDungeonsLoginFlow::Login(int ControllerId, APlayerController* PlayerController)
{
	LoggingInPlayerController = PlayerController;
	SetPlayerControllerId(ControllerId);
	PlatformLogin(PlayerController->GetLocalPlayer());
}

void UDungeonsLoginFlow::LocalPlayerLogin(int ControllerId, APlayerController* PlayerController)
{
	Login(ControllerId, PlayerController);
}

void UDungeonsLoginFlow::SetPlayerControllerId(int ControllerId)
{
	if (APlayerController *PlayerController = LoggingInPlayerController.Get())
	{
		ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
		LocalPlayer->SetControllerId(ControllerId);
	}
}

int UDungeonsLoginFlow::GetPlayerControllerId()
{
	if (APlayerController *PlayerController = LoggingInPlayerController.Get())
	{
		ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
		return LocalPlayer->GetControllerId();
	}
	return -1;
}

ELoginResult UDungeonsLoginFlow::SetLoginUser(TSharedPtr<const FUniqueNetId> UniqueId, int LocalPlayerIndex)
{
	APlayerController* PlayerController = LoggingInPlayerController.Get();

	ensureMsgf(PlayerController, TEXT("Cannot set the login user: no player controller set"));
	ensureMsgf(UniqueId.IsValid(), TEXT("Cannot set the login user: Net id is invalid"));
	ensureMsgf(LocalPlayerIndex != PLATFORMUSERID_NONE, TEXT("Cannot set the login user: local player index is not set"));

	if (!PlayerController || !UniqueId.IsValid() || LocalPlayerIndex == PLATFORMUSERID_NONE)
		return ELoginResult::InternalError;
	if (auto GameInstance = GetGameInstance())
	{
		if(GameInstance->IsUserAlreadyInGame(UniqueId))
			return ELoginResult::UserAlreadyInGame;
	}

	if (ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
	{
		LocalPlayer->SetCachedUniqueNetId(UniqueId);

		if (UDungeonsLocalPlayer* DungeonsLocalPlayer = Cast<UDungeonsLocalPlayer>(LocalPlayer)) {
			DungeonsLocalPlayer->SetUserSystemId(LocalPlayerIndex);
		}
		if (PlayerController->PlayerState != nullptr) {
			PlayerController->PlayerState->SetUniqueId(UniqueId);
		}
		return ELoginResult::Success;
	}
	return ELoginResult::InternalError;
}

void UDungeonsLoginFlow::LogoutOfOss(TSharedPtr<const FUniqueNetId> UniqueId)
{
	check(UniqueId.IsValid());
	DisableLoginRefresh();

	if (UniqueId.IsValid()) {
		if (const auto IdentityInterface = online::getIdentityInterface()) {
			online::removeLiveOps(Cast<UGameInstance>(GetOuter()), IdentityInterface->GetPlatformUserIdFromUniqueNetId(*UniqueId));
			IdentityInterface->Logout(*UniqueId);
		}
	}
}

void UDungeonsLoginFlow::LoadSaveData(int LocalPlayerIndex)
{
	auto GameInstance = GetGameInstance();
	if (GameInstance)
	{
		GameInstance->OnSaveStateLoaded.AddDynamic(this, &UDungeonsLoginFlow::OnLoadSaveDataCompleted);
		GameInstance->LoadSaveData(LocalPlayerIndex, LoggingInPlayerController.Get());
	}
}

void UDungeonsLoginFlow::OnLoadSaveDataCompleted()
{
	//D11.PS - maybe this could fail here?
	BroadcastLoginComplete(ELoginResult::Success);
	if (auto GameInstance = GetGameInstance())
	{
		GameInstance->OnSaveStateLoaded.RemoveDynamic(this, &UDungeonsLoginFlow::OnLoadSaveDataCompleted);
	}
}

void UDungeonsLoginFlow::DoPostLoginAction() {
	if (PostLoginAction.IsBound())
	{
		PostLoginAction.Execute();
		PostLoginAction.Unbind();
	}
}

void UDungeonsLoginFlow::RefreshLogin() const {
	GetLoginRefreshFunction()();
}

void UDungeonsLoginFlow::EnableLoginRefresh() {
#if !PLATFORM_WINDOWS || !UE_BUILD_SHIPPING
	GetGameInstance()->GetEntitlementsRepository()->RequestEntitlements();
#endif		
	if (auto gameInstance = GetGameInstance()) {
		if (auto world = gameInstance->GetWorld()) {
			world->GetTimerManager().SetTimer(PeriodicLoginRefreshHandle, GetLoginRefreshFunction(), AUTHENTICATION_REFRESH_PERIOD_SECONDS, true, 0.0);
		}
	}

	ResumptionLoginRefreshHandle = FCoreDelegates::ApplicationHasEnteredForegroundDelegate.AddLambda(GetLoginRefreshFunction());
}

void UDungeonsLoginFlow::DisableLoginRefresh() {
	if (auto GameInstance = GetGameInstance()) {
		if (auto world = GameInstance->GetWorld()) {
			world->GetTimerManager().ClearTimer(PeriodicLoginRefreshHandle);
		}
	}

	FCoreDelegates::ApplicationHasEnteredForegroundDelegate.Remove(ResumptionLoginRefreshHandle);
}

void UDungeonsLoginFlow::BroadcastLoginComplete(ELoginResult Result)
{
	APlayerController* PlayerController = LoggingInPlayerController.Get();
	ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
	OnLoginComplete.Broadcast(LocalPlayer->GetControllerId(), PlayerController, Result);
	LoggingInPlayerController = nullptr;
}

std::function<void()> UDungeonsLoginFlow::GetLoginRefreshFunction() const {
	return [&, gameInstance = GetGameInstance()] () {
#if defined(SUBSYSTEM_HAS_AUTH_TOKEN_FUNCTIONS)
		auto identityInterface = online::getCrossplayOss()->GetIdentityIF();
		identityInterface->AddAuthToken();
		auto playerController = gameInstance->GetUserManager()->GetInitialPlayerController();
		identityInterface->GetAuthTokenAsync("playfab_key", [ContrId = playerController->GetLocalPlayer()->GetControllerId(), GameInstance = GetGameInstance()](FString token, FString issuerId) {
			auto callback = [ContrId, GameInstance, token](bool success) {
				if (success) {
					if (IDungeonsAuth::IsAvailable()) {
						auto auth = IDungeonsAuth::Get().Auth();
						TSharedPtr<const FUniqueNetId> PrimaryId = nullptr;
						if (online::getCrossplayOss()->IsPS4Active()) {
							PrimaryId = online::getIdentityInterface()->GetUniquePlayerId(ContrId, PS4_SUBSYSTEM);
						} else {
							PrimaryId = online::getIdentityInterface()->GetUniquePlayerId(ContrId);
						}

						check(PrimaryId && "UniquePlayer should not be null!");
						if (PrimaryId) {
							auto* PfPlayer = PlayfabServices::GetPlayfabPlayer();
							const FString PlayfabToken = PfPlayer ? PfPlayer->GetSessionTicket() : "";

							if (auth && PrimaryId && !PlayfabToken.IsEmpty()) {
								online::createLiveOps(GameInstance);
								auth->Authenticate(XAuthData(PrimaryId->ToString(), PlayfabToken, token, online::GetMinecraftAPINamespace()));
							}
						}
					}
				}
			};
			PlayfabServices::RefreshAuthenticationCredentials({ token, issuerId, callback });
		});
#endif
	};
}

#if !UE_BUILD_SHIPPING && (PLATFORM_PS4 || PLATFORM_SWITCH)
void UDungeonsLoginFlow::SetDebugLoginResult(ELoginResult r) { debugLoginResult = r; }

bool UDungeonsLoginFlow::DebugLoginResultSet() { return debugLoginResult != ELoginResult::Success; }

void UDungeonsLoginFlow::ClearDebugLoginResult() { debugLoginResult = ELoginResult::Success; }
#endif