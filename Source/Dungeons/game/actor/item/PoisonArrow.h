#pragma once

#include "BaseProjectile.h"
#include "PoisonArrow.generated.h"

UCLASS()
class DUNGEONS_API APoisonArrow : public ABaseProjectile
{
	GENERATED_BODY()
	
public:	
	APoisonArrow(const FObjectInitializer& ObjectInitializer);

	
	
	virtual TOptional<FItemId> GetProjectileItemType() const override {
		return game::item::type::PoisonArrow.getId();
	}
	
	
};
