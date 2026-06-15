#include "TickInterleaving.h" 
#include "IConsoleManager.h"


TAutoConsoleVariable<int32> CVarInterleaveTargetingTicks(
	TEXT("Dungeons.Perf.InterleavedTargetingTicks"),
	0,
	TEXT("Enable/Disable Interleaved targeting ticks (alternate of ticking for occlusiong and targeting every other frame)."),
	ECVF_Scalability);

namespace tickstage
{
	TArray<UTargetingTickStageComponent*> LiveComponents;
	
	void CheckTickStageUpdate()
	{
		for(auto component : LiveComponents) {
			component->SetComponentTickEnabled(CVarInterleaveTargetingTicks.GetValueOnGameThread() > 0);
		}
	}

	FAutoConsoleVariableSink CVarBatchPhysXTasks(FConsoleCommandDelegate::CreateStatic(&CheckTickStageUpdate));
}

UTargetingTickStageComponent::UTargetingTickStageComponent (const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer) {
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = CVarInterleaveTargetingTicks.GetValueOnGameThread() > 0;
}

void UTargetingTickStageComponent::TickComponent(float deltaTime, ELevelTick tickType, FActorComponentTickFunction* thisTickFunction)
{
	CurrentTickStage = static_cast<ETargetingTickStage>((static_cast<int>(CurrentTickStage) + 1) % static_cast<int>(ETargetingTickStage::End));
}

void UTargetingTickStageComponent::BeginPlay()
{
	Super::BeginPlay();
	tickstage::LiveComponents.Add(this);
}

void UTargetingTickStageComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	tickstage::LiveComponents.Remove(this);
}

bool UTargetingTickStageComponent::ShouldTick(ETargetingTickStage stage) const
{
	return !IsComponentTickEnabled() || stage == CurrentTickStage;
}
