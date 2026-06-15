#include "Dungeons.h"
#include <Sound/SoundCue.h>
#include <Sound/SoundNodeWavePlayer.h>
#include "Engine/AssetManager.h"
#include "MusicSetTypes.h"


// maps (legacy) FMusicSet to EMusicSequenceState 'getter'

USoundCue* FMusicSet::GetMusicTrack(EMusicSequenceState sequenceState) 
{
	SyncLoadSet(); //we need it now

	switch (sequenceState) {
	case EMusicSequenceState::Intro:
		return introduction.Get();
	case EMusicSequenceState::Main:
		return track.Get();
	case EMusicSequenceState::Outro:
		return exit.Get();
	}

	return nullptr;
}

bool FMusicSet::AsyncLoadSet()
{	

	if (!StreamingHandle.IsValid())
	{
		TArray<FSoftObjectPath> MusicSetAssets = { track.ToSoftObjectPath() ,introduction.ToSoftObjectPath() , exit.ToSoftObjectPath() };

		StreamingHandle = UAssetManager::GetStreamableManager().RequestAsyncLoad(MusicSetAssets, []() {}, FStreamableManager::AsyncLoadHighPriority, false, false, TEXT("FMusicSet::AsyncLoadSet"));
		if (!StreamingHandle.IsValid())
		{
			//failed to find the assets
			return true;
		}
	}

	return StreamingHandle->HasLoadCompleted();
}

bool FMusicSet::SyncLoadSet()
{
	if (!AsyncLoadSet())
	{
		if (StreamingHandle.IsValid())
		{
#if !PLATFORM_SWITCH && !PLATFORM_XBOXONE && !PLATFORM_PS4//D11:MR - Causes a huge glitch on consoles
			StreamingHandle->WaitUntilComplete(); //just wait in here until we are done (this can prevent an async flush)
#endif
		}
	}

	return true;
}



bool FLevelAudio::AsyncLoadLevelAudio()
{
	if (!StreamingHandle.IsValid())
	{
		TArray<FSoftObjectPath> LevelAudioAssets = { LevelVOIntro.ToSoftObjectPath() ,LevelIntroMusic.ToSoftObjectPath() , LevelVOOutro.ToSoftObjectPath(), LevelBackgroundMusic.ToSoftObjectPath(), EnvironmentSFX.ToSoftObjectPath() };

		bool bManageActiveHandle = true;	//D11:MR - set this to true to fix intro music not plaing on some of the levels due to garbage collection
		StreamingHandle = UAssetManager::GetStreamableManager().RequestAsyncLoad(LevelAudioAssets, []() {}, FStreamableManager::AsyncLoadHighPriority, bManageActiveHandle, false, TEXT("FLevelAudio::AsyncLoadLevelAudio"));
		if (!StreamingHandle.IsValid())
		{
			//failed to find the assets
			return (LevelSideQuestBattleMusic.AsyncLoadSet() &&
				LevelEncounterMusic.AsyncLoadSet() &&
				LevelBossMusic.AsyncLoadSet());
		}
	}

	return (StreamingHandle->HasLoadCompleted() &&
		LevelSideQuestBattleMusic.AsyncLoadSet() &&
		LevelEncounterMusic.AsyncLoadSet() &&
		LevelBossMusic.AsyncLoadSet());
}

bool FLevelAudio::SyncLoadLevelAudio()
{
	if (!AsyncLoadLevelAudio())
	{
		if (StreamingHandle.IsValid())
		{
			StreamingHandle->WaitUntilComplete(); //just wait in here until we are done (this can prevent an async flush)
		}
	}

	return (LevelSideQuestBattleMusic.SyncLoadSet() &&
		LevelEncounterMusic.SyncLoadSet() &&
		LevelBossMusic.SyncLoadSet());
}




#if WITH_EDITOR

void StreamingPtrCheck(USoundCue* ptr)
{
	if (ptr)
	{
		TArray<USoundNodeWavePlayer*> WavePlayers;
		ptr->RecursiveFindNode<USoundNodeWavePlayer>(ptr->FirstNode, WavePlayers);

		for (USoundNodeWavePlayer* pSndWav : WavePlayers)
		{
			if (pSndWav)
			{
				USoundWave* SoundWave = pSndWav->GetSoundWave();

				if (!SoundWave->bStreaming)
				{
					auto Ressize = SoundWave->GetResourceSizeBytes(EResourceSizeMode::Exclusive);
					FPlatformMisc::LowLevelOutputDebugStringf(TEXT("WARNING : NON STREAMING SOUND IN AUDIO COLLECTION %s size (%d) (%.3fKb) GetDuration (%.3f)\n"), *SoundWave->GetFullName(), Ressize, Ressize /1024.0f, SoundWave->GetDuration());
				}
			}
		}
	}
}

void FMusicSet::CheckStreamingPtrs()
{
	SyncLoadSet();
	StreamingPtrCheck(track.Get());
	StreamingPtrCheck(introduction.Get());
	StreamingPtrCheck(exit.Get());
}


void FLevelAudio::CheckStreamingPtrs()
{
	SyncLoadLevelAudio();
	StreamingPtrCheck(LevelVOIntro.Get());
	StreamingPtrCheck(LevelIntroMusic.Get());
	StreamingPtrCheck(LevelVOOutro.Get());
	StreamingPtrCheck(LevelBackgroundMusic.Get());
	StreamingPtrCheck(EnvironmentSFX.Get());

	LevelSideQuestBattleMusic.CheckStreamingPtrs();
	LevelEncounterMusic.CheckStreamingPtrs();
	LevelBossMusic.CheckStreamingPtrs();
}

#endif

