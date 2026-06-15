#include "Dungeons.h"
#include "MountaineerBehaviour.h"
#include "game/ai/action/CommonActions.h"
#include "game/ai/bt/BtLogic.h"
#include "game/ai/bt/BtDelay.h"
#include "game/ai/bt/decorator/BtDropFor.h"
#include "game/ai/bt/decorator/BtEvery.h"
#include "game/ai/behavior/NodeFactory.h"
#include "game/ai/behavior/MeleeAttackBehavior.h"
#include "game/ai/behavior/MoveBehavior.h"
#include "game/ai/behavior/RoamBehavior.h"
#include "game/ai/provider/Actors.h"
#include "game/ai/provider/Locators.h"
#include "game/ai/condition/IsInRange.h"
#include "game/ai/condition/BtPredicate.h"
#include "game/ai/condition/BtProbability.h"
#include "game/ai/condition/LocatorPredicates.h"
#include "game/ai/condition/AttackPredicates.h"
#include "game/ai/action/FocusActions.h"
#include "game/ai/task/PlayAnimation.h"
#include "game/ai/condition/ActorPredicates.h"
#include "game/ai/debug/BtDebug.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/actor/MeleeTicketProvider.h"
#include "game/component/BehaviorOptionsComponent.h"
#include "game/component/MeleeAttackComponent.h"
#include "game/util/LocationQuery.h"
#include "util/Random.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/attributes/ResistanceAttributeSet.h"

class UBehaviorOptionsComponent;

UMountaineerWindResistanceGameplayEffect::UMountaineerWindResistanceGameplayEffect(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("Resistance.PushVolume.MountaineerWind"));
}

UMountaineerWindImmunityGameplayEffect::UMountaineerWindImmunityGameplayEffect(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("Immunity.PushVolume.MountaineerWind"));
}

namespace bt { namespace behavior { namespace entities {

	BehaviorTuple createMountaineer(AMobCharacter& mob, const UBehaviorOptionsComponent& options) {
		static Random rnd;

		const auto speedMultiplier = rnd.nextFloat(options.Get("WalkSpeedFactorMin", 0.4f), options.Get("WalkSpeedFactorMax", 0.6f));

		const auto bShouldBrace = makeSharedRef<bool>(true);
		

		auto GetWindLookDirection = [=]() -> bt::location::Provider {
			return[=](StateRef state) {
				auto pushComponent = state.owner->GetOrCreatePushVolumeReactiveComponent();
				return location::Self()(state) + (-pushComponent->GetResistanceIgnoredCumulativePushDirection());
			};
		};

		auto braceDuration = options.Get("BraceDuration", 1.25f);
		auto canBraceDelay = options.Get("CanBraceDelay", 1.25f);
		auto resistanceStrength = options.Get("resistanceStrength", 0.5f);
		auto abilitySystem = mob.GetAbilitySystemComponent();

		FGameplayTag windResistanceTag = FGameplayTag::RequestGameplayTag("Resistance.PushVolume.MountaineerWind");
		FGameplayTag windImmunityTag = FGameplayTag::RequestGameplayTag("Immunity.PushVolume.MountaineerWind");

		const FString WindBrace = "WindBrace";

		const auto AttackEnterDistance = options.Get("AttackEnterDistance", 350.f);
		const auto AttackExitDistance = options.Get("AttackExitDistance", 500.f);
		const auto AttackChargeRange = options.Get("AttackChargeRange", 300.f);
		const auto AttackChargeMultiplier = options.Get("AttackChargeMultiplier", .9f);
		const auto ChaseMultiplier = options.Get("ChaseMultiplier", .6f);

		auto HasWindImmunity = [=]() -> bt::Pred {
			return[=](StateRef state) {
				return abilitySystem->HasMatchingGameplayTag(windImmunityTag);
			};
		};

		auto HasWindResistance = [=]() -> bt::Pred {
			return[=](StateRef state) {
				return abilitySystem->HasMatchingGameplayTag(windResistanceTag);
			};
		};

		auto ApplyWindImmunity = [=]() -> bt::Action {
			return[=](StateRef state) {
				if (!HasWindImmunity())
				{
					FGameplayEffectSpec effect = effects::CreateGameplayEffectSpec<UMountaineerWindImmunityGameplayEffect>(abilitySystem);
					abilitySystem->ApplyGameplayEffectSpecToSelf(effect);
				}
			};
		};
		auto RemoveWindImmunity = [=]() -> bt::Action {
			return[=](StateRef state) {
				if(HasWindImmunity())
					abilitySystem->RemoveActiveEffectsWithAppliedTags(windImmunityTag.GetSingleTagContainer());
			};
		};

		auto ApplyWindResistance = [=](float resistance) -> bt::Action {
			return[=](StateRef state) {
				if (!HasWindResistance())
				{
					FGameplayEffectSpec effect = effects::CreateGameplayEffectSpec<UMountaineerWindResistanceGameplayEffect>(abilitySystem);
					effect.SetSetByCallerMagnitude(*UResistanceAttributeSet::WindResistanceMagnitudeAttribute().GetName(), resistance);
					abilitySystem->ApplyGameplayEffectSpecToSelf(effect);
				}
			};
		};

		auto RemoveWindResistance = [=]() -> bt::Action {
			return[=](StateRef state) {
				if(HasWindResistance())
					abilitySystem->RemoveActiveEffectsWithAppliedTags(windResistanceTag.GetSingleTagContainer());
			};
		};

		return BehaviorTuple(
			selector("Mountaineer",
				sequence(
					ifElse((actor::IsInWind(actor::Self()) || HasWindImmunity()) && equals(bShouldBrace, value(true)),
						parallel("InWind",							
							dropFor(0.3s,
								parallel(
									onStart(focus::Set(GetWindLookDirection(), EAIFocusPriority::Move)),
									onStop(focus::Clear(EAIFocusPriority::Move)),
									RemoveWindResistance(),
									ApplyWindImmunity(),
									playAnimation(options.Get(WindBrace), true)
								)
							),
							dropFor(braceDuration,
								parallel(
									RemoveWindImmunity(),
									ApplyWindResistance(1.0f / resistanceStrength),
									set(bShouldBrace, value(false))
								)
							)
						),
						parallel(
							sequence(
								dropFor(canBraceDelay,
									set(bShouldBrace, value(true))
								)
							),
							selector("NotInWind",
								sequence("attack",
									isInRange(
										actor::Target(),
										AttackEnterDistance,
										AttackExitDistance
									),
									ifElse(
										isInRange(
											actor::Target(),
											AttackChargeRange
										),
										behavior::meleeAttack(mob),
										behavior::moveTo(actor::Target(), Relative(AttackChargeMultiplier))
									)
								),
								behavior::chaseInOffensiveRange(mob, actor::Target(), Relative(ChaseMultiplier)),
								behavior::chaseIfAttacked(mob, Relative(speedMultiplier * 1.2f)),
								behavior::chaseIfWarned(mob, Relative(speedMultiplier * 1.2f)),
								behavior::defaultRoam()
							)
						)
					)
				)
			),
			parallel("update-targets",
				every(.8s * rnd.nextFloat(.7f, 1.3f),
					set(actor::Target(), actor::ClosestEnemy())
				),
				every(3s, sequence(
					isInRange(actor::Target(), mob.OffensiveRange),
					set(location::Anchor(), location::Self())
				))
			)
	);
}

}}}
