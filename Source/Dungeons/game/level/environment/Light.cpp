#include "Dungeons.h"
#include "Light.h"
#include "Kismet/KismetMathLibrary.h"
#include "game/actor/Dimmer.h"

namespace game {

void Light::reset(ULightComponent* newComponent) {
	component = newComponent;
	initialIntensity = component->Intensity;
	initialTemperature = component->Temperature;
	initialColor = component->GetLightColor();
}

void Light::dim(float intensityFraction, FNightColor lightColor) {
	if (!component.IsValid()) {
		return;
	}

	component->SetIntensity(FMath::Lerp(initialIntensity * .2f, initialIntensity, intensityFraction)) ;
	component->SetTemperature(FMath::Lerp(1700.f, initialTemperature, intensityFraction));
	FLinearColor color = lightColor.bCustomColor ? lightColor.NightColor : initialColor;
	component->SetLightColor(UKismetMathLibrary::LinearColorLerp(component->GetLightColor(), color, 0.08f));
}

void SkyLight::reset(USkyLightComponent* newComponent) {
	if (!newComponent) {
		return;
	}

	initialIntensity = newComponent->Intensity;
	component = newComponent;
	initialColor = component->GetLightColor();
}

void SkyLight::dim(float intensityFraction, FNightColor lightColor) {
	if (!component.IsValid()) {
		return;
	}

	component->SetIntensity(FMath::Lerp(initialIntensity * .2f, initialIntensity, intensityFraction));
	FLinearColor color = lightColor.bCustomColor ? lightColor.NightColor : initialColor;
	component->SetLightColor(UKismetMathLibrary::LinearColorLerp(component->GetLightColor(), color, 0.08f));
}

}
