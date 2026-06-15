#include "UpdraftTome.h"

#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/effects/StaggerGameplayEffect.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/component/HealthComponent.h"
#include "game/item/power/ItemPowerEffectDefs.h"
#include "game/item/ItemBulletPoint.h"
#include "game/util/ActorQuery.h"
#include "util/CharacterQuery.h"
#include "TimerManager.h"

UUpdraftTomeGameplayEffect::UUpdraftTomeGameplayEffect(const FObjectInitializer& ObjectInitializer) : Super() {
	DurationPolicy = EGameplayEffectDurationType::Instant;

	StackLimitCount = 1;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::NeverReset;

	InheritableGameplayEffectTags.AddTag(damageTag::wind());
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Updraft.Player"), 0, 1);
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Damage"), 0, 1);
}

AUpdraftTomeInstance::AUpdraftTomeInstance() {
	PowerEffects = { UItemArtifactDamageIncrease::StaticClass(), UStunDurationIncrease::StaticClass() };
	bHasManualCooldownActivation = true;
	bCanFail = true;
}

void AUpdraftTomeInstance::PopulateBulletPoints(TArray<FItemBulletPoint>& intoList, const ABaseCharacter& owner) const {
	Super::PopulateBulletPoints(intoList, owner);	
	intoList.Add(FItemBulletPoint::CreateTargetsMaxiumumBulletPoint(UpdraftMaxTargets));
}

void AUpdraftTomeInstance::Activate(const FPredictionKey& predictionKey) {
	Super::Activate(predictionKey);

	auto owningCharacter = Cast<APlayerCharacter>(GetOwner());
	auto* owningAbilitySystem = owningCharacter->GetAbilitySystemComponent();
	const auto hostile = characterquery::is::hostile(GetCharacterOwner());
	const auto targetedTag = FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Trigger.Updraft.Target"));
	const auto predicate = [&](const ABaseCharacter* v) { return actorquery::is::alive(v) && characterquery::is::targetable(v) && hostile(v) && (v->GetAbilitySystemComponent() && !v->GetAbilitySystemComponent()->HasMatchingGameplayTag(targetedTag)); };
	auto targets = actorquery::getNearbyInstanceTrackedActors<ABaseCharacter>(owningCharacter, UpdraftRange).FilterByPredicate(predicate);
	targets.Sort([&owningCharacter](const ABaseCharacter& a, const ABaseCharacter& b) -> bool {
		return actorquery::getActorDistance2D(owningCharacter, &a) < actorquery::getActorDistance2D(owningCharacter, &b);
	});

	auto TargetSpec = effects::CreateGameplayEffectSpecFromSubClass(owningAbilitySystem, UUpdraftTomeTargetGameplayEffect::StaticClass(), 1);
	TargetSpec.SetSetByCallerMagnitude(effects::DurationName, UpdraftEffectDelay);

	int affectedEnemyCount = 0;
	for (auto currentTarget : targets) {
		if (affectedEnemyCount >= UpdraftMaxTargets) {
			break;
		}

		auto targetHandle = owningAbilitySystem->ApplyGameplayEffectSpecToTarget(TargetSpec, currentTarget->GetAbilitySystemComponent(), predictionKey);
		if (targetHandle.WasSuccessfullyApplied())
		{
			AttackTargets.Add(currentTarget);
			affectedEnemyCount++;
		}
	}
	

	if(affectedEnemyCount>0){
		GetWorld()->GetTimerManager().ClearTimer(AttackDelayHandle);
		GetWorld()->GetTimerManager().SetTimer(AttackDelayHandle, FTimerDelegate::CreateUObject(this, &AUpdraftTomeInstance::ApplyUpdraft, predictionKey), UpdraftEffectDelay, false);
	} else {
		Cooldown().TriggerCooldown(CalculateCooldown()*0.1f, predictionKey);
		FGameplayCueParameters params;
		params.Location = owningCharacter->GetActorLocation();
		params.Instigator = owningCharacter;
		params.RawMagnitude = UpdraftRange;	
		owningAbilitySystem->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Trigger.Updraft.Missed")), params);
	}
}

bool AUpdraftTomeInstance::IsBusy() const {
	return Super::IsBusy() || AttackDelayHandle.IsValid();
}

void AUpdraftTomeInstance::ApplyUpdraft(const FPredictionKey predictionKey) {
	auto owningCharacter = Cast<APlayerCharacter>(GetOwner());
	auto* owningAbilitySystem = owningCharacter->GetAbilitySystemComponent();

	FPushback pushback;
	pushback.enablePushback = true;
	pushback.pushbackStrength = UpdraftLaunchHeight;

	auto damageSpec = effects::CreateGameplayEffectSpecFromSubClass(owningAbilitySystem, UpdraftTomeEffect, 1);
	damageSpec.SetSetByCallerMagnitude(effects::HealthName, -UpdraftDamage * GetPowerEffect()->GetMultiplier(ItemPower));

	auto stunSpec = effects::CreateGameplayEffectSpec<UStaggerGameplayEffect>(owningAbilitySystem, 1);
	stunSpec.SetSetByCallerMagnitude(effects::DurationName, UpdraftStunDuration);

	auto immuneSpec = effects::CreateGameplayEffectSpecFromSubClass(owningAbilitySystem, UUpdraftTomeImmunityGameplayEffect::StaticClass(), 1);
	immuneSpec.SetSetByCallerMagnitude(effects::DurationName, UpdraftApplyImmunityDuration);

	FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::Normal);

	int windCallerCount = 0;
	int affectedEnemyCount = 0;	
	for (TWeakObjectPtr<ABaseCharacter> currentTarget : AttackTargets.FilterByPredicate([=](const TWeakObjectPtr<ABaseCharacter> weakCharPtr) { return weakCharPtr.IsValid() && weakCharPtr->IsAlive(); })) {
		if (affectedEnemyCount >= UpdraftMaxTargets) {
			break;
		}

		if (auto targetAbilitySystem = currentTarget->GetAbilitySystemComponent()) {
			effects::StorePushbackInNormal(damageSpec, pushback::getLaunchVector(pushback, FVector::UpVector, *currentTarget, 1.0f));
			auto damageHandle = owningAbilitySystem->ApplyGameplayEffectSpecToTarget(damageSpec, targetAbilitySystem, predictionKey);

			if (damageHandle.WasSuccessfullyApplied()) {
				affectedEnemyCount++;
				if (owningCharacter->HasAuthority()) {
					pushback::pushback(pushback, FVector::UpVector, *currentTarget);
				}

				currentTarget->SetLastInjuredBy(owningCharacter);

				// Apply stun and immunity effects
				owningAbilitySystem->ApplyGameplayEffectSpecToTarget(stunSpec, targetAbilitySystem, predictionKey);
				owningAbilitySystem->ApplyGameplayEffectSpecToTarget(immuneSpec, targetAbilitySystem, predictionKey);

				if (auto mobTarget = Cast<AMobCharacter>(currentTarget.Get())) {
					if (mobTarget->EntityType == EntityType::WindCaller) {
						windCallerCount++;
					}
				}
			}

		}
	}
	AttackTargets.Reset();
	AttackDelayHandle.Invalidate();

	if (UStatTrackerComponent* tracker = owningCharacter->GetStatTracker()) {
		tracker->UpdraftWindCaller(windCallerCount);
	}
	Cooldown().TriggerCooldown(CalculateCooldown(), predictionKey);
	ActivationSucceeded(predictionKey);
}

float AUpdraftTomeInstance::GetStats(EItemStats stat) const {
	switch (stat) {	
	case EItemStats::LowestDamage:
	case EItemStats::HighestDamage:
		return UpdraftDamage;
	case EItemStats::StunDuration:
		return UpdraftStunDuration;
	default:
		break;
	}

	return -1;
}

UUpdraftTomeTargetGameplayEffect::UUpdraftTomeTargetGameplayEffect(const FObjectInitializer& ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	FSetByCallerFloat durationMagnitude;
	durationMagnitude.DataName = effects::DurationName;
	DurationMagnitude = durationMagnitude;

	StackLimitCount = 1;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::NeverReset;

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Updraft.Target"), 0, 1);
}

UUpdraftTomeImmunityGameplayEffect::UUpdraftTomeImmunityGameplayEffect(const FObjectInitializer& ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	FSetByCallerFloat durationMagnitude;
	durationMagnitude.DataName = effects::DurationName;
	DurationMagnitude = durationMagnitude;

	StackLimitCount = 1;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::NeverReset;
	
	GrantedApplicationImmunityTags.RequireTags.AddTag(damageTag::wind());
}
