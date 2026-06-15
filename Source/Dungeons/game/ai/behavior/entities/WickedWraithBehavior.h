#pragma once

#include "game/ai/behavior/BehaviorTuple.h"
#include <GameplayEffect.h>
#include "WickedWraithBehavior.generated.h"

class UBehaviorOptionsComponent;
class AMobCharacter;

namespace bt { namespace behavior { namespace entities {

BehaviorTuple createWickedWraith(class AMobCharacter&, const UBehaviorOptionsComponent&);

} } }

UENUM()
enum class EWickedWraithWaypointType : uint8 {
	Normal,
	Special,
};

UCLASS()
class DUNGEONS_API AWickedWraithWaypoint : public AActor {
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Dungeons")
	EWickedWraithWaypointType WaypointType = EWickedWraithWaypointType::Normal;
};

UCLASS()
class DUNGEONS_API UWickedWraithVulnerableGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UWickedWraithVulnerableGameplayEffect(const FObjectInitializer& ObjectInitializer);
};