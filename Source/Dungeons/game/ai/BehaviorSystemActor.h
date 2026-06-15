#pragma once

#include "GameFramework/Info.h"
#include "BehaviorSystem.h"
#include "CommonTypes.h"
#include "BehaviorSystemActor.generated.h"

UCLASS()
class DUNGEONS_API ABehaviorSystemActor : public AInfo {
	GENERATED_BODY()
public:
	ABehaviorSystemActor();

	void Tick(float deltaSeconds) override;
	void BeginPlay() override;
private:
	Unique<UBehaviorSystem> behavior;
};
