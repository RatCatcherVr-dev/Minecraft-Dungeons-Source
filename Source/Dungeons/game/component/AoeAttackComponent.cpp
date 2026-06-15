#include "Dungeons.h"
#include "AoeAttackComponent.h"

#include "game/component/HealthComponent.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include <AbilitySystemComponent.h>
#include <DrawDebugHelpers.h>
#include "game/abilities/effects/executions/DamageExecutionCalculation.h"
#include "game/abilities/effects/calculations/DamageModCalculations.h"
#include "Assets/DungeonsAssetManager.h"
#include "PlayerExperienceComponent.h"
#include "DungeonsGameInstance.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/ArmorProperties/ArmorPropertiesComponent.h"
#include "game/util/DungeonsGearUtilLibrary.h"

TAutoConsoleVariable<int32> CVarDebugDrawAoeAttack(
	TEXT("Dungeons.DebugDraw.AoeAttack"),
	0,
	TEXT("Enables debug drawing of melee attacks.\n")
	TEXT("<=0: off.\n")
	TEXT(">0: on.\n"),
	ECVF_Cheat);


UAoeAttackComponent::UAoeAttackComponent(const FObjectInitializer& ObjectInitializer) {
	DamageEffect = UAoeAttackDamageGameplayEffect::StaticClass();
}

void UAoeAttackComponent::BeginPlay() {
	Super::BeginPlay();
}

void UAoeAttackComponent::ApplyDamage(FSharedPredictionContext predictionContext) {

	if (TargetProvider == nullptr)
	{
		return;
	}

	auto attacker = Cast<ABaseCharacter>(GetOwner());
	UAbilitySystemComponent* abilitySystem = attacker->GetAbilitySystemComponent();

	const auto attackerHealthComponent = attacker->FindComponentByClass<UHealthComponent>();
	if (attackerHealthComponent && attackerHealthComponent->IsNotAlive()) {
		CompleteAttack();
		return;
	}	

	TArray<AActor*> targetDamageActors;
	FVector attackLocation = TargetProvider->RequestTargetForComponent(this, targetDamageActors);

	// Dead characters shouldn't be affected
	targetDamageActors.RemoveAll([=](const AActor* targetActor) { return targetActor && !Cast<ABaseCharacter>(targetActor)->IsAlive(); });

	FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::Normal);
	for (auto actor : targetDamageActors) {		
		if (auto healthComponent = actor->FindComponentByClass<UHealthComponent>()) {
			ABaseCharacter * damageTakerCharacter = Cast<ABaseCharacter, AActor>(actor);

			DungeonsGearUtilLibrary::OnBeforeAoeAttackDamage(attacker, damageTakerCharacter, nullptr, predictionContext);

			const auto targetAbilitySystem = damageTakerCharacter->GetAbilitySystemComponent();
			FGameplayEffectSpec spec(DamageEffect.GetDefaultObject(), abilitySystem->MakeEffectContext(), 1.f);
			spec.SetSetByCallerMagnitude(effects::HealthName, -attackDamage);
			auto context = spec.GetEffectContext();
			context.AddInstigator(attacker, attacker);
			context.AddOrigin(attackLocation);
			spec.DynamicAssetTags.AddTag(DamageType);
			
			if (!ignoreFriendlyFire) {
				spec.DynamicAssetTags.AddTag(damageTag::damageFriends());
			}

			if (abilitySystem->ApplyGameplayEffectSpecToTarget(spec, targetAbilitySystem).WasSuccessfullyApplied()) {
				const auto pushbackMultiplier = healthComponent->GetDamageMultiplier(spec.DynamicAssetTags);
				pushback::pushback(pushback, *attacker, *actor, pushbackMultiplier);
			}

			DungeonsGearUtilLibrary::OnAfterAoeAttackDamage(attacker, damageTakerCharacter, nullptr, predictionContext);
		}		
	}
	

	if (GetOwnerRole() == ROLE_Authority) {
		attacker->RemoveInvisibility();
	}

	if (EffectCue.IsValid()) {
		
		check(abilitySystem);
		FGameplayCueParameters params;
		params.Location = GetAttackLocation();
		abilitySystem->ExecuteGameplayCue(EffectCue, params);
	}

	OnAoe.Broadcast();
}

// rewrite
void UAoeAttackComponent::CompleteAttack() {
	SuccessfulAttackCounter++;

	auto attacker = Cast<ABaseCharacter>(GetOwner());
	if (auto mob = Cast<AMobCharacter>(attacker)) {
		mob->MobParams.totalAttacks.all++;
		mob->MobParams.successfulAttacks.all++;
	}

	if (AttackingEffectHandle.IsValid()) {
		attacker->GetAbilitySystemComponent()->RemoveActiveGameplayEffect(AttackingEffectHandle);
		AttackingEffectHandle.Invalidate();
	}
}

void UAoeAttackComponent::AttackCpp(AActor* attackTarget, int32 seed /*= 0*/, FSharedPredictionContext predictionContext) {
	// needed as this gets invoked twice
	if (GetWorld()->GetTimeSeconds() - lastAttackTimestampSeconds < totalTime) {
		return;
	}

	auto owner = Cast<ABaseCharacter>(GetOwner());
	check(owner && "Aoe attack component requires BaseCharacter owner.");

	Super::AttackCpp(attackTarget, seed, predictionContext);

	const auto attacker = Cast<ABaseCharacter>(GetOwner());
	const auto attackerHealthComponent = attacker->FindComponentByClass<UHealthComponent>();

	// is this necessary?
	if (attackerHealthComponent && attackerHealthComponent->IsNotAlive()) {
		return;
	}

	if (!attacker->HasAuthority()) {
		return;
	}
	
	DungeonsGearUtilLibrary::OnBeforeAoeAttack(attacker, attackTarget, nullptr, predictionContext);

	AttackCounter++;

	
	if (attacker->HasAuthority()) {
		attacker->MulticastPlayAnimationAsDynamicMontage(AoeSequence, Slot, 0, 0.2f, 1.f, 1, 0, 0, predictionContext.GetKey());
	} else {
		attacker->PlayAnimationAsDynamicMontage(AoeSequence, Slot);
	}

	if (AttackingEffect && !AttackingEffectHandle.IsValid()) {
		auto* abilitySystem = owner->GetAbilitySystemComponent();
		AttackingEffectHandle = abilitySystem->ApplyGameplayEffectToSelf(AttackingEffect.GetDefaultObject(), 1.f, abilitySystem->MakeEffectContext(), predictionContext.GetKey());
	}

	// move this timer logic to behaviors
	auto world = GetWorld();
	world->GetTimerManager().ClearTimer(delayTimerHandle);
	world->GetTimerManager().SetTimer(delayTimerHandle, FTimerDelegate::CreateUObject(this, &UAoeAttackComponent::ApplyDamage, predictionContext), damageDelay, false);

	world->GetTimerManager().ClearTimer(completeTimerHandle);
	world->GetTimerManager().SetTimer(completeTimerHandle, FTimerDelegate::CreateUObject(this, &UAoeAttackComponent::CompleteAttack ), totalTime, false);
}

void UAoeAttackComponent::StopAttack() {
	auto world = GetWorld();
	world->GetTimerManager().ClearTimer(delayTimerHandle);
	world->GetTimerManager().ClearTimer(completeTimerHandle);
	ABaseCharacter* owner = Cast<ABaseCharacter>(GetOwner());

	if (AoeSequence) {
		owner->MulticastStopSlotAnimation(Slot);
	}

	if (AttackingEffectHandle.IsValid()) {
		owner->GetAbilitySystemComponent()->RemoveActiveGameplayEffect(AttackingEffectHandle);
		AttackingEffectHandle.Invalidate();
	}
}

FVector UAoeAttackComponent::GetOwnerAttackSocketLocation() const
{
	if (ABaseCharacter* Character = Cast<ABaseCharacter>(GetOwner()))
	{
		return Character->GetMesh()->GetSocketTransform(AttackSocketName).GetLocation();
	}
	return GetOwner()->GetActorLocation();
}

FVector UAoeAttackComponent::GetOwnerAttackSocketRotation() const
{
	if (ABaseCharacter* Character = Cast<ABaseCharacter>(GetOwner()))
	{
		return Character->GetMesh()->GetSocketTransform(AttackSocketName).GetRotation().GetForwardVector();
	}
	return GetOwner()->GetActorForwardVector();
}

bool UAoeAttackComponent::IsAttackInProgress() const
{
	auto& timerManager = GetOwner()->GetWorld()->GetTimerManager();
	return delayTimerHandle.IsValid() && timerManager.IsTimerActive(delayTimerHandle);
}

UAoeAttackDamageGameplayEffect::UAoeAttackDamageGameplayEffect()
{
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
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Damage.Aoe"), 0, 1);
	GameplayCues.Last().MagnitudeAttribute = UHealthAttributeSet::HealthAttribute();
}
FVector UAoeTargetProvider::RequestTargetForComponent_Implementation(const UAoeAttackComponent*, TArray<AActor*>& outTargets) const {
	return FVector::ZeroVector;
}

float UAoeTargetProvider::GetAttackRange_Implementation() const {
	return 0.f;
}

FVector USphereInfrontAoeTargetProvider::RequestTargetForComponent_Implementation(const UAoeAttackComponent * component, TArray<AActor*>& outTargets) const
{
	AActor* owner = component->GetOwner();
	const TArray<TEnumAsByte<EObjectTypeQuery>> collisionObjectTypes{ UEngineTypes::ConvertToObjectType(ECC_Pawn), UEngineTypes::ConvertToObjectType(static_cast<ECollisionChannel>(ECustomTraceChannels::PlayerPawn)) };
	const TArray<AActor*> actorsToIgnore(&owner, 1);
	
	FVector attackLocation = component->GetAttackLocation() + (component->GetAttackRotation() * attackOffset) + (owner->GetActorRightVector() * attackOffsetY);

	UKismetSystemLibrary::SphereOverlapActors(
		component->GetWorld(),
		attackLocation,
		attackRadius,
		collisionObjectTypes,
		ABaseCharacter::StaticClass(),
		actorsToIgnore,
		outTargets
	);

	if (CVarDebugDrawAoeAttack.GetValueOnGameThread()) {
		DrawDebugSphere(component->GetWorld(), attackLocation, attackRadius, 32, FColor::Red, false, 5.f);
	}

	return attackLocation;
}


float USphereInfrontAoeTargetProvider::GetAttackRange_Implementation() const {
	return attackRadius;
}

FVector UCubeInfrontAoeTargetProvider::RequestTargetForComponent_Implementation(const UAoeAttackComponent* component, TArray<AActor*>& outTargets) const
{
	AActor* owner = component->GetOwner();
	const TArray<TEnumAsByte<EObjectTypeQuery>> collisionObjectTypes{ UEngineTypes::ConvertToObjectType(ECC_Pawn), UEngineTypes::ConvertToObjectType(static_cast<ECollisionChannel>(ECustomTraceChannels::PlayerPawn)) };
	const TArray<AActor*> actorsToIgnore(&owner, 1);

	FVector attackLocation = owner->GetActorLocation() + owner->GetActorForwardVector() * attackLength;

	TArray<FHitResult> HitResults;
	UKismetSystemLibrary::BoxTraceMultiForObjects(
		component->GetWorld(),
		owner->GetActorLocation(),
		attackLocation,
		FVector(attackWidth / 2, attackWidth / 2, attackWidth / 2),
		owner->GetActorRotation(),
		collisionObjectTypes,
		false,
		actorsToIgnore,
		EDrawDebugTrace::None,
		HitResults,
		true
	);

	for (FHitResult Hit : HitResults)
	{
		outTargets.AddUnique(Hit.Actor.Get());
	}

	return attackLocation;
}

float UCubeInfrontAoeTargetProvider::GetAttackRange_Implementation() const
{
	return attackLength;
}
