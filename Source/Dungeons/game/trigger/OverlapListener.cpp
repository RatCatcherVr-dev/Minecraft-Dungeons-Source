#include "Dungeons.h"
#include "OverlapListener.h"
#include <Components/PrimitiveComponent.h>
#include "Triggers.h"

void AOverlapListener::SetListener(Listener* listener) {
	mListener = listener;
}

void AOverlapListener::RegisterBeginOverlap(UPrimitiveComponent* component) {
	component->OnComponentBeginOverlap.AddDynamic(this, &AOverlapListener::OnBeginOverlap);
}

void AOverlapListener::OnBeginOverlap(UPrimitiveComponent* component, AActor* otherActor, UPrimitiveComponent* otherComponent, int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult) {
	if (mListener) {
		mListener->onBeginOverlap( { *component, *otherActor });
	}
}
