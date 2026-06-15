#include "Dungeons.h"
#include "MirrorTask.h"
#include "game/GameBP.h"
#include "game/ai/bt/BtEvalState.h"
#include "game/component/MobAnimationsComponent.h"
#include "game/mobspawn/MobSpawner.h"
#include "game/mobspawn/MobSpawnConfigs.h"
#include "game/mobspawn/MobSpawnProviders.h"
#include "game/util/Tags.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/util/LocationQuery.h"

UMirrorTask::UMirrorTask(
	bt::Duration teleportOutDuration,
	bt::Duration teleportInDuration,
	int count,
	float circleRadiusMax,
	float anchorDistanceMax,
	float mirrorDistanceMax,
	const bt::actor::Provider& target,
	const bt::location::Provider& anchor
)
	: teleportOutDuration { teleportOutDuration }
	, teleportInDuration { teleportInDuration }
	, count { count }
	, circleRadiusMax { circleRadiusMax }
	, anchorDistanceMax { anchorDistanceMax }
	, mirrorDistanceMaxSquare { FMath::Square(mirrorDistanceMax) }
	, target { target }
	, anchor { anchor }
	, spawnConfig(game::mobspawn::configs::DefaultNoVariants(false).AddTag(tags::noDrop))
{
	name = "mirror-task";
}

bool UMirrorTask::OnCanRun(bt::StateRef state) {	
	if (const auto maybeTarget = target(state)) {
		const auto targetLocation = maybeTarget->GetActorLocation();

		const auto teleportLocationFilter = [
			anchorLocation = anchor(state),
			circleRadiusMinSquared = FMath::Square(circleRadiusMax - 400),
			anchorDistanceMaxSquared = FMath::Square(anchorDistanceMax)
		](const FVector& target, const FVector& candidate) {
			return FVector::DistSquared2D(target, candidate) > circleRadiusMinSquared &&
				FVector::DistSquared2D(anchorLocation, candidate) < anchorDistanceMaxSquared;
		};

		if (const auto maybeAround = bt::locator::RandomReachablePointAround(target, circleRadiusMax, teleportLocationFilter)(state).GetLocationLike()) {
			const auto capsuleHalfHeight = state.owner->FindComponentByClass<UCapsuleComponent>()->GetScaledCapsuleHalfHeight();
			teleportInLocation = maybeAround.GetValue() + FVector { 0.f, 0.f, capsuleHalfHeight };

			return true;
		}
	}
	
	return false;
}

bool UMirrorTask::OnCanContinue(bt::StateRef state) {
	return !teleportInTime.IsPassed(state) ||
		!teleportDone ||
		!completeTime.IsPassed(state);
}

void UMirrorTask::OnStart(bt::StateRef state) {
	if (auto anim = state.anim()) {
		if (auto sequence = state.animPack()->Common.TeleportOut) {
			state.owner->MulticastPlayAnimationAsDynamicMontage(sequence, FName(TEXT("FullBody")), 0.f, 0.2f, 1.f, 1, 0.f, 0.f, FPredictionKey());
		}
	}

	disableTargetTime = bt::TimeStamp::FromNow(state, teleportOutDuration * .8f);
	disableTargetDone = false;
	
	teleportInTime = bt::TimeStamp::FromNow(state, teleportOutDuration);
	teleportDone = false;	

	state.params().lastAttackTime = bt::TimeStamp::Now(state);
}

void UMirrorTask::OnTick(bt::StateRef state) {
	if (disableTargetTime.IsPassed(state) && !disableTargetDone) {
		disableTargetDone = true;
		state.owner->SetTargetable(false);
	}

	if (teleportInTime.IsPassed(state) && !teleportDone && disableTargetDone) {
		teleportDone = true;
		completeTime = bt::TimeStamp::FromNow(state, teleportInDuration);

		for (auto particleSystem : state.owner->GetComponentsByClass(UParticleSystemComponent::StaticClass())) {
			particleSystem->Deactivate();
		}

		const auto maybeTarget = target(state);
		if (maybeTarget) {
			const auto yaw = FRotationMatrix::MakeFromX(maybeTarget->GetActorLocation() - teleportInLocation).Rotator().Yaw;
			state.owner->SetActorLocationAndRotation(teleportInLocation, FRotator { 0.f, yaw, 0.f }, false);
		} else {
			state.owner->SetActorLocation(teleportInLocation, false);
		}

		for (auto particleSystem : state.owner->GetComponentsByClass(UParticleSystemComponent::StaticClass())) {
			particleSystem->Activate();
		}

		state.owner->SetTargetable(true);

		if (auto sequence = state.animPack()->Common.TeleportIn) {
			state.owner->MulticastPlayAnimationAsDynamicMontage(sequence, FName(TEXT("FullBody")), 0.f, 0.2f, 1.f, 1, 0.f, 0.f, FPredictionKey());
		}			

		if (maybeTarget) {

			AGameBP* game = actorquery::getFirstActor<AGameBP>(state.owner->GetWorld());

			const auto anchorLocation = anchor(state);

			const auto targetLocation = maybeTarget->GetActorLocation();
			const auto delta = teleportInLocation - targetLocation;

			bool bOneSpawned = false;
			const auto angleIncrement { 360.f / count };
			for (auto i = 1; i < count; i++) {
				const auto location = targetLocation + delta.RotateAngleAxis(angleIncrement * i, FVector::UpVector);

				if (
					FVector::DistSquared2D(anchorLocation, location) < mirrorDistanceMaxSquare &&
					state.owner->IsLocationReachable(location)
				) {
					if (bOneSpawned)
					{
						//push the rest to distribute
						EntityType entityType = EntityType::FalseKing;
						FTransform MobTransform;
						
						if (!game::mobspawn::getSpawnData(state.world(), game::mobspawn::providers::Location(location), spawnConfig, entityType, MobTransform))
						{
							continue;
						}
												
						//request a spawn
						game->RequestMobSpawn(entityType, MobTransform, {}, spawnConfig);

					}
					else
					{
						bOneSpawned = (game::mobspawn::spawnNow(state.world(), EntityType::FalseKing, game::mobspawn::providers::Location(location), spawnConfig) != nullptr);
					}
				}
			}
		}
	}
}