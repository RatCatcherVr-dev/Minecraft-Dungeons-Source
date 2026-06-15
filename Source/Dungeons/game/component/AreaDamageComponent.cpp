#include "Dungeons.h"
#include "AoeAttackComponent.h"
#include "AreaDamageComponent.h"
#include "HealthComponent.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/effects/WorldDamageGameplayEffect.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/util/DungeonsEffectLibrary.h"
#include "game/util/Tags.h"
#include "world/entity/MobTags.h"

UAreaDamageComponent::UAreaDamageComponent() {
	PrimaryComponentTick.bCanEverTick = true;
}

void UAreaDamageComponent::BeginPlay() {
	Super::BeginPlay();
	SetComponentTickInterval(tickInterval);
}

void UAreaDamageComponent::ApplyDamage() {
	const auto attacker { GetOwner() };
	const auto attackerLocation{ attacker->GetActorLocation() };
	const auto attackerCharacter = Cast<ABaseCharacter>(attacker);

	FGameplayEffectSpecHandle handle;

	if(attackerCharacter) {
		auto abilitySystem = attackerCharacter->GetAbilitySystemComponent();
		handle = abilitySystem->MakeOutgoingSpec(UAoeAttackDamageGameplayEffect::StaticClass(), 1, abilitySystem->MakeEffectContext());
		handle.Data->SetSetByCallerMagnitude(effects::HealthName, -damage);
		handle.Data->GetContext().AddInstigator(attacker, attackerCharacter->GetMaster() ? attackerCharacter->GetMaster() : attacker);
		handle.Data->GetContext().AddOrigin(attacker->GetActorLocation());
		handle.Data->DynamicAssetTags.AddTag(damageType);

		if(affectsSelf) {
			handle.Data->DynamicAssetTags.AddTag(damageTag::damageFriends());
		}
	}
	

	const auto radiusSquared{ FMath::Square(radius) };
	FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
	
	for (auto targetCharacter : InstanceTracker<ABaseCharacter>::GetList(GetWorld())) {
		if(!Affects(targetCharacter, attackerCharacter)) continue;

		const auto targetLocation = targetCharacter->GetActorLocation();

		const bool isInAreaXy = blockBased
			? (FMath::Abs(attackerLocation.X - targetLocation.X) < radius && FMath::Abs(attackerLocation.Y - targetLocation.Y) < radius)
			: FVector::DistSquared2D(attackerLocation, targetLocation) < radiusSquared;

		if (!isInAreaXy) {
			continue;
		}

		const auto halfHeight = targetCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		if (
			attackerLocation.Z < targetLocation.Z - halfHeight - radius ||
			attackerLocation.Z > targetLocation.Z + halfHeight + radius
		) {
			continue;
		}

		if (pushback.enablePushback && GetOwnerRole() == ROLE_Authority) {
			UDungeonsEffectLibrary::PushBackOnActor(pushback, attacker, targetCharacter);
		}

		if (handle.IsValid()) {
			 targetCharacter->GetAbilitySystemComponent()->BP_ApplyGameplayEffectSpecToSelf(handle);
		}
		else {
			UAbilitySystemComponent* abilitySystem = targetCharacter->GetAbilitySystemComponent();
			FGameplayEffectSpec spec = effects::CreateGameplayEffectSpec<UWorldDamageGameplayEffect>(abilitySystem, effects::HealthName, -damage, attacker, attacker, attacker->GetActorLocation(), 1.f);
			spec.DynamicAssetTags.AddTag(damageType);
			abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
		}
	}
}

void UAreaDamageComponent::TickComponent(float deltaTime, ELevelTick levelTick, FActorComponentTickFunction* actorComponentTickFunction) {
	Super::TickComponent(deltaTime, levelTick, actorComponentTickFunction);

	if (tickSkip > 0) {
		tickSkip--;
		return;
	}

	if (tickMax == 0) {
		SetComponentTickEnabled(false);
		return;
	} else if (tickMax > 0) {
		tickMax--;
	}

	ApplyDamage();
}

bool UAreaDamageComponent::Affects(const ABaseCharacter* target, const ABaseCharacter* source) {
	const bool isSelf = target == source;
	if(isSelf) {
		return affectsSelf;
	}
	
	if (!source) {
		if( target->IsA<APlayerCharacter>() && affectsPlayers) {
			return true;
		}

		if (const auto* mob = Cast<AMobCharacter>(target)) {
			const bool isPet = hasMobTag(mob->EntityType, MobTags::HashTag_Pet);

			return (isPet && affectsPets) || (!isPet && affectsMobs);
		}
	} else {
		const bool isAllowedToDamage = source->CanDamageTarget(target);
		
		if (const auto* mob = Cast<AMobCharacter>(target)) {
			if(hasMobTag(mob->EntityType, MobTags::HashTag_Pet)) {
				return affectsPets && isAllowedToDamage;
			}
		}

		return isAllowedToDamage;
	}

	return false;
}
