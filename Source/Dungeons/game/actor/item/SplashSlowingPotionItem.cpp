#include "Dungeons.h"
#include "SplashSlowingPotionItem.h"

#include "game/util/ActorQuery.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include <GameplayEffect.h>
#include <AbilitySystemComponent.h>
#include "game/abilities/attributes/MovementAttributeSet.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"

extern TAutoConsoleVariable<int32> CVarDebugDrawItems;

ASplashSlowingPotion::ASplashSlowingPotion(const class FObjectInitializer& OI) : Super(OI) {
	Effect = USPlashSlowingPotionGameplayEffect::StaticClass();
}

void ASplashSlowingPotion::BeginPlay() {
	Super::BeginPlay();
}

void ASplashSlowingPotion::OnHitObject(class UPrimitiveComponent* ThisComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	Super::OnHitObject(ThisComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	if (Role == ROLE_SimulatedProxy)
		return;
	if (OtherComp->GetCollisionObjectType() != ECC_WorldStatic)
		return;
	if (OtherActor->Role == ROLE_SimulatedProxy) return;

	if (CVarDebugDrawItems.GetValueOnGameThread()) DrawDebugSphere(GetWorld(), GetActorLocation(), 400, 50, FColor::Green, false, 5.0f);
	auto owner = GetOwner();
	auto abilitySystem = owner->FindComponentByClass<UAbilitySystemComponent>();
	check(abilitySystem && "Thrower of potion must have ability system.");

	FGameplayEffectSpec spec(Cast<USPlashSlowingPotionGameplayEffect>(Effect->GetDefaultObject()), abilitySystem->MakeEffectContext(), 1);
	spec.SetSetByCallerMagnitude(FName(TEXT("Duration")), game::item::type::SplashSlowingPotion.getDurationSeconds());
	FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
	
	for (auto mob : actorquery::getNearbyActors<AMobCharacter>(this, 400)) {
		abilitySystem->ApplyGameplayEffectSpecToTarget(spec, mob->GetAbilitySystemComponent());
	}

	Destroy();
}


USPlashSlowingPotionGameplayEffect::USPlashSlowingPotionGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	FSetByCallerFloat durationMagnitude;
	durationMagnitude.DataName = FName(TEXT("Duration"));

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	DurationMagnitude = durationMagnitude;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;
	
	FScalableFloat magnitude = 0.5f;

	FGameplayModifierInfo speedInfo;
	speedInfo.Attribute = UMovementAttributeSet::SpeedMultiplierAttribute();
	speedInfo.ModifierOp = EGameplayModOp::Type::Multiplicitive;
	speedInfo.ModifierMagnitude = magnitude;

	Modifiers.Add(speedInfo);

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.StatusEffect.Slow")), 0, 1);
}