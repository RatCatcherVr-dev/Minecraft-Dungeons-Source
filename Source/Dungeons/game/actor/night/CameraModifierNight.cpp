#include "Dungeons.h"
#include "game/util/ActorQuery.h"
#include "game/actor/Dimmer.h"
#include "CameraModifierNight.h"

UCameraModifier_NightModifier::UCameraModifier_NightModifier() {
	AlphaInTime = 3.0f;
	AlphaOutTime = 1.0f;
	isOceans = false;
}

void UCameraModifier_NightModifier::AddedToCamera(APlayerCameraManager* Camera) {
	Super::AddedToCamera(Camera);

	DisableModifier(true);
	dimmer = actorquery::getFirstActor<ADimmer>(Camera->GetWorld());
	if (dimmer) {
		dimmer->OnBecomingNightChanged.AddUObject(this, &UCameraModifier_NightModifier::OnBecomingNightChanged);
	}
}

bool UCameraModifier_NightModifier::ModifyCamera(float DeltaTime, FMinimalViewInfo& InOutPOV) {
	const bool exclusive = Super::ModifyCamera(DeltaTime, InOutPOV);
	if (CameraOwner) {		
		
		CameraOwner->AddCachedPPBlend(isOceans ? NightOceansPostProcessSettings : NightPostProcessSettings, Alpha);
	}
	return exclusive;
}

void UCameraModifier_NightModifier::OnBecomingNightChanged() {

	isOceans = dimmer->isOceanNight();

	if (dimmer->IsBecomingNight()) {
		EnableModifier();
	} else {
		DisableModifier();
	}
}