#include "Dungeons.h"
#include "TriggerUtil.h"
#include "game/Conversion.h"
#include "game/util/ComponentUtils.h"
#include "lovika/BlockCuboid.h"
#include "lovika/Region.h"
#include "world/phys/Vec3.h"
#include <GameFramework/Actor.h>
#include <Components/BoxComponent.h>
#include "game/util/ActorQuery.h"

namespace game { namespace trigger {

UBoxComponent* createCollisionBox(AActor& actor, const lovika::Region& region, ExpandType expandType) {
	auto box = componentutils::CreateComponent<UBoxComponent>(actor);
	auto regionSize = region.area().size();

	auto boxRadii = 0.5f * conversion::posToUe(regionSize.x, 50, regionSize.z);
	auto location = conversion::posToUe(centerFloor(region));

	switch (expandType) {
	case ExpandType::Both:
		break;
	case ExpandType::Down:
		boxRadii.Z *= 0.5f;
		location.Z -= boxRadii.Z - 10.0f;
		break;
	default:
		checkNoEntry();
	}
	box->SetBoxExtent(boxRadii);
	box->SetWorldLocation(location);
	box->SetWorldScale3D(FVector::OneVector);
	box->SetCollisionResponseToAllChannels(ECR_Ignore);
	box->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	box->SetCollisionResponseToChannel(static_cast<ECollisionChannel>(ECustomTraceChannels::PlayerPawn), ECR_Overlap);
	box->bHiddenInGame = true;
	return box;
}

}}
