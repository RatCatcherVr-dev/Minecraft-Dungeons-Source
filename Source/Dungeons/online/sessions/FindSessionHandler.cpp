#include "FindSessionHandler.h"
#include "Ticker.h"
#include "online/sessions/OnlineUtil.h"
#include "game/actor/character/player/BasePlayerController.h"

UFindSessionHandler::UFindSessionHandler()
	: OnFindSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &UFindSessionHandler::OnFindSessionsComplete)) {
	TickDelegate = FTickerDelegate::CreateUObject(this, &UFindSessionHandler::Tick);
	ShouldFindSession = false;
	FindSessionIsActive = false;
}

UFindSessionHandler::~UFindSessionHandler() {
	FTicker::GetCoreTicker().RemoveTicker(TickTimerHandle);
	if (TickDelegate.IsBound()) {
		TickDelegate.Unbind();
	}
}

FDelegateHandle UFindSessionHandler::RequestFindSession(const FOnFindSessionDoneDelegate& InDelegate) {
	check(IsInGameThread());

	if (!TickTimerHandle.IsValid()) {
		TickTimerHandle = FTicker::GetCoreTicker().AddTicker(TickDelegate);
	}

	if (!FindSessionIsActive) {
		ShouldFindSession = true;
	}
	return OnFindSessionDone.Add(InDelegate);
}

void UFindSessionHandler::RemoveCompleteHandle(FDelegateHandle Handle) {
	check(IsInGameThread());
	OnFindSessionDone.Remove(Handle);
}

bool UFindSessionHandler::Tick(float delay) {
	check(IsInGameThread());

	if (ShouldDoFindSession()) {
		DoFindSession();
	}
	return true;
}

bool UFindSessionHandler::ShouldDoFindSession() const {
#if defined(SUBSYSTEM_HAS_FINDSESSION_CHECK)
	if (online::getSessionInterface()->FindSessionIsActive()) {
		return false;
	}
#endif
	return ShouldFindSession && !FindSessionIsActive;
}

void UFindSessionHandler::DoBroadcast(bool bWasSuccessful) {
	ShouldFindSession = false;
	OnFindSessionDone.Broadcast(bWasSuccessful, bWasSuccessful ? CurrentSessionSearch : TSharedPtr<FOnlineSessionSearch>({}));
	OnFindSessionDone.Clear();
}

void UFindSessionHandler::DoFindSession() {
	if (const auto* NetId = GetUniqueNetId()) {
		auto OnlineSession = online::getSessionInterface();
		OnFindSessionsCompleteDelegateHandle = OnlineSession->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegate);
		CurrentSessionSearch = MakeShared<FOnlineSessionSearch>();
		FindSessionIsActive = true;
		OnlineSession->FindSessions(*NetId, CurrentSessionSearch.ToSharedRef());
	}
	else {
		DoBroadcast(false);
	}
}

void UFindSessionHandler::OnFindSessionsComplete(bool bWasSuccessful) {
	check(IsInGameThread());
	auto OnlineSession = online::getSessionInterface();
	OnlineSession->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);
	DoBroadcast(bWasSuccessful);
	FindSessionIsActive = false;
}

const ABasePlayerController* UFindSessionHandler::GetPlayerController() {

	if (const UWorld* world = GetWorld()) {
		if (const ULocalPlayer* localPlayer = world->GetFirstLocalPlayerFromController()) {
			return (ABasePlayerController*)localPlayer->PlayerController;
		}
	}

	UE_LOG(LogMultiplayer, Error, TEXT("UFindDungeonsSessionCallbackProxy: Unable to get primary PlayerController."));
	return nullptr;
}

const FUniqueNetId* UFindSessionHandler::GetUniqueNetId() {
	if (const ABasePlayerController* playerController = GetPlayerController()) {
		const TSharedPtr<const FUniqueNetId> identityNetId = online::getIdentityInterface()->GetUniquePlayerId(playerController->GetLocalPlayerIndex());
		if (identityNetId.IsValid() && identityNetId->IsValid()) {
			return identityNetId.Get();
		}
	}
	UE_LOG(LogMultiplayer, Error, TEXT("UFindSessionHandler: Unable to get UniqueNetId from PlayerController."));
	return nullptr;
}

