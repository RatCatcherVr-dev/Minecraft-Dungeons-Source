#include "Dungeons.h"
#include "game/util/ActorQuery.h"
#include "MissionLootCameraActor.h"
#include <Components/DirectionalLightComponent.h>
#include <Engine/DirectionalLight.h>
#include <Engine/SkyLight.h>
#include <Engine/ExponentialHeightFog.h>
#include <Engine/PostProcessVolume.h>
#include <Components/SkyLightComponent.h>
#include <Components/ExponentialHeightFogComponent.h>

void AMissionLootCameraActor::BeginPlay() {
	Super::BeginPlay();
	//The light switching done here is old and does not fully work with current ambience system. Trying to assign correct ambience in AmbienceTracker instead. Leaving this file as is for now. 

	//SetEnableLights(false);
}

void AMissionLootCameraActor::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);
	//SetEnableLights(true);
}

void AMissionLootCameraActor::SetEnableLights(bool enable) {
	auto directionalLight = actorquery::getFirstComponentByActorType<UDirectionalLightComponent, ADirectionalLight>(GetWorld());
	if (directionalLight) {
		directionalLight->SetVisibility(enable, true);
	}

	auto skyLight = actorquery::getFirstComponentByActorType<USkyLightComponent, ASkyLight>(GetWorld());
	if (skyLight) {
		skyLight->SetVisibility(enable, true);
	}

	auto fog = actorquery::getFirstComponentByActorType<UExponentialHeightFogComponent, AExponentialHeightFog>(GetWorld());
	if (fog) {
		fog->SetVisibility(enable, true);
	}

	for (auto&& postProcess : TActorRange<APostProcessVolume>(GetWorld())) {
		if (postProcess->ActorHasTag(FName(TEXT("LovikaDecorActor")))) {
			postProcess->bEnabled = !enable;
			postProcess->bUnbound = static_cast<uint32>(!enable);
		}
		else {
			postProcess->bEnabled = enable;
		}
	}
}