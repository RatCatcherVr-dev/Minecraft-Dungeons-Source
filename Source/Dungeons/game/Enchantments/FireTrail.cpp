#include "Dungeons.h"
#include "FireTrail.h"
#include "game/Conversion.h"
#include "util/CharacterQuery.h"
#include "game/abilities/attributes/MovementAttributeSet.h"
#include "game/abilities/effects/requirements/BaseGameplayEffectApplicationRequirement.h"
#include <AbilitySystemComponent.h>
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/effects/calculations/DamageModCalculations.h"
#include "game/abilities/effects/executions/DamageExecutionCalculation.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/component/TeleportComponent.h"
#include "game/item/power/ItemPowerStats.h"
#include "game/component/ContinousDamageComponent.h"
#include "game/team/TeamQuery.h"

namespace firetrail {
	FName FireTrailSpeedEffectMagnitude(TEXT("FireTrailSpeedEffectMagnitude"));
	const float ZBIAS = 10.f;
}

UFireTrailDamageGameplayEffect::UFireTrailDamageGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	//StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	bExecutePeriodicEffectOnApplication = true;
	bRequireModifierSuccessToTriggerCues = true;
	bSuppressStackingCues = true;
	Period = 0.2f;

	FGameplayEffectExecutionDefinition damage;
	damage.CalculationClass = UDamageExecutionCalculation::StaticClass();
	Executions.Add(std::move(damage));

	FCustomCalculationBasedFloat healthMagnitude;
	healthMagnitude.CalculationClassMagnitude = UArmorItemPowerOnlyModDamageCalculation::StaticClass();
	healthMagnitude.Coefficient = Period.GetValueAtLevel(1);

	FGameplayModifierInfo healthInfo;
	healthInfo.Attribute = UHealthAttributeSet::HealthAttribute();
	healthInfo.ModifierMagnitude = healthMagnitude;
	healthInfo.ModifierOp = EGameplayModOp::Type::Additive;
	Modifiers.Add(healthInfo);

	const auto weakDamageTag = FGameplayTag::RequestGameplayTag(TEXT("Damage.Weak.Fire"));
	InheritableGameplayEffectTags.AddTag(weakDamageTag);
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Damage.Elemental.Fire")));

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Damage.Medium"), 0, 1);
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Damage"), 0, 1);
	GameplayCues.Last().MagnitudeAttribute = UHealthAttributeSet::HealthAttribute();
}

AFireBlock::AFireBlock() {
	DamageComponent->EffectClass = UFireTrailDamageGameplayEffect::StaticClass();
}

UFireTrail::UFireTrail() {
	TypeId = EEnchantmentTypeID::FireTrail;

	LevelMultiplier = [this](int level) -> float {
		return BaseDamagePerSecond * level;
	};
	ItemPowerMultiplier = game::item::power::DamageIncreaseMultiplier;
	MultiplierFormatter = valueformat::asDamage;
}

void UFireTrail::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (GetOwner() && GetOwner()->HasAuthority()) {
		const auto* ownerCharacter = GetCharacterOwner();
		auto zOffset = 0.0f;
		if (const auto* capsule = ownerCharacter->FindComponentByClass<UCapsuleComponent>()) {
			zOffset = capsule->GetScaledCapsuleHalfHeight() - firetrail::ZBIAS;
		}
		const auto& ownerLocation = ownerCharacter->GetActorLocation();
		const auto currentBlockPos = conversion::ueToBlock(FVector(ownerLocation.X, ownerLocation.Y, ownerLocation.Z - zOffset));
		if (currentBlockPos != lastBlockPos) {
			OnExitedBlock(lastBlockPos);
			lastBlockPos = currentBlockPos;
		}
	}
}

FText UFireTrail::CreateDescription() const {
	return FText::Format(Super::CreateDescription(), FText::FromString(valueformat::asForRoundedWordSecond(FireBlockDuration)));
}

void UFireTrail::OnStart() {
	Super::OnStart();

	bIsOwnerPlayer = GetOwner()->IsA<APlayerCharacter>();

	if (bIsOwnerPlayer) {
		if (auto teleport = GetOwner()->FindComponentByClass<UTeleportComponent>()) {
			teleport->OnTeleported.AddUObject(this, &UFireTrail::StopFireSpawning);
		}
	}
	else {
		StartFireSpawning();
	}
}

void UFireTrail::OnEnd() {
	if (bShouldSpawnFire && GetOwner()->HasAuthority()) {
		StopFireSpawning();
	}
}

void UFireTrail::OnDodgeRollStart(const FVector& dodgeDirection, FPredictionKey) {
	if (bIsOwnerPlayer) {
		if (GetOwner()->HasAuthority()) {
			StartFireSpawning();
			BroadcastEnchantmentTriggeredEvent();
		}
	}
}

void UFireTrail::OnDodgeRollEnd(FPredictionKey) {
	if (bIsOwnerPlayer) {
		if (GetOwner()->HasAuthority()) {
			if (bShouldSpawnFire) {
				GetWorld()->GetTimerManager().SetTimer(TrailTimerHandle, this, &UFireTrail::StopFireSpawning, DurationAfterDodgeEnd);
			}
		}
	}
}

void UFireTrail::OnExitedBlock(const BlockPos& blockPos) const {
	if (bShouldSpawnFire) {
		SpawnFireBlock(blockPos);
	}
}

void UFireTrail::StartFireSpawning() {
	ApplySpeedEffect();
	bShouldSpawnFire = true;
	if (TrailTimerHandle.IsValid() && GetWorld()->GetTimerManager().IsTimerActive(TrailTimerHandle)) {
		GetWorld()->GetTimerManager().ClearTimer(TrailTimerHandle);
	}
}

void UFireTrail::StopFireSpawning() {
	RemoveSpeedEffect();
	bShouldSpawnFire = false;
}

void UFireTrail::SpawnFireBlock(const BlockPos& blockPos) const {
	const auto spawnPos = conversion::blockCenterXZToUe(blockPos);

	FTransform transform;
	transform.SetLocation(spawnPos);

	auto isFriendly = [&](){
		if( auto characterOwner = GetCharacterOwner() ) {
			return teamquery::is::friendly(characterOwner->GetCurrentTeam(), ETeamName::Heroes);
		}
		return false;
	};
		
	if (AFireBlock* fireBlock = GetWorld()->SpawnActorDeferred<AFireBlock>(isFriendly() ? PlayerFireBlockClass : MobFireBlockClass, transform, GetOwner(), Cast<ABaseCharacter>(GetOwner()), ESpawnActorCollisionHandlingMethod::AlwaysSpawn)) {
		if (auto damage = fireBlock->FindComponentByClass<UContinousDamageComponent>()) {
			damage->DamagePerSecond = (LevelMultiplier(Level));
		}
		fireBlock->DamageDuration = FireBlockDuration;
		fireBlock->FinishSpawning(transform);	
	}
}

void UFireTrail::ApplySpeedEffect() {
	if (const auto* characterOwner = GetCharacterOwner()) {
		auto abilitySystem = characterOwner->GetAbilitySystemComponent();

		auto spec(effects::CreateGameplayEffectSpec<UFireTrailSpeedGameplayEffect>(abilitySystem, Level));
		spec.SetSetByCallerMagnitude(firetrail::FireTrailSpeedEffectMagnitude, 1.1f);
		SpeedEffectHandle = abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
	}
}

void UFireTrail::RemoveSpeedEffect() const {
	if (const auto* characterOwner = GetCharacterOwner()) {
		auto abilitySystem = characterOwner->GetAbilitySystemComponent();

		abilitySystem->RemoveActiveGameplayEffect(SpeedEffectHandle);
	}
}

UFireTrailSpeedGameplayEffect::UFireTrailSpeedGameplayEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;

	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	FGameplayModifierInfo info;
	info.Attribute = UMovementAttributeSet::SpeedMultiplierAttribute();

	FSetByCallerFloat speedMagnitude;
	speedMagnitude.DataName = firetrail::FireTrailSpeedEffectMagnitude;

	info.ModifierMagnitude = speedMagnitude;
	info.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(info);

	ApplicationRequirements.Emplace(UBaseGameplayEffectApplicationRequirement::StaticClass());
}
