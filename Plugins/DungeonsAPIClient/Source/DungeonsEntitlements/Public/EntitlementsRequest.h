#pragma once

#include "CoreMinimal.h"

struct DUNGEONSENTITLEMENTS_API EntitlementsRequest {	

	EntitlementsRequest(const FString& userId);
	const FString UserId;
};
