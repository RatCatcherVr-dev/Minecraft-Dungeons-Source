#pragma once
#include "MissionTheme.h"
#include <Text.h>

struct MissionThemeDef {
	MissionThemeDef(EMissionTheme theme);

	EMissionTheme theme() const {
		return mTheme;
	}

	const TOptional<FText>& displayText() const {
		return mDisplayText;
	}

	const TOptional<FText>& lockedDisplayText() const {
		return mLockedDisplayText;
	}

	const TOptional<FText>& lockedDescriptionText() const {
		return mLockedDescriptionText;
	}

	const FName& rowName() const {
		return mRowName;
	}

protected:
	EMissionTheme mTheme;
	TOptional<FText> mDisplayText;	
	TOptional<FText> mLockedDisplayText;
	TOptional<FText> mLockedDescriptionText;
	FName mRowName;
};
