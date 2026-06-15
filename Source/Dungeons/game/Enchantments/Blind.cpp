#include "Dungeons.h"
#include "Blind.h"
#include <AbilitySystemComponent.h>
#include <AbilitySystemGlobals.h>
#include <GameplayEffect.h>
#include <GameplayEffectExtension.h>
#include "game/actor/character/mob/MobCharacter.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "util/CharacterQuery.h"
#include "game/abilities/attributes/MeleeAttributeSet.h"
#include "game/abilities/attributes/RangedAttributeSet.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/effects/executions/DamageExecutionCalculation.h"
#include "game/item/power/ItemPowerStats.h"
#include "game/abilities/effects/calculations/StatusModCalculations.h"
#include "world/entity/MobTags.h"


UBlindGameplayEffect::UBlindGameplayEffect(const FObjectInitializer& ObjectInitializer)
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

	FGameplayModifierInfo meleeHitChanceInfo;
	meleeHitChanceInfo.Attribute = UMeleeAttributeSet::MeleeAttackHitChanceMultiplierAttribute();
	FSetByCallerFloat meleeHitChanceMagnitude;
	meleeHitChanceMagnitude.DataName = FName("MeleeHitChance");
	meleeHitChanceInfo.ModifierMagnitude = meleeHitChanceMagnitude;
	meleeHitChanceInfo.ModifierOp = EGameplayModOp::Type::Division;
	Modifiers.Add(meleeHitChanceInfo);

	FGameplayModifierInfo rangedHitChanceInfo;
	rangedHitChanceInfo.Attribute = URangedAttributeSet::RangedAttackHitChanceMultiplierAttribute();
	FSetByCallerFloat rangedHitChanceMagnitude;
	rangedHitChanceMagnitude.DataName = FName("RangedHitChance");
	rangedHitChanceInfo.ModifierMagnitude = rangedHitChanceMagnitude;
	rangedHitChanceInfo.ModifierOp = EGameplayModOp::Type::Division;
	Modifiers.Add(rangedHitChanceInfo);

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Negative"));
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.StatusEffect.Blind")), 0, 1);
}

float UBlindCloudSpawnHelper::MeleeHitChance = 0.5f;
float UBlindCloudSpawnHelper::RangeHitChance = 0.5f;

ABlindCloud* UBlindCloudSpawnHelper::SpawnBlindCloud(TSubclassOf<class AActor> blindCloudClass, TSubclassOf<class UGameplayEffect> gameplayEffect, AActor* owner, const FVector& location, float duration, bool squid)
{
	FTransform transform;
	transform.SetLocation(location);

	auto world = owner->GetWorld();
	auto blindCloud = world->SpawnActorDeferred<ABlindCloud>(blindCloudClass, transform, owner, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	blindCloud->InitialLifeSpan = duration;
	auto spec = CreateBlindCloudEffectSpec(gameplayEffect, owner, location,  blindCloud, duration);
	blindCloud->SetGameplaySpec(std::move(spec));
	blindCloud->Squid = squid;
	blindCloud->CloudDuration = duration;

	blindCloud->FinishSpawning(transform);

	return blindCloud;
}

FGameplayEffectSpec UBlindCloudSpawnHelper::CreateBlindCloudEffectSpec(TSubclassOf<UGameplayEffect> gameplayEffect, AActor* owner, const FVector& location, ABlindCloud* blindCloud, float duration)
{
	const auto abilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(owner);

	auto spec = FGameplayEffectSpec(gameplayEffect->GetDefaultObject<UGameplayEffect>(), abilitySystem->MakeEffectContext(), 1);

	spec.GetContext().AddInstigator(owner, owner);
	spec.GetContext().AddOrigin(location);

	spec.SetSetByCallerMagnitude(effects::DurationName, duration);
	spec.SetSetByCallerMagnitude(FName("MeleeHitChance"), 1.f / MeleeHitChance);
	spec.SetSetByCallerMagnitude(FName("RangedHitChance"), 1.f / RangeHitChance);

	return spec;
}

ABlindCloud::ABlindCloud()
{
	bReplicates = true;
	InitialLifeSpan = 2.5f;

	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(static_cast<ECollisionChannel>(ECustomTraceChannels::PlayerPawn), ECollisionResponse::ECR_Overlap);
}

void ABlindCloud::BeginPlay()
{
	Super::BeginPlay();
}

void ABlindCloud::NotifyActorBeginOverlap(AActor* OtherActor)
{
	if (HasAuthority()) {
		ABaseCharacter* target = Cast<ABaseCharacter>(OtherActor);
		ABaseCharacter* owner = GetCharacterOwner();
		if (target && owner && actorquery::is::alive(target) && characterquery::is::targetable(target)
			&& !hasMobTag( target, MobTags::HashTag_Miniboss ) 
			&& !hasMobTag( target, MobTags::HashTag_EventMob ) 
			&& !owner->IsFriendlyTowards(target)) {
			FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);

			UAbilitySystemComponent* abilitySystem = target->GetAbilitySystemComponent();
			if (abilitySystem)
			{
				abilitySystem->ApplyGameplayEffectSpecToSelf(Spec);
			}

			if (APlayerCharacter* playerCharacter = Cast<APlayerCharacter>(OtherActor))
			{
				if (!PlayersInside.Contains(playerCharacter))
				{
					OnPlayerCharacterEnteredNonFriendlyCloud(playerCharacter);
					PlayersInside.AddUnique(playerCharacter);
				}
			}
		}
	}
}


void ABlindCloud::SetGameplaySpec(FGameplayEffectSpec&& spec)
{
	Spec = spec;
}

class ABaseCharacter* ABlindCloud::GetCharacterOwner() const
{
	return Cast<ABaseCharacter>(GetOwner());
}

void ABlindCloud::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);

	if (APlayerCharacter* playerCharacter = Cast<APlayerCharacter>(OtherActor))
	{
		OnPlayerCharacterLeftNonFriendlyCloud(playerCharacter);
		PlayersInside.Remove(playerCharacter);
	}
}
