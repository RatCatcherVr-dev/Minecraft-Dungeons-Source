#pragma once

#include "BaseProjectile.h"
#include "HeavyHarpoon.generated.h"

UCLASS()
class DUNGEONS_API AHeavyHarpoon : public ABaseProjectile
{
	GENERATED_BODY()

public:
	AHeavyHarpoon(const FObjectInitializer& ObjectInitializer);

	virtual TOptional<FItemId> GetProjectileItemType() const override {
		return game::item::type::HeavyHarpoon.getId();
	}


};