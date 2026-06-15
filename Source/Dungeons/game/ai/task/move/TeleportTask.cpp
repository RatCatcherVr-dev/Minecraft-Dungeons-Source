#include "Dungeons.h"
#include "TeleportTask.h"
#include "game/ai/bt/BtEvalState.h"
#include "game/component/MobAnimationsComponent.h"
#include "game/util/Tags.h"
#include <Components/ChildActorComponent.h>
#include <Particles/ParticleSystemComponent.h>
#include <Components/CapsuleComponent.h>
#include "game/actor/character/mob/MobCharacter.h"

UTeleportTask::UTeleportTask(
	bt::Duration teleportOutDuration,
	bt::Duration teleportInDuration,
	const bt::Provider<bt::Duration>& disappearDurationProvider,
	const bt::locator::Provider& target,
	const bt::locator::Provider& lookAtTarget
)
	: teleportOutDuration { teleportOutDuration }
	, teleportInDuration { teleportInDuration }
	, disappearDurationProvider { disappearDurationProvider }
	, target { target }
	, lookAtTarget { lookAtTarget }
	, disappearDuration { 0.s }
{
	name = "teleport-task";
}

TOptional<FVector> UTeleportTask::FindTeleportInLocation(bt::StateRef state) const
{
	const auto maybeAround = [&]() -> TOptional<FVector> {
		for (float radius = 10.f; radius < 300.f; radius += 40.f) {
			if (const auto candidate = bt::locator::RandomReachablePointAround(target, radius)(state).GetLocationLike()) {
				return candidate;
			}
		}

		return {};
	}();

	if (maybeAround) {
		const auto capsuleHalfHeight = state.owner->FindComponentByClass<UCapsuleComponent>()->GetScaledCapsuleHalfHeight();
		return maybeAround.GetValue() + FVector { 0.f, 0.f, capsuleHalfHeight };
	}

	return {};
}

bool UTeleportTask::OnCanRun(bt::StateRef state) {	
	if (auto maybeTeleportLocation = FindTeleportInLocation(state)) {
		teleportInLocation = maybeTeleportLocation.GetValue();
		return true;
	}

	return false;
}

bool UTeleportTask::OnCanContinue(bt::StateRef state) {
	return !teleportInTime.IsPassed(state) ||
		!teleportDone ||
		!completeTime.IsPassed(state);
}

void UTeleportTask::OnStart(bt::StateRef state) {
	if (auto sequence = state.animPack()->Common.TeleportOut) {
		state.owner->MulticastPlayAnimationAsDynamicMontage(sequence, state.animPack()->Common.TeleportSlot);
	}

	state.owner->RemoveInvisibility();

	disableTargetTime = bt::TimeStamp::FromNow(state, teleportOutDuration * .8f);
	disableTargetDone = false;

	disappearTime = bt::TimeStamp::FromNow(state, teleportOutDuration);
	disappearDone = false;
	
	disappearDuration = disappearDurationProvider(state);
	teleportInTime = bt::TimeStamp::FromNow(state, teleportOutDuration + disappearDuration);
	teleportDone = false;
}

void UTeleportTask::OnTick(bt::StateRef state) {
	if (disableTargetTime.IsPassed(state) && !disableTargetDone) {
		disableTargetDone = true;
		state.owner->SetTargetable(false);
	}

	if (disappearTime.IsPassed(state) && !disappearDone) {
		disappearDone = true;		
		state.owner->SetWorldState(ECharacterWorldState::Disappeared);
	}

	if (teleportInTime.IsPassed(state) && !teleportDone && disableTargetDone && disappearDone) {
		teleportDone = true;
		state.params().teleportTime = bt::TimeStamp::Now(state);
		completeTime = bt::TimeStamp::FromNow(state, teleportInDuration);

		//If we disappeared for a long time - consider refreshing the target teleport location if possible.
		const auto finalTeleportInLocation = [&state, this]() {
			if (disappearDuration > 0.s) {
				if (auto maybeTeleportLocation = FindTeleportInLocation(state)) {
					return maybeTeleportLocation.GetValue();
				}
			}

			return teleportInLocation;
		}();

		if (const auto maybeLookAtTarget = lookAtTarget(state).GetLocationLike()) {
			const auto delta = maybeLookAtTarget.GetValue() - finalTeleportInLocation;
			const auto rotation = FRotationMatrix::MakeFromX(FVector { delta.X, delta.Y, 0.f }).Rotator();

			state.owner->SetActorLocationAndRotation(finalTeleportInLocation, rotation, false, nullptr, ETeleportType::ResetPhysics);
		} else {
			state.owner->SetActorLocation(finalTeleportInLocation, false, nullptr, ETeleportType::ResetPhysics);
		}
				
		state.owner->SetWorldState(ECharacterWorldState::InWorld);		

		state.owner->SetTargetable(true);

		if (auto sequence = state.animPack()->Common.TeleportIn) {
			state.owner->MulticastPlayAnimationAsDynamicMontage(sequence, state.animPack()->Common.TeleportSlot);
		}
	}
}