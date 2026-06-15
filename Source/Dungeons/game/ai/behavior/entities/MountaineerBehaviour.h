#pragma once

#include "game/ai/behavior/BehaviorTuple.h"
#include "game/component/BehaviorOptionsComponent.h"
#include "game/Enchantments/PushVolumeResistance.h"
#include "MountaineerBehaviour.generated.h"

namespace bt {namespace behavior { namespace entities {

BehaviorTuple createMountaineer(class AMobCharacter&, const UBehaviorOptionsComponent&);

}
}
}

UCLASS()
class DUNGEONS_API UMountaineerWindResistanceGameplayEffect : public UWindResistanceGameplayEffect {
	GENERATED_BODY()
public:
	UMountaineerWindResistanceGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UMountaineerWindImmunityGameplayEffect : public UWindImmunityGameplayEffect {
	GENERATED_BODY()
public:
	UMountaineerWindImmunityGameplayEffect(const FObjectInitializer& ObjectInitializer);
};
