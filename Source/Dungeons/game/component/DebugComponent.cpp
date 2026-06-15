#include "Dungeons.h"
#include "DebugComponent.h"
#include "game/ai/bt/BtNodeLogger.h"
#include "BehaviorComponent.h"

UDebugComponent::UDebugComponent() {
#if !UE_BUILD_SHIPPING && !UE_BUILD_TEST
	PrimaryComponentTick.bCanEverTick = true;
#else
	PrimaryComponentTick.bCanEverTick = false;
#endif
}

void UDebugComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (IsActive()) {
		if (const auto behaviorComponent = GetOwner()->FindComponentByClass<UBehaviorComponent>()) {
			if(auto node = behaviorComponent->Actions()._Current()){
				UBtNodeLogger::LogScreen(*node);
				UBtNodeLogger::LogConsole(*node);
			}			
		}
	}
}

void UDebugComponent::SetText(const FString& line, int lineIndex) {
	if (lineIndex >= lines.Num()) {
		for (auto i = lines.Num(); i < lineIndex; i++) {
			lines.Push("");
		}
		lines.Push(line);
	} else {
		lines[lineIndex] = line;
	}
}

void UDebugComponent::ClearText() {
	lines.Empty();
}