#include "Dungeons.h"
#include "PetCosmeticInfo.h"

#include "game/actor/character/BaseCharacter.h"
#include "game/mobspawn/alpha/AlphaSpawner.h"
#include "game/mobspawn/MobSpawnConfigs.h"
#include "game/mobspawn/MobSpawnProviders.h"
#include "game/mobspawn/MobSpawner.h"
#include "game/mobspawn/MobAction.h"
#include "game/util/Tags.h"
#include "lovika/LovikaLevelActor.h"
#include "game/abilities/effects/AffectorGameplayEffect.h"

APetCosmeticInfo::APetCosmeticInfo()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void APetCosmeticInfo::Equip(UChildActorComponent* assignedChildActorComponent, USkeletalMeshComponent* playerMesh) {
	Super::Equip(assignedChildActorComponent, playerMesh);	
	SetActorTickEnabled(true);
}

void APetCosmeticInfo::Unequip() {
	SetActorTickEnabled(false);
	if (target.IsValid()) {
		target->Destroy();
		target.Reset();
	}
}

void APetCosmeticInfo::Tick(const float deltaTime) {
	if (TrySpawnPet()) {
		SetActorTickEnabled(false);
	} 
}

void APetCosmeticInfo::OnSpawned(AMobCharacter* mob) {
	if(mob) {
		target = mob;
		GetWorld()->GetTimerManager().SetTimer(relocateHandle, this, &APetCosmeticInfo::Relocate, .5f);
		affector::effect::applyEffects(mob->GetAbilitySystemComponent(), true);
	} else {
		SetActorTickEnabled(true);
	}
}

void APetCosmeticInfo::Relocate() {
	if (target.IsValid()) {
		target->SetActorLocation(target->GetMaster()->GetActorLocation(), false, nullptr, ETeleportType::ResetPhysics);
	}
}

bool APetCosmeticInfo::TrySpawnPet() {
	if (target.IsValid()) {
		return true;
	}

	if (!actorquery::getFirstActor<ALovikaLevelActor>(GetWorld())) {
		// Is not in game level, will not be able to spawn pet.
		return true;
	}

	using namespace game::mobspawn;	
	if (auto owner = Cast<APlayerCharacter>(GetOwner())) {
		if (!owner->HasAuthority()) {
			return true;
		}

		if (!owner->IsLoadedInLevel()) {
			return false;
		}

		using namespace  game::mobspawn;

		game::mobspawn::spawnAsync(*GetWorld(), Mob, providers::Location(owner->GetActorLocation()),
			configs::PlayerPets().Action(ChangeMaster(owner)).Action(AddTag(tags::cosmetic)), [weakThis = TWeakObjectPtr<APetCosmeticInfo>(this)](auto mob) {
			if(weakThis.IsValid()) {
				weakThis->OnSpawned(mob);
			} else if(mob) {
				mob->Destroy();
			}
		}, MobSpawnPriority::ESpawnPriority_High);
		
	}

	return true;
}
