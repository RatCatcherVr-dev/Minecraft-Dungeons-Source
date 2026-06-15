#include "Dungeons.h"
#include "CospawnComponent.h"
#include "game/GameBP.h"
#include "game/mobspawn/MobSpawnConfigs.h"
#include "DungeonsGameInstance.h"
#include "game/mobspawn/MobAction.h"
#include "TeleportToOwnerComponent.h"

UCospawnComponent::UCospawnComponent() {
	PrimaryComponentTick.bCanEverTick = false;
}

void UCospawnComponent::BeginPlay() {
	Super::BeginPlay();
	if (GetOwnerRole() == ROLE_Authority) {
		SpawnAll();
	}
}

void UCospawnComponent::Spawn(FVector offset) {
	auto* gameBP = actorquery::getFirstActor<AGameBP>(GetWorld());
	auto characterOwner = Cast<ABaseCharacter>(GetOwner());

	auto spawnConfig = game::mobspawn::configs::DefaultNoVariants(true).SpawnAsUnderling().ChangeTeam(characterOwner->GetCurrentTeam()).SetTeleportToOwner(characterOwner);

	if (assignMaster) {
		spawnConfig.ChangeMaster(characterOwner);
	}

	FTransform transform;
	transform.SetTranslation(GetOwner()->GetActorLocation() + offset);
	transform.SetRotation(GetOwner()->GetActorRotation().Quaternion());
	if (scaleToOwner) {
		transform.SetScale3D(GetOwner()->GetActorScale3D());
	}

	gameBP->RequestMobSpawn(
		entityType,
		RETLAMBDA0(transform),
		enchantments,
		spawnConfig
	);
}

void UCospawnComponent::SpawnAll() {
	const auto forward = GetOwner()->GetActorForwardVector() * radius;

	for (auto i = 0; i < count; i++) {
		Spawn(forward.RotateAngleAxis((360.f / count) * i, FVector::UpVector));
	}
}