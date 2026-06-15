#include "Dungeons.h"
#include "TransformActions.h"
#include "game/actor/character/mob/MobCharacter.h"

namespace bt { namespace transform {

void lookAt(AActor& actor, FVector dst, bool yaw, bool pitch, bool roll) {
	auto targetRot = FRotationMatrix::MakeFromX(dst - actor.GetActorLocation()).Rotator();
	FRotator rot = actor.GetActorRotation();

	if (yaw) {
		rot.Yaw = targetRot.Yaw;
	}
	if (pitch) {
		rot.Pitch = targetRot.Pitch;
	}
	if (roll) {
		rot.Roll = targetRot.Roll;
	}
	actor.SetActorRotation(rot);
}

void lookAtYaw(AActor& actor, FVector dst) {
	lookAt(actor, dst, true, false, false);
}

Action LookAtYaw(const locator::Provider& locator) {
	return [locator](StateRef state) {
		if (auto dst = locator(state).GetLocationLike()) {
			lookAtYaw(*state.owner, dst.GetValue());
		}
	};
}

}}
