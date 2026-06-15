#pragma once
#include "CoreMinimal.h"
#include "common_header.h"
#include "UObject/Interface.h"
#include "Components/ActorComponent.h"
#include "TickInterleaving.generated.h"

struct FActorComponentTickFunction;
UENUM()
enum class ETargetingTickStage : uint8 {
	  HighlightingUpdate = 0
	, UpdateGamepadTargetSelection
	, End
};
ENUM_NAME(ETargetingTickStage);


UCLASS()
class UTargetingTickStageComponent : public UActorComponent
{
	GENERATED_BODY()

	ETargetingTickStage CurrentTickStage;
public:
	UTargetingTickStageComponent(const FObjectInitializer& ObjectInitializer);
	void TickComponent(float deltaTime, ELevelTick tickType, FActorComponentTickFunction* thisTickFunction) override;
	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	bool ShouldTick(ETargetingTickStage stage) const;
	
	ETargetingTickStage TickStage() const { return CurrentTickStage; }
};