#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include <WeakObjectPtrTemplates.h>
#include "Ricochet.h"
#include "ReliableRicochet.generated.h"

class ABaseProjectile;

/**
 * 
 */
UCLASS()
class DUNGEONS_API UReliableRicochet : public URicochet
{
	GENERATED_BODY()

	bool RollForTrigger(const FRandomStream& randStream) const;


public:
	UReliableRicochet();
private:
	bool RollForTrigger(const FRandomStream& randStream, ABaseProjectile* projectile) const override;

};
