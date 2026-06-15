#include "Dungeons.h"
#include "ReusedEffectActor.h"
#include "EffectActor.h"

FEffectActor::FEffectActor() {}

FEffectActor::FEffectActor(AReusedEffectActor* actor, float initialLifeSpan)
	: Actor(actor)
	, InitialLifeSpan(initialLifeSpan) {
}

