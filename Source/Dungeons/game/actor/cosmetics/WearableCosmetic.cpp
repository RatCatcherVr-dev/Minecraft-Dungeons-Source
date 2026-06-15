#include "Dungeons.h"
#include "WearableCosmetic.h"

AWearableCosmetic::AWearableCosmetic() {
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	Paperdoll = CreateDefaultSubobject<UPaperdollComponent>(TEXT("Paperdoll"));
	SetRootComponent(Paperdoll);
}

void AWearableCosmetic::AttachToPlayerMesh(USkeletalMeshComponent* playerMesh) {
	Paperdoll->AttachToPlayerMesh(playerMesh);
}

void AWearableCosmetic::BeginPlay() {
	Super::BeginPlay();
	if (Paperdoll) {
		Paperdoll->OnAttachedToPlayerMesh.AddUObject(this, &AWearableCosmetic::OnDetachedComponentCreated);
	}
}

void AWearableCosmetic::EndPlay(const EEndPlayReason::Type Reason)
{
#if !WITH_EDITOR
	//clear out any lasting deferred physics updating for sub components (physics system seems to hold on to raw pointers of them for some reason)
	if (auto* CurPhysicScene = GetWorld()->GetPhysicsScene())
	{
		for (auto detachComponent : DetachedComponents)
		{
			if(USkeletalMeshComponent* pSkelMeshComp = Cast<USkeletalMeshComponent>(detachComponent))
				CurPhysicScene->ClearPreSimKinematicUpdate(pSkelMeshComp);
		}
	}
#endif

	Super::EndPlay(Reason);
}

void AWearableCosmetic::OnDetachedComponentCreated(USceneComponent* component) {
	if (component) {
		DetachedComponents.Add(component);
	}
}
