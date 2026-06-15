#include "Dungeons.h"
#include "TempoTheft.h"

#include "AbilitySystemComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include <GameplayEffect.h>
#include "game/component/HealthComponent.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/component/MoveToTargetMovementComponent.h"
#include "game/abilities/attributes/MovementAttributeSet.h"
#include "util/CharacterQuery.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/effects/calculations/ResistanceModCalculations.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/abilities/effects/calculations/StatusModCalculations.h"

namespace TempoTheftMagnitudes {
	FName TempoTheftSpeedEffectMagnitude(TEXT("TempoTheftSpeedMagnitude"));
	FName TempoTheftDurationEffectMagnitude(TEXT("TempoTheftDurationMagnitude"));
}

UTempoTheft::UTempoTheft() {	
	TypeId = EEnchantmentTypeID::TempoTheft;

	LevelMultiplier = [this](int level) -> float {
		return AmountToStealPerLevel * level;
	};
	MultiplierFormatter = valueformat::asPercentage;
	TargetEffect = UTempoTheftGameplayEffect::StaticClass();
}

FText UTempoTheft::CreateDescription() const {
	return FText::Format(Super::CreateDescription(), FText::FromString(valueformat::asForRoundedWordSecond(StealTime)));
}

void UTempoTheft::OnAfterDealtRangedDamage(float Damage, ABaseProjectile* fromProjectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream) {	
	const auto target = Cast<ABaseCharacter>(toWhat);
	if (GetOwnerRole() != ROLE_Authority || !characterquery::is::targetable(target) || !actorquery::is::alive(target)) return;

	//BroadcastEnchantmentTriggeredEvent(); Status effect should be enough
	OnStealSpeed(target, atLocation);
}

void UTempoTheft::OnStealSpeed(ABaseCharacter* toStealFrom, FVector atLocation) {
	FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
	auto characterOwner = GetCharacterOwner();

	auto ownerAbilitySystem = characterOwner->GetAbilitySystemComponent();

	const float multiplier = IsOwnerMob() ? MobAmountToSteal : LevelMultiplier(Level);
	const float duration = IsOwnerMob() ? MobStealTime : StealTime;
	FGameplayEffectSpec targetSpec = effects::CreateGameplayEffectSpecFromSubClass(ownerAbilitySystem, TargetEffect, Level);
	targetSpec.SetSetByCallerMagnitude(USlowMultiplicativeResistanceModCalculation::SetByCallerKey, 1.f/ multiplier);
	targetSpec.SetSetByCallerMagnitude(effects::DurationName, duration);
	ownerAbilitySystem->ApplyGameplayEffectSpecToTarget(targetSpec, Cast<ABaseCharacter>(toStealFrom)->GetAbilitySystemComponent());

	FGameplayCueParameters params;
	params.Location = atLocation;

	ownerAbilitySystem->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Enchantment.TempoTheft"), params);

	MulticastSpawnSoul(characterOwner, toStealFrom->GetActorLocation(), multiplier, duration);
}

void UTempoTheft::MulticastSpawnSoul_Implementation(ABaseCharacter* owner, const FVector& spawnLocation, float speedBoostAmount, float speedBoostDuration) {
	FTransform spawnTransform;
	spawnTransform.SetLocation(spawnLocation);
	auto actor = GetWorld()->SpawnActorDeferred<ASpeedBoostOnOverlapActor>(ActorClass, spawnTransform, owner, Cast<APawn>(GetOwner()), ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
	actor->TargetCharacter = owner;
	actor->SpeedBoostAmount = speedBoostAmount;
	actor->SpeedBoostDuration = speedBoostDuration;
	actor->Level = Level;
	UGameplayStatics::FinishSpawningActor(actor, spawnTransform);
}

UTempoTheftBoostEffect::UTempoTheftBoostEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	FCustomCalculationBasedFloat durationMagnitude;
	durationMagnitude.CalculationClassMagnitude = UStatusDurationModCalculation::StaticClass();
	DurationMagnitude = durationMagnitude;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;

	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	FGameplayModifierInfo speedInfo;
	speedInfo.Attribute = UMovementAttributeSet::SpeedMultiplierAttribute();

	FSetByCallerFloat speedMagnitude;
	speedMagnitude.DataName = TempoTheftMagnitudes::TempoTheftSpeedEffectMagnitude;

	speedInfo.ModifierMagnitude = speedMagnitude;
	speedInfo.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(speedInfo);

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.Positive")));

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Enchantment.TempoTheft"), 0, 1);
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Enchantment.TempoTheft"), 0, 1);
}

UTempoTheftGameplayEffect::UTempoTheftGameplayEffect(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	FCustomCalculationBasedFloat durationMagnitude;
	durationMagnitude.CalculationClassMagnitude = UStatusDurationModCalculation::StaticClass();
	DurationMagnitude = durationMagnitude;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateBySource;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;

	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	FCustomCalculationBasedFloat speedMagnitude;
	speedMagnitude.CalculationClassMagnitude = USlowMultiplicativeResistanceModCalculation::StaticClass();

	FGameplayModifierInfo speedInfo;
	speedInfo.Attribute = UMovementAttributeSet::SpeedMultiplierAttribute();

	speedInfo.ModifierMagnitude = speedMagnitude;
	speedInfo.ModifierOp = EGameplayModOp::Type::Division;

	Modifiers.Add(speedInfo);

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.Negative")));
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Enchantment.Freezing"), 0, 1);
}


ASpeedBoostOnOverlapActor::ASpeedBoostOnOverlapActor() {
	OwnerEffect = UTempoTheftBoostEffect::StaticClass();
	Overlap = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Overlap->SetSphereRadius(5.f);
	Movement = CreateDefaultSubobject<UMoveToTargetMovementComponent>(TEXT("Movement"));
	Movement->Acceleration = 3000.f;
	Movement->MaxSpeed = 6000.f;
}

void ASpeedBoostOnOverlapActor::BeginPlay() {
	Super::BeginPlay();
	if (TargetCharacter.IsValid()) {
		Movement->SetTarget(TargetCharacter.Get());
	}
	else {
		Destroy();
	}
}

void ASpeedBoostOnOverlapActor::NotifyActorBeginOverlap(AActor* OtherActor) {
	if (OtherActor == TargetCharacter && HasAuthority()) {
		OnGrantSpeed(Cast<ABaseCharacter>(OtherActor));
		Destroy();
	}
}

void ASpeedBoostOnOverlapActor::OnGrantSpeed_Implementation(ABaseCharacter* character) {
	if (character) {
		auto characterAbilitySystem = character->GetAbilitySystemComponent();
		FGameplayEffectSpec spec = effects::CreateGameplayEffectSpecFromSubClass(characterAbilitySystem, OwnerEffect, Level);
		spec.SetSetByCallerMagnitude(TempoTheftMagnitudes::TempoTheftSpeedEffectMagnitude, 1.0f + SpeedBoostAmount );
		spec.SetSetByCallerMagnitude(effects::DurationName, SpeedBoostDuration);
		characterAbilitySystem->ApplyGameplayEffectSpecToSelf(spec);
	}
}
