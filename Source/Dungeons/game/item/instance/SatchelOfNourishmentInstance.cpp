#include "SatchelOfNourishmentInstance.h"
#include "Dungeons.h"
#include "util/Algo.h"

void ASatchelOfNourishmentInstance::Activate(const FPredictionKey& predictionKey) {
	Super::Activate(predictionKey);
	using namespace game::item::util;
	if (Role == ROLE_Authority) {
		PossibleItemsToDrop = algo::copy_if_map_tarray(GetItemRegistry().GetValues(), RETLAMBDA(it->hasTag(ItemTag::Food)), RETLAMBDA(FSpecifiedStoreCountItemDrop(it->getId())));
		DropItems(1, false, false);
	}
}