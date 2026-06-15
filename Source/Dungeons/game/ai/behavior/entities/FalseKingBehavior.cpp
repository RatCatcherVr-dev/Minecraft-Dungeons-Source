#include "Dungeons.h"
#include "FalseKingBehavior.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/ai/bt/BtLogic.h"
#include "game/ai/bt/decorator/BtEvery.h"
#include "game/ai/behavior/NodeFactory.h"
#include "game/ai/provider/Actors.h"
#include "game/ai/task/attack/SummonTask.h"
#include "game/ai/behavior/RangedAttackBehavior.h"
#include "game/ai/task/attack/RangedAttack.h"
#include "game/ai/bt/decorator/BtDropFor.h"
#include "game/ai/action/CommonActions.h"
#include "game/component/BehaviorOptionsComponent.h"
#include "game/actor/EffectsActor.h"
#include "game/component/BaseParticleAssetsComponent.h"
#include "Assets/DungeonsAssetManager.h"
#include "DungeonsGameInstance.h"
#include "Engine/World.h"

namespace bt {namespace behavior { namespace entities {

BehaviorTuple createFalseKing(class AMobCharacter& mob, const UBehaviorOptionsComponent& options) 
{
	return BehaviorTuple(
	parallel("FalseKing",
		sequence("attack",
			onStart(focus::Set(actor::Target())),
			rangedAttackInOrientation(mob, actor::Target()),
			onStop(focus::Clear())
		),
		dropFor(
			options.Get("SmokeDuration", 1.s),
			onStart(common::Apply([](AActor* mob) {
				for (auto particleSystem : mob->GetComponentsByClass(UParticleSystemComponent::StaticClass())) {
					particleSystem->Deactivate();
				}
			}))
		),
		dropFor(
			options.Get("TeleportOutEffectsDelay", 2.5s),			
			onStart(common::Exec([](StateRef state) 
			{
				TSoftObjectPtr<UParticleSystem> particleSystemObject = state.owner->FindComponentByClass<UBaseParticleAssetsComponent>()->TeleportOut;
				StaticLoadObject(UParticleSystem::StaticClass(), NULL, *particleSystemObject.ToSoftObjectPath().GetAssetPathString());
				if (const auto effectsActor = AEffectsActor::GetInstance(&state.world())) {
					const auto particles = particleSystemObject.Get();
					effectsActor->SpawnEffectsAtLocation(particles, nullptr, state.owner->GetActorLocation());
				}
			}))
		),
		dropFor(
			options.Get("LifetimeDuration", 2.8s),
			common::Apply([](AActor* mob) { mob->Destroy(); })
		)
	),
	every(10.s,
		set(actor::Target(), actor::ClosestPlayer())
	)
); }

}}}
