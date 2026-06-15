#pragma once

// D11.DJB

#include "game/ai/behavior/BehaviorTuple.h"
#include "game/component/BehaviorOptionsComponent.h"
#include "PerfectFormHeartBehavior.generated.h"

namespace bt {namespace behavior { namespace entities {

BehaviorTuple createPerfectFormHeart(class AMobCharacter&, const UBehaviorOptionsComponent&);

}}}

UCLASS()
class DUNGEONS_API UPerfectFormInvulnerabilityGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UPerfectFormInvulnerabilityGameplayEffect();
};