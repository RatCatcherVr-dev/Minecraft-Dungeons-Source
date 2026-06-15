#include "BeamAttackComponent.h"
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
#include "game/component/PlayerExperienceComponent.h"
#include "Dungeons/DungeonsGameInstance.h"
#include "game/util/Pushback.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/abilities/effects/executions/DamageExecutionCalculation.h"
#include "game/abilities/effects/calculations/DamageModCalculations.h"
#include "game/abilities/effects/requirements/BaseGameplayEffectApplicationRequirement.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MobCharacterMovementComponent.h"

extern TAutoConsoleVariable<int32> CVarDebugDrawItems;

namespace {
	FName BeamEffectDamageMagnitude(TEXT("BeamEffectDamageMagnitude"));
}

UBeamAttackComponent::UBeamAttackComponent() {
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.TickInterval = 0.0333333333f;
	damageEffect = UBeamDamageGameplayEffect::StaticClass();
	bReplicates = true;

	animSequences.Add(EBeamAttackPhase::Targeting, nullptr);
	animSequences.Add(EBeamAttackPhase::Damaging, nullptr);
}

void UBeamAttackComponent::BeginPlay() {
	Super::BeginPlay();

	attackRateSeconds = GetFullBeamTime() + delayTime;
	MobCharacterMovementComponent = Cast<UMobCharacterMovementComponent>(GetOwner()->GetComponentByClass(UMobCharacterMovementComponent::StaticClass()));

	owner = Cast<ABaseCharacter>(GetOwner());
	check(owner && "Beam attack component requires BaseCharacter owner.");

	if (owner) {
		if (UAbilitySystemComponent* abilitySystem = owner->GetAbilitySystemComponent()) {
			abilitySystem->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.Stunned"))).AddUObject(this, &UBeamAttackComponent::OnStunned);
		}
	}
}

void UBeamAttackComponent::AttackCpp(AActor* attackTarget, int32 seed /*= 0*/, FSharedPredictionContext predictionContext) {
	// needed as this gets invoked twice
	if (GetWorld()->GetTimeSeconds() - lastAttackTimestampSeconds < GetFullBeamTime() + delayTime) {
		return;
	}

	Super::AttackCpp(attackTarget, seed, predictionContext);

	UHealthComponent* ownerHealthComponent = owner->FindComponentByClass<UHealthComponent>();

	// is this necessary?
	if (ownerHealthComponent && ownerHealthComponent->IsNotAlive()) {
		return;
	}

	if (!owner->HasAuthority()) {
		return;
	}

	currentAttackTarget = attackTarget;
	if (!currentAttackTarget)
		return;

	AttackCounter++;

	sharedPredictionContext = predictionContext;

	if (unifiedBeamEffect && beamEffect && !unifiedBeamEffectHandle.IsValid()) {
		if (UAbilitySystemComponent* abilitySystem = owner->GetAbilitySystemComponent()) {
			if (sharedPredictionContext.GetKey().IsLocalClientKey() || owner->HasAuthority()) {
				auto context = abilitySystem->MakeEffectContext();
				context.AddInstigator(currentAttackTarget, currentAttackTarget);
				unifiedBeamEffectHandle = abilitySystem->ApplyGameplayEffectToSelf(beamEffect.GetDefaultObject(), 1.0f, context, sharedPredictionContext.GetKey());
			}
		}
	}

	SetNewPhase(EBeamAttackPhase::Stopped);

	GetWorld()->GetTimerManager().ClearTimer(attackBeginDelayHandle);
	GetWorld()->GetTimerManager().SetTimer(attackBeginDelayHandle, FTimerDelegate::CreateUObject(this, &UBeamAttackComponent::BeginTargetingBeam), delayTime, false);
}

float UBeamAttackComponent::GetAttackRange() const {
	return beamRange;
}

bool UBeamAttackComponent::IsAttackInProgress() const {
	auto& timerManager = GetOwner()->GetWorld()->GetTimerManager();
	return (attackNextPhaseDelayHandle.IsValid() && timerManager.IsTimerActive(attackNextPhaseDelayHandle)) || (attackBeginDelayHandle.IsValid() && timerManager.IsTimerActive(attackBeginDelayHandle));
}

void UBeamAttackComponent::ChangeBeamMode(bool continuous) {
	continuousDamage = continuous;
}

float UBeamAttackComponent::GetFullBeamTime() {
	float attackTime = 0;
	for (const TPair<EBeamAttackPhase, float>& pair : PhaseBeamTime) {
		attackTime += pair.Value;
	}

	return attackTime;
}

void UBeamAttackComponent::BeginTargetingBeam() {
	if (owner) {
		if (MobCharacterMovementComponent) {
			if (PhaseRotationsSpeed.Contains(EBeamAttackPhase::Targeting)) {
				MobCharacterMovementComponent->SetBlockRotationRateChange(true);
				MobCharacterMovementComponent->RotationRate.Yaw = PhaseRotationsSpeed[EBeamAttackPhase::Targeting];
			}
		}

		if (UAbilitySystemComponent* abilitySystem = owner->GetAbilitySystemComponent()) {
			if (sharedPredictionContext.GetKey().IsLocalClientKey() || owner->HasAuthority()) {
				if (!unifiedBeamEffect) {
					if (beamEffects.Contains(EBeamAttackPhase::Targeting)) {
						TSubclassOf<UGameplayEffect> targetingBeamEffect = beamEffects[EBeamAttackPhase::Targeting];
						if (targetingBeamEffect && !targetingBeamEffectHandle.IsValid()) {
							auto context = abilitySystem->MakeEffectContext();
							context.AddInstigator(currentAttackTarget, currentAttackTarget);
							targetingBeamEffectHandle = abilitySystem->ApplyGameplayEffectToSelf(beamEffect.GetDefaultObject(), 1.0f, context, sharedPredictionContext.GetKey());
						}
					}
				}
				SetComponentTickEnabled(beamPitchPhase == EPitchPhase::Target || beamPitchPhase == EPitchPhase::Both);
			}
			
			float beamTime = PhaseBeamTime.Contains(EBeamAttackPhase::Targeting) ? PhaseBeamTime[EBeamAttackPhase::Targeting] : 0;
			GetWorld()->GetTimerManager().ClearTimer(attackNextPhaseDelayHandle);
			GetWorld()->GetTimerManager().SetTimer(attackNextPhaseDelayHandle, FTimerDelegate::CreateUObject(this, &UBeamAttackComponent::BeginDamagingBeam), beamTime, false);
		}
	}
	SetNewPhase(EBeamAttackPhase::Targeting);
}

void UBeamAttackComponent::BeginDamagingBeam() {
	if (owner) {
		if (MobCharacterMovementComponent) {
			if (PhaseRotationsSpeed.Contains(EBeamAttackPhase::Damaging)) {
				MobCharacterMovementComponent->SetBlockRotationRateChange(true);
 				MobCharacterMovementComponent->RotationRate.Yaw = PhaseRotationsSpeed[EBeamAttackPhase::Damaging];
			}
		}

		if (UAbilitySystemComponent* abilitySystem = owner->GetAbilitySystemComponent()) {
			if (sharedPredictionContext.GetKey().IsLocalClientKey() || owner->HasAuthority()) {
				if (!unifiedBeamEffect) {
					if (targetingBeamEffectHandle.IsValid()) {
						owner->GetAbilitySystemComponent()->RemoveActiveGameplayEffect(targetingBeamEffectHandle);
						targetingBeamEffectHandle.Invalidate();
					}
					if (beamEffects.Contains(EBeamAttackPhase::Damaging)) {
						TSubclassOf<UGameplayEffect> damagingBeamEffect = beamEffects[EBeamAttackPhase::Damaging];
						if (damagingBeamEffect && !damagingBeamEffectHandle.IsValid()) {
							auto context = abilitySystem->MakeEffectContext();
							context.AddInstigator(currentAttackTarget, currentAttackTarget);
							damagingBeamEffectHandle = abilitySystem->ApplyGameplayEffectToSelf(beamEffect.GetDefaultObject(), 1.0f, context, sharedPredictionContext.GetKey());
						}
					}
				}
				SetComponentTickEnabled(continuousDamage || beamPitchPhase == EPitchPhase::Damage || beamPitchPhase == EPitchPhase::Both);
			}

			float beamTime = PhaseBeamTime.Contains(EBeamAttackPhase::Damaging) ? PhaseBeamTime[EBeamAttackPhase::Damaging] : 0;
			GetWorld()->GetTimerManager().ClearTimer(attackNextPhaseDelayHandle);
			GetWorld()->GetTimerManager().SetTimer(attackNextPhaseDelayHandle, FTimerDelegate::CreateUObject(this, &UBeamAttackComponent::EndBeam), beamTime, false);
		}
	}
	SetNewPhase(EBeamAttackPhase::Damaging);
}

void UBeamAttackComponent::EndBeam() {
	SuccessfulAttackCounter++;

	GetWorld()->GetTimerManager().ClearTimer(attackBeginDelayHandle);
	GetWorld()->GetTimerManager().ClearTimer(attackNextPhaseDelayHandle);

	if (owner) {
		if (UAbilitySystemComponent* abilitySystem = owner->GetAbilitySystemComponent()) {
			if (sharedPredictionContext.GetKey().IsLocalClientKey() || owner->HasAuthority()) {
				if (removeEffectOnAttackEnd) {
					if (unifiedBeamEffectHandle.IsValid()) {
						owner->GetAbilitySystemComponent()->RemoveActiveGameplayEffect(unifiedBeamEffectHandle);
						unifiedBeamEffectHandle.Invalidate();
					}
					if (targetingBeamEffectHandle.IsValid()) {
						owner->GetAbilitySystemComponent()->RemoveActiveGameplayEffect(targetingBeamEffectHandle);
						targetingBeamEffectHandle.Invalidate();
					}
					if (damagingBeamEffectHandle.IsValid()) {
						owner->GetAbilitySystemComponent()->RemoveActiveGameplayEffect(damagingBeamEffectHandle);
						damagingBeamEffectHandle.Invalidate();
					}
				}

				SetComponentTickEnabled(false);
				if (!continuousDamage) {
					ApplyDamageToTargetsInBeam(0);
				}
			}
		}
		if (animSequences.Contains(currentPhase)) {
			owner->MulticastStopSlotAnimation(slot);
		}
		if (auto mob = Cast<AMobCharacter>(owner)) {
			mob->MobParams.totalAttacks.all++;
			mob->MobParams.successfulAttacks.all++;
			if (MobCharacterMovementComponent) {
				MobCharacterMovementComponent->SetBlockRotationRateChange(false);
			}
		}
	}

	owner->SetActorRotation(FRotator(0, owner->GetActorRotation().Yaw, owner->GetActorRotation().Roll));
	SetNewPhase(EBeamAttackPhase::Stopped);
}

AActor* UBeamAttackComponent::GetCurrentTargetActor() {
	return currentAttackTarget;
}

void UBeamAttackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (const UHealthComponent* healthComponent = GetOwner()->FindComponentByClass<UHealthComponent>()) {
		if (healthComponent->IsNotAlive()) {
			EndBeam();
			return;
		}
	}

	if (owner && owner->HasAuthority()) {
		ActorPitchRotation();
		if (currentPhase == EBeamAttackPhase::Damaging) {
			ApplyDamageToTargetsInBeam(DeltaTime);
		}
	}
}

TArray<AActor*> UBeamAttackComponent::GetTargetsInBeam() {
	TArray<AActor*> targetsInBeam;
	if (owner) {
		FVector beamOrigin = owner->GetActorLocation();
		if (!originSocketName.IsNone() && owner->GetMesh()) {
			beamOrigin = owner->GetMesh()->GetSocketLocation(originSocketName);
		}
		FVector beamDirection = owner->GetActorForwardVector();
		FVector beamEndLocation = beamDirection * beamRange + beamOrigin;

		if (currentAttackTarget && currentPhase != EBeamAttackPhase::Stopped) {
			if (beamPitchType == EPitchType::Beam && (beamPitchPhase == EPitchPhase::Damage || beamPitchPhase == EPitchPhase::Both)) {
				FVector directionToTarget = currentAttackTarget->GetActorLocation() - beamOrigin;
				directionToTarget.Normalize();
				beamEndLocation.Z = FVector(directionToTarget * beamRange + beamOrigin).Z;
			}
		}

		FCollisionObjectQueryParams ObjectParams;
		ObjectParams.AddObjectTypesToQuery(ECC_WorldStatic);
		if (!piercing) {
			ObjectParams.AddObjectTypesToQuery(ECC_Pawn);
			ObjectParams.AddObjectTypesToQuery(ECC_GameTraceChannel11); // Player Pawn
		}

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(owner);

		if (!friendlyFire) {
			TArray<AMobCharacter*> friendlyMobs;
			UActorQuery::GetFriendlyMobCharactersInRange(this, owner, beamRange, friendlyMobs);
			for (AMobCharacter* friendlyMob : friendlyMobs) {
				QueryParams.AddIgnoredActor(friendlyMob);
			}
		}
		
		float beamLength = 0.0f;
		while (true) {
			FHitResult res;
			if (GetWorld()->LineTraceSingleByObjectType(res, beamOrigin, beamEndLocation, ObjectParams, QueryParams)) {
				if (const ABaseCharacter* hitCharacter = Cast<ABaseCharacter>(res.GetActor())) {
					const UHealthComponent* healthComponent = hitCharacter->GetHealthComponent();
					if (!healthComponent || !healthComponent->ShouldImpactProjectile(FGameplayTag::RequestGameplayTag(TEXT("Damage.Medium")))) {
						QueryParams.AddIgnoredActor(res.GetActor());
						continue;
					}
					if (!piercing) {
						targetsInBeam.Add(res.GetActor());
					}
				}
			}
			beamEndLocation = res.ImpactPoint;
			beamDirection = res.ImpactPoint - beamOrigin;
			beamLength = beamDirection.Size2D();
			beamDirection.Normalize();
			break;
		}

		if (CVarDebugDrawItems.GetValueOnGameThread()) {
			DrawDebugCylinder(GetWorld(), beamOrigin, beamEndLocation, beamRadius, 50, FColor::Green, false, 0.1f);
		}

		if (piercing) {
			actorquery::getActorsInCylinder(GetWorld(), beamOrigin, beamDirection, ABaseCharacter::StaticClass(), beamLength, beamRadius, 0.0f, targetsInBeam);
		}
	}
	return targetsInBeam;
}

void UBeamAttackComponent::ApplyDamage(const ABaseCharacter* target, FGameplayEffectSpec damageSpec) {
	if (owner) {
		if (UAbilitySystemComponent* ownerAbilitySystem = owner->GetAbilitySystemComponent()) {
			effects::StorePushbackInNormal(damageSpec, pushback::getLaunchVector(onKillPushback, *GetOwner(), *target, 1.0f, 0.0f));
			if (UAbilitySystemComponent* targetAbilitySystem = target->GetAbilitySystemComponent()) {
				ownerAbilitySystem->ApplyGameplayEffectSpecToTarget(damageSpec, targetAbilitySystem);
			}
		}
	}
}

void UBeamAttackComponent::ApplyDamageToTargetsInBeam(float DeltaTime) {
	if (owner) {
		TArray<AActor*> targetsInBeam = GetTargetsInBeam();
		if (UAbilitySystemComponent* abilitySystem = owner->GetAbilitySystemComponent()) {

			float healthModification = 0;
			if (continuousDamage) {
				healthModification = -damage * DeltaTime;
			}
			else {
				healthModification = -damage;
			}

			FGameplayEffectSpec damageSpec(damageEffect.GetDefaultObject(), abilitySystem->MakeEffectContext(), 1.f);
			damageSpec.SetSetByCallerMagnitude(effects::HealthName, healthModification);
			if (friendlyFire) {
				damageSpec.DynamicAssetTags.AddTag(damageTag::damageFriends());
			}
			FGameplayEffectContextHandle context = damageSpec.GetEffectContext();
			context.AddInstigator(owner, owner);
			context.AddOrigin(owner->GetActorLocation());

			UEnchantmentComponent* ownerEnchantmentComponent = owner->FindComponentByClass<UEnchantmentComponent>();

			for (AActor* target : targetsInBeam) {
				if (const ABaseCharacter* characterTarget = Cast<ABaseCharacter>(target)) {
					if (owner->CanDamageTarget(characterTarget) && characterTarget->IsTargetable()) {
						if (ownerEnchantmentComponent) {
							ownerEnchantmentComponent->OnBeforeAoeAttackDamage(target, nullptr, sharedPredictionContext);
						}
						ApplyDamage(characterTarget, damageSpec);
						if (ownerEnchantmentComponent) {
							ownerEnchantmentComponent->OnAfterAoeAttackDamage(target, nullptr, sharedPredictionContext);
						}
					}
				}
			}
		}
	}
}

void UBeamAttackComponent::StopAttack() {
	EndBeam();
}

void UBeamAttackComponent::OnStunned(const FGameplayTag tag, const int32 tagCount) {
	StopAttack();
}

void UBeamAttackComponent::SetNewPhase(EBeamAttackPhase newPhase) {
	if(owner && owner->HasAuthority()) {
		if(currentPhase != newPhase) {
			currentPhase = newPhase;
			if (animSequences.Contains(currentPhase)) {
				owner->MulticastPlayAnimationAsDynamicMontage(animSequences[currentPhase], slot, 0, 0.2f, 1.f, 1, 0, 0, sharedPredictionContext.GetKey());
			}
			else {
				owner->MulticastStopSlotAnimation(slot);
			}
			OnRep_PhaseChange();
		}
	}
}

void UBeamAttackComponent::OnRep_PhaseChange() {
	if (owner) {
		OnAttackPhaseChanged.Broadcast(currentPhase);
	}
}

void UBeamAttackComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UBeamAttackComponent, currentPhase)
}

void UBeamAttackComponent::ActorPitchRotation() {
	if (owner) {
		if (currentAttackTarget && beamPitchType == EPitchType::Actor && beamPitchPhase != EPitchPhase::None && currentPhase != EBeamAttackPhase::Stopped) {
			if ((beamPitchPhase == EPitchPhase::Target && currentPhase == EBeamAttackPhase::Targeting) || (beamPitchPhase == EPitchPhase::Damage && currentPhase == EBeamAttackPhase::Damaging) || beamPitchPhase == EPitchPhase::Both) {
				FVector beamOrigin = owner->GetActorLocation();
				if (!originSocketName.IsNone() && owner->GetMesh()) {
					beamOrigin = owner->GetMesh()->GetSocketLocation(originSocketName);
				}
				FVector beamDirection = owner->GetActorForwardVector();
				FVector beamEndLocation = beamDirection * beamRange + beamOrigin;
				FVector directionToTarget = currentAttackTarget->GetActorLocation() - beamOrigin;
				directionToTarget.Normalize();
				beamEndLocation.Z = FVector(directionToTarget * beamRange + beamOrigin).Z;
				owner->SetActorRotation(FRotator(-FRotationMatrix::MakeFromX(owner->GetActorLocation() - beamEndLocation).Rotator().Pitch, owner->GetActorRotation().Yaw, owner->GetActorRotation().Roll));
			}
		}
	}
}

UBeamDamageGameplayEffect::UBeamDamageGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FCustomCalculationBasedFloat healthMagnitude;
	healthMagnitude.CalculationClassMagnitude = UMeleeModDamageCalculation::StaticClass();

	FGameplayModifierInfo healthInfo;
	healthInfo.Attribute = UHealthAttributeSet::HealthAttribute();
	healthInfo.ModifierMagnitude = healthMagnitude;
	healthInfo.ModifierOp = EGameplayModOp::Type::Additive;
	Modifiers.Add(healthInfo);

	FGameplayEffectExecutionDefinition damage;
	damage.CalculationClass = UDamageExecutionCalculation::StaticClass();
	Executions.Add(std::move(damage));

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Damage.Medium")));
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Damage.Aoe"), 0, 1);
	GameplayCues.Last().MagnitudeAttribute = UHealthAttributeSet::HealthAttribute();
}