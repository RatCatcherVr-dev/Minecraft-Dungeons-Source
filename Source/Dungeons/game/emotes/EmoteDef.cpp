#include "EmoteDef.h"
#include <Engine/Texture2D.h>


EmoteDef::EmoteDef(EEmote emote) : mEmote(emote) {}

bool EmoteDef::IsDetachable() const {
	return bDetachable;
}

const FText& EmoteDef::GetText() const {
	return mText;
}

FSoftObjectPath EmoteDef::GetIconTexturePath() const {
	return mIconPath;
}

UTexture2D* EmoteDef::GetIconTexture() const {
	return Cast<UTexture2D>(mIconPath.TryLoad());
}

FSoftClassPath EmoteDef::GetMapPinWidgetClassPath() const {
	return mMapPinWidgetPath;
}

