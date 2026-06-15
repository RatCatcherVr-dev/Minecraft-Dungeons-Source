#pragma once

#include "Emotes.h"
#include "Internationalization/Text.h"
#include <SoftObjectPath.h>

class UTexture2D;

struct EmoteDef {
	EmoteDef(EEmote emote);

	EEmote emote() const {
		return mEmote;
	}

	bool IsDetachable() const;
	const FText& GetText() const;
	FSoftObjectPath GetIconTexturePath() const;
	UTexture2D* GetIconTexture() const;
	FSoftClassPath GetMapPinWidgetClassPath() const;

protected:	
	EEmote mEmote;
	FText mText;
	FSoftObjectPath mIconPath;
	FSoftClassPath mMapPinWidgetPath;
	bool bDetachable = false;
};
