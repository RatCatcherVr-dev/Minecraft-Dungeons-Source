#pragma once

#include <Camera/CameraModifier.h>
#include <Engine/Scene.h>
#include "CameraModifierNight.generated.h"

class APlayerCameraManager;
class ADimmer;

UCLASS()
class DUNGEONS_API UCameraModifier_NightModifier : public UCameraModifier {
	GENERATED_BODY()

	UCameraModifier_NightModifier();

	void AddedToCamera(APlayerCameraManager* Camera) override;

	bool ModifyCamera(float DeltaTime, FMinimalViewInfo& InOutPOV) override;

private:
	UPROPERTY()
	ADimmer* dimmer;

	bool isOceans;

	UFUNCTION()
	void OnBecomingNightChanged();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	FPostProcessSettings NightPostProcessSettings;
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons-Oceans")
	FPostProcessSettings NightOceansPostProcessSettings;
};