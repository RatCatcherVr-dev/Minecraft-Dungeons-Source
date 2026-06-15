#include "Dungeons.h"
#include <EngineUtils.h>
#include "world/entity/EntityTypes.h"
#include "Runtime/Engine/Classes/Components/AudioComponent.h"
#include "Runtime/Engine/Classes/Sound/SoundMix.h"
#include "MusicOverrideComponent.h"


UMusicOverrideComponent::UMusicOverrideComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

USoundCue* UMusicOverrideComponent::GetMusicOverrideTrack(EMusicSequenceState state) const {
	if (MusicTracks.Contains(state))
		return MusicTracks.at(state);
	return nullptr;
}
