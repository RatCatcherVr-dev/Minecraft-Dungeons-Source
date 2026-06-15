#pragma once

#include "CoreMinimal.h"
#include "EventTypes.generated.h"

UENUM(BlueprintType)
enum class EEventType : uint8 {
	None,
	ArenaBattle,
	Boss,
	DoorOpened,
	SideQuestBattle
};
ENUM_NAME(EEventType)

// TOptional can't be replicated so need this
USTRUCT()
struct DUNGEONS_API FObjectiveEventLocation {
	GENERATED_BODY()
	FObjectiveEventLocation();
	FObjectiveEventLocation(FVector location, EEventType type = EEventType::None);

	explicit operator bool() const {
		return bIsSet;
	}

	bool IsSet() const;

	UPROPERTY()
	FVector Location;

	UPROPERTY()
	EEventType EventType;
private:
	UPROPERTY()
	bool bIsSet;
};
