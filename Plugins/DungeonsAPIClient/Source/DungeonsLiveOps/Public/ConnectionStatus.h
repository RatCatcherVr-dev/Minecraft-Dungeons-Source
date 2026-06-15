#pragma once

#include "CoreMinimal.h"
#include "ConnectionStatus.generated.h"

UENUM(BlueprintType)
enum class EMinecraftAPIConnectionStatus : uint8 {
	Connected,
	TimingOut,
	NoConnection,
	GameClientTooOld,
};
