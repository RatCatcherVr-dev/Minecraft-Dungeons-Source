#include "Dungeons.h"
#include "ReusedEffectActor.h"

void AReusedEffectActor::ResetEffect(float newLifeSpan) {
	SetLifeSpan(newLifeSpan);
	OnEffectReset();
}

void AReusedEffectActor::OnEffectReset_Implementation() {
}