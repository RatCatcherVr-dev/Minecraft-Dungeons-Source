#pragma once

#include "CoreMinimal.h"
#include "NavMesh/RecastNavMesh.h"
#include "NavMesh/PImplRecastNavMesh.h"
#include "LovikaRecastNavMesh.generated.h"


class DUNGEONS_API FLovikaRecastQueryFilter : public FRecastQueryFilter
{
public:
	virtual void GetAllAreaCosts(float* CostArray, float* FixedCostArray, const int32 Count) const override;

protected:
	virtual float getVirtualCost(const float* pa, const float* pb,
		const dtPolyRef prevRef, const dtMeshTile* prevTile, const dtPoly* prevPoly,
		const dtPolyRef curRef, const dtMeshTile* curTile, const dtPoly* curPoly,
		const dtPolyRef nextRef, const dtMeshTile* nextTile, const dtPoly* nextPoly) const override;
};


UCLASS()
class DUNGEONS_API ALovikaRecastNavMesh : public ARecastNavMesh
{
	GENERATED_BODY()
	
public:
	ALovikaRecastNavMesh(const FObjectInitializer& ObjectInitializer);

	FLovikaRecastQueryFilter DefaultNavFilter;
};
