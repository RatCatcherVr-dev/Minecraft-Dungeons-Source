#pragma once

// D11.DB

#include "game/ai/behavior/BehaviorTuple.h"
#include "game/component/BehaviorOptionsComponent.h"
#include "JungleAbominationBehavior.generated.h"

namespace bt {namespace behavior { namespace entities {

BehaviorTuple createJungleAbomination(class AMobCharacter&, const UBehaviorOptionsComponent&);

}}}

UCLASS()
class DUNGEONS_API UJungleAbominationInvulnerabilityGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UJungleAbominationInvulnerabilityGameplayEffect();
};

