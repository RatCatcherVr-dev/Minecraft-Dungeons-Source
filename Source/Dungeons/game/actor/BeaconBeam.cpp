#include "Dungeons.h"
#include "BeaconBeam.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/effects/executions/DamageExecutionCalculation.h"
#include "game/abilities/effects/calculations/DamageModCalculations.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"

ABeaconBeam::ABeaconBeam(const FObjectInitializer& objectInitializer)
	: Super(objectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void ABeaconBeam::BeginPlay() {
	Super::BeginPlay();

	for (auto* component : GetComponentsByClass(UParticleSystemComponent::StaticClass())) {
		beams.Add(Cast<UParticleSystemComponent>(component));
	}

	startTime = GetWorld()->GetTimeSeconds();
	damageTime = 0.f;	
}

void ABeaconBeam::TickBeam(FVector& beamStartLocation, float& beamLength) {
	beamLength = beamLengthMax;
	beamStartLocation = GetActorLocation();
	FVector beamEndLocation = beamStartLocation + GetActorForwardVector() * beamLength;

	FHitResult hitResult;
	FCollisionObjectQueryParams params;
	params.AddObjectTypesToQuery(ECC_WorldStatic);

	if (GetWorld()->LineTraceSingleByObjectType(hitResult, beamStartLocation, beamEndLocation, params)) {
		beamLength = hitResult.Distance;
		beamEndLocation = hitResult.ImpactPoint;
	}

	for (auto* beam : beams) {
		beam->SetBeamTargetPoint(emitterIndex, beamEndLocation, targetIndex);
	}

	// DrawDebugCylinder(GetWorld(), beamStartLocation, beamEndLocation, 20, 50, FColor::Green, false, 0.1f);
}

void ABeaconBeam::TickDamage(FVector beamStartLocation, float beamLength) {
	TArray<AActor*> charactersHit;
	actorquery::getActorsInCylinder(
		GetWorld(),
		beamStartLocation,
		GetActorForwardVector(),
		ABaseCharacter::StaticClass(),
		beamLength,
		beamRadius,
		0.f,
		charactersHit
	);

	const auto attacker = [&] {
		const auto parent = GetAttachParentActor();
		if (const auto maybeCharacter = Cast<ABaseCharacter>(parent)) {
			return maybeCharacter;
		} else {
			return Cast<ABaseCharacter>(parent->GetAttachParentActor());
		}
	}();

	if (attacker) {
		UAbilitySystemComponent* abilitySystem = attacker->GetAbilitySystemComponent();
		FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);

		for (auto* characterHit : charactersHit) {
			if (attacker->IsHostileTowards(Cast<ABaseCharacter>(characterHit))) {
				const auto targetAbilitySystem = Cast<ABaseCharacter>(characterHit)->GetAbilitySystemComponent();

				FGameplayEffectSpec spec = effects::CreateGameplayEffectSpec<UArchBeamDamageGameplayEffect>(
					abilitySystem,
					effects::HealthName,
					-damageAmount,
					attacker,
					attacker,
					characterHit->GetActorLocation(),
					1.f
					);

				abilitySystem->ApplyGameplayEffectSpecToTarget(spec, targetAbilitySystem);
			}
		}
	}
}

void ABeaconBeam::Tick(float deltaTime) {
	Super::Tick(deltaTime);	

	FVector beamStartLocation;
	float beamLength;
	TickBeam(beamStartLocation, beamLength);
	if (HasAuthority()) {
		const float time = GetWorld()->GetTimeSeconds();
		if (time < startTime + delay) {
			return;
		}

		if (damageDuration > 0 && time > startTime + delay + damageDuration) {
			return;
		}

		damageTime += deltaTime;
		if (damageTime < damageInterval) {
			return;
		}
		damageTime -= damageInterval;
		TickDamage(beamStartLocation, beamLength);
	}
}

UArchBeamDamageGameplayEffect::UArchBeamDamageGameplayEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FCustomCalculationBasedFloat healthMagnitude;
	healthMagnitude.CalculationClassMagnitude = UMeleeModDamageCalculation::StaticClass();

	FGameplayModifierInfo healthInfo;
	healthInfo.Attribute = UHealthAttributeSet::HealthAttribute();
	healthInfo.ModifierMagnitude = healthMagnitude;
	healthInfo.ModifierOp = EGameplayModOp::Type::Additive;
	Modifiers.Add(healthInfo);

	FGameplayEffectExecutionDefinition damage;
	damage.CalculationClass = UDamageExecutionCalculation::StaticClass();
	Executions.Add(std::move(damage));
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Damage.Aoe"), 0, 1);
	GameplayCues.Last().MagnitudeAttribute = UHealthAttributeSet::HealthAttribute();
}