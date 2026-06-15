#include "Dungeons.h"
#include "RespawnAsTeamUsingLivesComponent.h"
#include "DungeonsGameState.h"
#include "DungeonsGameMode.h"
#include "game/Game.h"
#include "game/LevelSettings.h"
#include "ui/MissionProgressHandler.h"

URespawnAsTeamUsingLivesComponent::URespawnAsTeamUsingLivesComponent() {
	bWantsInitializeComponent = true;
}

void URespawnAsTeamUsingLivesComponent::InitializeComponent() {
	Super::InitializeComponent();
	if (const auto gamemode = Cast<ADungeonsGameMode>(GetOwner())) {
		gamemode->OnPlayerCharacterAdded.AddUObject(this, &URespawnAsTeamUsingLivesComponent::OnPlayerCharacterAdded);
		gamemode->OnPlayerCharacterRemoved.AddUObject(this, &URespawnAsTeamUsingLivesComponent::RefreshTeamDeathState);
		gamemode->OnInitDifficulty.AddUObject(this, &URespawnAsTeamUsingLivesComponent::InitDifficultyLives);
	}
	else {
		check(false && "This component is only valid on an ADungeonsGameMode actor");
	}
}

void URespawnAsTeamUsingLivesComponent::EndPlay(EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);
	GetGameState()->SetShowLives(false);
	GetGameState()->SetIsGameOver(false);
}

bool URespawnAsTeamUsingLivesComponent::HasPlayerForcedDownState(UWorld* world, APlayerCharacter* player) {
	return player->GetAbilitySystemComponent()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("ForcedDownState"));
}

bool URespawnAsTeamUsingLivesComponent::HasAnyPlayerForcedDownState(UWorld* world) {
	for (auto&& player : actorquery::getActors<APlayerCharacter>(world)) {
		return HasPlayerForcedDownState(world, player);
	}
	return false;
}

void URespawnAsTeamUsingLivesComponent::InitDifficultyLives(const FLevelSettings& levelSettings) {
	const auto game = actorquery::getGame(GetWorld());
	const auto gameState = GetGameState();

	if (game->missionDef().isInfiniteLives()) {
		gameState->SetLives(1000000000);
		gameState->SetShowLives(false);
	} else {
		const auto lives = [&] {
			const auto unmodifiedLives = game->affectors().GetStartupLives(levelSettings.getDifficulty());
			return FMath::Clamp(unmodifiedLives - levelSettings.missionState.livesLost, 0, unmodifiedLives);
		}();
		gameState->SetLives(lives);
		gameState->SetShowLives(true);
	}

	LevelSettings = levelSettings;
}

void URespawnAsTeamUsingLivesComponent::OnAffectorsChanged()
{
	InitDifficultyLives(LevelSettings);
}

void URespawnAsTeamUsingLivesComponent::OnPlayerCharacterAdded(APlayerCharacter* player) {
	player->OnAliveStateChanged.AddUObject(this, &URespawnAsTeamUsingLivesComponent::OnPlayerAliveStateChanged, player);
	RefreshTeamDeathState();
}

void URespawnAsTeamUsingLivesComponent::OnPlayerAliveStateChanged(APlayerCharacter* player) {
	RefreshTeamDeathState();
}

void URespawnAsTeamUsingLivesComponent::SetTeamDeathState(const ETeamDeathState state) {
	if (teamDeathState != state) {
		teamDeathState = state;
		//Always refresh state after set since different logic can be ran in the next state
		RefreshTeamDeathState();
	}
}

void URespawnAsTeamUsingLivesComponent::RefreshTeamDeathState() {
	auto gameState = GetGameState();
	if (!gameState) {
		//Error state - no refresh possible
		return;
	}

	//Execute logic depending on which state we are currently in.
	switch (teamDeathState) {
		case ETeamDeathState::WaitingForMissionStart:
			//Keep respawning players forever until we leave this state.
			if (gameState->IsAnyPlayerMatchingAliveState(EAliveState::Alive)) {
				SetTeamDeathState(ETeamDeathState::AnyPlayerAlive);
			}
		break;
		case ETeamDeathState::AnyPlayerAlive:
			if (AreAllPlayersDead(gameState) && !HasAnyPlayerForcedDownState(GetWorld())) {
				//Check if we have any lives left.
				if (gameState->GetLives() > 0) {
					//Deduct a life
					gameState->SubtractLives();

					//Respawn all players
					for (auto&& player : actorquery::getActors<APlayerCharacter>(GetWorld())) {
						player->RespawnPlayerMulticast();
					}

					//switch state.
					SetTeamDeathState(ETeamDeathState::LifeDeductedAndReviving);
				}
				else 
				{
					//Game is over. No more respawning
					gameState->SetIsGameOver(true);
					SetTeamDeathState(ETeamDeathState::GameOver);
					if (AMissionProgressHandler* progressHandler = actorquery::getFirstActor<AMissionProgressHandler>(GetWorld())) {
						progressHandler->GameOver();
					} else {
						UE_LOG(LogTemp, Warning, TEXT("Could not find mission progress handler actor to multicast game over."));
					}
				}
			}
			break;		
		case ETeamDeathState::LifeDeductedAndReviving:
			if (gameState->IsAnyPlayerMatchingAliveState(EAliveState::Alive) || 
				gameState->IsAnyPlayerMatchingAliveState(EAliveState::Reviving)) {
				//A player is alive!
				SetTeamDeathState(ETeamDeathState::AnyPlayerAlive);
			}
			break;
		case ETeamDeathState::GameOver:
			break;
		default:
			break;
		break;
	}
}

bool URespawnAsTeamUsingLivesComponent::AreAllPlayersDead(ADungeonsGameState* gameState) {
	return !gameState->IsAnyPlayerMatchingAliveState(EAliveState::Alive) && 
			!gameState->IsAnyPlayerMatchingAliveState(EAliveState::Reviving);
}

ADungeonsGameState* URespawnAsTeamUsingLivesComponent::GetGameState() const {
	const auto gameState = Cast<ADungeonsGameMode>(GetOwner())->GetGameState<ADungeonsGameState>();
	check(gameState && "Missing GameState in DungeonsGameMode");
	return gameState;
}
