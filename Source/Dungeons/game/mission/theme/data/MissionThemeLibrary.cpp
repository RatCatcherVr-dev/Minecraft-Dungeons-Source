#include "Dungeons.h"
#include <ConstructorHelpers.h>
#include "game/mission/theme/MissionThemeDefs.h"
#include "ui/util/TextureCache.h"
#include "MissionThemeLibrary.h"

const FString UMissionThemeLibrary::ContextString = FString(TEXT("Themedefs"));

UMissionThemeLibrary::UMissionThemeLibrary() {
	static const auto objectFinder = ConstructorHelpers::FObjectFinder<UDataTable>(TEXT("DataTable'/Game/Content_DLC3/UI/Theme/MissionThemes.MissionThemes'"));
	mThemeDataTable = objectFinder.Object;
}

const FMissionThemeRow* UMissionThemeLibrary::findRow(EMissionTheme theme) {
	return mThemeDataTable ? mThemeDataTable->FindRow<FMissionThemeRow>(UMissionThemeDefs::getRowName(theme), ContextString) : nullptr;			
}

UTextureCache* UMissionThemeLibrary::getTextureCache(EMissionTheme theme) {
	if (mTextureCaches.Contains(theme)) {
		return mTextureCaches[theme];
	}
	return nullptr;
}

UTextureCache* UMissionThemeLibrary::getOrCreateTextureCache(EMissionTheme theme) {
	if(!mTextureCaches.Contains(theme)){
		mTextureCaches.Add(theme, NewObject<UTextureCache>(this));	
	}
	return mTextureCaches[theme];
}

void UMissionThemeLibrary::unloadThemeTextures(EMissionTheme theme) {
	if (auto* cache = getTextureCache(theme)) {
		cache->unloadTextures();		
	}
}

void UMissionThemeLibrary::preloadThemeTextures(EMissionTheme theme) {		
	if(auto* cache = getOrCreateTextureCache(theme)){
		if(!cache->isLoadingOrLoaded()){
			if (auto* row = findRow(theme)) {
				cache->loadAndCacheTextures(row->getSoftTexturePtrs());
			}
		}
	}
}

