#include "Dungeons.h"
#include "DownedNightModeComponent.h"
#include "DungeonsGameState.h"
#include "DungeonsGameMode.h"

void UDownedNightModeComponent::BeginPlay() {
	Super::BeginPlay();
	if (const auto gamemode = Cast<ADungeonsGameMode>(GetOwner())) {
		gamemode->OnPlayerCharacterAdded.AddUObject(this, &UDownedNightModeComponent::OnPlayerCharacterAdded);
		gamemode->OnPlayerCharacterRemoved.AddUObject(this, &UDownedNightModeComponent::RefreshNightMode);

		if (const auto dimmer = actorquery::getFirstActor<ADimmer>(GetWorld())) {
			mDimmer = dimmer;
		}
	}
}

void UDownedNightModeComponent::EnterPermaNight() {
	mPermanent = true;
	EnterNight();
}

void UDownedNightModeComponent::OnPlayerCharacterAdded(APlayerCharacter* player) {
	player->OnAliveStateChanged.AddUObject(this, &UDownedNightModeComponent::OnPlayerAliveStateChanged, player);
	RefreshNightMode();
}

void UDownedNightModeComponent::OnPlayerAliveStateChanged(APlayerCharacter* player) {
	RefreshNightMode();
}

void UDownedNightModeComponent::EnterNight() {
	if (mDimmer.IsValid()) {
		if (!mDimmer->IsBecomingNight()) {
			mDimmer->EnterNight();
		}
	}
}

void UDownedNightModeComponent::RefreshNightMode() {
	const auto gamestate = GetGameState();
	if (!gamestate) {
		//Error state - no refresh possible
		return;
	}

	if (mPermanent) {
		return;
	}

	if (mDimmer.IsValid()) {
		const auto downedPlayersCount = gamestate->GetPlayersMatchingAliveStateCount(EAliveState::Down);
		if (downedPlayersCount > 0) {
			if(!mDimmer->IsBecomingNight()){
				if (!mBecomeNightDelayHandle.IsValid() || !GetWorld()->GetTimerManager().IsTimerActive(mBecomeNightDelayHandle)) {
					GetWorld()->GetTimerManager().SetTimer(mBecomeNightDelayHandle, this, &UDownedNightModeComponent::EnterNight, FMath::Max(nightModeDelay, 0.01f), false);
				}				
			}
		} else {
			if (mBecomeNightDelayHandle.IsValid() && GetWorld()->GetTimerManager().IsTimerActive(mBecomeNightDelayHandle)) {
				GetWorld()->GetTimerManager().ClearTimer(mBecomeNightDelayHandle);
			}
			if (!mDimmer->IsBecomingDay()) {
				mDimmer->ExitNight();
			}
		}
	}
}

ADungeonsGameState* UDownedNightModeComponent::GetGameState() const {
	const auto gameState = Cast<ADungeonsGameMode>(GetOwner())->GetGameState<ADungeonsGameState>();
	check(gameState && "Missing GameState in DungeonsGameMode");
	return gameState;
}