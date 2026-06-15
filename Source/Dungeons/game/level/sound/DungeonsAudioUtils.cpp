// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "DungeonsAudioUtils.h"
#include "DungeonsGameInstance.h"
#include "DungeonsGameState.h"

#include "Engine/Classes/Sound/SoundCue.h"
#include "Engine/Classes/Sound/SoundNodeWavePlayer.h"

#include "game/actor/DungeonsPlayerCameraManager.h"
#include "game/Game.h"
#include "game/level/sound/AudioCollectionComponent.h"
#include "game/levels.h"
#include "game/util/ActorQuery.h"
#include "util/Algo.h"


static AAudioMusicManager* audioMusicManager(const UWorld* world) {
	return Cast<UDungeonsGameInstance>(world->GetGameInstance())->GetAudioMusicManager();
}

ELevelNames UDungeonsAudioUtils::GetLevelName(const UObject* WorldContextObject)
{
	ELevelNames levelname = ELevelNames::Invalid;
	if (WorldContextObject)
	{
		// this is used by (1/22 2021): UMG_InventoryItemInspector.uasset,
		// UMG_InventoryHUD.uasset and UMG_IngameMenu.uasset, but should really be 
		// refactored/replaced by: ELevelNames UMissionQuery::GetLevelName(UObject* WorldContextObject)
		levelname = WorldContextObject->GetWorld()->GetGameInstance<UDungeonsGameInstance>()->GetLevelSettingsLastStarted().getLevelName();
	}
	return levelname;
}

FLevelAudio UDungeonsAudioUtils::GetCurrentLevelAudioCollection(const UObject* WorldContextObject)
{
	if (!WorldContextObject)
	{
		UE_LOG(LogDungeons, Error, TEXT("No valid WorldContextObject, returning empty LevelAudio"));
		return FLevelAudio();
	}
	UWorld* world = WorldContextObject->GetWorld();
	return audioMusicManager(world)->GetCurrentLevelAudioCollection();
	
}

bool UDungeonsAudioUtils::CinematicPlaying(const UObject* WorldContextObject)
{
	if (WorldContextObject)
	{
		if (auto* gameState = WorldContextObject->GetWorld()->GetGameState<ADungeonsGameState>())
			return gameState->IsCinematicPlaying();
	}
	return false;
}


// code callable audio utils
namespace audioutils {
	float GetSoundCueDuration(USoundCue* Cue)
	{
		const float NO_SOUND_DURATION = 0;
		// will return the shortest duration of the referenced sound cues set up to loop
		if (Cue != nullptr)
		{
			TArray<USoundNodeWavePlayer*> WavePlayers;
			Cue->RecursiveFindNode<USoundNodeWavePlayer>(Cue->FirstNode, WavePlayers);
			if (auto cue = algo::min_element_by(WavePlayers, RETLAMBDA(it->GetDuration()))) {
				return cue.GetValue()->GetDuration();
			}
		}
		return NO_SOUND_DURATION;
	}

	USoundCue* GetEncounterMusicTrack(const UWorld* world, EMusicSequenceState sequenceState, EEventType eventType, ELevelNames level) 
	{
		if (auto* manager = audioMusicManager(world))
		{
			if (UAudioCollectionComponent* collectionComponent = audioMusicManager(world)->collectionComponent()) {
				auto audioCollection = collectionComponent->GetMusicTracksForLevel(level);
				switch (eventType)
				{
				case EEventType::SideQuestBattle:
					return audioCollection.LevelSideQuestBattleMusic.GetMusicTrack(sequenceState);
				case EEventType::ArenaBattle:
					return audioCollection.LevelEncounterMusic.GetMusicTrack(sequenceState);
				case EEventType::Boss:
					return audioCollection.LevelBossMusic.GetMusicTrack(sequenceState);

				}
			}
		}

		return nullptr;
	}

	ELevelNames CurrentLevelContext(UWorld* world)
	{
		assert(world);
		ELevelNames levelname = ELevelNames::Invalid;
		if (auto gi = world->GetGameInstance<UDungeonsGameInstance>())
		{
			// check audioMusicManager for a 'context' to prefer:
			levelname = gi->GetLevelSettingsLastStarted().getLevelName();
			if (const auto* musicManager = gi->GetAudioMusicManager())
			{
				if (musicManager->GetCurrentLevelContext() != ELevelNames::Invalid)
					levelname = musicManager->GetCurrentLevelContext();
			}
		}
		return levelname;
	}

	AActor* CameraViewTargetPosition(UWorld* world) {
		if (auto gi = Cast<UDungeonsGameInstance>(world->GetGameInstance<UDungeonsGameInstance>())) {
			if (auto cameraManager = Cast<ADungeonsPlayerCameraManager>(gi->GetFirstLocalPlayerController()->PlayerCameraManager)) // single BP?
			{
				return cameraManager->GetViewTarget(); // BPCoopCamera OR the PlayerCharacter.
			}
		}
		return nullptr;

	}
}

