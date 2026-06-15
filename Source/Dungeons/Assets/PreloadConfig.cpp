#include "PreloadConfig.h"
#include "game/util/EnvironmentUtils.h"
#include <Engine/World.h>

TAutoConsoleVariable<int32> CVarPreloadItems(
	TEXT("Dungeons.Preload.Items"),
	-1,
	TEXT("Enable or disable preloading of (all) items\n")
	TEXT(" 1: Enable\n")
	TEXT(" 0: Disable\n")
	TEXT("-1: Use default\n"),
	ECVF_Cheat
);

namespace preloadconfig {

bool varOrDefault(const TAutoConsoleVariable<int32>& var, bool defaultValue) {
	auto value = var.GetValueOnGameThread();
	return value > 0 ? true : (value == 0 ? false : defaultValue);
}

//
// Helpers
//
bool isEditorBuild() {
	return environment::startedFromEditor();
}

bool isPlayInEditor(const UWorld* world) {
	return world->IsPlayInEditor();
}

bool defaultPreloadCondition() {
	return !isEditorBuild();
}

//
// Items
//
bool shouldPreloadItems() {
	return varOrDefault(CVarPreloadItems, true);
}
}
