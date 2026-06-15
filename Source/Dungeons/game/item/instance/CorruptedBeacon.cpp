// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "CorruptedBeacon.h"
#include "game/component/SoulComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/item/power/ItemPowerStats.h"
#include "game/item/power/ItemPowerEffectDefs.h"
#include "AbilitySystemComponent.h"
#include <GameplayEffect.h>
#include <Runtime/Engine/Classes/Engine/Engine.h>
#include "game/actor/character/player/BasePlayerController.h"
#include "game/abilities/attributes/MovementAttributeSet.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/attributes/ItemAttributeSet.h"
#include "Assets/DungeonsAssetManager.h"
#include "game/component/PlayerExperienceComponent.h"
#include "Dungeons/DungeonsGameInstance.h"
#include "game/util/Pushback.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"

extern TAutoConsoleVariable<int32> CVarDebugDrawItems;

ACorruptedBeacon::ACorruptedBeacon() {
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	PrimaryActorTick.TickInterval = 0.1f;
	bCanBeInterruptedWithUserAction = true;
	Effect = UCorruptedBeaconGameplayEffect::StaticClass();	
	PowerEffects = { UItemArtifactDamagePerSecondIncrease::StaticClass() };
	bHasManualSoulConsumption = true;
}


float ACorruptedBeacon::GetStats(EItemStats stat) const {
	switch (stat) {	
	case EItemStats::DamagePerSecond:
		return DamagePerSoulPerTick / PrimaryActorTick.TickInterval * static_cast<float>(GetItemType().getSoulCost()) ;
	}
	return -1;
}

void ACorruptedBeacon::Toggle(const FPredictionKey& predictionKey, bool Enabled) {
	Super::Toggle(predictionKey, Enabled);

	auto* playerOwner = GetPlayerOwner();
	const auto abilitySystem = playerOwner->GetAbilitySystemComponent();
	const bool hasAuthority = HasAuthority();
	
	if(Enabled) {
		//Apply spec if we are authorative or predictive
		if(predictionKey.IsLocalClientKey() || hasAuthority) {
			FGameplayEffectSpec spec(Cast<UCorruptedBeaconGameplayEffect>(Effect->GetDefaultObject()), abilitySystem->MakeEffectContext(), 1.0f);
			abilitySystem->ApplyGameplayEffectSpecToSelf(spec, predictionKey);
		}
		playerOwner->SetAttackState(EAttackState::Magic);
	} else {
		if(hasAuthority) {
			FGameplayEffectQuery Query;
			Query.EffectDefinition = Effect;
			abilitySystem->RemoveActiveEffects(Query);
		}
		playerOwner->SetAttackState(EAttackState::None);
	}

	if(hasAuthority) {
		SetActorTickEnabled(Enabled);
	}
	
	if(playerOwner->IsLocallyControlled()) {
		auto controller = Cast<ABasePlayerController>(playerOwner->GetPlayerController());
		controller->ToggleTargetingState(Enabled);
	}
	
}

void ACorruptedBeacon::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	
	if (HasAuthority()) {
		if (auto* playerOwner = GetPlayerOwner()) {

			const auto abilitySystem = playerOwner->GetAbilitySystemComponent();
			const auto beamLocation = playerOwner->GetActorLocation();
			const auto beamDirection = playerOwner->GetActorForwardVector();
			const auto beamOffset = 0.0f;
			const auto beamMaxLength = 5000.0f;
			const auto beamRadius = 200.0f;

			const auto offsetedBeamStartLocation = beamLocation + (beamDirection * beamOffset);
			auto offsetedBeamEndLocation = offsetedBeamStartLocation + (beamDirection * beamMaxLength);

			auto beamLength = beamMaxLength;

			FHitResult res;
			if (GetWorld()->LineTraceSingleByChannel(res, offsetedBeamStartLocation, offsetedBeamEndLocation, (ECollisionChannel)ECustomTraceChannels::TerrainOnly)) {
				FVector startToImpactDelta = res.ImpactPoint - beamLocation;
				beamLength = startToImpactDelta.Size();
				offsetedBeamEndLocation = offsetedBeamStartLocation + (beamDirection * beamLength);
			}

			if (CVarDebugDrawItems.GetValueOnGameThread()) DrawDebugCylinder(GetWorld(), offsetedBeamStartLocation, offsetedBeamEndLocation, beamRadius, 50, FColor::Green, false, 0.1f);

			TArray<AActor*> charsInBeam;
			actorquery::getActorsInCylinder(GetWorld(), beamLocation, beamDirection, ABaseCharacter::StaticClass(), beamLength, beamRadius, beamOffset, charsInBeam);

			const float SoulsToConsume = GetItemType().getSoulCost();

			const float ItemPowerMultiplier = GetPowerEffect()->GetMultiplier(ItemPower);
			const float ScaledDamage = (DamagePerSoulPerTick * SoulsToConsume) * ItemPowerMultiplier;
			const float healthModification = -ScaledDamage;
			FGameplayEffectSpec spec = effects::CreateGameplayEffectSpec<UCorruptedBeaconDamageGameplayEffect>(abilitySystem, effects::HealthName, healthModification, playerOwner, this, playerOwner->GetActorLocation(), 1.f);
			FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::Normal);
			
			for (const auto inBeamCharacter : charsInBeam) {
				effects::StorePushbackInNormal(spec, pushback::getLaunchVector(OnKillPushback, *GetOwner(), *inBeamCharacter, 1.0f, 0.0f));

				auto charActor = Cast<ABaseCharacter>(inBeamCharacter);
				if (playerOwner->CanDamageTarget(charActor) && charActor->IsTargetable()) {
					auto targetAbilitySystem = charActor->GetAbilitySystemComponent();
					abilitySystem->ApplyGameplayEffectSpecToTarget(spec, targetAbilitySystem);
				}
			}
			
			GetSoulComponent()->ServerAddSouls(-SoulsToConsume);
			if (GetSoulComponent()->GetSoulCount() <= 0) {
				InterruptServer();
			}
		}
	}
}

UCorruptedBeaconGameplayEffect::UCorruptedBeaconGameplayEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;

	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.StatusEffect.CorruptedBeacon")), 0, 1);
}

UCorruptedBeaconDamageGameplayEffect::UCorruptedBeaconDamageGameplayEffect()
{
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Damage.Medium")));
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Damage.Elemental.Soul")));
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Damage.Item"), 0, 1);
}
