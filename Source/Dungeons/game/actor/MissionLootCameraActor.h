#pragma once

#include "Camera/CameraActor.h"
#include "MissionLootCameraActor.generated.h"

UCLASS()
class DUNGEONS_API AMissionLootCameraActor : public ACameraActor {
	GENERATED_BODY()
public:

	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	void SetEnableLights(bool enable);
};
