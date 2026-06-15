#include "Dungeons.h"
#include "PickupComponent.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "GameplayEffectTypes.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/component/BackpackComponent.h"
#include "game/component/RelocateComponent.h"
#include "game/util/Tags.h"
#include "DungeonsGameInstance.h"
#include "game/abilities/effects/GameplayEffectUtil.h"

namespace game { namespace pickup {
	bool wasKilledByHealthChange(const FOnAttributeChangeData& data) {
		return data.NewValue != data.OldValue && data.OldValue >= 0.f && data.NewValue <= 0.f;
	}

	FGameplayTag getDamageType(const FOnAttributeChangeData& data) {
		if (data.GEModData) {
			
		}

		return damageTag::def();
	}
	
	bool isEnvironmentalDamage(const FOnAttributeChangeData& data) {
		const auto damageTypes = effects::GetDungeonsDamageTypes(*data.GEModData);

		static const FGameplayTagContainer enviromentalDamage = FGameplayTagContainer::CreateFromArray(TArray<FGameplayTag>({ damageTag::killzone(), damageTag::lava()}));

		return damageTypes.HasAnyExact(enviromentalDamage);
	}
	
	AActor* getHealthChangeInstigator(const FOnAttributeChangeData& data) {
		if (data.GEModData) {
			const auto& handle = data.GEModData->EffectSpec.GetEffectContext();
			return handle.GetInstigator();
		}
		return nullptr;
	}

	bool isOnTeam(const AActor* actor, const ETeamName team) {
		if (const auto* character = Cast<ABaseCharacter>(actor)) {
			return character->GetCurrentTeam() == team;
		}
		return false;
	}

	bool isAllowedToPickUp(const AActor* instigator) {
		const auto mobOnHeroesTeam = instigator->IsA<AMobCharacter>() && isOnTeam(instigator, ETeamName::Heroes);
		return !mobOnHeroesTeam && !instigator->ActorHasTag(tags::noPickUp);
	}
	
}}

UPickupComponent::UPickupComponent() {
	PrimaryComponentTick.bCanEverTick = false;
}

void UPickupComponent::BeginPlay() {
	Super::BeginPlay();
	StoredLocation = GetOwner()->GetActorLocation();
}

void UPickupComponent::OnAttributeHealthChange(const FOnAttributeChangeData& data) const {
	if (game::pickup::wasKilledByHealthChange(data) && (game::pickup::isEnvironmentalDamage(data) || !AttemptPickUp(data))) {
		const auto ownerMob = GetOwnerAsMob();
		const auto resetLocation = StoredLocation.Get(ownerMob->GetActorLocation());
		ownerMob->RespawnAt(resetLocation);
	}
}

bool UPickupComponent::AttemptPickUp(const FOnAttributeChangeData& data) const {
	const auto* instigator = game::pickup::getHealthChangeInstigator(data);
	if (instigator && game::pickup::isAllowedToPickUp(instigator)) {
		if (auto* backpackComponent = instigator->FindComponentByClass<UMochilaComponent>()) {
			if (Cast<ABaseCharacter>(GetOwner())->IsLocationReachable(instigator->GetActorLocation())) {
				PickUp(backpackComponent);
				return true;
			}
		}
	}
	return false;
}

void UPickupComponent::PickUp(UMochilaComponent* backpackComponent) const {
	const auto ownerMob = GetOwnerAsMob();

	if (PickupParticleSystem != nullptr) {
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PickupParticleSystem, ownerMob->GetActorTransform());
	}

	if (!bIsFakePickUp)
	{
		backpackComponent->Store(
			MountedBlueprint,
			ownerMob->EntityType,
			StoredLocation.Get(ownerMob->GetActorLocation()),
			ownerMob->FindComponentByClass<URelocateComponent>()->RelocateLocation
		);
	}
	else
	{
		ownerMob->Destroy();
	}
}

AMobCharacter* UPickupComponent::GetOwnerAsMob() const {
	return Cast<AMobCharacter>(GetOwner());
}
