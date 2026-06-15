// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "Chilling.h"
#include "game/util/ActorQuery.h"
#include "Dungeons/util/CharacterQuery.h"
#include <AbilitySystemComponent.h>
#include "game/actor/character/mob/MobCharacter.h"
#include "game/abilities/attributes/MeleeAttributeSet.h"
#include "game/abilities/attributes/MovementAttributeSet.h"
#include "Freezing.h"
#include "game/abilities/effects/calculations/ResistanceModCalculations.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/abilities/effects/GameplayEffectUtil.h"

UChilling::UChilling() {
	TargetEffect = UChillingGameplayEffect::StaticClass();
	TypeId = EEnchantmentTypeID::Chilling;
	LevelMultiplier = [this](int level) {
		return 1.0f - (0.2f * (float)level);
	};
	MultiplierFormatter = valueformat::asRelativeMultiplierPercentageChange;
}

FText UChilling::CreateDescription() const {
	return FText::Format(Super::CreateDescription(), FText::FromString(valueformat::asEveryRoundedWordSecond(FreezeInterval)), FText::FromString(valueformat::asForRoundedWordSecond(FreezeTime)));
}

void UChilling::BeginPlay() {
	Super::BeginPlay();

	if (GetOwner()->IsA(AMobCharacter::StaticClass())) {
		SpawnChillingAura();
		GetCharacterOwner()->OnDeath.AddUObject(this, &UChilling::OnOwnerDeath);
	}
}

void UChilling::EndPlay(EEndPlayReason::Type endPlayReason) {
	Super::EndPlay(endPlayReason);

	if (GetOwner()->IsA(AMobCharacter::StaticClass())) {
		RemoveChillingAura();
	}
}

void UChilling::Execution() {
	if (GetOwner()->HasAuthority()) {
		OnBlast();
	}
}

float UChilling::GetExecutionInterval() const 
{
	return FreezeInterval;
}

void UChilling::OnOwnerDeath() {
	RemoveChillingAura();
}

void UChilling::OnBlast() {
	auto characterOwner = GetCharacterOwner();
	const auto hostile = characterquery::is::hostile(characterOwner);
	const auto predicate = [&](const ABaseCharacter* v) { return hostile(v) && characterquery::is::targetable(v) && actorquery::is::alive(v);  };

	TArray<ABaseCharacter*> targets = actorquery::getNearbyActors<ABaseCharacter>(characterOwner, Radius).FilterByPredicate(predicate);

	auto ownerAbilitySystem = characterOwner->GetAbilitySystemComponent();

	const float multiplier = characterOwner->IsA(AMobCharacter::StaticClass()) ? 0.6f : LevelMultiplier(Level);
	const float divider = 1.0f / multiplier;
	
	FGameplayEffectSpec targetspec(Cast<UFreezingGameplayEffect>(TargetEffect->GetDefaultObject()), ownerAbilitySystem->MakeEffectContext(), Level);
	targetspec.SetSetByCallerMagnitude(USlowMultiplicativeResistanceModCalculation::SetByCallerKey, divider);
	targetspec.SetSetByCallerMagnitude(effects::DurationName, FreezeTime);

	FParameterFilterContextWindow window(EGameplayCueParametersField::EMPTY);
	
	for (auto target : targets) {
		auto targetAbilitySystem = target->GetAbilitySystemComponent();
		ownerAbilitySystem->ApplyGameplayEffectSpecToTarget(targetspec, targetAbilitySystem);
	}
}

void UChilling::SpawnChillingAura() {
	const auto mesh = Cast<ABaseCharacter>(GetOwner())->GetMesh();
	const auto capsule = GetOwner()->FindComponentByClass<UCapsuleComponent>();

	ChillingAuraChildActor = NewObject<UChildActorComponent>(mesh);
	ChillingAuraChildActor->RegisterComponent();
	ChillingAuraChildActor->SetChildActorClass(ChillingAuraActorClass);

	FAttachmentTransformRules rules(EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, EAttachmentRule::SnapToTarget, false);

	ChillingAuraChildActor->AttachToComponent(mesh, rules);
	ChillingAuraChildActor->AddLocalOffset(FVector(0, 0, -capsule->GetScaledCapsuleHalfHeight()));
	ChillingAuraChildActor->SetAbsolute(false, true);
	ChillingAuraChildActor->SetWorldRotation(FRotator(0, 0, 0));
}

void UChilling::RemoveChillingAura() {
	if (ChillingAuraChildActor.IsValid()) {
		ChillingAuraChildActor->DestroyChildActor();
	}
}

UChillingGameplayEffect::UChillingGameplayEffect(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	GameplayCues.Last().GameplayCueTags.AddTag(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Enchantment.Chilling"));
}