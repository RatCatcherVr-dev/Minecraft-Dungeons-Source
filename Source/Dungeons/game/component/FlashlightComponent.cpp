#include "Dungeons.h"
#include "game/util/ActorQuery.h"
#include "FlashlightComponent.h"

UFlashlightComponent::UFlashlightComponent() {
	PrimaryComponentTick.bCanEverTick = true;
	bReplicates = true;
}

void UFlashlightComponent::UpdateLight() {
	if (fraction <= 0.f && flashlight->IsVisible()) {
		flashlight->SetVisibility(false);
	} else if (fraction > 0.f) {
		if (!flashlight->IsVisible()) {
			flashlight->SetVisibility(true);
		}

		flashlight->SetIntensity(FMath::Lerp(0.f, initialIntensity, fraction));
		flashlight->SetTemperature(FMath::Lerp(1700.f, initialTemperature, fraction));
	}
}

void UFlashlightComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!dimmer.IsValid()) {
		dimmer = actorquery::getFirstActor<ADimmer>(GetWorld());
		
		if (!dimmer.IsValid()) {
			return;
		}
	}

	if (!flashlight.IsValid()) {
		auto candidates = GetOwner()->GetComponentsByTag(ULightComponent::StaticClass(), FName { "flashlight" });
		
		if (candidates.Num() > 0) {
			flashlight = Cast<ULightComponent>(candidates[0]);
			initialIntensity = flashlight->Intensity;
			initialTemperature = flashlight->Temperature;			
		} else {
			return;
		}
	}

	const auto TargetFraction = 1.f - dimmer->GetLightIntensity();
	if (TargetFraction != fraction) {
		fraction  = TargetFraction;
		UpdateLight();
	}
}

