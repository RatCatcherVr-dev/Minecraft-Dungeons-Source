#include "Dungeons.h"
#include "EndermiteBehavior.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/ai/action/CommonActions.h"
#include "game/ai/action/HealthActions.h"
#include "game/ai/behavior/MeleeAttackBehavior.h"
#include "game/ai/behavior/MoveBehavior.h"
#include "game/ai/behavior/NodeFactory.h"
#include "game/ai/behavior/RoamBehavior.h"
#include "game/ai/bt/BtLogic.h"
#include "game/ai/bt/decorator/BtEvery.h"
#include "game/ai/condition/ActorPredicates.h"
#include "game/ai/condition/AttackPredicates.h"
#include "game/ai/condition/BtPredicate.h"
#include "game/ai/condition/IsInRange.h"
#include "game/ai/provider/Actors.h"
#include "game/component/PerfectFormMinionComponent.h"

namespace bt { namespace behavior { namespace entities {

bool isPerfectForm(AActor * candidate) {
	if (auto mob = Cast<AMobCharacter>(candidate)) {
		return mob->EntityType == EntityType::PerfectFormHeart && mob->IsAlive();
	}
	return false;
}

BehaviorTuple createEndermite(AMobCharacter& mob, const UBehaviorOptionsComponent& options, bool smart, bool minion)
{
	static Random rnd;

	// Components
	const auto minionComp = mob.FindComponentByClass<UPerfectFormMinionComponent>(); // WARN - Can be nullptr, this is intended as only a minion variant should contain this component.
	const auto meleeAttackComp = mob.FindComponentByClass<UMeleeAttackComponent>();

	// Options
	const auto attackInDistance = options.Get("AttackInDistance", 250.f);
	const auto attackOutDistance = options.Get("AttackOutDistance", 400.f);
	const auto speedMultiplier = options.Get("speed-multiplier", 1.f);
	const auto enchantedChance = options.Get("enchantment-chance", 0.f);
	const auto minion_sacrifice_heal_amount = options.Get("minion-sacrifice-heal-amount", 0.01f);

	const auto shouldEnchantMob = [&enchantedChance]() -> bool {
		return rnd.nextFloat() < enchantedChance;
	};

	if (minion && minionComp && shouldEnchantMob()) {
		TArray<FEnchantmentData> possibleEnchantments = minionComp->GetPossibleEnchantments();
		if (possibleEnchantments.Num() != 0) {
			TArray<FEnchantmentData> enchantmentsToApply;
			if (minionComp->ShouldRandomiseEnchants()) {
				int enchantmentCount = FMath::RandRange(1, FMath::Min(3, possibleEnchantments.Num()));
				for (size_t i = 0; i < enchantmentCount; i++) {
					FEnchantmentData enchant = possibleEnchantments[FMath::RandRange(0, possibleEnchantments.Num() - 1)];
					enchantmentsToApply.Add(enchant);
					possibleEnchantments.Remove(enchant);
				}
			}
			else {
				for (size_t i = 0; i < FMath::Min(3, possibleEnchantments.Num()); i++) {
					enchantmentsToApply.Add(possibleEnchantments[i]);
				}
			}
			mob.GetEnchantmentComponent()->AddEnchantments(enchantmentsToApply);
		}
	}

	if (minion && minionComp) {
		float dsq = 0.0f;
		if (auto closestMob = Cast<AMobCharacter>(actorquery::getClosestMob(&mob, 2000.0f, dsq, isPerfectForm))) {
			mob.ChangeMaster(closestMob);
			minionComp->BindPerfectFormDelegate(closestMob);
		}
	}

	// Lambda
	auto canFeast = [&minionComp]() -> Pred { return [=](StateRef state) {
		if (minionComp && actor::IsAlive(actor::Master())) {
			return minionComp->GetFeastState();
		};
		return false;
	}; };

	auto selfSacrifice = [=, &options]() { return sequence(
		startPredicate( canFeast() ),
		health::Heal(actor::Master(), minion_sacrifice_heal_amount),
		common::Exec([=]() { 
			if (minionComp) {
				minionComp->UnbindPerfectFormDelegate(); 
			}
		}),
		common::Apply(actor::Self(), [](AActor* mob) { mob->Destroy(); })
	); };

	// Behavior
	return BehaviorTuple(
		selector("Endermite",
			sequence("Feast",
				startPredicate( value(minion) && canFeast() ),
				chain(
					behavior::moveTo(move::withSettings(actor::Master(), move::defaultRequest().SetUsePathfinding(smart)), Relative(speedMultiplier)),
					selfSacrifice()
				)
			),
			sequence("Attack", 
				isInRange(actor::Target(), attackInDistance, attackOutDistance),
				ifElse(attack::InAttackRangeOrAttacking(actor::Target(), meleeAttackComp),
					behavior::meleeAttack(mob),
					sequence(
						predicate(!canFeast()),
						behavior::moveTo(move::withSettings(actor::Target(), move::defaultRequest().SetUsePathfinding(smart)), Relative(speedMultiplier))
					)
				)
			),
			behavior::chaseInOffensiveRange(mob, move::withSettings(actor::Target(), move::defaultRequest().SetUsePathfinding(smart)), Relative(speedMultiplier)),
			behavior::chaseIfAttacked(mob, Relative(speedMultiplier * 1.2f)),
			behavior::chaseIfWarned(mob, Relative(speedMultiplier * 1.2f)), 
			behavior::defaultRoam()
		),
		parallel("update-targets",
			sequence( predicate( !canFeast() ),
				every(.5s, set(actor::Target(), actor::ClosestEnemy()))
			),
			every(3s, sequence(
				isInRange(actor::Target(), mob.OffensiveRange),
				set(location::Anchor(), location::Self())
			))
		)
	);
}

}}}
