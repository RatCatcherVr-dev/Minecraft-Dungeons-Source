#include "Dungeons.h"
#include "BaseParticleAssetsComponent.h"

UBaseParticleAssetsComponent::UBaseParticleAssetsComponent() {
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}
