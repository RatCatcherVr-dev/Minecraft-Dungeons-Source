#pragma once
#include "CoreMinimal.h"
#include "EffectActor.generated.h"

class AReusedEffectActor;

USTRUCT()
struct DUNGEONS_API FEffectActor {
	GENERATED_BODY()
public:
	FEffectActor();
	FEffectActor(AReusedEffectActor* actor, float initialLifeSpan);

	UPROPERTY()
	TWeakObjectPtr<AReusedEffectActor> Actor;

	float InitialLifeSpan = 0.f;
};

