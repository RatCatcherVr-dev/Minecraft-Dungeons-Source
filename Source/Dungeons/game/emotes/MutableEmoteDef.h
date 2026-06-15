#pragma once

#include "EmoteDef.h"
#include <SoftObjectPath.h>
#include <Text.h>

namespace emotes {
	struct MutableEmoteDef : public EmoteDef {
		using EmoteDef::EmoteDef;
		MutableEmoteDef& text(FText txt) { mText = txt; return *this; }		
		MutableEmoteDef& detachable() { bDetachable = true; return *this; }		
		MutableEmoteDef& icon(FSoftObjectPath iconPath) { mIconPath = iconPath; return *this; }
		MutableEmoteDef& mapPin(FSoftClassPath mapPinWidgetPath) { mMapPinWidgetPath = mapPinWidgetPath; return *this; }
	};
}
