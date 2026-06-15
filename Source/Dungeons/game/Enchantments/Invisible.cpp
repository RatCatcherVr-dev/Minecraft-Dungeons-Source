#include "Dungeons.h"
#include "Invisible.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/abilities/attributes/HealthAttributeSet.h"

#include <AbilitySystemComponent.h>

UInvisibilityGameplayEffect::UInvisibilityGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::Infinite;
	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = false;

	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.Invisible.Affector")));
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.Untargetable")));
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.StatusEffect.Invisible")), 0, 1);
}

const FName UInvisibilityFadeGameplayEffect::DurationKey(TEXT("Duration"));

UInvisibilityFadeGameplayEffect::UInvisibilityFadeGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::HasDuration;

	FSetByCallerFloat durationMagnitude;
	durationMagnitude.DataName = DurationKey;

	DurationMagnitude = durationMagnitude;
	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = false;

	RoutineExpirationEffectClasses.Add(UInvisibilityGameplayEffect::StaticClass());

	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.Fading")));
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.Fading")));
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.StatusEffect.Fade")), 0, 1);
}


UInvisible::UInvisible() {
	TypeId = EEnchantmentTypeID::Invisible;
}

void UInvisible::BeginPlay() {
	Super::BeginPlay();
	GetAbilitySystemComponent()->RegisterAndCallGameplayTagEvent(FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.Stunned")), FOnGameplayEffectTagCountChanged::FDelegate::CreateUObject(this, &UInvisible::OnStunChanged));
	OnInvisibilityRemoved();
}

void UInvisible::OnBeforeMeleeAttack(AActor* attackTarget, FVector attackVector, int32 index, FRandomStream&, FSharedPredictionContext) {
	GetCharacterOwner()->RemoveInvisibility();
}

void UInvisible::OnBeforeRangedAttack(AActor * attackTarget, bool& attackDenied, FPredictionKey) {
	GetCharacterOwner()->RemoveInvisibility();
}

void UInvisible::OnBeforeAoeAttack(AActor* attackTarget, FRandomStream&, FSharedPredictionContext) {
	GetCharacterOwner()->RemoveInvisibility();
}

void UInvisible::OnInvisibilityRemoved() {
	auto* abilitySystem = GetAbilitySystemComponent();

	if (!abilitySystem->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.Stunned"))) && GetCharacterOwner()->IsAlive()) {
		auto spec = effects::CreateGameplayEffectSpec<UInvisibilityFadeGameplayEffect>(abilitySystem, Level);
		spec.SetSetByCallerMagnitude(UInvisibilityFadeGameplayEffect::DurationKey, FadeDurationSeconds);
		abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
	}
}

void UInvisible::OnStunChanged(const FGameplayTag, const int32 tagCount) {
	if (tagCount <= 0) {
		OnInvisibilityRemoved();
	}
	else {
		RemoveAllEffects();
	}
}

void UInvisible::OnAfterReceivedDamage(const struct FGameplayEffectModCallbackData&, FRandomStream&) {
	GetCharacterOwner()->RemoveInvisibility();
}

void UInvisible::RemoveAllEffects() const {
	FGameplayTagContainer tags;
	tags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.Invisible.Affector")));
	tags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.Fading")));
	GetAbilitySystemComponent()->RemoveActiveEffects(FGameplayEffectQuery::MakeQuery_MatchAnyEffectTags(tags));
}

void UInvisible::OnEnd() {
	RemoveAllEffects();
}

bool invisible::isAllowedMob(const ABaseCharacter* character) {
	const auto* mob = Cast<AMobCharacter>(character);
	if (mob == nullptr) {
		return false;
	}
	if (!mob->IsHostileTowardsPlayers()) {
		return false;
	}

	if (mob->IsRequiredForObjective()) {
		return false;
	}

	static const auto Disallowed = {
		EntityType::PiggyBank,
		EntityType::GeomancerWall, EntityType::QuickGrowingVine, EntityType::QuickGrowingVineSimple,
		EntityType::SheepFireRed, EntityType::SheepPoisonGreen, EntityType::SheepSpeedBlue
	};

	return !algo::contains(Disallowed, mob->EntityType);
}
