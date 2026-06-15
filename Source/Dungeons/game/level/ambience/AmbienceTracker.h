#pragma once

#include "Ambience.h"

class APlayerCharacter;
class AAmbienceVolume;

namespace game {

class Tiles;
class Tile;

namespace ambience {

class AmbienceTracker {
	
public:
	explicit AmbienceTracker(UWorld&, const Tiles&);

	void updatePlayerAmbience() const;

private:
	EAmbienceID getAmbienceFor(const AAmbienceVolume*, const Tile*) const;
	EAmbienceAudioID getAmbienceAudioFor(const AAmbienceVolume*, const Tile*) const;

	void updateAmbienceFor(APlayerCharacter*, const FString& ambienceGroup) const;
	AAmbienceVolume* getAmbienceVolumeAt(const FVector& location) const;

	bool isAnyLocalPlayerInInventory() const;
	EAmbienceID mLastAmbience = EAmbienceID::AMBIENCE_DEFAULT;
	EAmbienceAudioID mLastAudioAmbience = EAmbienceAudioID::AUDIO_FROM_AMBIENCE;

	UWorld& mWorld;
	const Tiles& mTiles;
};

}}
