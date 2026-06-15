#pragma once

#include "game/objective/Objective.h"
#include "lovika/io/IoObjectiveTypes.h"

class USoundCue;

namespace game { namespace objective {

class SoundData : public Objective {
public:
	SoundData(const io::ObjectiveSoundData&);

	Validation validate(ValidationType) const override;

	void onInit() override;
	void onStart() override {}
	void onStop() override;

	USoundCue* getSound();
private:
	io::ObjectiveSoundData mData;
	TArray<USoundCue*> mSounds;
protected:
	virtual void onTick() {}
};

}}
