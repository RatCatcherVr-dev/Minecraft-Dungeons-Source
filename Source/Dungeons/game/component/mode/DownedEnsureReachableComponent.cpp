#include "Dungeons.h"
#include "DownedEnsureReachableComponent.h"
#include "DungeonsGameState.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "DungeonsGameMode.h"
#include "game/component/HealthComponent.h"

const TArray<EMaterialTypeEnum> UDownedEnsureReachableComponent::DangerousMaterials = { EMaterialTypeEnum::Lava, EMaterialTypeEnum::Fire };

void UDownedEnsureReachableComponent::BeginPlay() {
	Super::BeginPlay();
	if (const auto gameMode = Cast<ADungeonsGameMode>(GetOwner())) {
		gameMode->OnPlayerCharacterAdded.AddUObject(this, &UDownedEnsureReachableComponent::OnPlayerCharacterAdded);
	}
}

void UDownedEnsureReachableComponent::OnPlayerCharacterAdded(APlayerCharacter* player) {
	player->OnPlayerDown.AddUObject(this, &UDownedEnsureReachableComponent::OnPlayerCharacterDown, player);
	player->OnCharacterWalking.AddUObject(this, &UDownedEnsureReachableComponent::OnPlayerCharacterLanded, player);
	player->OnPlayerRevive.AddUObject(this, &UDownedEnsureReachableComponent::OnPlayerCharacterRevive, player);
	player->OnStepMaterialChanged.AddUObject(this, &UDownedEnsureReachableComponent::OnPlayerCharacterMaterial, player);
	player->OnOverlapMaterialChanged.AddUObject(this, &UDownedEnsureReachableComponent::OnPlayerCharacterMaterial, player);

	if (auto* hc = player->GetHealthComponent()) {
		hc->OnDamageTypeReceived.AddUObject(this, &UDownedEnsureReachableComponent::OnPlayerCharacterDamageType, player);
	}

	if (IsDown(player)) {
		OnPlayerCharacterDown(player);
	}
}

void UDownedEnsureReachableComponent::OnPlayerCharacterDown(APlayerCharacter* player) const {	
	if (!player->GetCharacterMovement()->IsFalling() && IsAnyPlayerAlive() && (IsPlayerInDangerousMaterial(player) || player->IsStuck())) {
		UE_LOG(LogDungeons, Display, TEXT("Downed player to be teleported."));
		player->TeleportToSafety();
	}
}

void UDownedEnsureReachableComponent::OnPlayerCharacterLanded(APlayerCharacter* player) const {
	if (IsDown(player) && IsAnyPlayerAlive() && (IsPlayerInDangerousMaterial(player) || player->IsStuck())) {
		UE_LOG(LogDungeons, Display, TEXT("Landed player to be teleported."));
		player->TeleportToSafety();
	}
}

void UDownedEnsureReachableComponent::OnPlayerCharacterRevive(APlayerCharacter* player) const {
	for (auto&& otherPlayer : actorquery::getActors<APlayerCharacter>(GetWorld())) {
		if (player != otherPlayer && IsDown(otherPlayer) && player->IsStuck()) {
			UE_LOG(LogDungeons, Display, TEXT("Other player to be teleported on revive."));
			player->TeleportToSafety();
		}
	}
}

void UDownedEnsureReachableComponent::OnPlayerCharacterMaterial(const EMaterialTypeEnum material, APlayerCharacter* player) const {
	if (IsDown(player) && DangerousMaterials.Contains(material)) {
		UE_LOG(LogDungeons, Display, TEXT("Downed player is in dangerous material."));
		UE_LOG(LogDungeons, Display, TEXT("Player with changed material to be teleported."));
		player->TeleportToSafety();
	}
}

void UDownedEnsureReachableComponent::OnPlayerCharacterDamageType(float damageAmount, const FGameplayTagContainer& damageTag, APlayerCharacter* player) const {
	if (IsDown(player) && IsAnyPlayerAlive() && (IsDamageTypeUnreachable(damageTag) || player->IsStuck())) {
		UE_LOG(LogDungeons, Display, TEXT("Damaged player to be teleported."));
		player->TeleportToSafety();
	}
}

bool UDownedEnsureReachableComponent::IsPlayerInDangerousMaterial(APlayerCharacter* player) {
	const auto playerIsInDangerousMaterial = DangerousMaterials.Contains(player->GetOverlapMaterial()) || DangerousMaterials.Contains(player->GetStepMaterial());

	if (playerIsInDangerousMaterial) {
		UE_LOG(LogDungeons, Display, TEXT("Downed player is in dangerous material."));
	}

	return playerIsInDangerousMaterial;
}

bool UDownedEnsureReachableComponent::IsDamageTypeUnreachable(const FGameplayTagContainer& damageTags) {
	static FGameplayTagContainer unreachable = FGameplayTagContainer::CreateFromArray(TArray<FGameplayTag>({ damageTag::killzone(), damageTag::lava(), damageTag::toxicWater(), damageTag::trap() }));

	const auto damageTypeIsUnreachable = damageTags.HasAny(unreachable);
	
	if (damageTypeIsUnreachable) {
		UE_LOG(LogDungeons, Display, TEXT("Downed player received unreachable damage."));
	}

 	return damageTypeIsUnreachable;
}

bool UDownedEnsureReachableComponent::IsAnyPlayerAlive() const {
	return IsAnyPlayer(EAliveState::Alive) || IsAnyPlayer(EAliveState::Reviving);
}

bool UDownedEnsureReachableComponent::IsAnyPlayer(const EAliveState aliveState) const {
	return GetGameState()->GetPlayersMatchingAliveStateCount(aliveState) > 0;
}

bool UDownedEnsureReachableComponent::IsDown(APlayerCharacter*& player) {
	return player->GetAliveState() == EAliveState::Down;
}

ADungeonsGameState* UDownedEnsureReachableComponent::GetGameState() const {
	const auto gameState = Cast<ADungeonsGameMode>(GetOwner())->GetGameState<ADungeonsGameState>();
	check(gameState && "Missing GameState in DungeonsGameMode");
	return gameState;
}