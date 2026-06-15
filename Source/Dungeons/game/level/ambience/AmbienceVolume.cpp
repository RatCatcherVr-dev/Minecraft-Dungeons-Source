#include "Dungeons.h"
#include "AmbienceVolume.h"

AAmbienceVolume::AAmbienceVolume(): ambience(EAmbienceID::AMBIENCE_DEFAULT) {
}

TOptional<EAmbienceID> AAmbienceVolume::GetAmbience() const {
	return EnableSetAmbience? TOptional<EAmbienceID>(ambience): TOptional<EAmbienceID>();
}

TOptional<EAmbienceAudioID> AAmbienceVolume::GetAmbienceAudio() const {
	return EnableSetAudioAmbience ? TOptional<EAmbienceAudioID>(ambienceAudio) : TOptional<EAmbienceAudioID>();
}
