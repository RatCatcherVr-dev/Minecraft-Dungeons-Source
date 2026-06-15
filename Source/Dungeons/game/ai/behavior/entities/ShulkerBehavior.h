#pragma once

#include "game/ai/behavior/BehaviorTuple.h"
#include "game/component/BehaviorOptionsComponent.h"
#include <GameplayEffect.h>
#include "ShulkerBehavior.generated.h"

namespace bt {namespace behavior { namespace entities {

BehaviorTuple createShulker(class AMobCharacter&, const UBehaviorOptionsComponent&);

}}}

UCLASS()
class DUNGEONS_API UShulkerGuardGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UShulkerGuardGameplayEffect(const FObjectInitializer& ObjectInitializer);

	static const FName MeleeDamageTakenName;
	static const FName RangeDamageTakenName;
};