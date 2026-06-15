#pragma once

#include "BaseProjectile.h"
#include "VoidArrow.generated.h"

UCLASS()
class DUNGEONS_API AVoidArrow : public ABaseProjectile
{
	GENERATED_BODY()
	
public:	
	AVoidArrow(const FObjectInitializer& ObjectInitializer);

	
	
	virtual TOptional<FItemId> GetProjectileItemType() const override {
		return game::item::type::VoidArrow.getId();
	}
	
	
};
