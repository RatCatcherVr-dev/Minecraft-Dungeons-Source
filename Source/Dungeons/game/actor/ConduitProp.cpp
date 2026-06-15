#include "ConduitProp.h"
#include "DungeonsGameInstance.h"

AConduitProp::AConduitProp(const FObjectInitializer& objectInitializer) : Super(objectInitializer) {
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}