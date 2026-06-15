#include "Dungeons.h"
#include "lovika/LovikaLobbyActor.h"
#include "LobbyBP.h"
#include "util/ConfigFileUtil.h"
#include "DungeonsGameInstance.h"
#include "game/item/ItemFunctionLibrary.h"
#include "component/CharacterSerializeComponent.h"
#include "actor/character/player/BasePlayerController.h"

ALobbyBP::ALobbyBP() {
	PrimaryActorTick.bCanEverTick = true;
	bAllowTickBeforeBeginPlay = false;

	bAlwaysRelevant = true;
	SetReplicates(true);
}

namespace lobby {

	UDungeonsGameInstance* nonDedicatedDungeonsGameInstance(UWorld* world) {
		if (world) {
			const auto gameInstance = world->GetGameInstance<UDungeonsGameInstance>();
			if (gameInstance && !gameInstance->IsDedicatedServerInstance()) {
				return gameInstance;
			}
		}
		return nullptr;
	}
	
	TOptional<game::FDifficulty> calculateHighestCompletedMissionDifficulty(UWorld* context) {
		if (const auto* gameInstance = nonDedicatedDungeonsGameInstance(context)) {
			if (const auto* playerController = Cast<ABasePlayerController>(gameInstance->GetFirstLocalPlayerController())) {
				return playerController->GetCharacterSerializeComponent()->GetHighestCompletedDifficulty();
			}
		}
		return {};
	}
}

void ALobbyBP::BeginPlay() {
	Super::BeginPlay();

	UItemFunctionLibrary::PreloadGearIconTextures();
	
	if (HasAuthority()) {
		// #D11.CM - We currently don't use objectives in the lobby.
		bUsesObjectives = false;
	}
}

FLevelSettings ALobbyBP::CreateLevelSettings() const {
	check(HasAuthority() && "Level settings should always be created by the authoriative part.");
	const auto highestDifficulty = lobby::calculateHighestCompletedMissionDifficulty(GetWorld()).Get(game::FDifficulty::LOWEST);
	auto levelSettings = GetDungeonsGameInstance()->Configuration.GetLevelSettings();
	if (!levelSettings.IsSet()) {
		check(WITH_EDITOR && "There should always be a valid level settings unless you start the lobby from the editor.");
		levelSettings = levelsettingsutil::generateLobbySettings(GetWorld());
	}

	if (levelSettings.GetValue().getLevelName() != ELevelNames::Invalid) {
		check(false && "You can't play a normal mission with ALobbyBP. Something has gone wrong.");
		levelSettings = levelsettingsutil::generateLobbySettings(GetWorld());
	}
	levelSettings->setMissionDifficulty({ levelSettings.GetValue().getLevelName(), highestDifficulty.chosen(), highestDifficulty.threatLevel(), highestDifficulty.endlessStruggle() });
	levelSettings->setSeed(1);
	return levelSettings.GetValue();
}


void ALobbyBP::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	UItemFunctionLibrary::ClearPreloadedGearIconTextures();
}

bool ALobbyBP::HasLocalGameplayStarted() {
	return AllStartWidgetsAreClosed;
}

const FObjectiveLocations& ALobbyBP::GetObjectiveLocations() {
	return UnlockableChestLocations;
}

void ALobbyBP::AddUnlockableChestObjective(const FVector Location) {
	UnlockableChestLocations.Locations.AddUnique(Location);
}

void ALobbyBP::RemoveUnlockableChestObjective(const FVector Location) {
	UnlockableChestLocations.Locations.Remove(Location);
}

bool ALobbyBP::QuitToMenuAfterRewardScreen() const {
	return configfile::IsDemo();
}
