#include "GuardianEyeInstance.h"
#include "Dungeons.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/component/HealthComponent.h"
#include "game/actor/TargetingActor.h"
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
#include "Dungeons/DungeonsGameInstance.h"
#include "game/util/Pushback.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/abilities/effects/requirements/BaseGameplayEffectApplicationRequirement.h"

extern TAutoConsoleVariable<int32> CVarDebugDrawItems;

AGuardianEyeInstance::AGuardianEyeInstance() {
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	PrimaryActorTick.TickInterval = 0.0333333333f;
	PowerEffects = { UItemArtifactDamagePerSecondIncrease::StaticClass() };
	laserEffect = UGuardianEyeGameplayEffect::StaticClass();
	chargeEffect = UGuardianEyeChargeGameplayEffect::StaticClass();
}

float AGuardianEyeInstance::GetStats(EItemStats stat) const {
	switch (stat) {
	case EItemStats::DamagePerSecond:
		return baseDamage;
	}
	return -1;
}

void AGuardianEyeInstance::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);
	bIsActive = false;
	InterruptLocal();
}

void AGuardianEyeInstance::Activate(const FPredictionKey& predictionKey) {
	Super::Activate(predictionKey);
	bIsActive = true;
	BeginCharge(predictionKey);
}

void AGuardianEyeInstance::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	if (HasAuthority()) {
		ApplyDamageToTargetsInBeam();
	}
}

void AGuardianEyeInstance::BeginCharge(const FPredictionKey predictionKey) {
	if (APlayerCharacter* playerOwner = GetPlayerOwner()) {
		if (UAbilitySystemComponent* abilitySystem = playerOwner->GetAbilitySystemComponent()) {
			if (HasAuthority()) {
				FGameplayEffectSpec spec(Cast<UGuardianEyeChargeGameplayEffect>(chargeEffect->GetDefaultObject()), abilitySystem->MakeEffectContext(), 1.0f);
				abilitySystem->ApplyGameplayEffectSpecToSelf(spec, predictionKey);
			}
		}

		if (playerOwner->IsLocallyControlled()) {
			if (ABasePlayerController* controller = Cast<ABasePlayerController>(playerOwner->GetPlayerController())) {
				controller->ToggleTargetingState(true);
				controller->ToggleStateChangeAllowed(false);
			}
			playerOwner->CancelActions.AddUObject(this, &AGuardianEyeInstance::InterruptLocal);
		}

		playerOwner->SetAttackState(EAttackState::Magic);
		playerOwner->GetMovementComponent()->StopMovementImmediately();

		GetWorld()->GetTimerManager().ClearTimer(attackDelayHandle);
		GetWorld()->GetTimerManager().SetTimer(attackDelayHandle, FTimerDelegate::CreateUObject(this, &AGuardianEyeInstance::BeginBeam, predictionKey), aimingTime, false);
	}
}

void AGuardianEyeInstance::BeginBeam(const FPredictionKey predictionKey) {
	if (APlayerCharacter* playerOwner = GetPlayerOwner()) {
		if (UAbilitySystemComponent* abilitySystem = playerOwner->GetAbilitySystemComponent()) {
			if (HasAuthority()) {
				FGameplayEffectQuery Query;
				Query.EffectDefinition = chargeEffect;
				abilitySystem->RemoveActiveEffects(Query);
				FGameplayEffectSpec spec(Cast<UGuardianEyeGameplayEffect>(laserEffect->GetDefaultObject()), abilitySystem->MakeEffectContext(), 1.0f);
				abilitySystem->ApplyGameplayEffectSpecToSelf(spec, predictionKey);
				SetActorTickEnabled(true);
			}

			GetWorld()->GetTimerManager().ClearTimer(attackEndDelayHandle);
			GetWorld()->GetTimerManager().SetTimer(attackEndDelayHandle, FTimerDelegate::CreateUObject(this, &AGuardianEyeInstance::EndBeam, predictionKey), laserTime, false);
		}
	}
}

void AGuardianEyeInstance::EndBeam(const FPredictionKey predictionKey) {
	allowInterrupt = true;
	Interrupt();
}

void AGuardianEyeInstance::Interrupt() {
	if (allowInterrupt) {
		GetWorld()->GetTimerManager().ClearTimer(attackDelayHandle);
		GetWorld()->GetTimerManager().ClearTimer(attackEndDelayHandle);

		if (APlayerCharacter* playerOwner = GetPlayerOwner()) {
			if (UAbilitySystemComponent* abilitySystem = playerOwner->GetAbilitySystemComponent()) {
				if (HasAuthority()) {
					FGameplayEffectQuery Query;
					Query.EffectDefinition = laserEffect;
					abilitySystem->RemoveActiveEffects(Query);
					Query.EffectDefinition = chargeEffect;
					abilitySystem->RemoveActiveEffects(Query);
					SetActorTickEnabled(false);
				}

				if (playerOwner->IsLocallyControlled()) {
					playerOwner->CancelActions.RemoveAll(this);
					if (ABasePlayerController* controller = Cast<ABasePlayerController>(playerOwner->GetPlayerController())) {
						controller->ToggleStateChangeAllowed(true);
						controller->ToggleTargetingState(false);
					}
				}

				playerOwner->SetAttackState(EAttackState::None);
			}

			if (UAnimMontage* montage = Cast<UAnimMontage>(ActivationAnimation)) {
				playerOwner->StopMontage(0.2f, montage);
			}
			else if (ActivationAnimation) {
				playerOwner->StopAnimation(0.2f, ActivationAnimation, ActivationAnimationSlot);
			}
		}

		bIsActive = false;
	}
}

void AGuardianEyeInstance::InterruptLocal_Implementation() {
	if (bIsActive) {
		Interrupt();
		if (!HasAuthority()) {
			InterruptServer();
		}
	}
}

bool AGuardianEyeInstance::InterruptLocal_Validate() {
	return true;
}

void AGuardianEyeInstance::InterruptServer_Implementation() {
	if (bIsActive) {
		Interrupt();
		if (!GetPlayerOwner()->IsLocallyControlled()) {
			InterruptLocal();
		}
	}
}

bool AGuardianEyeInstance::InterruptServer_Validate() {
	return true;
}

TArray<AActor*> AGuardianEyeInstance::GetTargetsInBeam() {
	TArray<AActor*> targetsInBeam;
	if (ABaseCharacter* owner = Cast<ABaseCharacter>(GetOwner())) {
		UAbilitySystemComponent* abilitySystem = owner->GetAbilitySystemComponent();
		const FVector beamLocation = owner->GetActorLocation();
		const FVector beamDirection = owner->GetActorForwardVector();
		const float beamOffset = 0.0f;
		const float beamMaxLength = 5000.0f;
		const float beamRadius = 100.0f;

		const FVector offsetBeamStartLocation = beamLocation + (beamDirection * beamOffset);
		FVector offsetBeamEndLocation = offsetBeamStartLocation + (beamDirection * beamMaxLength);
		float beamLength = beamMaxLength;

		FHitResult res;
		FCollisionObjectQueryParams params;
		params.AddObjectTypesToQuery(ECC_WorldStatic);

		if (GetWorld()->LineTraceSingleByObjectType(res, offsetBeamStartLocation, offsetBeamEndLocation, params)) {
			FVector startToImpactDelta = res.ImpactPoint - beamLocation;
			beamLength = startToImpactDelta.Size();
			offsetBeamEndLocation = offsetBeamStartLocation + (beamDirection * beamLength);
		}

		if (CVarDebugDrawItems.GetValueOnGameThread()) {
			DrawDebugCylinder(GetWorld(), offsetBeamStartLocation, offsetBeamEndLocation, beamRadius, 50, FColor::Green, false, 0.1f);
		}

		actorquery::getActorsInCylinder(GetWorld(), beamLocation, beamDirection, ABaseCharacter::StaticClass(), beamLength, beamRadius, beamOffset, targetsInBeam);
	}
	return targetsInBeam;
}

void AGuardianEyeInstance::ApplyDamage(const ABaseCharacter* target, FGameplayEffectSpec damageSpec) {
	if (ABaseCharacter* owner = Cast<ABaseCharacter>(GetOwner())) {
		if (UAbilitySystemComponent* ownerAbilitySystem = owner->GetAbilitySystemComponent()) {
			effects::StorePushbackInNormal(damageSpec, pushback::getLaunchVector(onKillPushback, *GetOwner(), *target, 1.0f, 0.0f));
			if (UAbilitySystemComponent* targetAbilitySystem = target->GetAbilitySystemComponent()) {
				ownerAbilitySystem->ApplyGameplayEffectSpecToTarget(damageSpec, targetAbilitySystem);
			}
		}
	}
}

void AGuardianEyeInstance::ApplyDamageToTargetsInBeam() {
	if (ABaseCharacter* owner = Cast<ABaseCharacter>(GetOwner())) {
		TArray<AActor*> targetsInBeam = GetTargetsInBeam();
		if (UAbilitySystemComponent* abilitySystem = owner->GetAbilitySystemComponent()) {

			const float ItemPowerMultiplier = GetPowerEffect()->GetMultiplier(ItemPower);
			const float ScaledDamage = -baseDamage * PrimaryActorTick.TickInterval * ItemPowerMultiplier;
			FGameplayEffectSpec damageSpec = effects::CreateGameplayEffectSpec<UGuardianEyeDamageGameplayEffect>(abilitySystem, effects::HealthName, ScaledDamage, owner, this, owner->GetActorLocation(), 1.f);

			for (AActor* target : targetsInBeam) {
				if (const ABaseCharacter* characterTarget = Cast<ABaseCharacter>(target)) {
					if (owner->CanDamageTarget(characterTarget) && characterTarget->IsTargetable()) {
						ApplyDamage(characterTarget, damageSpec);
					}
				}
			}
		}
	}
}

UGuardianEyeGameplayEffect::UGuardianEyeGameplayEffect(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;

	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	ApplicationRequirements.Emplace(UBaseGameplayEffectApplicationRequirement::StaticClass());

	FScalableFloat speedMagnitude;
	speedMagnitude.SetValue(0.0f);

	FGameplayModifierInfo speedModifier;
	speedModifier.ModifierOp = EGameplayModOp::Override;
	speedModifier.ModifierMagnitude = speedMagnitude;
	speedModifier.Attribute = UMovementAttributeSet::SpeedMultiplierAttribute();
	Modifiers.Add(speedModifier);

	FScalableFloat rotationMagnitude;
	rotationMagnitude.SetValue(0.02f);

	FGameplayModifierInfo rotationModifier;
	rotationModifier.ModifierOp = EGameplayModOp::Override;
	rotationModifier.ModifierMagnitude = rotationMagnitude;
	rotationModifier.Attribute = UMovementAttributeSet::RotationMultiplierAttribute();
	Modifiers.Add(rotationModifier);

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.GuardianEye")));
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.StatusEffect.GuardianEyeBeam")), 0, 1);
}

UGuardianEyeDamageGameplayEffect::UGuardianEyeDamageGameplayEffect()
{
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Damage.Medium")));
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Damage.Item"), 0, 1);
}

UGuardianEyeChargeGameplayEffect::UGuardianEyeChargeGameplayEffect(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;

	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	ApplicationRequirements.Emplace(UBaseGameplayEffectApplicationRequirement::StaticClass());

	FScalableFloat speedMagnitude;
	speedMagnitude.SetValue(0.0f);

	FGameplayModifierInfo speedModifier;
	speedModifier.ModifierOp = EGameplayModOp::Override;
	speedModifier.ModifierMagnitude = speedMagnitude;
	speedModifier.Attribute = UMovementAttributeSet::SpeedMultiplierAttribute();
	Modifiers.Add(speedModifier);

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.GuardianEye")));
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.StatusEffect.GuardianEyeCharge")), 0, 1);
}