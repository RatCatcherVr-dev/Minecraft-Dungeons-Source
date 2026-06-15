#pragma once
#include "Components/SkyLightComponent.h"
#include "Components/LightComponent.h"
#include "game/actor/Dimmer.h"


namespace game {

class Light {
	float initialIntensity;
	float initialTemperature;
	TWeakObjectPtr<ULightComponent> component;

public:
	Light() {}; //
	Light(ULightComponent* component) { reset(component); };
	void reset(ULightComponent* component);
	void dim(float intensityFraction, FNightColor);

private:
	FLinearColor initialColor;
};

class SkyLight {
	float initialIntensity;
	TWeakObjectPtr<USkyLightComponent> component;

public:
	SkyLight() {}; //
	SkyLight(USkyLightComponent* component) { reset(component); };
	void reset(USkyLightComponent* component);
	void dim(float, FNightColor);
private:
	FLinearColor initialColor;
};

}
