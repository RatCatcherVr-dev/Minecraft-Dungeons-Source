#include "Dungeons.h"
#include "AudioManager.h"
#include "Runtime/Engine/Classes/Sound/SoundCue.h"
#include "Runtime/Engine/Classes/Sound/SoundWave.h"
#include "Engine/Classes/Sound/SoundNodeWavePlayer.h"


AAudioManager::AAudioManager() {
	PrimaryActorTick.bCanEverTick = true;
}

float AAudioManager::GetSoundCueFirstSoundWaveDuration(USoundCue* cue) {
	TArray<USoundNodeWavePlayer*> WavePlayers;
	cue->RecursiveFindNode<USoundNodeWavePlayer>(cue->FirstNode, WavePlayers);
	return WavePlayers[0]->GetSoundWave()->GetDuration();
}
