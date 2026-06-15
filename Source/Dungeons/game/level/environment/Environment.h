#pragma once
#include <Array.h>
#include "Light.h"
#include "DungeonsDefsMinimal.h"
#include "game/actor/Dimmer.h"
#include "game/FlickeringLightComponent.h"


class AAmbienceActor;
struct FStreamableHandle;

namespace levelgen { struct LevelDef; }

namespace game {

class Tiles;

class Environment {
	
public:
	Environment(UWorld*, const game::Tiles&, const levelgen::LevelDef&);
	~Environment();

	void dimLights(float, FNightColor);
	void add(const TArray<AActor*>& decorTiles);

	
	bool completedLoadEnvironmentalEffects();

private:

	void asyncSpawnEffectActors(const TArray<FString>& AttachedNames, FString group, UWorld* world, AAmbienceActor* pAmbienceActor);
	void asyncLoadEnvironmentalAmbienceActor(UWorld* world, const TArray<FSoftObjectPath>& paths);
	void beginLoadEnvironmentalEffects(UWorld*, const game::Tiles&, const FString& mainGroup);

	void addEffectActor(AActor*);

	bool RequestAsyncLoadWrapper(const TArray<FSoftObjectPath>& TargetsToStream, TFunction<void()>&& Callback);

	bool mHasLoadedMainActors;
	int  mEnvironmentalMainActorsLoading;
	int  mEnvironmentalEffectActorsLoading;

	TArray<Light>		directionalLights;
	TArray<SkyLight>	skyLights;
	TArray<AActor*>		postprocess;
	TArray<Light>		lights;
	TArray < TWeakObjectPtr < UFlickeringLightComponent > > LightFlickeringComponents;
	
	TArray<TSharedPtr<FStreamableHandle>> mAsyncTasks;
};

}
