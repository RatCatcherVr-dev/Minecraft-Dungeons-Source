#include "Dungeons.h"
#include "ActorCounterWidgetBase.h"

TOptional<int> UActorCounterWidgetBase::FetchBoundValue(const AActor&) const {
	return {};
}

void UActorCounterWidgetBase::Refresh() {
	if (mBoundActor){		
		if (auto newValue = FetchBoundValue(*mBoundActor)){
			SetValue(newValue.GetValue());			
		}
	}
}

void UActorCounterWidgetBase::Bind(AActor* actor) {
	if (mBoundActor) {
		UnbindFrom(*mBoundActor);
		ResetValue();
	}
	mBoundActor = actor;
	if (actor) {
		BindTo(*actor);
		Refresh();
	}
}
