#include "Dungeons.h"
#include "NavBoundsVolume.h"
#include "game/Conversion.h"
#include <NavigationSystem.h>

ANavBoundsVolume::ANavBoundsVolume(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Cast<UPrimitiveComponent>(GetRootComponent())->SetGenerateOverlapEvents(false);
}

FBox ANavBoundsVolume::GetComponentsBoundingBox(bool bNonColliding /*= false*/) const {
	return Bounds;
}

void ANavBoundsVolume::SetBounds(const BlockCuboid& bounds) {
	SetBounds(conversion::blockCuboidToUe(bounds));
}

void ANavBoundsVolume::SetBounds(const FBox& bounds) {
	if (bounds == Bounds) {
		return;
	}
	GetRootComponent()->SetMobility(EComponentMobility::Movable);
	SetActorLocation(bounds.GetCenter());
	// Assuming this is a unit cube, division by 200 gives result in block
	// "radius" (200 also happens to be the dimensions of the box brush that's
	// used in ANavMeshBoundsVolume:s that are created in-editor)
	SetActorScale3D(bounds.GetSize() / 200.0f);
	Bounds = bounds;
	GetRootComponent()->SetMobility(EComponentMobility::Static);
}

void ANavBoundsVolume::UpdateNavigation() {
	UNavigationSystemV1* navigationSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	navigationSystem->OnNavigationBoundsUpdated(this);
}
