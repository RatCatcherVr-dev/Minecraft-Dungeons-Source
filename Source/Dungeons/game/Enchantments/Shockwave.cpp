// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "Shockwave.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/component/MeleeAttackComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/effects/calculations/DamageModCalculations.h"
#include "game/abilities/effects/executions/DamageExecutionCalculation.h"
#include "EnchantmentUtil.h"
#include <AbilitySystemGlobals.h>
#include "util/CharacterQuery.h"
#include "../item/power/ItemPowerStats.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"

extern TAutoConsoleVariable<int32> CVarDebugDrawEnchantments;

UShockwave::UShockwave() {
	TypeId = EEnchantmentTypeID::Shockwave;
	PredictiveExecution = true;

	LevelMultiplier = [this](int level) -> float {
		return (1.0f + (level-1)*0.5f) * BaseDamage;
	};
	ItemPowerMultiplier = game::item::power::DamageIncreaseMultiplier;
	MultiplierFormatter = valueformat::asDamage;
}

void UShockwave::OnAfterMeleeAttack(AActor* attackTarget, int32 hitCount, FVector attackVector, int32 index, FRandomStream& randStream, FSharedPredictionContext window) {
	const auto characterOwner = GetCharacterOwner();
	if (const auto meleeAttackComponent = characterOwner->FindComponentByClass<UMeleeAttackComponent>()) {
		const auto& activeAttackVariants = meleeAttackComponent->GetActiveAttackVariants();
		
		if (activeAttackVariants.Num() > 0) {
			if ((activeAttackVariants.Num() - 1) == index) {
				FVector direction = GetOwner()->GetActorForwardVector();
	
				if (GetOwnerRole() == ROLE_Authority) {
					MulticastSpawnShockwave(direction, window.GetKey());
					BroadcastEnchantmentTriggeredEvent();
				}
				else {
					SpawnShockwave(direction);
				}
			}
		}
	}
}

void UShockwave::SpawnShockwave(FVector direction) {
	FTransform trans;

	auto StartLocation = GetOwner()->GetActorLocation();
	const auto RotationVector = GetOwner()->GetActorRotation().Vector();
	auto DesiredLocation = StartLocation + RotationVector * spawnOffset;
	FHitResult result;
	if (GetWorld()->LineTraceSingleByChannel(result, StartLocation, DesiredLocation, (ECollisionChannel)ECustomTraceChannels::TerrainOnly)) {
		DesiredLocation = result.Location - RotationVector * spawnSafetyMargin;
	}

	trans.SetLocation(DesiredLocation);
	trans.SetRotation(GetOwner()->GetActorRotation().Quaternion());

	const float Damage = IsOwnerMob() ? MobDamage : LevelMultiplier(Level);
	const float Speed = (1.f + (Level - 1)*0.1f) * BaseMoveSpeed;

	SpawnedShockWave = GetWorld()->SpawnActorDeferred<AShockWaveProjectile>(ShockWaveProjectileClass, trans, GetOwner(), nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (SpawnedShockWave.IsValid()) {
		SpawnedShockWave->Damage = Damage;
		SpawnedShockWave->MoveVector = direction * Speed;
		UGameplayStatics::FinishSpawningActor(SpawnedShockWave.Get(), trans);
	}
}


void UShockwave::MulticastSpawnShockwave_Implementation(FVector direction, FPredictionKey key /* = FPredictionKey() */) {
	if (!key.IsLocalClientKey() || GetOwnerRole() == ROLE_Authority) {
		SpawnShockwave(direction);
	}
}

AShockWaveProjectile::AShockWaveProjectile() {
	PrimaryActorTick.bCanEverTick = true;
}

void AShockWaveProjectile::BeginPlay() {
	Super::BeginPlay();
	if (HasAuthority()) {
		SetLifeSpan(Duration);
	}
}

void AShockWaveProjectile::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (CVarDebugDrawEnchantments.GetValueOnGameThread()) {
		if (const auto box = FindComponentByClass<UBoxComponent>()) {
			DrawDebugBox(GetWorld(), GetActorLocation(), box->GetScaledBoxExtent(), GetActorRotation().Quaternion(), FColor::Green);
		}
	}

	AddActorWorldOffset(MoveVector * DeltaTime, true);
}


void AShockWaveProjectile::NotifyActorBeginOverlap(AActor* OtherActor) {
	if (!HasAuthority()) return;

	if (const auto target = Cast<ABaseCharacter>(OtherActor)) {
		ABaseCharacter* owner = Cast<ABaseCharacter>(GetOwner());
		if(!owner) return;

		if (characterquery::is::targetable(target) && !owner->IsFriendlyTowards(target) && actorquery::isAlive(target)) {
			FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::Normal);
			FPushback push;
			push.enablePushback = true;
			push.pushbackStrength = 0.1f;

			if (target->HasAuthority()) {
				pushback::pushback(push, *this, *target);
			}

			UAbilitySystemComponent* targetAbilitySystem = target->GetAbilitySystemComponent();
			UAbilitySystemComponent* abilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
			FGameplayEffectSpec spec = effects::CreateGameplayEffectSpec<UShockwaveDamageGameplayEffect>(abilitySystem, effects::HealthName, -Damage, GetOwner(), this, GetActorLocation(), 1.f);
			effects::SetStunMultiplier(spec, effects::ENCHANTMENT_STUN_MULTIPLIER);
			abilitySystem->ApplyGameplayEffectSpecToTarget(spec, targetAbilitySystem);
		}
	}
}

void AShockWaveProjectile::NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) {
	if(HasAuthority()){
		if (const auto owner = Cast<ABaseCharacter>(GetOwner())) {
			if(auto abilitySystem = owner->GetAbilitySystemComponent()){
				FGameplayCueParameters params;
				params.Location = HitLocation;
				abilitySystem->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Enchantment.ShockWave.Impact"), params);
			}
		}
	}
	Destroy();
}

UShockwaveDamageGameplayEffect::UShockwaveDamageGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FCustomCalculationBasedFloat healthMagnitude;
	healthMagnitude.CalculationClassMagnitude = UMeleeItemPowerOnlyModDamageCalculation::StaticClass();

	FGameplayModifierInfo healthInfo;
	healthInfo.Attribute = UHealthAttributeSet::HealthAttribute();
	healthInfo.ModifierMagnitude = healthMagnitude;
	healthInfo.ModifierOp = EGameplayModOp::Type::Additive;
	Modifiers.Add(healthInfo);

	FGameplayEffectExecutionDefinition damage;
	damage.CalculationClass = UDamageExecutionCalculation::StaticClass();
	Executions.Add(std::move(damage));

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Damage.Melee.Shockwave")));
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Damage.Melee.Shockwave"), 0, 1);
	GameplayCues.Last().MagnitudeAttribute = UHealthAttributeSet::HealthAttribute();
}