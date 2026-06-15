#pragma once

#include <GameFramework/Actor.h>
#include "OverlapListener.generated.h"

namespace game { namespace trigger {
class Triggers;
}}

UCLASS()
class AOverlapListener : public AActor {
	using Listener = game::trigger::Triggers;
	GENERATED_BODY()
public:
	void SetListener(Listener*);
	void RegisterBeginOverlap(UPrimitiveComponent*);
private:
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* component, AActor* otherActor, UPrimitiveComponent* otherComponent, int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult);

	Listener* mListener = nullptr;
};
