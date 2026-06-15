#include "Dungeons.h"
#include "game/actor/character/BaseCharacter.h"
#include "TurnComponent.h"
#include "Assets/DungeonsAssetManager.h"
#include "PlayerExperienceComponent.h"
#include "Dungeons/DungeonsGameInstance.h"

UTurnComponent::UTurnComponent() {
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

float UTurnComponent::GetWinding(float a, float b) {
	if (a < b) {
		return b - a > a + 360.f - b ? 1.f : -1.f;
	} else if (b < a) {
		return a - b > b + 360.f - a ? -1.f : 1.f;
	} else {
		return 0.f;
	}	
}

float UTurnComponent::WrapAngle(float angle) {
	const auto positive = (static_cast<int>(angle) + 3600) % 360;
	return positive <= 180 ? positive : 180 - positive;
}

void UTurnComponent::Turn(float _targetAngle, float _speed/* = 180.f*/) {
	if (IsComponentTickEnabled()) {
		return;
	}

	targetAngle = WrapAngle(_targetAngle);
	speed = _speed;

	const auto owner = GetOwner();
	const auto currentAngle = owner->GetActorRotation().Yaw;
	sign = GetWinding(targetAngle, currentAngle);

	SetComponentTickEnabled(true);

	if (sequence) {
		auto characterOwner = Cast<ABaseCharacter>(owner);
		characterOwner->MulticastPlayAnimationAsDynamicMontage(sequence, FName(TEXT("FullBody")));
	}
}

bool UTurnComponent::IsTurning() const {
	return IsComponentTickEnabled();
}

void UTurnComponent::TickComponent(float deltaTime, ELevelTick, FActorComponentTickFunction*) {
	auto owner = GetOwner();

	const auto currentAngle = owner->GetActorRotation().Yaw;
	
	if (FMath::Abs(targetAngle - currentAngle) > deltaTime * speed) {
		owner->AddActorLocalRotation(FRotator {
			0.f,
			deltaTime * speed * sign,
			0.f
		}, false);
	} else {
		owner->SetActorRotation(FRotator {
			0.f,
			targetAngle,
			0.f
		});
		
		SetComponentTickEnabled(false);
	}
}