#include "Dungeons.h"
#include "SoundData.h"
#include "game/Game.h"
#include "game/objective/ObjectiveUtil.h"
#include <Sound/SoundCue.h>

namespace game { namespace objective {

SoundData::SoundData(const io::ObjectiveSoundData& data)
	: mData(data) {
}

Validation SoundData::validate(ValidationType) const {
	return validationErrorsIf({
		{mData.object.empty(), "No sound object defined in the objective json"},
		{!mData.object.empty() && mSounds.Num() == 0, "Could not load sound: " + mData.object}
	});
}

void SoundData::onInit() {
	auto objectPath = FString("SoundCue'/Game/AudioForce/04_playback_soundCue/") + FString(mData.object.c_str()) + FString(".") + FString(mData.object.c_str()) + FString("'");
	if (!objectPath.IsEmpty()) {
		if (auto object = loadSound(objectPath)) {
			mSounds.Add(object);
		}
	}
	else
		UE_LOG(LogDungeons, Error, TEXT("Could not load audio at %s. will skip sound playback"), *objectPath);
}

USoundCue* SoundData::getSound() {
	// NOTE isaveg: for now only one sound is supported, maybe we want slightly different version of a sound in the future??
	if (mSounds.Num() > 0 && mSounds[0]->IsValidLowLevelFast()) {
		return mSounds[0];
	}
	return nullptr;
}

void SoundData::onStop() {
	for (auto& sound : mSounds) {
		sound->MarkPendingKill();
	}
	mSounds.Empty();
}

}}
