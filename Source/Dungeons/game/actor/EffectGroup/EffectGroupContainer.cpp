#include "Dungeons.h"
#include "EffectGroup.h"
#include "ReusedEffectActor.h"
#include "EffectGroupContainer.h"
#include "game/item/instance/AItemInstance.h"
#include "game/util/ActorQuery.h"


void AEffectsGroupContainerActor::BeginPlay() {
	Super::BeginPlay();
	InstanceTracker< AEffectsGroupContainerActor >::AddInstance(GetWorld(),this);
}
void AEffectsGroupContainerActor::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);
	InstanceTracker< AEffectsGroupContainerActor >::RemoveInstance(GetWorld(),this);
}

void AEffectsGroupContainerActor::SpawnEffectInGroup(TSubclassOf<AReusedEffectActor> actorClass, const FTransform& transform, FEffectGroupParams parameters) {
	if(!actorClass) return;
	
	UEffectGroup* group = nullptr;

	if(auto found = Groups.Find(actorClass.Get())) {
		group = *found;
	} else {
		group = NewObject<UEffectGroup>(this);
		Groups.Add(actorClass.Get(), group);
	}

	group->SetParams(std::move(parameters));
	group->SpawnEffect(actorClass, transform);	
}


void UEffectGroupContainer::SpawnEffectInGroup(UObject* worldContext, TSubclassOf<AReusedEffectActor> actorClass, const FTransform& transform, FEffectGroupParams parameters) {
	UWorld* world = worldContext->GetWorld();
	
	AEffectsGroupContainerActor* actor = actorquery::getFirstActorTracked<AEffectsGroupContainerActor>(worldContext->GetWorld());

	if(!actor) {
		actor = world->SpawnActor<AEffectsGroupContainerActor>();
		ensureMsgf(actor, TEXT("Failed to spawn effects group actor, something is horribly wrong"));
	}

	actor->SpawnEffectInGroup(actorClass, transform, parameters);
}