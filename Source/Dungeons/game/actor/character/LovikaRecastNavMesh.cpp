#include "Dungeons.h"
#include "LovikaRecastNavMesh.h"

void FLovikaRecastQueryFilter::GetAllAreaCosts(float* CostArray, float* FixedCostArray, const int32 Count) const {
	FRecastQueryFilter::GetAllAreaCosts(CostArray, FixedCostArray, Count);
}

float FLovikaRecastQueryFilter::getVirtualCost(const float* pa, const float* pb,
	const dtPolyRef prevRef, const dtMeshTile* prevTile, const dtPoly* prevPoly,
	const dtPolyRef curRef, const dtMeshTile* curTile, const dtPoly* curPoly,
	const dtPolyRef nextRef, const dtMeshTile* nextTile, const dtPoly* nextPoly) const 
{
	float cost = getInlineCost(pa, pb,
		prevRef, prevTile, prevPoly,
		curRef, curTile, curPoly,
		nextRef, nextTile, nextPoly);

	return cost;
}


ALovikaRecastNavMesh::ALovikaRecastNavMesh(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	if (DefaultQueryFilter.IsValid()) {
		DefaultQueryFilter->SetFilterImplementation(dynamic_cast<const INavigationQueryFilterInterface*>(&DefaultNavFilter));
	}

	//D11.SC Do fully Async nav mesh gathering to prevent game thread stalls
	bDoFullyAsyncNavDataGathering = true;
}
