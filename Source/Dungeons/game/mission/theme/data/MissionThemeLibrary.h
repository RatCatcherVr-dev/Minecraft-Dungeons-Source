#pragma once

#include "CoreMinimal.h"
#include "Engine/ObjectLibrary.h"
#include "Engine/DataTable.h"
#include "game/mission/theme/MissionTheme.h"
#include "MissionThemeRow.h"
#include "MissionThemeLibrary.generated.h"

class UTextureCache;

UCLASS()
class DUNGEONS_API UMissionThemeLibrary final : public UObject {
public:
	GENERATED_BODY()

	UMissionThemeLibrary();

	const FMissionThemeRow* findRow(EMissionTheme theme);
	
	void unloadThemeTextures(EMissionTheme theme);

	void preloadThemeTextures(EMissionTheme theme);
private:
	UTextureCache* getOrCreateTextureCache(EMissionTheme theme);
	UTextureCache* getTextureCache(EMissionTheme theme);

	UPROPERTY()
	UDataTable* mThemeDataTable;	

	UPROPERTY()
	TMap<EMissionTheme, UTextureCache*> mTextureCaches;

	static const FString ContextString;	
};
