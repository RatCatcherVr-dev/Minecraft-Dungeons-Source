#include "Dungeons.h"
#include "PerfectFormHeartBehavior.h"

#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/effects/AffectorGameplayEffect.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/effects/WeakenGameplayEffect.h"
#include "game/actor/character/animinstance/PerfectFormAnimInstance.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/ai/action/AnimActions.h"
#include "game/ai/action/CommonActions.h"
#include "game/ai/action/FocusActions.h"
#include "game/ai/action/StateActions.h"
#include "game/ai/behavior/MeleeAttackBehavior.h"
#include "game/ai/behavior/MoveBehavior.h"
#include "game/ai/behavior/NodeFactory.h"
#include "game/ai/behavior/RangedAttackBehavior.h"
#include "game/ai/behavior/RoamBehavior.h"
#include "game/ai/bt/BtDelay.h"
#include "game/ai/bt/BtLogic.h"
#include "game/ai/bt/BtTypes.h"
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
#include "game/ai/task/attack/AoeAttack.h"
#include "game/ai/task/attack/MeleeAttack.h"
#include "game/ai/task/attack/RangedAttack.h"
#include "game/ai/task/attack/SummonFormationTask.h"
#include "game/component/BehaviorOptionsComponent.h"
#include "game/component/GravityPulseComponent.h"
#include "game/component/MeleeAttackComponent.h"
#include "game/component/MultiOffsetRangedAttackComponent.h"
#include "game/component/PerfectFormComponent.h"
#include "game/component/RangedAttackComponent.h"
#include "game/util/LocationQuery.h"
#include "game/util/Tags.h"
#include "util/Random.h"
#include "util/RandomUtil.h"

#include "EngineUtils.h"
#include <Runtime/Engine/Classes/Engine/Engine.h>
#include "game/abilities/DungeonsAbilitySystemGlobals.h"

class UAnimationSequenceBase;

UPerfectFormInvulnerabilityGameplayEffect::UPerfectFormInvulnerabilityGameplayEffect()
{
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

	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("Immunity.Damage"));
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("Immunity.Damage"));
	GrantedApplicationImmunityTags.RequireTags.AddTag(FGameplayTag::RequestGameplayTag("Damage.Melee"));
	GrantedApplicationImmunityTags.RequireTags.AddTag(FGameplayTag::RequestGameplayTag("Damage.Ranged"));
}

namespace EAIFocusPriority {
	const Type Fixate = 4;
}

namespace bt { namespace behavior { namespace entities {

BehaviorTuple createPerfectFormHeart(AMobCharacter& mob, const UBehaviorOptionsComponent& options) {
	enum class EndermiteAttackType {
		Scatter = 0,
		Feast = 1,
		VoidPull = 2,
	};

	static Random rnd;

	//Component Tags
	const auto aoeAtkCompTagCharge = makeSharedRef<FName>("AoeCharge");
	const auto aoeAtkCompTagVoidpull = makeSharedRef<FName>("AoeVoidpull");
	const auto rAtkCompTagBombSpew = makeSharedRef<FName>("BombSpew");
	const auto rAtkCompTagEndermite = makeSharedRef<FName>("EndermiteScatter");
	const auto rAtkCompTagEndermitePool = makeSharedRef<FName>("EndermitePool");
	const auto rAtkCompTagOffset_S1F = makeSharedRef<FName>("Seg_1_Eye_F");
	const auto rAtkCompTagOffset_S1B = makeSharedRef<FName>("Seg_1_Eye_B");
	const auto rAtkCompTagOffset_S1L = makeSharedRef<FName>("Seg_1_Eye_L");
	const auto rAtkCompTagOffset_S1R = makeSharedRef<FName>("Seg_1_Eye_R");
	const auto rAtkCompTagOffset_S2F = makeSharedRef<FName>("Seg_2_Eye_F");
	const auto rAtkCompTagOffset_S2B = makeSharedRef<FName>("Seg_2_Eye_B");
	const auto rAtkCompTagOffset_S2L = makeSharedRef<FName>("Seg_2_Eye_L");
	const auto rAtkCompTagOffset_S2R = makeSharedRef<FName>("Seg_2_Eye_R");

	// Components
	const auto abilitySystem = mob.GetAbilitySystemComponent();
	const auto animInstance = Cast<UPerfectFormAnimInstance>(mob.GetMesh()->GetAnimInstance());
	const auto healthComponent = mob.FindComponentByClass<UHealthComponent>();
	const auto perfectFormComponent = mob.FindComponentByClass<UPerfectFormComponent>();
	const auto gravityPulseComponent = mob.FindComponentByClass<UGravityPulseComponent>();
	const auto rAtkComp_BombSpew = Cast<URangedAttackComponent>(mob.GetComponentsByTag(URangedAttackComponent::StaticClass(), *rAtkCompTagBombSpew)[0]);
	const auto rAtkComp_Endermite = Cast<URangedAttackComponent>(mob.GetComponentsByTag(URangedAttackComponent::StaticClass(), *rAtkCompTagEndermite)[0]);
	const auto rAtkComp_EndermitePool = Cast<URangedAttackComponent>(mob.GetComponentsByTag(URangedAttackComponent::StaticClass(), *rAtkCompTagEndermitePool)[0]);
	const auto rAtkComp_Laser_S1F = Cast<URangedAttackComponent>(mob.GetComponentsByTag(URangedAttackComponent::StaticClass(), *rAtkCompTagOffset_S1F)[0]);
	const auto rAtkComp_Laser_S1B = Cast<URangedAttackComponent>(mob.GetComponentsByTag(URangedAttackComponent::StaticClass(), *rAtkCompTagOffset_S1B)[0]);
	const auto rAtkComp_Laser_S1L = Cast<URangedAttackComponent>(mob.GetComponentsByTag(URangedAttackComponent::StaticClass(), *rAtkCompTagOffset_S1L)[0]);
	const auto rAtkComp_Laser_S1R = Cast<URangedAttackComponent>(mob.GetComponentsByTag(URangedAttackComponent::StaticClass(), *rAtkCompTagOffset_S1R)[0]);
	const auto rAtkComp_Laser_S2F = Cast<URangedAttackComponent>(mob.GetComponentsByTag(URangedAttackComponent::StaticClass(), *rAtkCompTagOffset_S2F)[0]);
	const auto rAtkComp_Laser_S2B = Cast<URangedAttackComponent>(mob.GetComponentsByTag(URangedAttackComponent::StaticClass(), *rAtkCompTagOffset_S2B)[0]);
	const auto rAtkComp_Laser_S2L = Cast<URangedAttackComponent>(mob.GetComponentsByTag(URangedAttackComponent::StaticClass(), *rAtkCompTagOffset_S2L)[0]);
	const auto rAtkComp_Laser_S2R = Cast<URangedAttackComponent>(mob.GetComponentsByTag(URangedAttackComponent::StaticClass(), *rAtkCompTagOffset_S2R)[0]);

	// Options
	const auto optTickDelay = options.Get("micro-delay", 0.01s);
	const auto optSpeedMultiplier = options.Get("speed-multiplier", 1.f);

	const auto optAtkSelectDelay = options.Get("attack-select-delay", 0.2s);
	const auto optAtkSelectCooldown = options.Get("attack-select-cooldown", 5s);

	const auto optIntroAnimDuration = options.Get("intro-anim-duration", 1.887s);
	const auto optPhase2HealthThreshold = options.Get("phase2-health-threshold", 0.7f);
	const auto optPhase3HealthThreshold = options.Get("phase3-health-threshold", 0.3f);

	const auto optPhase1CoreDuration = options.Get("phase1-core-duration", 10.0s);
	const auto optPhase2CoreDuration = options.Get("phase2-core-duration", 10.0s);
	const auto optCoreDamageMultiplier = options.Get("core-damage-multiplier", 1.5f);

	const auto optAtkTypeWeightCharge = options.Get("attack-weight-charge", 0.45f);
	const auto optAtkTypeWeightSpew = options.Get("attack-weight-spew", 0.12f);
	const auto optAtkTypeWeightScatter = options.Get("attack-weight-scatter", 0.12f);
	const auto optAtkTypeWeightFeast = options.Get("attack-weight-feast", 0.11f);
	const auto optAtkTypeWeightLaser = options.Get("attack-weight-laser", 0.2f);

	const auto optChargeAtkMaxTargetRange = options.Get("charge-target-max-range", 3000.0f);
	const auto optChargeAtkTraceDistance = options.Get("charge-max-trace-distance", 5400.0f);
	const auto optChargeAtkEndDistanceFromWall = options.Get("charge-end-distance-from-wall", 250.0f);
	const auto optChargeAtkRotateMinDelay = options.Get("charge-rotate-min-delay", 2.0f);
	const auto optChargeAtkRotateMaxDelay = options.Get("charge-rotate-max-delay", 4.0f);
	const auto optChargeAtkExecuteDelay = options.Get("charge-execute-delay", 0.5s);
	const auto optChargeAtkAoeInterval = options.Get("charge-aoe-interval", 0.05s);
	const auto optChargeAtkSpeedModifier = options.Get("charge-speed-modifier", 10.f);
	const auto optChargeReverseDelay = options.Get("charge-reverse-delay", 0.05f);
	const auto optChargeReverseRotateDelay = options.Get("charge-reverse-rotate-delay", 0.3f);
	const auto optChargeReverseSpeedModifier = options.Get("charge-reverse-speed-modifier", 2.0f);
	const auto optChargeAtkDistanceToReverse = options.Get("charge-distance-to-reverse", 700.0f);
	const auto optChargeAtkStartDuration = options.Get("charge-start-duration", 0.667s);

	const auto optBombSpewRotateMinDelay = options.Get("bombspew-rotate-min-delay", 1.0f);
	const auto optBombSpewRotateMaxDelay = options.Get("bombspew-rotate-max-delay", 3.0f);
	const auto optBombSpewAmmoCountMin = options.Get("bombspew-ammo-count-min", 15);
	const auto optBombSpewAmmoCountMax = options.Get("bombspew-ammo-count-max", 30);
	const auto optBombSpewVoidAmmoWeight = options.Get("bombspew-void-ammo-weight", 0.25f);
	const auto optBombSpewExecuteDelay = options.Get("bombspew-execute-delay", 0.5s);
	const auto optBombSpewStartDuration = options.Get("bombspew-start-duration", 0.625s);
	const auto optBombSpewEndDuration = options.Get("bombspew-end-duration", 0.583s);

	const auto optEndermiteScatterCountMin = options.Get("endermite-scatter-count-min", 10);
	const auto optEndermiteScatterCountMax = options.Get("endermite-scatter-count-max", 25);
	const auto optEndermiteScatterSummonDuration = options.Get("endermite-scatter-summon-duration", 5.0f);
	const auto optEndermiteScatterStartDuration = options.Get("endermite-scatter-start-duration", 0.375s);
	const auto optEndermiteScatterEndDuration = options.Get("endermite-scatter-end-duration", 0.625s);

	const auto optEndermiteFeastCountMin = options.Get("endermite-feast-count-min", 4);
	const auto optEndermiteFeastCountMax = options.Get("endermite-feast-count-max", 10);
	const auto optEndermiteFeastSummonDuration = options.Get("endermite-feast-summon-duration", 5.0f);
	const auto optEndermiteFeastConsumeDelay = options.Get("endermite-feast-consume-delay", 0.5s);
	const auto optEndermiteFeastConsumeDuration = options.Get("endermite-feast-consume-duration", 5.0s);
	const auto optEndermiteFeastStartDuration = options.Get("endermite-feast-start-duration", 0.583s);
	const auto optEndermiteFeastEndDuration = options.Get("endermite-feast-end-duration", 0.625s);

	const auto optVoidpoolEnterDuration = options.Get("voidpool-start-duration", 1.583s);
	const auto optVoidpoolExitDuration = options.Get("voidpool-end-duration", 2.917s);

	const auto optVoidPullBeginDelay = options.Get("voidpull-begin-delay", 2.0s);
	const auto optVoidPullEndDelay = options.Get("voidpull-end-delay", 3.0s);
	const auto optVoidPullAoeBlastInitialDelay = options.Get("voidpull-aoe-blast-initial-delay", 4.0s);
	const auto optVoidPullAoeBlastInterval = options.Get("voidpull-aoe-blast-interval", 4.0s);
	const auto optVoidPullAoeBlastAttacksCount = options.Get("voidpull-aoe-blast-attacks-count", 4);
	const auto optVoidPullEndermiteIntervalMin = options.Get("voidpull-endermite-interval-min", 3.0f);
	const auto optVoidPullEndermiteIntervalMax = options.Get("voidpull-endermite-interval-max", 6.0f);
	const auto optVoidPullEndermiteCountMin = options.Get("voidpull-endermite-count-min", 1);
	const auto optVoidPullEndermiteCountMax = options.Get("voidpull-endermite-count-max", 3);
	const auto optVoidpullStartDuration = options.Get("voidpull-start-duration", 0.792s);
	const auto optVoidpullEndDuration = options.Get("voidpull-end-duration", 0.417s);

	const auto optLaserBeginDelay = options.Get("laser-begin-delay", 2.0s);
	const auto optLaserEndDelay = options.Get("laser-end-delay", 3.0s);
	const auto optLaserTotalDuration = options.Get("laser-total-duration", 20.0s);
	const auto optLaserRotationRateFactor = options.Get("laser-rotation-rate-factor", 1.0f);
	const auto optLaserRateOfFireFactor = options.Get("laser-rate-of-fire-factor", 0.5f);
	const auto optLaserBulletDelayMin = options.Get("laser-bullet-delay-min", 0.5f);
	const auto optLaserBulletDelayMax = options.Get("laser-bullet-delay-max", 3.0f);
	const auto optLaserBulletDelayRounding = options.Get("laser-bullet-delay-rounding", 0.5f);
	const auto optLaserMirrorHealthThreshold = options.Get("laser-mirror-health-threshold", 0.5f);
	const auto optLaserMirrorActivationProbability = options.Get("laser-mirror-activation-chance", 0.85f);
	const auto optLaserStartDuration = options.Get("laser-start-duration", 3.417s);
	const auto optLaserEndDuration = options.Get("laser-end-duration", 1.375s);


	// Variables
	const auto combatPhase = makeSharedRef<int>(0);
	const auto attackSelection = makeSharedRef<int>(0);
	const auto transitionPhase = makeSharedRef<int>(static_cast<int>(TransitionPhase::NONE));
	const auto laserPhase = makeSharedRef<int>(static_cast<int>(LaserPhase::NONE));
	const auto attackCounter = makeSharedRef<int>(0);
	const auto bIsSubmerged = makeSharedRef<bool>(false);
	const auto bVoidPoolActive = makeSharedRef<bool>(false);
	const auto bMirrorsActive = makeSharedRef<bool>(false);

	
	// --------------------------
	if (perfectFormComponent) {
		perfectFormComponent->SetTransitionPhaseDebug(static_cast<TransitionPhase>(*transitionPhase));
		perfectFormComponent->SetLaserPhaseDebug(static_cast<LaserPhase>(*laserPhase));
	}

	// Debugging Lambdas
	auto debugMessageViaComponent = [&perfectFormComponent](const FString& msg, float duration) -> bt::Action { return [=](StateRef state) {
		perfectFormComponent->PrintOnce(msg, duration);
	}; };

	// --------------------------

	// Predicates
	const auto isCombatPhase = [&combatPhase](CombatPhase query) -> Pred { return [=](bt::StateRef state) {
		return *combatPhase == static_cast<int>(query);
	}; };

	const auto isAttackType = [&attackSelection](AttackType query) -> Pred { return [=](bt::StateRef state) {
		return *attackSelection == static_cast<int>(query);
	}; };

	const auto isTransitionPhase = [&transitionPhase](TransitionPhase query) -> Pred { return [=](bt::StateRef state) {
		return *transitionPhase == static_cast<int>(query);
	}; };

	const auto isLaserPhase = [&laserPhase](LaserPhase query) -> Pred { return [=](bt::StateRef state) {
		return *laserPhase == static_cast<int>(query);
	}; };

	const auto thresholdPredicate = [=]() -> Pred {
		return logicalOr(
			isCombatPhase(CombatPhase::PhaseOne) && lessThanOrNearlyEquals(actorStats::healthFraction(), value(optPhase2HealthThreshold)),
			isCombatPhase(CombatPhase::PhaseTwo) && lessThanOrNearlyEquals(actorStats::healthFraction(), value(optPhase3HealthThreshold))
	); };

	// Lambdas
	const auto applyInvulnerability = [abilitySystem]() -> bt::Action { return [=](bt::StateRef state) {
		if (abilitySystem) {
			auto spec = effects::CreateGameplayEffectSpec<UPerfectFormInvulnerabilityGameplayEffect>(abilitySystem);
			spec.SetSetByCallerMagnitude(*UHealthAttributeSet::TakeDamageMultiplierAttribute().GetName(), 0.0f);
			abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
		}
		state.owner->SetIsBlocking(true);
	}; };

	const auto removeInvulnerability = [abilitySystem]() -> bt::Action { return [=](bt::StateRef state) {
		if (abilitySystem) {
			FGameplayEffectQuery query;
			query.EffectDefinition = UPerfectFormInvulnerabilityGameplayEffect::StaticClass();
			abilitySystem->RemoveActiveEffects(query);
		}
		state.owner->SetIsBlocking(false);
	}; };

	const auto arrayAttackWeights = [attackSelection, optAtkTypeWeightCharge, optAtkTypeWeightSpew, optAtkTypeWeightScatter, optAtkTypeWeightFeast, optAtkTypeWeightLaser]() -> const TArray<float> {
		return TArray<float>{optAtkTypeWeightCharge, optAtkTypeWeightSpew, optAtkTypeWeightScatter,
			static_cast<AttackType>(*attackSelection) == AttackType::EndermiteFeast ? 0.f : optAtkTypeWeightFeast,
			static_cast<AttackType>(*attackSelection) == AttackType::LaserPhaser ? 0.f : optAtkTypeWeightLaser
		};
	};
	
	const auto incCombatPhase = [=]() -> bt::Action { return [=](bt::StateRef state) {
		if (*combatPhase < static_cast<int>(CombatPhase::PhaseFinal)) {
			*combatPhase += 1;
			perfectFormComponent->SetCombatPhaseDebug(static_cast<CombatPhase>(*combatPhase));
		}
	}; };

	const auto incTransitionPhase = [=]() -> bt::Action { return [=](bt::StateRef state) {
		if (*transitionPhase < static_cast<int>(TransitionPhase::NONE)) {
			*transitionPhase += 1;
			perfectFormComponent->SetTransitionPhaseDebug(static_cast<TransitionPhase>(*transitionPhase));
		}
	}; };

	const auto incLaserPhase = [=]() -> bt::Action { return [=](bt::StateRef state) {
		if (*laserPhase < static_cast<int>(LaserPhase::NONE)) {
			*laserPhase += 1;
			perfectFormComponent->SetLaserPhaseDebug(static_cast<LaserPhase>(*laserPhase));
		}
	}; };

	const auto setTransitionPhase = [=](TransitionPhase request) -> bt::Action { return [=](bt::StateRef state) {
		*transitionPhase = static_cast<int>(request);
		perfectFormComponent->SetTransitionPhaseDebug(static_cast<TransitionPhase>(*transitionPhase));
	}; };

	const auto setLaserPhase = [=](LaserPhase request) -> bt::Action { return [=](bt::StateRef state) {
		*laserPhase = static_cast<int>(request);
		perfectFormComponent->SetLaserPhaseDebug(static_cast<LaserPhase>(*laserPhase));
	}; };
	
	const auto setRandomWeightedAttackType = [=]() -> bt::Action { return [=](StateRef state) {
		*attackSelection = Util::randomWeightedItemIndex(arrayAttackWeights(), &rnd);
		perfectFormComponent->SetAttackTypeDebug(static_cast<AttackType>(*attackSelection));
	}; };

	const auto getLocationTowardsCamera = []() -> location::Provider { return [](StateRef state) {
		return state.owner->GetActorLocation() + FVector{ -100.f, -100.f, 0.f }; 
	}; };

	const auto getChargeReverseFocalPoint = [&optChargeAtkEndDistanceFromWall]() -> location::Provider { return [=](StateRef state) {
		const FVector origin = state.owner->GetMesh()->GetSocketTransform(FName("J_Hips")).GetLocation();
		const FVector centre = state.params().startPos;
		// remove Z from consideration, we don't care about elevation change
		FVector direction = FVector(centre.X, centre.Y, 0.f) - FVector(origin.X, origin.Y, 0.f);
		direction.Normalize();

		return FVector{ origin - direction * optChargeAtkEndDistanceFromWall };
	}; };

	const auto teleportToCentre = [&perfectFormComponent]() { return [=](StateRef state) {
		const auto teleportLocation = state.params().startPos + FVector{ -200.f, -200.f, 0.f };
		const auto rotation = FRotationMatrix::MakeFromX(FVector{ -1.f, -1.f, 0.f }).Rotator();
		state.owner->SetActorLocationAndRotation(teleportLocation, rotation, false, nullptr, ETeleportType::ResetPhysics);
	}; };

	const auto chargeForwardProvider = [&perfectFormComponent, &optChargeAtkTraceDistance, &optChargeAtkEndDistanceFromWall]() -> move::Provider { return [=](StateRef state) {
		UWorld* world = state.owner->GetWorld();
		const FVector origin = state.owner->GetMesh()->GetSocketTransform(FName("J_Hips")).GetLocation();
		const FVector forwardNormal = state.owner->GetActorForwardVector();
		EDrawDebugTrace::Type drawDebugType = perfectFormComponent->GetDebugState() ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

		FHitResult hitResult;
		UKismetSystemLibrary::LineTraceSingle(world, origin, origin + forwardNormal * optChargeAtkTraceDistance, UEngineTypes::ConvertToTraceType(static_cast<ECollisionChannel>(ECustomTraceChannels::TerrainOnly)), false, {}, drawDebugType, hitResult, true);
		
		FVector target = hitResult.ImpactPoint - forwardNormal * optChargeAtkEndDistanceFromWall;

		hitResult.Init();
		UKismetSystemLibrary::LineTraceSingle(world, target, target + -state.owner->GetActorUpVector() * 600.f, UEngineTypes::ConvertToTraceType(static_cast<ECollisionChannel>(ECustomTraceChannels::TerrainOnly)), false, {}, drawDebugType, hitResult, true);

		FAIMoveRequest request = move::defaultRequest();
		request.SetCanStrafe(false);
		request.SetAcceptanceRadius(200.f);
		request.SetGoalLocation(hitResult.ImpactPoint);

		return request;
	}; };

	const auto chargeReverseProvider = [&perfectFormComponent, &optChargeAtkTraceDistance, &optChargeAtkDistanceToReverse]() -> move::Provider { return [=](StateRef state) {
		UWorld* world = state.owner->GetWorld();
		const FVector origin = state.owner->GetMesh()->GetSocketTransform(FName("J_Hips")).GetLocation();
		const FVector start = origin + -state.owner->GetActorForwardVector() * optChargeAtkDistanceToReverse;
		EDrawDebugTrace::Type drawDebugType = perfectFormComponent->GetDebugState() ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

		FHitResult hitResult;
		UKismetSystemLibrary::LineTraceSingle(world, start, start + -state.owner->GetActorUpVector() * 600.f, UEngineTypes::ConvertToTraceType(static_cast<ECollisionChannel>(ECustomTraceChannels::TerrainOnly)), false, {}, drawDebugType, hitResult, true);

		FAIMoveRequest request = move::defaultRequest();
		request.SetGoalLocation(hitResult.ImpactPoint);
		
		return request;
	}; };

	const auto returnToCentreProvider = []() -> move::Provider { return [=](StateRef state) {
		FVector goal = state.params().startPos;
		if (const auto maybeGround = locationquery::findGround(state.world(), state.params().startPos, true)) {
			goal.Z = maybeGround.GetValue();
		}
 		FAIMoveRequest request = move::defaultRequest();
		request.SetGoalLocation(goal);
 		request.SetAcceptanceRadius(200.f);
		return request;
	}; };

	const auto setAnimState = [&perfectFormComponent](EPerfectFormAnimLocomotion inAnimLocamotionState) -> bt::Action { return [=](StateRef state) {
		perfectFormComponent->SetAnimLocomotionState(inAnimLocamotionState);
	}; };

	const auto setMirrorsActive = [&bMirrorsActive, &perfectFormComponent, &optLaserMirrorActivationProbability](bool bInActive) -> bt::Action { return [=](StateRef state) {
		if (*bMirrorsActive != bInActive) {
			perfectFormComponent->SetMirrorsActive(bInActive, optLaserMirrorActivationProbability);
			*bMirrorsActive = bInActive;
		}
	}; };

	const auto setVoidPoolActive = [&perfectFormComponent, &bVoidPoolActive]() -> bt::Action { return [=](StateRef state) {
		if (!*bVoidPoolActive) {
			perfectFormComponent->SetVoidPoolActive(true);
			*bVoidPoolActive = true;
		}
	}; };

	const auto setMinionsToFeast = [&perfectFormComponent](bool bInActive) -> bt::Action { return [=](StateRef state) {
		perfectFormComponent->SetEndermiteFeast(bInActive);
	}; };

	const auto setActiveGravPulse = [&gravityPulseComponent](bool bInActive) -> bt::Action { return [=](StateRef state) {
		gravityPulseComponent->SetPulseActive(bInActive);
	}; };

	const auto randomiseBombSpewAmmo = [=]() -> bt::Action { return [=](StateRef state) {
		if (rAtkComp_BombSpew) {
			auto& projectileDefs = rAtkComp_BombSpew->AttackDefinitionByRef().ProjectileDefinition;
			if (projectileDefs.Num() == 6) {
				int totalAmmo = rnd.nextInt(optBombSpewAmmoCountMin, optBombSpewAmmoCountMax + 1);
				int voidAmmo = FMath::CeilToInt(totalAmmo * optBombSpewVoidAmmoWeight);
				int splitVoidAmmo = FMath::CeilToInt(voidAmmo * 0.5f);
				int splitExplosiveAmmo = FMath::CeilToInt((totalAmmo - voidAmmo) * 0.25f);
				int splitExplosiveHeavyAmmo = (totalAmmo - voidAmmo - (splitExplosiveAmmo * 2.f)) * 0.5f;

				projectileDefs[0].NumberOfProjectiles = splitExplosiveAmmo;							// Explosive
				projectileDefs[1].NumberOfProjectiles = splitExplosiveHeavyAmmo;					// Explosive Heavy
				projectileDefs[2].NumberOfProjectiles = splitVoidAmmo; 								// Void
				projectileDefs[3].NumberOfProjectiles = splitExplosiveAmmo;							// Explosive
				projectileDefs[4].NumberOfProjectiles = voidAmmo - splitVoidAmmo;					// Void Heavy
				projectileDefs[5].NumberOfProjectiles = splitExplosiveHeavyAmmo;					// Explosive Heavy
			}
		}
	}; };

	const auto setEndermiteAttackVariant = [&rAtkComp_Endermite](int index) -> bt::Action { return [&rAtkComp_Endermite, index](StateRef state) {
		rAtkComp_Endermite->SetRangeAttackVariant(index);
	}; };

	const auto randomiseEndermiteAmmo = [=](const EndermiteAttackType inType) -> bt::Action { return [=](StateRef state) {
		if (rAtkComp_Endermite && inType != EndermiteAttackType::VoidPull)	{
			auto& projectileDefs = rAtkComp_Endermite->AttackDefinitionByRef().ProjectileDefinition;
			if (projectileDefs.Num() == 1) {
				int totalEndermites = 0;
				float perSecDelay = 0.f;
				switch (inType) {
				case EndermiteAttackType::Scatter:
					totalEndermites = rnd.nextInt(optEndermiteScatterCountMin, optEndermiteScatterCountMax + 1);
					perSecDelay = optEndermiteScatterSummonDuration / static_cast<float>(totalEndermites);
					break;
				case EndermiteAttackType::Feast:
					totalEndermites = rnd.nextInt(optEndermiteFeastCountMin, optEndermiteFeastCountMax + 1);
					perSecDelay = optEndermiteFeastSummonDuration / static_cast<float>(totalEndermites);
					break;
				}
				projectileDefs[0].NumberOfProjectiles = totalEndermites;
				projectileDefs[0].DelaySecondsPerProjectile = perSecDelay;
			}
		}
		else if (rAtkComp_EndermitePool) {
			auto& attackDef = rAtkComp_EndermitePool->AttackDefinitionByRef();
			attackDef.ProjectileDefinition[0].NumberOfProjectiles = rnd.nextInt(optVoidPullEndermiteCountMin, optVoidPullEndermiteCountMax + 1);
			attackDef.AttackRateSeconds = rnd.nextFloat(optVoidPullEndermiteIntervalMin, optVoidPullEndermiteIntervalMax);
		}
	}; };

	const auto randomiseLaserPairTimings = [=]() -> bt::Action { return [=](StateRef state) {
		enum class ESegmentEyePairing {
			Segment_One_FrontBack,
			Segment_One_LeftRight,
			Segment_Two_FrontBack,
			Segment_Two_LeftRight
		};

		const auto randRoundedTimingInRange = [=](ESegmentEyePairing Pairing) {
			enum class ERoundingType {
				None,
				RoundUp,
				Custom,
			};
			ERoundingType rType = ERoundingType::Custom;

			if (optLaserBulletDelayRounding <= 0.f) {
				rType = ERoundingType::None;
			}
			if (optLaserBulletDelayRounding >= 1.f) {
				rType = ERoundingType::RoundUp;
			}

			float finalValue = 1.f;
			switch (rType)
			{
			case ERoundingType::None:
				finalValue = rnd.nextFloat(optLaserBulletDelayMin, optLaserBulletDelayMax);
				break;
			case ERoundingType::RoundUp:
				finalValue = FMath::RoundToFloat(rnd.nextFloat(optLaserBulletDelayMin, optLaserBulletDelayMax));
				break;
			case ERoundingType::Custom:
			default:
				// Yes, this will incur some small inaccuracies depending on the rounding value. It's fine for this, these timings are nothing critical.
				const float roundingMagnitude = 1.f / optLaserBulletDelayRounding;
				float baseRandVal = rnd.nextFloat(optLaserBulletDelayMin, optLaserBulletDelayMax);
				float valByMag = baseRandVal * roundingMagnitude;
				float roundedValByMag = FMath::RoundToFloat(valByMag);
				float customRoundedVal = roundedValByMag * optLaserBulletDelayRounding;
				float healthFactor = 1.f - ((1.0f - healthComponent->GetCurrentHealthPercentage()) * optLaserRateOfFireFactor);
				finalValue = customRoundedVal * healthFactor;
				break;
			}

			switch (Pairing)
			{
			case ESegmentEyePairing::Segment_One_FrontBack:
				rAtkComp_Laser_S1F->AttackDefinitionByRef().AttackRateSeconds = finalValue;
				rAtkComp_Laser_S1B->AttackDefinitionByRef().AttackRateSeconds = finalValue;
				break;
			case ESegmentEyePairing::Segment_One_LeftRight:
				rAtkComp_Laser_S1L->AttackDefinitionByRef().AttackRateSeconds = finalValue;
				rAtkComp_Laser_S1R->AttackDefinitionByRef().AttackRateSeconds = finalValue;
				break;
			case ESegmentEyePairing::Segment_Two_FrontBack:
				rAtkComp_Laser_S2F->AttackDefinitionByRef().AttackRateSeconds = finalValue;
				rAtkComp_Laser_S2B->AttackDefinitionByRef().AttackRateSeconds = finalValue;
				break;
			case ESegmentEyePairing::Segment_Two_LeftRight:
				rAtkComp_Laser_S2L->AttackDefinitionByRef().AttackRateSeconds = finalValue;
				rAtkComp_Laser_S2R->AttackDefinitionByRef().AttackRateSeconds = finalValue;
				break;
			default:
				checkNoEntry();
				break;
			}
		};
		randRoundedTimingInRange(ESegmentEyePairing::Segment_One_FrontBack);
		randRoundedTimingInRange(ESegmentEyePairing::Segment_One_LeftRight);
		randRoundedTimingInRange(ESegmentEyePairing::Segment_Two_FrontBack);
		randRoundedTimingInRange(ESegmentEyePairing::Segment_Two_LeftRight);
	}; };

	const auto updateLaserBodyRotationRate = [&perfectFormComponent, &healthComponent, &optLaserRotationRateFactor]() -> bt::Action { return [=](StateRef state) {
		perfectFormComponent->SetLaserBodyRotationRate(1.0f + ((1.0f - healthComponent->GetCurrentHealthPercentage()) * optLaserRotationRateFactor));
	}; };

	const auto enterVoidPool = [=](bt::Duration inDelaySubmerged) { return sequence("enter-void-pool",
		onStart(applyInvulnerability()),
		onStart(setVoidPoolActive()),
		onStart(set(bIsSubmerged, value(true))),
		chain(
			sequence(delay(0.3s), onStart(focus::Set(location::StartPos())), onStop(focus::Clear())),
			behavior::moveTo(move::withSettings(returnToCentreProvider(), move::defaultRequest()), speed::Speed(EMovementState::Running, optChargeAtkSpeedModifier)),
			sequence(delay(optVoidpoolEnterDuration),
				onStart(bt::state::SetIsTargetable(false)),
				onStop(teleportToCentre()),
				setAnimState(EPerfectFormAnimLocomotion::Submerge)
			),
			delay(inDelaySubmerged)
		)
	); };

	const auto exitVoidPool = [=](bt::Duration inDelaySubmerged) { return sequence("exit-void-pool",
		onStop(removeInvulnerability()),
		onStop(bt::state::SetIsTargetable(true)),
		onStop(set(bIsSubmerged, value(false))),
		chain(delay(inDelaySubmerged),
			sequence(delay(optVoidpoolExitDuration), setAnimState(EPerfectFormAnimLocomotion::Idle))
		)
	); };
	

	// Attack Behaviors
	const auto btPformChargeAttack = [=, &options]() { return sequence("charge-attack",
		onStart(set(actor::Target(), actor::FurthestPlayer(optChargeAtkMaxTargetRange))),
		onStart(focus::Set(actor::Target(), EAIFocusPriority::Attacking)),
		chain( delay(bt::Seconds(rnd.nextFloat(optChargeAtkRotateMinDelay, optChargeAtkRotateMaxDelay))),
			sequence( delay(optChargeAtkExecuteDelay), focus::Clear(EAIFocusPriority::Attacking) ),
			chain(
				sequence( delay(optChargeAtkStartDuration), setAnimState(EPerfectFormAnimLocomotion::Charge) ),
				sequence(
					behavior::moveTo(chargeForwardProvider(), speed::Speed(EMovementState::Dashing, optChargeAtkSpeedModifier)),
					alwaysTrue(sequence( delay(optChargeAtkAoeInterval), make_unique<UAoeAttack>(*aoeAtkCompTagCharge) )),
					onStop(setAnimState(EPerfectFormAnimLocomotion::Idle))
				),
				delay(optChargeReverseDelay), // Brief pause so attack doesn't seem weightless.
				sequence(delay(optChargeReverseRotateDelay), onStart(focus::Set(getChargeReverseFocalPoint(), EAIFocusPriority::Fixate))),
				uninterruptibleSequence(
					onStart(setAnimState(EPerfectFormAnimLocomotion::WalkReverse)),
					behavior::moveTo(chargeReverseProvider(), speed::Speed(EMovementState::Running, optChargeReverseSpeedModifier)),
					onStop(setAnimState(EPerfectFormAnimLocomotion::Idle)),
					onStop(focus::Clear(EAIFocusPriority::Fixate))
				),
				sequence(delay(optTickDelay), make_unique<UAoeAttack>(*aoeAtkCompTagCharge))
			)
		)
	); };
 
	const auto btPformBombSpew = [=, &options]() { return sequence("bomb-spew",
		onStart(set(actor::Target(), actor::FurthestPlayer(optChargeAtkMaxTargetRange))),
		onStart(focus::Set(actor::Target(), EAIFocusPriority::Attacking)),
		onStart(randomiseBombSpewAmmo()),
		chain( delay(bt::Seconds(rnd.nextFloat(optBombSpewRotateMinDelay, optBombSpewRotateMaxDelay))),
			sequence( delay(optBombSpewExecuteDelay), focus::Clear(EAIFocusPriority::Attacking) ),
			chain(
				sequence( delay(optChargeAtkStartDuration), setAnimState(EPerfectFormAnimLocomotion::Bombspew) ),
				make_unique<URangedAttack>(*rAtkCompTagBombSpew),
				sequence( delay(optTickDelay), setAnimState(EPerfectFormAnimLocomotion::Idle) )
			)
		)
	); };

	const auto btPformEndermiteScatter = [=, &options](EndermiteAttackType inType = EndermiteAttackType::Scatter) { return sequence("endermite-scatter",
		onStart(randomiseEndermiteAmmo(inType)),
		onStart(setAnimState(EPerfectFormAnimLocomotion::Scatter)),
		onStop(setAnimState(EPerfectFormAnimLocomotion::Idle)),
		onStop(focus::Clear()),
		ifElse(isInRange(location::StartPos(), 400.f), 
			chain(
				delay(optEndermiteScatterStartDuration),
				make_unique<URangedAttack>(*rAtkCompTagEndermite)
			),
			chain(
				sequence(delay(optTickDelay), focus::Set(location::StartPos())),
				delay(optEndermiteScatterStartDuration),
				make_unique<URangedAttack>(*rAtkCompTagEndermite)
			)
		)
	); };

	const auto btPformEndermiteFeast = [=, &options]() { return chain("endermite-feast",
		btPformEndermiteScatter(EndermiteAttackType::Feast),
		delay(optEndermiteFeastConsumeDelay),
		sequence( delay(optEndermiteFeastStartDuration), onStart(setAnimState(EPerfectFormAnimLocomotion::Feast)) ),
		sequence( delay(optEndermiteFeastConsumeDuration), 
			onStart(setMinionsToFeast(true)),
			onStop(setMinionsToFeast(false)),
			onStop(setAnimState(EPerfectFormAnimLocomotion::Idle))
		)
	); };

	const auto btPformLaserPhaser = [=, &options]() { return uninterruptibleSequence("laser-phaser",
		onStart(setLaserPhase(LaserPhase::Begin)),
		onStart(randomiseLaserPairTimings()),
		onStart(updateLaserBodyRotationRate()),
		ifElse( startPredicate(thresholdPredicate()),
			sequence(
				onStop(setLaserPhase(LaserPhase::NONE)),
				sequence( delay(optLaserEndDuration), alwaysTrue(setAnimState(EPerfectFormAnimLocomotion::Submerge)), alwaysTrue(setMirrorsActive(false)) )
			),
			selector(
				sequence("enter-void-pool", startPredicate(isLaserPhase(LaserPhase::Begin)),
					onStop(incLaserPhase()),
					enterVoidPool(optLaserBeginDelay)
				),
				sequence("exit-void-pool", startPredicate(isLaserPhase(LaserPhase::Ending)),
					onStop(incLaserPhase()),
					exitVoidPool(optLaserEndDelay)
				),
				parallel( delay(optTickDelay),
					sequence("laser-active", startPredicate(isLaserPhase(LaserPhase::Active)),
						chain(
							parallel( delay(optTickDelay), // This delay is required to prevent the chain bug on an initial node returning false and causing the whole behaviour to fail
								sequence( startPredicate(lessThanOrEquals(actorStats::healthFraction(), value(optLaserMirrorHealthThreshold)) ),
									sequence( delay(optTickDelay), setMirrorsActive(true) )
								)
							),
							sequence( delay(optLaserStartDuration), alwaysTrue(setAnimState(EPerfectFormAnimLocomotion::Laser)) ),
							sequence( maxRunTime(optLaserTotalDuration),
								alwaysTrue(make_unique<URangedAttack>(*rAtkCompTagOffset_S1F)),
								alwaysTrue(make_unique<URangedAttack>(*rAtkCompTagOffset_S1B)),
								alwaysTrue(make_unique<URangedAttack>(*rAtkCompTagOffset_S1L)),
								alwaysTrue(make_unique<URangedAttack>(*rAtkCompTagOffset_S1R)),
								alwaysTrue(make_unique<URangedAttack>(*rAtkCompTagOffset_S2F)),
								alwaysTrue(make_unique<URangedAttack>(*rAtkCompTagOffset_S2B)),
								alwaysTrue(make_unique<URangedAttack>(*rAtkCompTagOffset_S2L)),
								alwaysTrue(make_unique<URangedAttack>(*rAtkCompTagOffset_S2R))
							),
							sequence( delay(optLaserEndDuration), alwaysTrue(setAnimState(EPerfectFormAnimLocomotion::Submerge)), alwaysTrue(setMirrorsActive(false)) ),
							sequence( delay(optTickDelay), incLaserPhase() )					
						)
					)
				)
			)
		)
	); };


	// Transition Logic
	const auto btOpeningTransition = [=, &options]() { return uninterruptibleSequence("opening-transition",
		startPredicate(isCombatPhase(CombatPhase::PhaseInit)),
		onStart(setAnimState(EPerfectFormAnimLocomotion::Idle)),
		onStop(removeInvulnerability()),
		onStop(incCombatPhase()),
		delay(optIntroAnimDuration)
	); };

	const auto btPhaseTransition = [=, &options]() { return uninterruptibleSequence("phase-transition",
		startPredicate( thresholdPredicate() ),
 		onStart(randomiseEndermiteAmmo(EndermiteAttackType::VoidPull)),
 		onStart(setAnimState(EPerfectFormAnimLocomotion::Idle)),
		onStart(focus::Clear(EAIFocusPriority::Attacking)),
		onStart(setTransitionPhase(bIsSubmerged ? TransitionPhase::Active : TransitionPhase::Begin)),
		onStop(incCombatPhase()),
		onStop(set(attackCounter, value(0))),
		selector(
			sequence("enter-void-pool", startPredicate(isTransitionPhase(TransitionPhase::Begin)),
				onStop(incTransitionPhase()),
				enterVoidPool(optVoidPullBeginDelay)
			),
			sequence("exit-void-pool", startPredicate(isTransitionPhase(TransitionPhase::Ending)),
				onStop(incTransitionPhase()),
				exitVoidPool(optVoidPullEndDelay)
			),
	 		parallel( delay(optTickDelay),
	 			sequence( predicate(isTransitionPhase(TransitionPhase::Active)),
	 				chain(
	 					sequence( "void-pull-start", delay(optVoidpullStartDuration),
	 						setAnimState(EPerfectFormAnimLocomotion::Voidpull),
	 						onStop(setActiveGravPulse(true))
	 					),
						parallel(
							alwaysTrue(make_unique<URangedAttack>(*rAtkCompTagEndermitePool)),
							chain(
								delay(optVoidPullAoeBlastInitialDelay),
								ifElse( predicate(equals(attackCounter, value(optVoidPullAoeBlastAttacksCount))),
									sequence("void-pull-end",
										onStart(setActiveGravPulse(false)),
										chain(
											sequence( delay(optVoidpullEndDuration), setAnimState(EPerfectFormAnimLocomotion::Submerge) ),
											sequence( delay(optTickDelay), incTransitionPhase() )
										)
									),
									alwaysTrue(sequence( delay(optVoidPullAoeBlastInterval), alwaysTrue(make_unique<UAoeAttack>(*aoeAtkCompTagVoidpull)), onStop(inc(attackCounter)) ))
								)
							)
						)
	 				)
	 			)
	 		)
		)
	); };

	// Select Attack Mode
	const auto btAttackModeSelect = [=]() { return chain("attack-mode-select",
		sequence(delay(optTickDelay), setRandomWeightedAttackType()),
		selector(
			sequence("Charge", startPredicate(isAttackType(AttackType::Charge)), btPformChargeAttack()),
			sequence("BombSpew", startPredicate(isAttackType(AttackType::BombSpew)), btPformBombSpew()),
			sequence("EndermiteScatter", startPredicate(isAttackType(AttackType::EndermiteScatter)), btPformEndermiteScatter()),
			sequence("EndermiteFeast", startPredicate(isAttackType(AttackType::EndermiteFeast)), btPformEndermiteFeast()),
			sequence("LaserPhaser", startPredicate(isAttackType(AttackType::LaserPhaser)), btPformLaserPhaser())
		)
	); };
	
	// Initialisation
 	if (abilitySystem) {
 		auto spec = effects::CreateGameplayEffectSpec<UPerfectFormInvulnerabilityGameplayEffect>(abilitySystem);
 		spec.SetSetByCallerMagnitude(*UHealthAttributeSet::TakeDamageMultiplierAttribute().GetName(), 0.0f);
 		abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
 	}

	// Behavior
	return BehaviorTuple(
		root(selector("Perfect-Form",
			btOpeningTransition(),
			btPhaseTransition(),
			btAttackModeSelect()
 		))
	);
}

}}}
