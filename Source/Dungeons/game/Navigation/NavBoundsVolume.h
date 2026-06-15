#pragma once

#include "CoreMinimal.h"
#include <NavMeshBoundsVolume.h>
#include "NavBoundsVolume.generated.h"

struct BlockCuboid;

UCLASS(Blueprintable, BlueprintType)
class DUNGEONS_API ANavBoundsVolume : public ANavMeshBoundsVolume {
	GENERATED_BODY()
public:
	ANavBoundsVolume(const FObjectInitializer& ObjectInitializer);

	FBox GetComponentsBoundingBox(bool bNonColliding = false) const override;

	void SetBounds(const BlockCuboid&);
	void SetBounds(const FBox&);
	
	void UpdateNavigation();
private:
	FBox Bounds;
};
