#pragma once

#include "CoreMinimal.h"
#include "../EntitlementsRepository.h"
#include "SwitchEntitlementsRepository.generated.h"

UCLASS()
class DUNGEONS_API USwitchEntitlementsRepository : public UEntitlementsRepository {

	GENERATED_BODY()
public:
	void RequestEntitlements() override;
protected:
	void ConfigureForPlatform() override;
};
