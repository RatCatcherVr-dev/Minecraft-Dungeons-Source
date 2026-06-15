#include "Dungeons.h"
#include "ReconnectComponent.h"
#include "DungeonsGameInstance.h"
#include "lovika/LovikaLevelActor.h"
#include "TeleportComponent.h"
#include "online/sessions/FindDungeonsSessionCallbackProxy.h"
#include "online/sessions/JoinDungeonsSessionCallbackProxy.h"
#include "online/sessions/SessionSettings.h"
#include "online/sessions/OnlineUtil.h"
#include "online/reconnect/ReconnectUtil.h"

#include "game/actor/character/player/BasePlayerController.h"
#include "CharacterSerializeComponent.h"
#include "../actor/character/player/DungeonsLocalPlayer.h"
#include "DungeonsUserManagement.h"

namespace Reconnect {
	const float MAXIMUM_TIME_RESPONSE_IS_VALID = 15.0f;
	const float SESSION_UPDATE_INTERVAL = 10.0f;
};

UReconnectComponent::UReconnectComponent() {
	SetIsReplicated(false);
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bAllowTickOnDedicatedServer = false;
	OnFindSessionDoneDelegate = FOnFindSessionDoneDelegate::CreateUObject(this, &UReconnectComponent::OnFindSessionsComplete);
}

void UReconnectComponent::BeginPlay() {
	Super::BeginPlay();
	Instance = Cast<UDungeonsGameInstance>(GetOwner()->GetGameInstance());
	if (!IsLocal()) {
		PrimaryComponentTick.bCanEverTick = false;
		return;
	}
	int initialUserControllerId = Cast<UDungeonsLocalPlayer>(Instance->GetLocalPlayerByIndex(0))->GetSystemUserId();
	if (!Instance->ReadGlobalSaveState(initialUserControllerId)) {
		Instance->OnSaveStateLoaded.AddDynamic(this, &UReconnectComponent::OnSaveStateLoaded);
	} else 	{
		OnSaveStateLoaded();
	}
}

void UReconnectComponent::OnSaveStateLoaded() {
	auto& reconnectSaveData = GetReconnectSaveData();

	int32 initialUserControllerId = Instance->GetUserManager()->GetInitialUserSystemId();
	if (IsClientInGame()) {
		if (const AActor* owner = GetOwner()) {
			if (!owner->HasAuthority()) {
				const auto& sid = online::getCurrentSessionId();
				reconnectSaveData.set(sid.Get(""));


				Instance->EditGlobalSaveState(initialUserControllerId)->Save(initialUserControllerId);
				UE_LOG(LogMultiplayer, Log, TEXT("Reconnect: Saving reconnect information, sessionId='%s'."), *sid.Get("n/a"));
			}
		}
	}
	else if (Instance->IsSaveStateValid(initialUserControllerId) && GetReconnectSaveData().isSet()) {
		UE_LOG(LogMultiplayer, Log, TEXT("Reconnect: Loaded reconnect information from file (sessionId='%s')."), *GetReconnectSaveData().sessionId);
	}
	else {
		UE_LOG(LogMultiplayer, VeryVerbose, TEXT("Reconnect: There's no reconnect information to load."));
	}
	SetComponentTickEnabled(true);	
}

void UReconnectComponent::EndPlay(const EEndPlayReason::Type reason) {
	UE_LOG(LogMultiplayer, Log, TEXT("Reconnect: UReconnectComponent::EndPlay"));
	Super::EndPlay(reason);

	Instance->OnSaveStateLoaded.RemoveDynamic(this, &UReconnectComponent::OnSaveStateLoaded);
	Instance->GetFindSessionHandler()->RemoveCompleteHandle(OnFindSessionDoneDelegateHandle);
	OnFindSessionDoneDelegateHandle.Reset();

	if (GetWorld() == nullptr) {
		return;
	}
	if (!IsLocal()) {
		return;
	}
	if (!IsClientInGame()) {
		return;
	}
	const auto game = actorquery::getGame(GetWorld());
	if (game != nullptr && game->isCompleted()) {
		ClearReconnect();
	}
}

void UReconnectComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	auto userManagement = GetWorld()->GetGameInstance<UDungeonsGameInstance>()->GetUserManager();
	if (!IsLocal() || IsClientInGame() || userManagement->GetInitialPlayerController() != GetOwner() || !IsLoggedIn()) {
		return;
	}
	UpdateSession();
	TimeSinceLastCheck += DeltaTime;
	TimeSinceLastResponse += DeltaTime;
}

void UReconnectComponent::UpdateSession() {
	if (OnFindSessionDoneDelegateHandle.IsValid()) {
		//Already registered for FindSession
		return;
	}

	int32 initialUserControllerId = Instance->GetUserManager()->GetInitialUserSystemId();
	if (!Instance->IsSaveStateValid(initialUserControllerId) || !GetReconnectSaveData().isSet()) {
		return;
	}

	if (ResponseCount == 0 || TimeSinceLastCheck >= Reconnect::SESSION_UPDATE_INTERVAL) {
		UE_LOG(LogMultiplayer, Log, TEXT("Reconnect: Looking for the session to reconnect to: %s"), *GetReconnectSaveData().sessionId);
		OnFindSessionDoneDelegateHandle = Instance->GetFindSessionHandler()->RequestFindSession(OnFindSessionDoneDelegate);
		TimeSinceLastCheck = 0;
	}
}

bool UReconnectComponent::CanReconnect() const {
	return TimeSinceLastResponse < Reconnect::MAXIMUM_TIME_RESPONSE_IS_VALID && ClientCanReconnect;
}

bool UReconnectComponent::IsCrossplaySession() const {
	return SessionSearchResult.IsSet() &&
			HasGuid() &&
			SessionSearchResult->OnlineResult.Session.OwningUserId->GetType().IsEqual(FName(TEXT("DUNGEONS")));
}

void UReconnectComponent::PrivilageCheckComplete(const FUniqueNetId& UserId, EUserPrivileges::Type Privilege, uint32 PrivilegeResults)
{
	if (PrivilegeResults == static_cast<uint32>(IOnlineIdentity::EPrivilegeResults::NoFailures))
	{
		UE_LOG(LogMultiplayer, Log, TEXT("Reconnect: Has session result, rejoining session."));
		SessionSettings ss(SessionSearchResult.GetValue());
		Instance->GetReconnectStates().Connect(GetGuid());
		JoinSessionProxy = UJoinDungeonsSessionCallbackProxy::JoinDungeonsSession(GetWorld(), GetWorld()->GetFirstPlayerController(), SessionSearchResult.GetValue());
		JoinSessionProxy->Activate();
	}
	else
	{
		Instance->OnPrivilegeError(UserId, PrivilegeResults);
	}
}


void UReconnectComponent::Reconnect() {
	UE_LOG(LogMultiplayer, Log, TEXT("Reconnect: Activated, sessionId: "), *GetReconnectSaveData().sessionId);
	
	if (SessionSearchResult.IsSet()) {
		if(HasGuid()){
			Instance->StartOnlinePrivilegeTask(IOnlineIdentity::FOnGetUserPrivilegeCompleteDelegate::CreateUObject(this, &UReconnectComponent::PrivilageCheckComplete), EUserPrivileges::CanPlayOnline, Instance->GetUserManager()->GetInitialLocalPlayer()->GetCachedUniqueNetId().GetUniqueNetId());
		}else {
			UE_LOG(LogMultiplayer, Warning, TEXT("Reconnect: Can't reconnect with no Guid set."));
		}
	} else {
		UE_LOG(LogMultiplayer, Log, TEXT("Reconnect: No session result - traveling without joining session."));
	}
}

bool UReconnectComponent::HasGuid() const {
	return !GetGuid().IsEmpty();
}

FString UReconnectComponent::GetGuid() const {
	return reconnect::getGuid(Cast<APlayerController>(GetOwner()));
}

bool UReconnectComponent::IsLocal() const {
	if (Instance && Instance->IsDedicatedServerInstance()) {
		return false;
	}
	return Cast<APlayerController>(GetOwner())->IsLocalController();
}

bool UReconnectComponent::IsLoggedIn() const {
	return Instance && Instance->GetUserManager() && online::getIdentityInterface()->GetLoginStatus(Instance->GetUserManager()->GetInitialUserSystemId()) == ELoginStatus::LoggedIn;
}

ReconnectSaveData& UReconnectComponent::GetReconnectSaveData() const {
	// Reconnect component stores globally per physical device.
	UDungeonsGameInstance* GameInstance = GetWorld()->GetGameInstance<UDungeonsGameInstance>();
	int32 initialUserControllerId = GameInstance->GetUserManager()->GetInitialUserSystemId();
	UGlobalStateData* globalStateData = GameInstance->EditGlobalSaveState(initialUserControllerId);
	if (!globalStateData)
	{
		// D11.DH
		// Return a default reconnect save data here to prevent a crash which happens on suspend on xbox
		static auto defaultReconnectData = ReconnectSaveData();
		return defaultReconnectData;
	}

	return globalStateData->GetReconnectState();
}

void UReconnectComponent::ClearReconnect() {
	if (!Instance) { // Player can get kicked before beginplay
		Instance = Cast<UDungeonsGameInstance>(GetOwner()->GetGameInstance());
		if (!Instance)
			return;
	}
	if (!IsLocal()) {
		return;
	}
	UE_LOG(LogMultiplayer, Log, TEXT("Reconnect: Clearing reconnect save data."));
	GetReconnectSaveData().clear();
	
	SessionSearchResult.Reset();
	ClientCanReconnect = false;
}

bool UReconnectComponent::IsClientInGame() const {
	return actorquery::getFirstActor<ALovikaLevelActor>(GetWorld()) != nullptr;
}

void UReconnectComponent::OnFindSessionsComplete(bool bWasSuccessful, TSharedPtr<FOnlineSessionSearch> SessionResult) {
	ResponseCount++;
	ClientCanReconnect = false;
	TimeSinceLastResponse = 0;
	OnFindSessionDoneDelegateHandle.Reset();

	SessionSearchResult.Reset();
	if (bWasSuccessful) {
		UE_LOG(LogMultiplayer, Log, TEXT("Reconnect: Sessions retrieved. Looking for session id %s"), *GetReconnectSaveData().sessionId);
		for (const auto& searchResult : SessionResult->SearchResults) {

			auto reconnectSaveData = GetReconnectSaveData();
			if (!reconnectSaveData.isSet()) {
				continue;
			}

			if (!searchResult.IsValid() || !searchResult.IsSessionInfoValid()) {
				continue;
			}

			if (searchResult.GetSessionIdStr() == reconnectSaveData.sessionId) {
				FailedResponseCount = 0;
				UE_LOG(LogMultiplayer, Log, TEXT("Reconnect: The reconnect session is among the found sessions."));
				FBlueprintSessionResult r;
				r.OnlineResult = searchResult;
				SessionSearchResult = r;
				SessionSettings ss(r);
				ClientCanReconnect = ss.CanReconnect(GetGuid());
				return;
			}
		}
	}

	if (++FailedResponseCount > 3) {
		UE_LOG(LogMultiplayer, Log, TEXT("Reconnect: Three responses without the reconnect session id, stop looking."));
		ClearReconnect();
	}
}
