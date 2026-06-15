#include "Dungeons.h"
#include "Pushback.h"
#include "game/component/MassComponent.h"
#include "DrawDebugHelpers.h"
#include "IConsoleManager.h"
#include "util/FloatRange.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/component/RagdollOnDeathComponent.h"
#include "../abilities/attributes/ResistanceAttributeSet.h"
#include "../actor/character/BaseCharacter.h"

//Console variables are public black magic things...
static TAutoConsoleVariable<int32> CVarDebugDrawPushback(
	TEXT("Dungeons.DebugDraw.Pushback"),
	0,
	TEXT("Enables debug drawing of pushback.\n")
	TEXT("<=0: off.\n")
	TEXT(">0: on.\n"),
	ECVF_Cheat);

namespace pushback {

//Science proves that this number gives us pushback where the strength is equal to the number of blocks moved (for a zombie, at z factor 1)
const float REFERENCE_PUSHBACK_STRENGTH = 690.0f;

bool _canPushback(const FPushback& push, const UMassComponent& massComponent, float externalPushbackMultiplier){
	if (externalPushbackMultiplier == 0.f) {
		return false;
	}

	if (!push.enablePushback) {
		return false;
	}

	if (massComponent.IsImmovable()) {
		return false;
	}

	return true;
}

float _calculatePushbackMultiplier(const FPushback& push, const UMassComponent& massComponent, float externalPushbackMultiplier, const AActor& affecting, bool bApplyResistance)
{
	// Get affected actor pushback resistance
	float AffectionPushbackResistance = 1.f;
	if (bApplyResistance)
	{
		if (const auto abilitySystem = Cast<ABaseCharacter>(&affecting)->GetAbilitySystemComponent()) {
			AffectionPushbackResistance = abilitySystem->GetNumericAttribute(UResistanceAttributeSet::PushbackResistanceMagnitudeAttribute());
		}
	}
	return FMath::Lerp(1.0f, massComponent.GetPushMultiplier(), push.pushbackMultiplierRespectFactor) * externalPushbackMultiplier * AffectionPushbackResistance;
}

void pushback(const FPushback& push, const AActor& by, const AActor& affecting, float externalPushbackMultiplier/* = 1.f*/, bool bApplyResistance/* = true*/, bool ignoreLevelClamps/* = false*/) {
 	if (const auto massComponent = affecting.FindComponentByClass<UMassComponent>()) {
		if (!_canPushback(push, *massComponent, externalPushbackMultiplier)) {
			return;
		}

		auto pushbackMultiplier = _calculatePushbackMultiplier(push, *massComponent, externalPushbackMultiplier, affecting, bApplyResistance);
		auto launchVector = getLaunchVector(push, by, affecting, pushbackMultiplier);

		// #D11.CM - Clamp underwater pushback Z Height
		if (!ignoreLevelClamps) {
			if (const auto abilitySystem = Cast<ABaseCharacter>(&affecting)->GetAbilitySystemComponent()) {
				float ZClamp = abilitySystem->GetNumericAttribute(UResistanceAttributeSet::PushbackZClampMagnitudeAttribute());
				if (ZClamp >= 0) {
					launchVector = FVector{ launchVector.X, launchVector.Y, FMath::Min(launchVector.Z, ZClamp) };
				}
			}
		}

		if (auto characterMovementComponent = affecting.FindComponentByClass<UCharacterMovementComponent>()) {
			characterMovementComponent->StopActiveMovement();
			characterMovementComponent->Launch(launchVector);
			characterMovementComponent->HandlePendingLaunch();
		}
	}	
}

void pushback(const FPushback& push, const FVector& direction, const AActor& affecting, float externalPushbackMultiplier, bool bApplyResistance/* = true*/, bool ignoreLevelClamps/* = false*/){
	if (const auto massComponent = affecting.FindComponentByClass<UMassComponent>()) {
		if (!_canPushback(push, *massComponent, externalPushbackMultiplier)) {
			return;
		}

		auto pushbackMultiplier = _calculatePushbackMultiplier(push, *massComponent, externalPushbackMultiplier, affecting, bApplyResistance);
		auto launchVector = getLaunchVector(push, direction, affecting, pushbackMultiplier);

		// #D11.CM - Clamp underwater pushback Z Height
		if (!ignoreLevelClamps) {
			if (const auto abilitySystem = Cast<ABaseCharacter>(&affecting)->GetAbilitySystemComponent()) {
				float ZClamp = abilitySystem->GetNumericAttribute(UResistanceAttributeSet::PushbackZClampMagnitudeAttribute());
				if (ZClamp >= 0) {
					launchVector = FVector{ launchVector.X, launchVector.Y, FMath::Min(launchVector.Z, ZClamp) };
				}
			}
		}

		if (auto characterMovementComponent = affecting.FindComponentByClass<UCharacterMovementComponent>()) {
			characterMovementComponent->StopActiveMovement();
			characterMovementComponent->Launch(launchVector);
			characterMovementComponent->HandlePendingLaunch();
		}
	}
}

FVector getLaunchVector(const FPushback& push, const AActor& by, const AActor& affecting, float pushbackMultiplier, float minimumPushbackStrength)
{
	auto pushStructStrength = (push.enablePushback ? push.pushbackStrength : 0.0f);
	FVector attackDirection = push.useForwardDirection ? by.GetActorForwardVector() : affecting.GetActorLocation() - by.GetActorLocation();
	auto launchDirection = adjustDirectionZ(push, attackDirection, affecting);
	auto launchAmount = REFERENCE_PUSHBACK_STRENGTH * FMath::Sqrt((minimumPushbackStrength + pushStructStrength) * pushbackMultiplier);
	return launchDirection * launchAmount;
}

FVector getLaunchVector(const FPushback& push, const FVector& direction, const AActor& affecting, float pushbackMultiplier /*= 1.f*/, float minimumPushbackStrength /*= 0.f*/)
{
	auto pushStructStrength = (push.enablePushback ? push.pushbackStrength : 0.0f);	
	auto launchDirection = adjustDirectionZ(push, direction, affecting);
	auto launchAmount = REFERENCE_PUSHBACK_STRENGTH * FMath::Sqrt((minimumPushbackStrength + pushStructStrength) * pushbackMultiplier);
	return launchDirection * launchAmount;
}

FVector adjustDirectionZ(const FPushback& push, const FVector& direction, const AActor& affecting) {
	FVector xyAttackDirection(direction.X, direction.Y, 0);
	xyAttackDirection.Normalize();

	FVector launchDirection(xyAttackDirection.X, xyAttackDirection.Y, push.pushbackZFactor);
	launchDirection.Normalize();

	if (CVarDebugDrawPushback.GetValueOnGameThread() > 0) {
		auto debugOrigin = affecting.GetActorLocation();
		float length = REFERENCE_PUSHBACK_STRENGTH * push.pushbackStrength;
		DrawDebugLine(affecting.GetWorld(), debugOrigin, debugOrigin + xyAttackDirection * length, FColor::Black, true, 1.5f, 0, 5.f);
		DrawDebugLine(affecting.GetWorld(), debugOrigin, debugOrigin + launchDirection * length, FColor::Turquoise, true, 1.5f, 0, 5.f);
	}

	return launchDirection;
}


}