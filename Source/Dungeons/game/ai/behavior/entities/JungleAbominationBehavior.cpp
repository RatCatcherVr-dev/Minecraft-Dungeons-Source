#include "Dungeons.h"
#include "JungleAbominationBehavior.h"

#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/effects/AffectorGameplayEffect.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/effects/WeakenGameplayEffect.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/ai/action/CommonActions.h"
#include "game/ai/action/FocusActions.h"
#include "game/ai/behavior/FleeBehavior.h"
#include "game/ai/behavior/MeleeAttackBehavior.h"
#include "game/ai/behavior/MoveBehavior.h"
#include "game/ai/behavior/NodeFactory.h"
#include "game/ai/behavior/RoamBehavior.h"
#include "game/ai/bt/BtDelay.h"
#include "game/ai/bt/BtLogic.h"
#include "game/ai/bt/decorator/BtDropFor.h"
#include "game/ai/bt/decorator/BtEvery.h"
#include "game/ai/condition/ActorPredicates.h"
#include "game/ai/condition/BtInvert.h"
#include "game/ai/condition/BtPredicate.h"
#include "game/ai/condition/BtProbability.h"
#include "game/ai/condition/IsInRange.h"
#include "game/ai/condition/LocatorPredicates.h"
#include "game/ai/debug/BtDebug.h"
#include "game/ai/provider/ActorStats.h"
#include "game/ai/provider/Actors.h"
#include "game/ai/provider/BtRandom.h"
#include "game/ai/task/PlayAnimation.h"
#include "game/ai/task/TurnTask.h"
#include "game/ai/task/attack/MeleeAttack.h"
#include "game/ai/task/attack/SummonFormationTask.h"
#include "game/ai/task/attack/formations/JungleAbominationFormations.h"
#include "game/component/BehaviorOptionsComponent.h"
#include "game/component/MeleeAttackComponent.h"
#include "game/util/LocationQuery.h"
#include "game/util/Tags.h"
#include "util/Random.h"

#include "EngineUtils.h"
#include <Runtime/Engine/Classes/Engine/Engine.h>
#include "game/abilities/DungeonsAbilitySystemGlobals.h"

class UAnimationSequenceBase;

UJungleAbominationInvulnerabilityGameplayEffect::UJungleAbominationInvulnerabilityGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	FGameplayModifierInfo damageInfo;
	damageInfo.Attribute = UHealthAttributeSet::TakeDamageMultiplierAttribute();
	damageInfo.ModifierMagnitude = FScalableFloat(0.f);
	damageInfo.ModifierOp = EGameplayModOp::Type::Override;
	Modifiers.Add(damageInfo);
}

namespace bt { namespace behavior { namespace entities {

	BehaviorTuple createJungleAbomination(AMobCharacter& mob, const UBehaviorOptionsComponent& options) { 
		static Random rnd;

		auto meleeAttackComponent = mob.FindComponentByClass<UMeleeAttackComponent>();

		auto abilitySystem = mob.GetAbilitySystemComponent();
		if( abilitySystem ) {
			auto spec = effects::CreateGameplayEffectSpec<UJungleAbominationInvulnerabilityGameplayEffect>( abilitySystem );
			spec.SetSetByCallerMagnitude(*UHealthAttributeSet::TakeDamageMultiplierAttribute().GetName(), 0.0f);
			abilitySystem->ApplyGameplayEffectSpecToSelf( spec );
		}

		// Options
		const auto optWhispererCount = static_cast<int>( options.Get( "whisperer-count", 3.0f ));
		const auto optLeaperCount = static_cast<int>( options.Get( "leaper-count", 6.0f ));
		const auto optVineCount1 = static_cast<int>( options.Get( "vine-count1", 2.0f ));
		const auto optVineCount2 = static_cast<int>( options.Get( "vine-count2", 4.0f ));
		const auto optVineCount3 = static_cast<int>( options.Get( "vine-count3", 8.0f ));

		const auto optFallDuration = options.Get( "fall-duration", 0.3s );
		const auto optLandDuration = options.Get( "land-duration", 2.5s );

		const auto optSummonAnimationDuration = options.Get( "summon-duration", 2.8s);
		const auto optSummonDelay = options.Get( "summon-delay", 0.2s );
		const auto optSummonCooldown = options.Get("summon-cooldown", 30.0s);
		const auto optSummonMinLeaperCount = static_cast<int>( options.Get( "summon-min-leaper-count", 3.0f ) );
		const auto optSummonMaxLeaperCount = static_cast<int>( options.Get( "summon-max-leaper-count", 6.0f ) );
		const auto optSummonMinWhispererCount = options.Get("summon-min-whisperer-count", 3.0f);
		const auto optSummonMaxWhispererCount = options.Get("summon-max-whisperer-count", 6.0f);
		const auto optSummonSpawnRate = options.Get("summon-spawn-rate", 1.0f);
		const auto optSummonRadius = options.Get("summon-radius", 1500.0f);
		const auto optSummonLeaperThreshold = static_cast<int>(options.Get( "summon-leaper-threshold", 1.0f ));
		const auto optSummonWhispererThreshold = static_cast<int>(options.Get( "summon-whisperer-threshold", 1.0f ));

		const auto optTargetRadius = options.Get( "ideal-radius", 500.0f );
		const auto optChaseSpeed = options.Get( "chase-speed", 1.2f );
		const auto optMeleeCooldown = options.Get( "attack-cooldown", 3.0s );
		const auto optMeleeAttackConeRadius = options.Get( "melee-attack-cone-radius", 60.0f );

		const auto optPhase2HealthTrigger = options.Get( "phase2-start-health", 0.5f );
		const auto optPhase1CoreHealthTrigger = options.Get( "phase1-core-start-health", 0.8f );
		const auto optPhase2CoreHealthTrigger = options.Get( "phase2-core-start-health", 0.3f );
		const auto optPhase1CoreDuration = options.Get( "phase1-core-duration", 10.0s );
		const auto optPhase2CoreDuration = options.Get( "phase2-core-duration", 10.0s );
		const auto optCoreDamageMultiplier = options.Get( "core-damage-multiplier", 1.5f );

		const auto optAbominationVineSummonStartDuration = options.Get( "abomination-vine-summon-start-duration", 1.8s );
		const auto optAbominationVineSummonDuration = options.Get( "abomination-vine-summon-duration", 3.5s );
		const auto optAbominationVineSummonDelay = options.Get( "abomination-vine-summon-delay", 1.8s );

		// Variables
		const auto phase = makeSharedRef<int>(0);
		const auto coreActive = makeSharedRef<bool>(false);
		const auto phase1Core = makeSharedRef<bool>(false);
		const auto phase2Core = makeSharedRef<bool>(false);
		const auto meleeCount = makeSharedRef<int>(0);
		const auto canRangeAttack = makeSharedRef<int>(0);
		const auto varCanSummon = sharedRef(true);

		// Functions
		auto applyWeakening = [=](float magnitude) -> bt::Action {
			return [=](bt::StateRef state) {
				auto mob = state.owner;
				auto abilitySystem = mob->GetAbilitySystemComponent();
				if( abilitySystem ) {
					FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
					auto spec = effects::CreateGameplayEffectSpec<UWeakenGameplayEffect>( abilitySystem );
					spec.SetSetByCallerMagnitude( UWeakenGameplayEffect::DurationName, 1000.0f );
					spec.SetSetByCallerMagnitude( UWeakenGameplayEffect::DamageTakenName, optCoreDamageMultiplier );
					spec.SetSetByCallerMagnitude( UWeakenGameplayEffect::DamageDealtName, 1.0f );
					abilitySystem->ApplyGameplayEffectSpecToSelf(spec); 
				}
			};
		};

		auto removeWeakening = [=]() -> bt::Action {
			return [=](bt::StateRef state) {
				auto mob = state.owner;
				auto abilitySystem = mob->GetAbilitySystemComponent();
				if( abilitySystem ) {
					FGameplayEffectQuery query;
					query.EffectDefinition = UWeakenGameplayEffect::StaticClass();
					abilitySystem->RemoveActiveEffects( query ); 
				}
			};
		};

		auto coreLocation = [=]() -> bt::locator::Provider {
			return [=](bt::StateRef state) {
				FVector pos = state.owner->GetActorLocation();
				float distance = 500.0f;
				FVector vec( rnd.nextFloat( 0.0f, 1.0f ), rnd.nextFloat( 0.0f, 1.0f ), 0.0f );
				vec.Normalize();
				vec *= -distance;
				return FLocator( pos + vec );
			};
		};

		auto removeDamageAbsorption = [=]() -> bt::Action {
			return [=](bt::StateRef state) {
				auto mob = state.owner;
				auto abilitySystem = mob->GetAbilitySystemComponent();
				if( abilitySystem ) {
					FGameplayEffectQuery query;
					query.EffectDefinition = UJungleAbominationInvulnerabilityGameplayEffect::StaticClass();
					abilitySystem->RemoveActiveEffects( query );
				}
			};
		};

		auto btPhase1Transition = [=,&options]() { return uninterruptibleSequence("phase1-transition",
			startPredicate(equals(phase, value(0))),
			onStop(set(phase, value(1))),
			onStop(removeDamageAbsorption()),
			chain(
				playAnimation(options.Get("Fall"), false, true, true),
				delay(optFallDuration),
				playAnimation(options.Get("Land"), false, true, true),
				delay(optLandDuration),
				sequence(
					chain( make_unique<USummonFormationTask>(
							options.Get("SummonWhisperers"),
							10.0s, 0.0s, 0.0s, 0.0s, 0.0s,
							formation::JungleAbominationRadial( EntityType::Whisperer, optWhispererCount, 1500.0f ),
							USummonFormationTask::SummonMob,
							optSummonDelay
						), delay( 1000.0s ) 
					),
					delay( optSummonAnimationDuration )
				)
			)
		); };

		auto btPhase2Transition = [=,&options]() { return uninterruptibleSequence("phase2-transition",
			predicate(
				equals( phase, value(1) ) && 
				lessThan( actorStats::healthFraction(), value( optPhase2HealthTrigger ) )
			),
			onStop( set( phase, value( 2 ) ) ),
			onStop( set( phase1Core, value( true ) ) ),
			onStop( set( coreActive, value( false ) ) ),
			chain(
				playAnimation( options.Get( "Phase2" ) ),
				sequence(
					chain( make_unique<USummonFormationTask>(
							options.Get( "SummonLeapers" ),
							10.0s, 0.0s, 0.0s, 0.0s, 0.0s,
							formation::JungleAbominationRadial( EntityType::Leaper, optLeaperCount, 1500.0f ),
							USummonFormationTask::SummonMob,
							optSummonDelay
						), delay( 1000.0s ) 
					),
					delay( optSummonAnimationDuration )
				)
			)
		); };

		auto btPhase1Core = [=,&options] () { return uninterruptibleSequence( "phase1-core",
			startPredicate(
				equals( phase, value( 1 ) ) &&
				equals( phase1Core, value( false ) ) &&
				lessThan( actorStats::healthFraction(), value( optPhase1CoreHealthTrigger ) )
			),
			onStart( set( coreActive, value( true ) ) ),
			onStop( set( coreActive, value( false ) ) ),
			onStop( set( phase1Core, value( true ) ) ),
			chain(
				sequence( // D11.DB - Turn to face the camera before falling.
					behavior::moveTo( bt::move::From( coreLocation() ), Relative( 1.0f ) ),
					delay( 0.5s )
				),
				playAnimation( options.Get( "CoreStart" ) ),
				sequence(
					predicate(
						equals( phase1Core, value( false ) ) &&
						greaterThan( actorStats::healthFraction(), value( optPhase2HealthTrigger ) )
					),
					onStart( applyWeakening( optCoreDamageMultiplier ) ),
					onStop( removeWeakening() ),
					playAnimation( options.Get( "Core" ), true ),
					alwaysTrue( dropFor( optPhase1CoreDuration, set( phase1Core, value( true ) ) ) )
				),
				playAnimation( options.Get( "CoreEnd" ) )
			)
		); };

		auto btPhase2Core = [=,&options] () { return uninterruptibleSequence( "phase2-core",
			startPredicate(
				equals( phase, value( 2 ) ) &&
				equals( phase2Core, value( false ) ) &&
				lessThan( actorStats::healthFraction(), value( optPhase2CoreHealthTrigger ) )
			),
			onStart( set( coreActive, value( true ) ) ),
			onStop( set( coreActive, value( false ) ) ),
			onStop( set( phase2Core, value( true ) ) ),
			onStop( set( meleeCount, value( 0 ) ) ),
			chain(
				sequence( // D11.DB - Turn to face the camera before falling.
					behavior::moveTo( bt::move::From( coreLocation() ), Relative( 1.0f ) ),
					delay( 0.5s )
				),
				playAnimation( options.Get( "CoreStart" ) ),
				sequence(
					predicate(
						equals( phase2Core, value( false ) )
					),
					onStart( applyWeakening( optCoreDamageMultiplier ) ),
					onStop( removeWeakening() ),
					playAnimation( options.Get( "Core" ), true ),
					alwaysTrue( dropFor( optPhase2CoreDuration, set( phase2Core, value( true ) ) ) )
				),
				playAnimation( options.Get( "CoreEnd" ) )
			)
		); };

		auto vineSummon = [=,&options] ( float vineCount ) {
			return sequence(
				chain(
					sequence( delay( optAbominationVineSummonStartDuration ), playAnimation( options.Get( "SummonVines" ) ) ),
					playAnimation( options.Get( "SummonVinesLoop" ), true )
				),
				chain(
					delay( optAbominationVineSummonDelay ),
					make_unique<USummonFormationTask>( nullptr, 10.0s, 0.0s, 0.0s, 0.0s, 0.0s,
						formation::JungleAbominationPlayer( EntityType::AbominationVine, static_cast<int>(vineCount), 1500.0f ),
						USummonFormationTask::SummonMob, 1.0s ),
					delay( 1000.0s )
				),
				delay( optAbominationVineSummonDuration )
			);
		};

		const auto summonFormation = [=] ( bt::StateRef state, USummonFormationTask::AddEntryRef addEntry ) {
			const auto leaperCount = rnd.nextInt( optSummonMinLeaperCount, optSummonMaxLeaperCount );
			const auto whispererCount = rnd.nextInt( optSummonMinWhispererCount, optSummonMaxWhispererCount );
			float minDelay = 0.2f * optSummonSpawnRate;
			float maxDelay = 0.8f * optSummonSpawnRate;

			float delay = 0.0f;
			for( int i = 0; i < leaperCount; ++i ) {
				if( auto location = locationquery::getRandomLocationAround(state.world(), state.owner->GetActorLocation(), optSummonRadius) ) {
					addEntry(bt::Seconds(delay), EntityType::Leaper, nullptr, FTransform( location.GetValue() ), true, false);
				}
				delay += rnd.nextFloat( minDelay, maxDelay );
			}
			delay = 0.2f;
			for( int i = 0; i < whispererCount; ++i ) { 
				if( auto location = locationquery::getRandomLocationAround(state.world(), state.owner->GetActorLocation(), optSummonRadius) ) {
					addEntry(bt::Seconds(delay), EntityType::Whisperer, nullptr, FTransform( location.GetValue() ), true, false);
				}
				delay += rnd.nextFloat( minDelay, maxDelay );
			}
		};

		auto btJbomSummon = [=] () { return uninterruptibleSequence( "jbom-summon",
			startPredicate( equals( varCanSummon, value( true ) ) ),
			onStop( set( varCanSummon, value(false) ) ),
			minTimeBetweenStopAndStart( optSummonCooldown ),
			sequence(
				make_unique<USummonFormationTask>( nullptr,
					0.0s, 0.0s, 0.0s, 0.0s, 0.0s,
					summonFormation,
					USummonFormationTask::SummonMob,
					0.0s
				)
			)
		); };

		auto btJbomRangeAttack = [=,&options] () { return uninterruptibleSequence( "jbom-range-attack",
			startPredicate(
				!actor::HasRecentlyAttacked( optMeleeCooldown ) &&
				equals( phase, value( 2 ) ) &&
				( greaterThan( meleeCount, value( 2 ) ) || greaterThan( canRangeAttack, value( 1 ) ) )
			),
			onStop( set( meleeCount, value( 0 ) ) ),
			onStop( set( canRangeAttack, value( 0 ) ) ),
			chain(
				chain(
					vineSummon(optVineCount1),
					random::probability( 0.5f ),
					playAnimation( options.Get( "SummonFail" ) ),
					vineSummon(optVineCount2),
					random::probability( 0.5f ),
					playAnimation( options.Get( "SummonFail" ) ),
					vineSummon(optVineCount3)
				),
				playAnimation( options.Get( "SummonSuccess" ) )
			)
		); };

		auto btJbomMeleeAttack = [=] () { return uninterruptibleSequence( "jbom-melee-attack",
			startPredicate(
				!actor::HasRecentlyAttacked( optMeleeCooldown ) &&
				locator::IsInRange( actor::Target(), optTargetRadius ) &&
				actor::IsAnyPlayersInCone( FMath::DegreesToRadians( optMeleeAttackConeRadius ) )
			),
			onStop( inc( meleeCount ) ),
			make_unique<UMeleeAttack>( actor::Target(), true )
		); };

		return BehaviorTuple(
			root( selector( "JungleAbomination",
				btPhase1Transition(),
				btPhase2Transition(),
				btPhase1Core(),
				btPhase2Core(),
				btJbomSummon(),
				btJbomRangeAttack(),
				btJbomMeleeAttack(),
				behavior::chase(actor::Target(), 5000.0f, Relative(optChaseSpeed)),
				behavior::defaultRoam()
			)),
			parallel( "update-targets",
				every(0.5s, set(actor::Target(), actor::ClosestPlayer())),
				sequence(
					predicate( equals( phase, value( 2 ) ) ),
					every(10.0s, inc( canRangeAttack ) )
				),
				every(5.0s, 
					set( varCanSummon, 
						lessThanOrEquals( actor::CountInRange( actor::Self(), 2000.0f, actorquery::is::entityType(EntityType::Leaper) ), value(optSummonLeaperThreshold)) &&
						lessThanOrEquals( actor::CountInRange( actor::Self(), 2000.0f, actorquery::is::entityType(EntityType::Whisperer) ), value(optSummonWhispererThreshold))
					)
				)
			)
		);

	}

}}}
