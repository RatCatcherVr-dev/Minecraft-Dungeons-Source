#pragma once

#include "CoreMinimal.h"
#include "../EntitlementsRepository.h"
#include "PS4EntitlementsRepository.generated.h"

UCLASS()
class DUNGEONS_API UPS4EntitlementsRepository : public UEntitlementsRepository {

	GENERATED_BODY()
public:	
	void RequestEntitlements() override;
protected:
	void ConfigureForPlatform() override;
};
