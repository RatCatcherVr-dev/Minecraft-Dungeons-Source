#pragma once

#include "BaseProjectile.h"
#include "BurningArrow.generated.h"

UCLASS()
class DUNGEONS_API ABurningArrow : public ABaseProjectile
{
	GENERATED_BODY()
	
public:	
	ABurningArrow(const FObjectInitializer& ObjectInitializer);

	
	
	virtual TOptional<FItemId> GetProjectileItemType() const override {
		return game::item::type::BurningArrow.getId();
	}
	
	
};
