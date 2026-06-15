#include "Dungeons.h"
#include "WildRage.h"
#include "game/abilities/effects/requirements/BaseGameplayEffectApplicationRequirement.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "world/entity/MobTags.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/component/KillTrackerComponent.h"

namespace {
	FName WildRageEffectDuration(TEXT("WildRageEffectDuration"));
}

UWildRage::UWildRage() {
	TypeId = EEnchantmentTypeID::WildRage;
	EffectClass = UWildRageGameplayEffect::StaticClass();

	LevelMultiplier = [this](int level) -> float {
		return 0.1 + (0.1f * level);
	};
	MultiplierFormatter = valueformat::asPercentageChance;
}

void UWildRage::OnAfterDealtRangedDamage(float Damage, ABaseProjectile* fromProjectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream) {
	if (GetOwner()->HasAuthority()) {
		if (randStream.FRandRange(0.0f, 1.0f) < LevelMultiplier(Level)) {
			if (auto mobCharacter = Cast<AMobCharacter>(toWhat)) {
				if (CanRage(mobCharacter)) {
					FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
					
					BroadcastEnchantmentTriggeredEvent();

					auto ownerAbilitySystem = GetCharacterOwner()->GetAbilitySystemComponent();
					auto targetAbilitySystem = mobCharacter->GetAbilitySystemComponent();

					auto& gameplayTagEventDelegate = targetAbilitySystem->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag("StatusEffect.Rage"));
					if (!gameplayTagEventDelegate.IsBoundToObject(this)) {
						gameplayTagEventDelegate.AddUObject(this, &UWildRage::OnRage, TWeakObjectPtr<AMobCharacter>(mobCharacter));
					}

					auto spec = effects::CreateGameplayEffectSpecFromSubClass(ownerAbilitySystem, EffectClass, Level);
					spec.SetSetByCallerMagnitude(::WildRageEffectDuration, Duration);
					ownerAbilitySystem->ApplyGameplayEffectSpecToTarget(spec, targetAbilitySystem);
				}
			}
		}
	}	
}

bool UWildRage::CanRage(AMobCharacter* mob) const {
	return !hasMobTag(mob, MobTags::HashTag_Miniboss) &&
		   !hasMobTag(mob, MobTags::HashTag_Special);
}

void UWildRage::OnRage(const FGameplayTag tag, const int32 tagCount, TWeakObjectPtr<AMobCharacter> mob) {
	if (mob.IsValid()) {
		const auto mobAbilitySystem = mob->GetAbilitySystemComponent();
		if (mobAbilitySystem->HasMatchingGameplayTag(tag)) {
			mob->ChangeTeam(ETeamName::Loner);
			mob->OnKilledOther.AddDynamic(this, &UWildRage::HandleMobKillMob);
		} else {
			mob->ChangeTeam(ETeamName::Villains);
			mob->OnKilledOther.RemoveDynamic(this, &UWildRage::HandleMobKillMob);
		}
	}
}

void UWildRage::HandleMobKillMob(AActor* effectCauser, AActor* mob, AActor* mobKilled) {
	auto playerOwner = Cast<APlayerCharacter>(GetOwner());
	if (playerOwner->IsA<APlayerCharacter>()) {
		APlayerCharacter *player = Cast<APlayerCharacter>(playerOwner);
		if (UKillTrackerComponent* tracker = Cast<UKillTrackerComponent>(player->GetComponentByClass(UKillTrackerComponent::StaticClass()))) {
			tracker->HandleMobKill(mob, player, mobKilled);
		}
	}
}

UWildRageGameplayEffect::UWildRageGameplayEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	FSetByCallerFloat durationMagnitude;
	durationMagnitude.DataName = ::WildRageEffectDuration;
	DurationMagnitude = durationMagnitude;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	ApplicationRequirements.Emplace(UBaseGameplayEffectApplicationRequirement::StaticClass());

	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Rage"));

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Enchantment.WildRage"), 0, 1);
}