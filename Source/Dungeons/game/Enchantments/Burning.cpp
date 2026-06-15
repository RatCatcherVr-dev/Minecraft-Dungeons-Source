// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "Burning.h"
#include "game/abilities/effects/executions/DamageExecutionCalculation.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/effects/calculations/DamageModCalculations.h"
#include "util/CharacterQuery.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/item/power/ItemPowerStats.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"

UBurningDamageGameplayEffect::UBurningDamageGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FGameplayEffectExecutionDefinition damage;
	damage.CalculationClass = UDamageExecutionCalculation::StaticClass();
	Executions.Add(std::move(damage));

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Damage.Weak")));
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Damage.Elemental.Fire")));

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Damage"), 0, 1);
	GameplayCues.Last().MagnitudeAttribute = UHealthAttributeSet::HealthAttribute();
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Enchantment.Burning"), 0, 1);
	GameplayCues.Last().MagnitudeAttribute = UHealthAttributeSet::HealthAttribute();

	FCustomCalculationBasedFloat healthMagnitude;
	healthMagnitude.CalculationClassMagnitude = UArmorItemPowerOnlyModDamageCalculation::StaticClass();

	FGameplayModifierInfo healthInfo;
	healthInfo.Attribute = UHealthAttributeSet::HealthAttribute();
	healthInfo.ModifierMagnitude = healthMagnitude;
	healthInfo.ModifierOp = EGameplayModOp::Type::Additive;
	Modifiers.Add(healthInfo);
}


UBurning::UBurning() {
	LevelMultiplier = [this](int level) -> float {
		return 1.0f * (float)level * BurnBaseDamage;
	};
	ItemPowerMultiplier = game::item::power::DamageIncreaseMultiplier;
	MultiplierFormatter = valueformat::asDamage;
	TypeId = EEnchantmentTypeID::Burning;
}


FText UBurning::CreateDescription() const {
	return FText::Format(Super::CreateDescription(), FText::FromString(valueformat::asEverySingleDecimalSecond(BurnInterval)));
}

void UBurning::BeginPlay() {
	Super::BeginPlay();
	SpawnFireAura();
}

void UBurning::EndPlay(EEndPlayReason::Type endPlayReason) {
	Super::EndPlay(endPlayReason);
	RemoveFireAura();
}

void UBurning::OnOwnerDeath() {
	RemoveFireAura();
}

void UBurning::SpawnFireAura() {
	const auto mesh = Cast<ABaseCharacter>(GetOwner())->GetMesh();
	const auto capsule = GetOwner()->FindComponentByClass<UCapsuleComponent>();

	FireAuraChildActor = NewObject<UChildActorComponent>(mesh);
	FireAuraChildActor->RegisterComponent();
	FireAuraChildActor->SetChildActorClass(FireAuraActorClass);

	FAttachmentTransformRules rules(EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, false);

	FireAuraChildActor->AttachToComponent(mesh, rules, "J_Status_Socket_Center");
	FireAuraChildActor->AddLocalOffset(FVector(0, 0, -capsule->GetScaledCapsuleHalfHeight()));
	FireAuraChildActor->SetWorldRotation(FRotator(0, 0, 0));

	GetCharacterOwner()->OnDeath.AddUObject(this, &UBurning::OnOwnerDeath);
}

void UBurning::Execution() {
	if (GetOwner()->HasAuthority()) {
		OnBurn();
	}
}

float UBurning::GetExecutionInterval() const {
	return BurnInterval;
}

void UBurning::RemoveFireAura() {
	if (FireAuraChildActor.IsValid()) {
		FireAuraChildActor->DestroyChildActor();
	}
}

void UBurning::OnBurn() {
	auto characterOwner = GetCharacterOwner();
	const auto hostile = characterquery::is::hostile(characterOwner);
	const auto predicate = [&](const ABaseCharacter* v) { return hostile(v) && characterquery::is::targetable(v) && actorquery::is::alive(v);  };

	TArray<ABaseCharacter*> targets = actorquery::getNearbyActors<ABaseCharacter>(characterOwner, Radius).FilterByPredicate(predicate);

	auto ownerAbilitySystem = characterOwner->GetAbilitySystemComponent();


	float damage = (characterOwner->IsA(AMobCharacter::StaticClass()) ? MobBurnDamage : LevelMultiplier(Level));
	FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
	for (auto target : targets) {
		auto targetAbilitySystem = target->GetAbilitySystemComponent();
		FGameplayEffectSpec targetspec = effects::CreateGameplayEffectSpec<UBurningDamageGameplayEffect>(ownerAbilitySystem, Level);
		targetspec.SetSetByCallerMagnitude(effects::HealthName, -damage);
		ownerAbilitySystem->ApplyGameplayEffectSpecToTarget(targetspec, targetAbilitySystem);
	}
}