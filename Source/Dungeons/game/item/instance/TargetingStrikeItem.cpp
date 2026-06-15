// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "TargetingStrikeItem.h"

#include "game/component/SoulComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/item/power/ItemPowerEffectDefs.h"
#include "AbilitySystemComponent.h"
#include <GameplayEffect.h>
#include <Runtime/Engine/Classes/Engine/Engine.h>
#include "game/actor/character/player/BasePlayerController.h"
#include <TimerManager.h>
#include <IConsoleManager.h>
#include "game/abilities/attributes/MovementAttributeSet.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/attributes/ItemAttributeSet.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/component/HealthComponent.h"
#include "util/CharacterQuery.h"
#include "Assets/DungeonsAssetManager.h"
#include "game/input/TargetController.h"

extern TAutoConsoleVariable<int32> CVarDebugDrawItems;

namespace targetingstrikeitem {
	FVector GetReticuleSpawnLocation(ABasePlayerController* controller, AActor* targetActor, float GamepadOffset, APlayerCharacter* playerCharacter) {
		if(targetActor) return targetActor->GetActorLocation();
		
		FHitResult hit;
		FVector Start = playerCharacter->GetActorLocation() + playerCharacter->GetActorForwardVector() * GamepadOffset;
		
		if(controller->GetGamepadActive()) {
			controller->GetWorld()->LineTraceSingleByChannel(hit, Start + FVector(0.f, 0.f, 1000.f), Start - FVector(0, 0, 20000.0f), ECollisionChannel::ECC_WorldStatic);
		} else {
			controller->GetHitResultUnderCursorByChannel(ETraceTypeQuery::TraceTypeQuery11, true, hit);
		}

		return hit.IsValidBlockingHit() ? hit.ImpactPoint : Start;
	}
}


ATargetingStrikeItem::ATargetingStrikeItem() {
	PowerEffects = { UItemArtifactDamageIncrease::StaticClass() };
	bHasManualCooldownActivation = true;
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	bCanBeInterruptedWithUserAction = false;
	bHasManualSoulConsumption = true;
}

float ATargetingStrikeItem::GetStats(EItemStats stat) const {
	switch (stat) {
	case EItemStats::HighestDamage:
	case EItemStats::LowestDamage:		
		return GetItemType().getSoulCost()*StrikeDamagePerSoul;
	}
	return -1;
}

void ATargetingStrikeItem::Toggle(const FPredictionKey& predictionKey, bool Enabled) {
	Super::Toggle(predictionKey, Enabled);
	auto player = GetPlayerOwner();
	auto abilitySystem = player->GetAbilitySystemComponent();
	
	if(Enabled) {
		if (player->IsLocallyControlled()) {
			SpawnTargetingActor();
			GetWorld()->GetTimerManager().SetTimer(StrikeTimerHandle, this, &ATargetingStrikeItem::OnCharged, StrikeIntervalSeconds);
		}

		abilitySystem->AddGameplayCue(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.StatusEffect.TargetingStrikeItem")));
	} else {
		if(player->IsLocallyControlled()) {
			GetWorld()->GetTimerManager().ClearTimer(StrikeTimerHandle);
			if (CurrentTargetingActor != nullptr) {
				CurrentTargetingActor->Destroy();
				CurrentTargetingActor = nullptr;
			}
		}
		abilitySystem->RemoveGameplayCue(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.StatusEffect.TargetingStrikeItem")));
	}
}

void ATargetingStrikeItem::SpawnTargetingActor() {
	auto owner = GetPlayerOwner();
	auto controller = Cast<ABasePlayerController>(owner->GetController());
	auto target = controller->GetCurrentTarget();

	FTransform transform;
	transform.SetLocation(targetingstrikeitem::GetReticuleSpawnLocation(controller, target, DefaultTargetOffset, owner));
	check(TargetingActorClass && "Must have targeting actor class!")
	CurrentTargetingActor = GetWorld()->SpawnActorDeferred<ALightingRodReticule>(TargetingActorClass, transform, owner, owner, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	if(GetGameInstance()->GetNumLocalPlayers() > 1) {
		CurrentTargetingActor->SetColor(owner->GetPlayerColor());
	}

	UGameplayStatics::FinishSpawningActor(CurrentTargetingActor, transform);
	
	if(target) {
		CurrentTargetingActor->AttachToActor(target, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	}
}

void ATargetingStrikeItem::ServerStartStrike_Implementation(FVector strikeLocation) {
	GetWorld()->GetTimerManager().SetTimer(PreStrikeTimerHandle, FTimerDelegate::CreateUObject(this, &ATargetingStrikeItem::Strike, strikeLocation), PreStrikeDelaySeconds, false);    

    FGameplayCueParameters cueParams;
    cueParams.NormalizedMagnitude = 1.0f;

    const auto abilitySystem = GetPlayerOwner()->GetAbilitySystemComponent();

    abilitySystem->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.TargetingStrikeItem.PreStrike"), cueParams);
}
bool ATargetingStrikeItem::ServerStartStrike_Validate(FVector strikeLocation) {
	return true;
}

void ATargetingStrikeItem::Strike(FVector strikeLocation) {
	if (const auto playerController = Cast<ABasePlayerController>(GetPlayerOwner()->GetController())) {
		if (CVarDebugDrawItems.GetValueOnGameThread()) DrawDebugSphere(GetWorld(), strikeLocation, StrikeRadius, 50, FColor::Green, false, 5.0f);
		
		const float soulCost = GetSoulActivationCost();		
		const auto playerOwner = GetPlayerOwner();

		FGameplayCueParameters cueParams;
		cueParams.NormalizedMagnitude = 1.0f;
		cueParams.Location = strikeLocation;
		cueParams.Instigator = playerOwner;

		const auto abilitySystem = playerOwner->GetAbilitySystemComponent();

		abilitySystem->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.TargetingStrikeItem.MainStrike"), cueParams);

		const float ItemPowerMultiplier = GetPowerEffect()->GetMultiplier(ItemPower);

		const float ScaledDamage = StrikeDamagePerSoul * soulCost * ItemPowerMultiplier;
		FGameplayEffectSpec spec = effects::CreateGameplayEffectSpec<UTargetingStrikeDamageItemGameplayEffect>(abilitySystem, ItemPower);
		FDungeonsGameplayEffectContext* context = effects::GetDungeonsContextFromSpec(spec);
		spec.SetSetByCallerMagnitude(effects::HealthName, -ScaledDamage);
		context->AddInstigator(GetOwner(), this);
		context->AddOrigin(GetOwner()->GetActorLocation());
		for (auto charActor : actorquery::getNearbyActors<ABaseCharacter>(GetWorld(), strikeLocation, StrikeRadius)) {
			if (playerOwner->CanDamageTarget(charActor) && charActor->IsTargetable()) {
				abilitySystem->ApplyGameplayEffectSpecToTarget(spec, charActor->GetAbilitySystemComponent());
			}
		}

		GetSoulComponent()->ServerAddSouls(-soulCost);
		InterruptServer();
	}
}	

void ATargetingStrikeItem::OnCharged() {
	if (CurrentTargetingActor != nullptr) {
		CurrentTargetingActor->OnCharged();
		ServerStartStrike(CurrentTargetingActor->GetActorLocation());
	}
}

UTargetingStrikeItemGameplayEffect::UTargetingStrikeItemGameplayEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;

	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	FGameplayModifierInfo speedInfo;
	speedInfo.Attribute = UMovementAttributeSet::SpeedMultiplierAttribute();
	speedInfo.ModifierMagnitude = FScalableFloat(0.f);
	speedInfo.ModifierOp = EGameplayModOp::Override;

	Modifiers.Add(speedInfo);
}

ALightingRodReticule::ALightingRodReticule() {}


UTargetingStrikeDamageItemGameplayEffect::UTargetingStrikeDamageItemGameplayEffect(const FObjectInitializer& ObjectInitializer) {
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Damage.Elemental.Lightning")));
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Damage.Item"), 0, 1);
	GameplayCues.Last().MagnitudeAttribute = UHealthAttributeSet::HealthAttribute();
}
