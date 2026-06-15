#include "Dungeons.h"
#include "Barrier.h"
#include <AbilitySystemComponent.h>
#include <AbilitySystemGlobals.h>
#include <GameplayEffect.h>
#include <GameplayEffectExtension.h>
#include "game/actor/character/mob/MobCharacter.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "util/CharacterQuery.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"


UBarrier::UBarrier() {
	TypeId = EEnchantmentTypeID::Barrier;

	LevelMultiplier = [this](int level) -> float {
		return 1.f / (1.f - 0.03f * level);
	};
	MultiplierFormatter = valueformat::asRelativeMultiplierPercentageChange;
}

void UBarrier::OnStart() {
	const auto owner = GetOwner();

	if (owner->HasAuthority()) {
		radiusSphere = NewObject<USphereComponent>(owner);
		FString name(TEXT("BarrierSphere"));
		radiusSphere->AppendName(name);
		radiusSphere->RegisterComponent();
		radiusSphere->AttachTo(owner->GetRootComponent());
		radiusSphere->SetSphereRadius(AffectionRadius);
		radiusSphere->SetCollisionObjectType(ECC_WorldDynamic);
		radiusSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
		radiusSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECR_Overlap);
		radiusSphere->SetCollisionResponseToChannel(static_cast<ECollisionChannel>(ECustomTraceChannels::PlayerPawn), ECR_Overlap);
		radiusSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

		radiusSphere->OnComponentBeginOverlap.AddDynamic(this, &UBarrier::OnRadiusEnter);
		radiusSphere->OnComponentEndOverlap.AddDynamic(this, &UBarrier::OnRadiusLeave);
	}
}

void UBarrier::OnEnd() {
	const auto owner = GetOwner();

	if (owner->HasAuthority() && radiusSphere) {
		radiusSphere->OnComponentBeginOverlap.RemoveDynamic(this, &UBarrier::OnRadiusEnter);
		radiusSphere->OnComponentEndOverlap.RemoveDynamic(this, &UBarrier::OnRadiusLeave);

		radiusSphere->DestroyComponent();
		radiusSphere = nullptr;
	}
}

void UBarrier::OnRadiusEnter(UPrimitiveComponent* ThisComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	const auto target = Cast<ABaseCharacter>(OtherActor);
	const auto characterOwer = GetCharacterOwner();
	if (target && !target->IsFriendlyTowards(characterOwer) && characterquery::is::targetable(target) && !SeenTargets.Contains(target)) {
		if (SeenTargets.Num() < effects::DefaultObject<UBarrierGameplayEffect>()->StackLimitCount) {
			FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::NormalizedMagnitude);
			auto abilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
			abilitySystem->ApplyGameplayEffectToSelf(effects::DefaultObject<UBarrierGameplayEffect>(), Level, abilitySystem->MakeEffectContext());
		}
		SeenTargets.Emplace(target);
	}
}

void UBarrier::OnRadiusLeave(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	const auto target = Cast<ABaseCharacter>(OtherActor);
	if (target && SeenTargets.Contains(target)) {
		SeenTargets.Remove(target);
		if (SeenTargets.Num() < effects::DefaultObject<UBarrierGameplayEffect>()->StackLimitCount) {
			auto abilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
			FGameplayEffectQuery query;
			query.EffectDefinition = UBarrierGameplayEffect::StaticClass();
			abilitySystem->RemoveActiveEffects(query, 1);
		}
	}
}

float UBarrierModLevelCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const {
	return 1.f/(1.f - 0.03f * Spec.GetLevel());
}

UBarrierGameplayEffect::UBarrierGameplayEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	StackLimitCount = 10;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;

	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;
	//bSuppressStackingCues = true;

	FCustomCalculationBasedFloat calculation;
	calculation.CalculationClassMagnitude = UBarrierModLevelCalculation::StaticClass();

	FGameplayModifierInfo modifier;
	modifier.Attribute = UHealthAttributeSet::TakeDamageMultiplierAttribute();
	modifier.ModifierOp = EGameplayModOp::Division;
	modifier.ModifierMagnitude = calculation;
	Modifiers.Emplace(std::move(modifier));
	
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.StatusEffect.Enchantment.Barrier")), 1, 3);
	GameplayCues.Last().MagnitudeAttribute = UHealthAttributeSet::TakeDamageMultiplierAttribute();
}