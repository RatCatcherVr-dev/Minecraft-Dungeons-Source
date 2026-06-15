#include "Dungeons.h"
#include "LevelCounterWidget.h"
#include "game/component/PlayerExperienceComponent.h"

TOptional<int> ULevelCounterWidget::FetchBoundValue(const AActor&) const {
	if (mExperience) {
		return mExperience->CurrentLevel();
	}
	return {};
}

void ULevelCounterWidget::BindTo(AActor& actor) {
	if (auto* experience = actor.FindComponentByClass<UPlayerExperienceComponent>()) {
		experience->OnLevelChangedInternal.AddUObject(this, &ULevelCounterWidget::Refresh);
		mExperience = experience;
	}
}

void ULevelCounterWidget::UnbindFrom(AActor&) {
	if (mExperience) {
		mExperience->OnLevelChangedInternal.RemoveAll(this);
	}
}

