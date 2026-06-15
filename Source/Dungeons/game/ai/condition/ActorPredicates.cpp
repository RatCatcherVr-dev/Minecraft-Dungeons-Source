#include "Dungeons.h"
#include "ActorPredicates.h"
#include "game/util/ActorQuery.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "game/component/MobActivationComponent.h"
#include <AbilitySystemComponent.h>

namespace bt { namespace actor {

Pred IsAlive(Provider prov) {
	return [provider = std::move(prov)](StateRef state) {
		return actorquery::isAlive(provider(state));
	};
}

Pred IsRecentlyDamagedFromAttack(Seconds maxSecondsSince) {
	return [maxSecondsSince](StateRef state) {
		return state.params().lastDamaged && (!state.params().lastDamaged->timeStamp.IsPassed(state, maxSecondsSince));
	};
}

Pred IsRecentlyDamagedFromAttackType(Seconds maxSecondsSince, FGameplayTag gameplayTag) {
	return [maxSecondsSince, gameplayTag](StateRef state) {
		if (state.params().lastDamaged && (!state.params().lastDamaged->timeStamp.IsPassed(state, maxSecondsSince)))
		{
			return state.params().lastDamaged->gameplayTagContainer.HasTagExact(gameplayTag);
		}
		return false;
	};
}

Pred IsRecentlyWarned(Seconds maxSecondsSince) {
	return [maxSecondsSince](StateRef state) {
		return state.params().lastWarning && (!state.params().lastWarning->timeStamp.IsPassed(state, maxSecondsSince));
	};
}

Pred IsNotSelf(Provider prov) {
	return [provider = std::move(prov)](StateRef state) {
		if (auto mob = Cast<AMobCharacter>(provider(state))) {
			return mob != state.owner;
		} else {
			return true;
		}
	};
}

Pred CanTargetLastAttacker() {
	return [](StateRef state) {
		if (state.params().lastDamaged) {
			return state.params().lastDamaged->actorSource.IsValid() && 
				state.params().lastDamaged->actorSource.Get() != state.owner && 
				actorquery::is::aliveAndAiVisible(state.params().lastDamaged->actorSource.Get());
		}
		return false;
	};
}

Pred CanTargetLastWarning() {
	return [](StateRef state) {
		if (state.params().lastWarning) {
			return state.params().lastWarning->actorSource.IsValid() && state.params().lastWarning->actorSource.Get() != state.owner;
		}
		return false;
	};
}

Pred IsPlayer(Provider prov) {
	return [provider = std::move(prov)](StateRef state) {		
		if (const auto player = provider(state)) {
			return player->IsA<APlayerCharacter>();
		}

		return false;
	};
}

Pred IsStuckInWeb(Provider prov) {
	return [provider = std::move(prov)](StateRef state) {
		auto player = Cast<APlayerCharacter>(provider(state));
		if (!player) {
			return false;
		}
		UAbilitySystemComponent* component = player->GetAbilitySystemComponent();
		return component->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("StatusEffect.Immobile.SpiderWeb"));
	};
}

Pred IsSlowed(Provider prov) {
	return[provider = std::move(prov)](StateRef state) {
		auto player = Cast<APlayerCharacter>(provider(state));
		if (!player) {
			return false;
		}
		UAbilitySystemComponent* component = player->GetAbilitySystemComponent();
		return component->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Slow"));
	};
}

Pred IsFreezing( Provider prov ) {
	return [provider = std::move(prov)](StateRef state) {
		auto player = Cast<APlayerCharacter>(provider(state));
		if (!player) {
			return false;
		}
		UAbilitySystemComponent* component = player->GetAbilitySystemComponent();
		return component->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Enchantment.Freezing"));
	};
}

Pred IsVisible( Provider prov ) {
	return [provider = std::move(prov)](StateRef state) {
		auto player = Cast<APlayerCharacter>(provider(state));
		if (!player) {
			return false;
		}
		UAbilitySystemComponent* component = player->GetAbilitySystemComponent();
		return !component->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Invisibility"));
	};
}

Pred IsVoided(Provider prov) {
	return[provider = std::move(prov)](StateRef state) {
		auto player = Cast<APlayerCharacter>(provider(state));
		if (!player) {
			return false;
		}
		UAbilitySystemComponent* component = player->GetAbilitySystemComponent();
		return component->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.VoidTouched")) ||
			component->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Voided"));
	};
}

Pred IsInWind(Provider prov) {
	return[provider = std::move(prov)](StateRef state) {
		auto character = Cast<ABaseCharacter>(provider(state));
		if (!character) {
			return false;
		}
		UPushVolumeReactiveComponent* component = character->GetPushVolumeReactiveComponent();
		if (!component) {
			return false;
		}
		return component->IsBeingEffectedByPushVolumes();
	};
}

Pred IsActivated(Provider prov) {
	return[provider = std::move(prov)](StateRef state) {
		auto character = Cast<ABaseCharacter>(provider(state));
		if (!character) {
			return false;
		}
		UMobActivationComponent* component = character->FindComponentByClass<UMobActivationComponent>();
		if (!component) {
			return false;
		}
		return component->IsMobActive();
	};
}

bool IsInCone(const FVector& u, const FVector& v, float angleMax = HALF_PI) {
	return (u | v.GetSafeNormal2D()) > FMath::Cos(angleMax);
}

Pred IsBeingLookedAt(actor::Provider observerProv, float angleMax/* = HALF_PI*/) {
	return [observerProvider = std::move(observerProv), angleMax](StateRef state) {
		if (const auto observer = observerProvider(state)) {
			return IsInCone(
				observer->GetActorForwardVector(),
				state.owner->GetActorLocation() - observer->GetActorLocation(),
				angleMax
			);
		}

		return false;
	};
}

Pred IsBeingLookedAway(actor::Provider observerProv, float angleMax/* = HALF_PI*/) {
	return [observerProvider = std::move(observerProv), angleMax](StateRef state) {
		if (const auto observer = observerProvider(state)) {
			return IsInCone(
				observer->GetActorForwardVector(),
				observer->GetActorLocation() - state.owner->GetActorLocation(),
				angleMax
			);
		}
		return false;
	};
}

Pred IsInLineOfSight(actor::Provider observerProv) {
	return [observerProvider = std::move(observerProv)](StateRef state) {
		if (const auto observer = observerProvider(state)) {
			const auto observerCapsule = observer->FindComponentByClass<UCapsuleComponent>();
			const auto observerLocation = observer->GetActorLocation() + FVector { 0.f, 0.f, observerCapsule->GetScaledCapsuleHalfHeight() * .5f };

			const auto ownerCapsule = state.owner->FindComponentByClass<UCapsuleComponent>();
			const auto ownerLocation = state.owner->GetActorLocation() + FVector { 0.f, 0.f, ownerCapsule->GetScaledCapsuleHalfHeight() * .5f };

			{
				FHitResult hitResult;
				if (state.world().LineTraceSingleByChannel(hitResult, ownerLocation, observerLocation, static_cast<ECollisionChannel>(ECustomTraceChannels::TerrainOnly))) {
					// DrawDebugLine(&state.world(), ownerLocation, observerLocation, FColor::Red, false, 5.f);
					return false;
				} else {
					// DrawDebugLine(&state.world(), ownerLocation, observerLocation, FColor::White, false, 5.f);
					return true;
				}
			}
		}

		return false;
	};
}

Pred IsInFront(locator::Provider loc, float angleMax/* = HALF_PI*/) {
	return [locator = std::move(loc), angleMax](StateRef state) {
		if (const auto target = locator(state).GetLocationLike()) {
			return IsInCone(
				state.owner->GetActorForwardVector(),
				target.GetValue() - state.owner->GetActorLocation(),
				angleMax
			);
		}
		return false;
	};
}

bool IsInFront(const AActor& owner, const AActor& target) {
	return IsInCone(
		owner.GetActorForwardVector(),
		target.GetActorLocation() - owner.GetActorLocation()		
	);
}

bool IsInCone(const AActor& owner, const AActor& target, float angleMax) {
	return IsInCone(
		owner.GetActorForwardVector(),
		target.GetActorLocation() - owner.GetActorLocation(),
		angleMax
	);
}

Pred IsAllPlayersInFront() {
	return [](StateRef state) {
		for (auto&& player : InstanceTracker<APlayerCharacter>::GetList(&state.world())) {
			if (!IsInFront(*state.owner, *player)) {
				return false;
			}
		}
		return true;
	};
}

Pred IsAnyPlayersInFront() {
	return [](StateRef state) {
		for (auto&& player : InstanceTracker<APlayerCharacter>::GetList(&state.world())) {
			if (IsInFront(*state.owner, *player)) {
				return true;
			}
		}
		return false;
	};
}

Pred IsAnyPlayersInCone(float angleMax) {
	return [angleMax](StateRef state) {
		for (auto&& player : InstanceTracker<APlayerCharacter>::GetList(&state.world())) {
			if (IsInCone(*state.owner, *player, angleMax)) {
				return true;
			}
		}
		return false;
	};
}

Pred IsInRight(locator::Provider loc) {
	return [locator = std::move(loc)](StateRef state) {
		if (const auto target = locator(state).GetLocationLike()) {
			return FVector::CrossProduct(
				state.owner->GetActorForwardVector(),
				target.GetValue() - state.owner->GetActorLocation()
			).Z > 0.f;
		}
		return false;
	};
}

Pred IsInLeft(locator::Provider loc) {
	return [locator = std::move(loc)](StateRef state) {
		if (const auto target = locator(state).GetLocationLike()) {
			return FVector::CrossProduct(
				state.owner->GetActorForwardVector(),
				target.GetValue() - state.owner->GetActorLocation()
			).Z < 0.f;
		}
		return false;
	};
}

Pred HasRecentlyAttacked(const bt::Duration& maxDurationSince) {
	return [=](StateRef state) {
		const auto& lastAttackTime = state.params().lastAttackTime;
		return ! lastAttackTime.IsPassedOrZero(state, maxDurationSince);
	};
}

Pred HasRecentlyTeleported(const bt::Duration& maxDurationSince) {
	return [=](StateRef state) {
		const auto& teleportTime = state.params().teleportTime;

		return ! teleportTime.IsPassedOrZero(state, maxDurationSince);
	};
}

Pred HasRecentlyDefended(const bt::Duration& maxDurationSince)
{
	return [=](StateRef state) {
		const auto& defenseTime = state.params().lastDefenseTime;
		return !defenseTime.IsPassedOrZero(state, maxDurationSince);
	};
}

//! AT: should be rewritten as a provider to get all players and a "any" provider with a filter
Pred IsAnyPlayersInRange(float range) {
	const auto rangeSquared = range * range;

	return [rangeSquared](StateRef state) {
		const auto ownerLocation = state.owner->GetActorLocation();

		for (auto&& player : InstanceTracker<APlayerCharacter>::GetList(&state.world())) {
			if (FVector::DistSquared(ownerLocation, player->GetActorLocation()) < rangeSquared) {
				return true;
			}
		}

		return false;
	};
}

Pred HasTag(const FName& tag) {
	return [tag](StateRef state) {
		return state.owner->ActorHasTag(tag);
	};
}

Pred HasTag(Provider prov, const FName& tag) {
	return [provider = std::move(prov), tag](StateRef state) {
		const auto actor = provider(state);
		return actor != nullptr && actor->ActorHasTag(tag);
	};
}

}}
