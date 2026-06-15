#include "Dungeons.h"
#include "VoidTouched.h"
#include "AbilitySystemComponent.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "util/CharacterQuery.h"
#include <GameplayEffect.h>
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/effects/requirements/BaseGameplayEffectApplicationRequirement.h"
#include "world/entity/MobTags.h"
#include "game/abilities/ui/DungeonsGameplayEffectUIData.h"
#include "Internationalization.h"
#include "SoftObjectPtr.h"
#include "game/GameTypes.h"

UVoidTouched::UVoidTouched() {
	MultiplierFormatter = valueformat::asPercentage;
}

FText UVoidTouched::CreateDescription() const {
	return FText::Format(Super::CreateDescription(), FText::FromString(valueformat::asForRoundedWordSecond(LevelMultiplier(Level) * 100.0f)));
}

void UVoidTouched::AfterDealtDamage(AActor* toWhat) {
	const TWeakObjectPtr<ABaseCharacter> target = Cast<ABaseCharacter>(toWhat);
	const AMobCharacter* mob = Cast<AMobCharacter>(toWhat);
	const bool isEnderMob = mob && hasMobTag(mob->EntityType, MobTags::HashTag_Ender);
	if (!target.IsValid() || !characterquery::is::targetable(target.Get()) || !actorquery::is::alive(target.Get()) || isEnderMob) return;

	if (UWorld* world = GetWorld()) {
		world->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([this, target]() {
			if (target.IsValid()) {
				FGameplayEffectQuery query;
				query.EffectDefinition = UVoidLiquidGameplayEffect::StaticClass();
				auto effects = target->GetAbilitySystemComponent()->GetActiveEffects(query);
				if (effects.Num() == 0) {
					UAbilitySystemComponent* abilitySystem = GetCharacterOwner()->GetAbilitySystemComponent();
					FGameplayEffectSpec effectSpec(Cast<UVoidStrikeGameplayEffect>(UVoidStrikeGameplayEffect::StaticClass()->GetDefaultObject()), abilitySystem->MakeEffectContext(), LevelMultiplier(Level));
					effectSpec.SetSetByCallerMagnitude(FName("Duration"), maxTime);
					effectSpec.SetSetByCallerMagnitude(FName("Magnitude"), baseMagnitude);
					FActiveGameplayEffectHandle GEHandle = abilitySystem->ApplyGameplayEffectSpecToTarget(effectSpec, target->GetAbilitySystemComponent());
					if (GEHandle.WasSuccessfullyApplied()) {
						FGameplayEffectSpec effectMagnitudeExtenderSpec(Cast<UVoidStrikeMagnitudeExtenderGameplayEffect>(UVoidStrikeMagnitudeExtenderGameplayEffect::StaticClass()->GetDefaultObject()), abilitySystem->MakeEffectContext(), LevelMultiplier(Level));
						effectMagnitudeExtenderSpec.SetSetByCallerMagnitude(FName("Duration"), timeToMax);
						abilitySystem->ApplyGameplayEffectSpecToTarget(effectMagnitudeExtenderSpec, target->GetAbilitySystemComponent());
						if (GetOwnerRole() == ROLE_Authority) BroadcastEnchantmentTriggeredEvent();
					}
				}
			}
		}));
	}
}

UVoidTouchedMelee::UVoidTouchedMelee() {
	TypeId = EEnchantmentTypeID::VoidTouchedMelee;
	PredictiveExecution = true;
	LevelMultiplier = [this](int level) -> float { return level * 2; };
}

void UVoidTouchedMelee::OnAfterDealtMeleeDamage(float Damage, bool missedAttack, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream, FSharedPredictionContext window) {
	AfterDealtDamage(toWhat);
}

UVoidTouchedRanged::UVoidTouchedRanged() {
	TypeId = EEnchantmentTypeID::VoidTouchedRanged;
	PredictiveExecution = true;
}

void UVoidTouchedRanged::OnAfterDealtRangedDamage(float Damage, ABaseProjectile* fromProjectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream) {
	AfterDealtDamage(toWhat);
}

UVoidStrikeMagnitudeExtenderGameplayEffect::UVoidStrikeMagnitudeExtenderGameplayEffect() : UVoidLiquidMagnitudeExtenderGameplayEffect()
{
	magnitudeIncreasePeriod = 0.1f;

	Period = magnitudeIncreasePeriod;

	Executions.Reset();
	FGameplayEffectExecutionDefinition voidStrikeMagnitudeExtenderExecution;
	voidStrikeMagnitudeExtenderExecution.CalculationClass = UVoidStrikeMagnitudeExtenderExecution::StaticClass();
	Executions.Add(std::move(voidStrikeMagnitudeExtenderExecution));

	InheritableGameplayEffectTags.CombinedTags.Reset();
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.VoidStrike"));

	RoutineExpirationEffectClasses.Add(UVoidStrikePinnacleTimeGameplayEffect::StaticClass());
}

UVoidStrikeMagnitudeExtenderExecution::UVoidStrikeMagnitudeExtenderExecution() 
{
	effectToApply = UVoidStrikeGameplayEffect::StaticClass();
	exponetialIncrease = 1.0f;
	periodIncrease = 0.025f;
	maxMagnitude = 2.0f;
}

UVoidStrikeGameplayEffect::UVoidStrikeGameplayEffect() : UVoidLiquidGameplayEffect()
{
	GameplayCues.Reset();
	InheritableGameplayEffectTags.CombinedTags.Reset();
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.StatusEffect.VoidStrike")), 0, 1);
	GameplayCues.Last().MagnitudeAttribute = UHealthAttributeSet::TakeDamageMultiplierAttribute();
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.VoidStrike"));
}

UVoidStrikeImmunityGameplayEffect::UVoidStrikeImmunityGameplayEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;
	StackingType = EGameplayEffectStackingType::None;

	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("Immunity.VoidStrike"));
	GrantedApplicationImmunityTags.RequireTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.VoidStrike"));
}

UVoidStrikePinnacleTimeGameplayEffect::UVoidStrikePinnacleTimeGameplayEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.StatusEffect.VoidStrike.Pinnacle")), 0, 1);
}