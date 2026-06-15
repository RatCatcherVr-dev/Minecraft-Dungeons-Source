#pragma once

#include "MissionTheme.h"
#include "MissionThemeDef.h"
#include "MutableMissionThemeDef.h"
#include "game/defs/DefsContainer.h"
#include "locale/LocalizationUtils.h"
#include <Kismet/BlueprintFunctionLibrary.h>
#include "data/MissionThemeRow.h"
#include "MissionThemeDefs.generated.h"

class UMissionThemeLibrary;

namespace themes {
	using MissionThemeDefs = DefsContainer<MutableMissionThemeDef, MissionThemeDef, EMissionTheme>;
	const MissionThemeDefs& defs();	
}


UCLASS(BlueprintType)
class UMissionThemeDefs : public UBlueprintFunctionLibrary {
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FName& getRowName(EMissionTheme theme);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FText getDisplayText(EMissionTheme theme);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FText getLockedDisplayText(EMissionTheme theme);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FText getLockedDescriptionText(EMissionTheme theme);
	
	static UMissionThemeLibrary* getThemeLibrary(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static void unloadThemeTextures(UObject* WorldContextObject, EMissionTheme theme);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static void preloadThemeTextures(UObject* WorldContextObject, EMissionTheme theme);
};
