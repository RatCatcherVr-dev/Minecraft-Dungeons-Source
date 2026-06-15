#include "Dungeons.h"
#include "Conversion.h"
#include "GameBP.h"
#include "Map.h"
#include "MapTexture.h"

const int UMapUtils::defaultWidth = 256;
const int UMapUtils::defaultHeight = 256;

UMapTexture2D* UMapUtils::createMapTexture2D(UObject* WorldContextObject, EMapTextureSampler samplerMode)
{
	auto mapTexture = NewObject<UMapTexture2D>(WorldContextObject);
	mapTexture->initTexture(defaultWidth, defaultHeight, samplerMode);
	return mapTexture;
}