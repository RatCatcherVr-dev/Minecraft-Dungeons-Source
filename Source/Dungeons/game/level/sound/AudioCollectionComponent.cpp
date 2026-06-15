#include "Dungeons.h"
#include "AudioCollectionComponent.h"

#include "game/levels.h"


void UAudioCollectionComponent::BeginPlay() {
	Super::BeginPlay();

	// replace empty references with default set
	TArray<ELevelNames> keys;
	LevelAudioTable.GetKeys(keys);
	for (auto& key : keys) {
		if (LevelAudioTable[key].LevelEncounterMusic == FMusicSet())
			LevelAudioTable[key].LevelEncounterMusic = DefaultEncounterMusic;
		if (LevelAudioTable[key].LevelBossMusic == FMusicSet())
			LevelAudioTable[key].LevelBossMusic = DefaultBossMusic;
	}
#if WITH_EDITOR
	for (auto& la : LevelAudioTable)
	{
		FLevelAudio& lvlAudio = la.Value;
		lvlAudio.CheckStreamingPtrs();
	}

	LobbyBackgroundAudio.CheckStreamingPtrs();

	if (LoadingScreenSound)
	{
		if (!LoadingScreenSound->bStreaming)
		{
			FPlatformMisc::LowLevelOutputDebugStringf(TEXT("NON STREAMING SOUND %s \n"), *LoadingScreenSound->GetFullName());
		}
	}
#endif

}

FLevelAudio UAudioCollectionComponent::GetMusicTracksForLevel(ELevelNames level) const {
	if (!LevelAudioTable.Contains(level))
		return FLevelAudio();
	return LevelAudioTable[level];
}

FMusicSet UAudioCollectionComponent::GetEncounterMusic(ELevelNames level) const {
	if (!LevelAudioTable.Contains(level))
		return DefaultEncounterMusic;
	return LevelAudioTable[level].LevelEncounterMusic;
}

FMusicSet UAudioCollectionComponent::GetBossMusic(ELevelNames level) const {
	if (!LevelAudioTable.Contains(level))
		return DefaultBossMusic;
	return LevelAudioTable[level].LevelBossMusic;
}

FLevelAudio UAudioCollectionComponent::GetLobbyAudio() const {
	return LobbyBackgroundAudio;
}
