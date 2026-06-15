#include "Dungeons.h"
#include "MeleeAttackComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "game/util/ActorQuery.h"
#include "util/CharacterQuery.h"
#include "game/util/DungeonsDrawDebugHelpers.h"
#include "game/component/HealthComponent.h"
#include "game/item/instance/GearItemInstance.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "AbilitySystemComponent.h"
#include "DungeonsGameMode.h"
#include "game/item/instance/MeleeWeaponGearItemInstance.h"
#include "EquipmentComponent.h"
#include <GameplayTagContainer.h>
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/attributes/MeleeAttributeSet.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/abilities/effects/executions/LifestealExecution.h"
#include "game/abilities/effects/executions/DamageExecutionCalculation.h"
#include "game/abilities/effects/calculations/DamageModCalculations.h"
#include "game/util/DungeonsGearUtilLibrary.h"

#include <limits>

TAutoConsoleVariable<int32> CVarDebugDrawMeleeAttack(
	TEXT("Dungeons.DebugDraw.MeleeAttack"),
	0,
	TEXT("Enables debug drawing of melee attacks.\n")
	TEXT("<=0: off.\n")
	TEXT(">0: on.\n"),
	ECVF_Cheat);

UMeleeDamageGameplayEffect::UMeleeDamageGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FCustomCalculationBasedFloat healthMagnitude;
	healthMagnitude.CalculationClassMagnitude = UMeleeModDamageCalculation::StaticClass();
	
	FGameplayModifierInfo healthInfo;
	healthInfo.Attribute = UHealthAttributeSet::HealthAttribute();
	healthInfo.ModifierMagnitude = healthMagnitude;
	healthInfo.ModifierOp = EGameplayModOp::Type::Additive;
	Modifiers.Add(healthInfo);

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Damage"), 0, 1);
	GameplayCues.Last().MagnitudeAttribute = UHealthAttributeSet::HealthAttribute();


	FGameplayEffectExecutionDefinition damage;
	damage.CalculationClass = UDamageExecutionCalculation::StaticClass();
	Executions.Add(std::move(damage));
	FGameplayEffectExecutionDefinition lifesteal;
	lifesteal.CalculationClass = UMeleeLifestealExecution::StaticClass();
	Executions.Add(std::move(lifesteal));
}

MeleeAttackParametersCapture::MeleeAttackParametersCapture(const UAbilitySystemComponent* component)
	: AttackSpeedMultiplier(component->GetNumericAttribute(UMeleeAttributeSet::MeleeAttackSpeedMultiplierAttribute()))
	, AngleMultiplier(component->GetNumericAttribute(UMeleeAttributeSet::MeleeAttackAngleMultiplierAttribute()))
	, RangeMultiplier(component->GetNumericAttribute(UMeleeAttributeSet::MeleeAttackRangeMultiplierAttribute()))
	, PushbackMultiplier(component->GetNumericAttribute(UMeleeAttributeSet::MeleeAttackPushbackMultiplierAttribute()))
{
}

UMeleeAttackComponent::UMeleeAttackComponent() {
}

void UMeleeAttackComponent::BeginPlay() {
	Super::BeginPlay();

	SetAttackVariants(ConfiguredAttackVariants);
	ActiveAttackVariantResetSeconds = AttackVariantResetSeconds;

	if (auto* equipmentComponent = GetOwner()->FindComponentByClass<UEquipmentComponent>()) {
		auto slots = equipmentComponent->GetSlotsOfType(ESlotType::MeleeWeapon);

		// Shouldn't be done here, item stash equips items before this happens, which means OnWeaponSlotUpdated won't be called in time on initialization 
		if (slots.Num()) {
			OnWeaponSlotUpdated(slots[0]); 
			slots[0]->OnItemInstanceReplicatedInternal.AddUObject(this, &UMeleeAttackComponent::OnWeaponSlotUpdated);
		}
		OnWeaponSlotUpdated(slots[0]);
	}

	if(auto character = Cast<ABaseCharacter>(GetOwner())) {
		character->CancelActions.AddUObject(this, &UMeleeAttackComponent::CancelActions);
		character->OnDeath.AddUObject(this, &UMeleeAttackComponent::OnDeath);
	}
}

const FMeleeAttackComponentAttackVariant& UMeleeAttackComponent::GetCurrentAttackVariant() const {
	return ActiveAttackVariants[Math::clamp(currentIndex, 0, ActiveAttackVariants.Num() - 1)];
}

int32 UMeleeAttackComponent::GetAttackVariantIndex() const {
	return currentIndex;
}

const TArray<FMeleeAttackComponentAttackVariant>& UMeleeAttackComponent::GetActiveAttackVariants() const {
	return ActiveAttackVariants;
}

const TArray<FMeleeAttackComponentAttackVariant>& UMeleeAttackComponent::GetConfiguredAttackVariants() const {
	return ConfiguredAttackVariants;
}

float UMeleeAttackComponent::GetAttackRange() const {
	const auto owner = Cast<ABaseCharacter>(GetOwner());
	check(owner && "Melee attack component requires BaseCharacter owner.");
	
	const auto abilitySystem = owner->GetAbilitySystemComponent();
	check(abilitySystem && "Owning actor must have dungeons ability system component.");
	
	const auto& variant = ActiveAttackVariants[currentIndex];
	return variant.AttackRange + variant.ConeOffsetUnits + mRangeBoost;
}

float UMeleeAttackComponent::GetAttackActivationRange() const
{
	const auto owner = Cast<ABaseCharacter>(GetOwner());
	check(owner && "Melee attack component requires BaseCharacter owner.");

	const auto abilitySystem = owner->GetAbilitySystemComponent();
	check(abilitySystem && "Owning actor must have dungeons ability system component.");

	const auto& variant = ActiveAttackVariants[currentIndex];
	return variant.AttackRange * variant.AttackActivationRangeMultiplier + variant.ConeOffsetUnits + mRangeBoost;

}

float UMeleeAttackComponent::GetMinAttackRange() const {
	auto min = FLT_MAX;

	for(auto index = 0; index < ActiveAttackVariants.Num(); ++index) {
		const auto& param = ActiveAttackVariants[index];
		min = std::min(min, param.AttackRange + param.ConeOffsetUnits);
	}

	return min;
}

void UMeleeAttackComponent::ResetAttackVariantTimer() {
	SetAttackVariantIndex(0);
	RemoveVariantEffects();
}

void UMeleeAttackComponent::SetAttackVariantIndex(int32 index) {
	currentIndex = Math::clamp(index, 0, ActiveAttackVariants.Num() - 1);
}

void UMeleeAttackComponent::SetHoldingAttack(bool holding) {
	if (holding != bHoldingAttack) {
		if (!GetOwner()->HasAuthority()) {
			ServerSetHoldingAttack(holding);
		}
		bHoldingAttack = holding;
	}
}

void UMeleeAttackComponent::ServerSetHoldingAttack_Implementation(bool holding) {
	SetHoldingAttack(holding);
}
bool UMeleeAttackComponent::ServerSetHoldingAttack_Validate(bool holding) {
	return true;
}

FPredictionKey UMeleeAttackComponent::AttackLocal(AActor* attackTarget) {
	if (!CanAttack(attackTarget)) {
		return FPredictionKey();
	}

	const auto owner = GetOwner();

	auto* abilitySystem = owner->FindComponentByClass<UAbilitySystemComponent>();
	check(abilitySystem && "Attack Component owned by character without AbilitySystemComponent.");

	// call back ui state here.
	if (const auto player = Cast<APlayerCharacter>(owner)){
		OnPlayerMeleeAttack.Broadcast(player);

		if (attackTarget) {
			player->OnAttack();
		}
	}

	const auto seed = static_cast<uint32>(FMath::Rand());
	const auto attackVector = attackTarget ? attackTarget->GetActorLocation() : owner->GetActorLocation();
			
	if (!GetOwner()->HasAuthority()) {
		FSharedPredictionContext context = FSharedPredictionContext::WithNewKey(abilitySystem, outstandingPrediction);
		outstandingPrediction = context.GetKey();
		AttackWithVariantIndexServer(attackTarget, attackVector, currentIndex, seed, outstandingPrediction);
		outstandingPrediction.NewRejectOrCaughtUpDelegate(FPredictionKeyEvent::CreateUObject(Cast<UAttackComponent>(this), &UAttackComponent::PredictionKeyRejectedOrCaughtUpCallback, context.GetKey().Current));
		AttackWithVariantIndex(attackTarget, attackVector, currentIndex, seed, context);
	}
	else {
		AttackWithVariantIndex(attackTarget, attackVector, currentIndex, seed);
	}

	return outstandingPrediction;
}

bool UMeleeAttackComponent::AttackWithVariantIndexServer_Validate(AActor* attackTarget, FVector attackVector, int32 variantIndex, int32 seed, FPredictionKey key) {
	return true;
}

void UMeleeAttackComponent::AttackWithVariantIndexServer_Implementation(AActor* attackTarget, FVector attackVector, int32 variantIndex, int32 seed, FPredictionKey key) {
	UAbilitySystemComponent* abilitySystem = GetOwner()->FindComponentByClass<UAbilitySystemComponent>();
	check(abilitySystem && "Attack Component owned by character without AbilitySystemComponent.");
	AttackWithVariantIndex(attackTarget, attackVector, variantIndex, seed, FSharedPredictionContext(abilitySystem, key));
}

void UMeleeAttackComponent::AttackWithVariantIndex(AActor* attackTarget, FVector attackVector, int32 index, int32 seed /*= 0*/, FSharedPredictionContext predictionContext) {
	auto owner = Cast<ABaseCharacter>(GetOwner());
	check(owner && "Melee attack component requires BaseCharacter owner.")
	UAbilitySystemComponent* abilitySystem = owner->GetAbilitySystemComponent();
	check(abilitySystem && "Owning actor must have dungeons ability system component.");
	FUseSpecfiedKeyScopedPredictionWindow window(predictionContext);

	UWorld* world = owner->GetWorld();
	check(world && "Need world to execute attack.");
	
	FTimerManager& timerManager = world->GetTimerManager();

	if (timerManager.IsTimerActive(attackVariantEndTimerHandle)) {
		OnAttackVariantEnd(attackTarget, attackVector, lastAttackIndex, seed, predictionContext, loopCount > 0);
		timerManager.ClearTimer(attackVariantEndTimerHandle);
	}

	FRandomStream stream(seed);

	AttackCounter++;

	//Theoretically there could be desync causing client to send invalid index to server, so we have to guard against this here...
	if (index >= ActiveAttackVariants.Num()) {
		index = ActiveAttackVariants.Num() - 1;
	} 

	//Debug drawing of attack.
	if (CVarDebugDrawMeleeAttack.GetValueOnGameThread()) {
		UE_LOG(LogTemp, Warning, TEXT("attack target: %s"), attackTarget == nullptr ? *FString("NULL") : *attackTarget->GetName());
		if (attackTarget != nullptr) {
			auto attacker = Cast<ACharacter>(GetOwner()); 
			DrawDebugLine(GetWorld(), attacker->GetActorLocation(), attackTarget->GetActorLocation(), FColor::Emerald, false, 5.f);
		}
	}

	DungeonsGearUtilLibrary::OnBeforeMeleeAttack(owner, attackTarget, attackVector, index, &stream, predictionContext);

	const MeleeAttackParametersCapture capture(abilitySystem);
	const FMeleeAttackComponentAttackVariant& currentVariant = ActiveAttackVariants[index];
	const float damageDelay = currentVariant.DamageDelaySeconds / capture.AttackSpeedMultiplier;

	if (loopCount == 0) {
		if (currentVariant.VariantActiveEffect && !VariantActiveEffectHandle.IsValid()) {
			auto spec = effects::CreateGameplayEffectSpecFromSubClass(abilitySystem, currentVariant.VariantActiveEffect);
			spec.GetContext().AddInstigator(GetOwner(), GetOwner());
			VariantActiveEffectHandle = abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
		}
	}

	int nextIndex = index + 1;
	if (currentVariant.shouldLoopAgain(loopCount)) {
		loopCount++;
		nextIndex = index;
	}
	else {
		loopCount = 0;
	}

	const bool loop = (index == nextIndex);

	UAnimSequenceBase* sequence = currentVariant.AttackSequence;
	float animationPlayRate = sequence->GetPlayLength() / (currentVariant.AttackAnimationTimeDurationSeconds / capture.AttackSpeedMultiplier);
	
	if (owner->HasAuthority()) {
		owner->MulticastPlayMeleeAttackVariantAnimation(index, animationPlayRate, predictionContext.GetKey());
	} else {
		owner->PlayMeleeAttackVariantAnimation(index, animationPlayRate);
	}

	lastAttackIndex = index;

	//Set cooldowns.
	const auto now = world->GetTimeSeconds();
	const float attackDurationWithoutCooldown = currentVariant.AttackAnimationTimeDurationSeconds / capture.AttackSpeedMultiplier;
	const float attackDurationIncludingCooldown = (currentVariant.AttackAnimationTimeDurationSeconds + currentVariant.CooldownSeconds) / capture.AttackSpeedMultiplier;
	const float evaluatedAttackDuration = loop ? attackDurationWithoutCooldown : attackDurationIncludingCooldown;
	canAttackFromSecond = now + evaluatedAttackDuration;
	lastAttackTimestampSeconds = now;

	//Move attack variant reset forward.
	SetAttackVariantIndex(nextIndex % ActiveAttackVariants.Num());
	
	timerManager.ClearTimer(attackVariantResetTimerHandle);
	timerManager.ClearTimer(attackVariantResetAnimationTimerHandle);

	if (currentIndex != 0) {
		// This causes timing issues, if multiplier is set to 0, this delay is the same as variant duration, which means it can trigger before next variant, resetting the index prematurely, adding an offset as a hack
		const float attackVariantResetDelay = ActiveAttackVariantResetSeconds * currentVariant.AttackVariantResetMultiplier + evaluatedAttackDuration + 0.1f;
		timerManager.SetTimer(attackVariantResetTimerHandle, FTimerDelegate::CreateUObject(this, &UMeleeAttackComponent::ResetAttackVariantTimer), attackVariantResetDelay, false);
	}

	timerManager.SetTimer(attackVariantEndTimerHandle, FTimerDelegate::CreateUObject(this, &UMeleeAttackComponent::OnAttackVariantEnd, attackTarget, attackVector, index, seed, predictionContext, loop), attackDurationWithoutCooldown, false);
	if (!loop) {
		timerManager.SetTimer(attackVariantCooldownTimerHandle, FTimerDelegate::CreateUObject(this, &UMeleeAttackComponent::OnVariantCooldownDone, index), evaluatedAttackDuration, false);
	}

	const float BaseCooldown = SharedCooldownConstant / capture.AttackSpeedMultiplier;
	const float activeCooldown = FMath::Min(BaseCooldown + evaluatedAttackDuration * (currentVariant.bOverrideCancelPoint ? currentVariant.CancelPointOverride * 0.75f : activeCancelPoint), evaluatedAttackDuration);
	const float passiveCooldown = BaseCooldown + evaluatedAttackDuration * (currentVariant.bOverrideCancelPoint ? currentVariant.CancelPointOverride : passiveCancelPoint);
	cachedOwner->SetSharedCooldown(activeCooldown, passiveCooldown);

	bool continousDamage = currentVariant.ContinuousDamageInterval > 0;
	if (damageDelay <= 0) 
	{	
		ApplyDamage(capture, currentVariant, index, attackTarget, attackVector, seed, predictionContext);
		if (continousDamage) {
			if (!timerManager.IsTimerActive(continousDamageTimerHandle)) {
				timerManager.SetTimer(continousDamageTimerHandle, FTimerDelegate::CreateUObject(this, &UMeleeAttackComponent::ApplyDamage, capture, currentVariant, index, TWeakObjectPtr<AActor>(attackTarget), attackVector, seed, predictionContext), currentVariant.ContinuousDamageInterval, true);
			}
		}
	}
	else 
	{
		if (continousDamage) {
			if (!timerManager.IsTimerActive(continousDamageTimerHandle)) {
				timerManager.SetTimer(continousDamageTimerHandle, FTimerDelegate::CreateUObject(this, &UMeleeAttackComponent::ApplyDamage, capture, currentVariant, index, TWeakObjectPtr<AActor>(attackTarget), attackVector, seed, predictionContext), damageDelay + currentVariant.ContinuousDamageInterval, true);
			}
		} else {
			timerManager.ClearTimer(delayTimerHandle);
			timerManager.SetTimer(delayTimerHandle, FTimerDelegate::CreateUObject(this, &UMeleeAttackComponent::ApplyDamage, capture, currentVariant, index, TWeakObjectPtr<AActor>(attackTarget), attackVector, seed, predictionContext), damageDelay, false);
		}
	}
}

void UMeleeAttackComponent::SetRangeBoost(float boost /*= 0.0f*/)
{
	auto& timerManager = GetOwner()->GetWorld()->GetTimerManager();
	mRangeBoost = boost;

	if (timerManager.IsTimerActive(rangeBoostTimerHandle))
	{
		mRangeBoost = 0.0f;
		return;
	}

	if (!FMath::IsNearlyZero(boost))
	{
		// D11.DB - Make sure the range timer automatically clears itself after a delay.
		const float delay = 0.3f;
		timerManager.ClearTimer(rangeBoostTimerHandle);
		timerManager.SetTimer(rangeBoostTimerHandle, FTimerDelegate::CreateUObject(this, &UMeleeAttackComponent::ResetRangeBoost), delay, false);
	}
}

bool UMeleeAttackComponent::IsAttackInProgress() const {
	auto& timerManager = GetOwner()->GetWorld()->GetTimerManager();
	return delayTimerHandle.IsValid() && timerManager.IsTimerActive(delayTimerHandle);
}

bool UMeleeAttackComponent::IsSameTargetWithTimeThreshold(AActor* target, float timeThreshold) const {
	if (lastPrimaryTargetDamaged.IsValid() && lastPrimaryTargetDamaged.Get() == target) {
		return UGameplayStatics::GetTimeSeconds(GetOwner()) < lastPrimaryTargetDamagedStamp + timeThreshold;
	}
	return false;
}

bool UMeleeAttackComponent::CanTarget(const ABaseCharacter* target) const {
	const auto attacker = Cast<ABaseCharacter>(GetOwner());
	return actorquery::is::alive(target) && IsAttackAllowed(attacker, target) && characterquery::is::targetable(target);
}

bool UMeleeAttackComponent::IsAttackAllowed(const ABaseCharacter* attacker, const ABaseCharacter* target) const {
	return !attacker->IsFriendlyTowards(target) && bAllowSameTeamSplashDamage || attacker->GetCurrentTeam() != target->GetCurrentTeam();
}

void UMeleeAttackComponent::PlayAttackVariantMontage(int32 variantIndex, float playRate) {
	const auto& variants = GetActiveAttackVariants();
	if (variants.IsValidIndex(variantIndex)) {
		auto&& variant = variants[variantIndex];
		if (auto owner = Cast<ABaseCharacter>(GetOwner())) {
			if (UAnimMontage* montage = variant.DynamicMontageCache) {
				owner->PlayMontage(variant.DynamicMontageCache, playRate);
			}
		}
	}
}

void UMeleeAttackComponent::ApplyDamage(const MeleeAttackParametersCapture capture, const FMeleeAttackComponentAttackVariant variant, int index, TWeakObjectPtr<AActor> attackTarget, FVector attackVector, int32 seed, FSharedPredictionContext predictionContext) 
{
	auto attacker = Cast<ABaseCharacter>(GetOwner());
	FRandomStream stream(seed);

	// do not apply damage if we are dead
	const auto healthComponent = attacker->FindComponentByClass<UHealthComponent>();
	if (healthComponent != nullptr && healthComponent->IsNotAlive()) {
		return;
	}

	FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::Normal | EGameplayCueParametersField::GELevel | EGameplayCueParametersField::SourceObject);
	FUseSpecfiedKeyScopedPredictionWindow PredictionWindow(predictionContext);

	//Find all actors that will be hit	
	FindActorsInAttackWedge(capture, variant, attackTarget, targetDamageCharacters);	

	auto successfulAttack = false;

	unsigned actualHitCount = 0;
	const auto totalDamage = 0.f;

	const auto attackerLocation = attacker->GetActorLocation();
	const auto hasAuthority = attacker->HasAuthority();
	
	const auto filteredTargets = targetDamageCharacters.FilterByPredicate([this](const ABaseCharacter* v) { return CanTarget(v); });

	const auto primaryDamageActor = FindPrimaryDamageActor(attackTarget, attacker, filteredTargets);

	lastPrimaryTargetDamaged = primaryDamageActor;
	lastPrimaryTargetDamagedStamp = UGameplayStatics::GetTimeSeconds(GetOwner());

	auto abilitySystem = attacker->GetAbilitySystemComponent();

	const auto isStrong = abilitySystem->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.Melee.Buff")));
	const auto hitChance = abilitySystem->GetNumericAttribute(UMeleeAttributeSet::MeleeAttackHitChanceMultiplierAttribute());
	
	TArray<EntityType> killedMobs;
	for (auto actor : filteredTargets) {
		const auto baseCharacter = Cast<ABaseCharacter>(actor);
		check(baseCharacter && "All attack targets should be baseclassing ABaseCharacter");

		const auto targetAbilitySystem = baseCharacter->GetAbilitySystemComponent();

		const auto targetIsVoided = targetAbilitySystem->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.StatusEffect.VoidStrike")));

		// #D11.CM - Dreadnought
		if(targetAbilitySystem->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("Immunity.Damage.Melee"))) {
			FGameplayCueParameters params;
			params.Instigator = attacker;
			params.Location = actor->GetActorLocation();
			targetAbilitySystem->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Dreadnought")), params);
			continue;
		}

		// D11.DB - Hit chance
		if (stream.GetFraction() > hitChance || targetAbilitySystem->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("Immunity.Damage"))) {
			FGameplayCueParameters params;
			params.Instigator = attacker;
			params.Location = actor->GetActorLocation();
			targetAbilitySystem->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Miss")), params);
			continue;
		}

		++actualHitCount;

		auto AdjustedDamage = variant.Damage;
		switch (variant.AttackVariantType)
		{
		case EMeleeAttackVariantType::TargetAndSplash:
			AdjustedDamage = GetSplashAdjustedDamage(variant.Damage, variant.DamageSplashMultiplier, primaryDamageActor, actor);
			break;
		case EMeleeAttackVariantType::DistanceBased:
			AdjustedDamage = GetDistanceBasedDamage(variant.Damage, variant.AttackRange, primaryDamageActor->GetActorLocation(), actor);
			break;
		default:
			break;
		}

		const auto byWhat = FindDamageSource(attacker);

		auto spec = effects::CreateGameplayEffectSpec<UMeleeDamageGameplayEffect>(abilitySystem, effects::HealthName, -AdjustedDamage, attacker, byWhat, attackerLocation, index);
		auto context = effects::GetDungeonsContextFromSpec(spec);
		spec.GetContext().AddSourceObject(weapon.IsValid() ? Cast<UObject>(weapon.Get()) : this);

		if (isStrong) {
			spec.DynamicAssetTags.AddTag(FGameplayTag::RequestGameplayTag("Damage.Melee.Buffed"));
			context->AdditionalCues.Add(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Hit.Melee.Buffed")));
			context->AdditionalCues.Add(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Impact.Strong")));
		}
		else {
			context->AdditionalCues.Add(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Impact")));
		}

		auto attackerPushback = variant.pushback;
		attackerPushback.pushbackStrength *= capture.PushbackMultiplier;
		const auto pushbackMultiplier = baseCharacter->GetHealthComponent()->GetDamageMultiplier(spec.DynamicAssetTags);

		auto mutablePushback = attackerPushback.pushbackStrength;
		DungeonsGearUtilLibrary::OnBeforeMeleeDamageDealt(attacker, mutablePushback, spec, targetAbilitySystem, &stream, predictionContext);
		attackerPushback.pushbackStrength = mutablePushback;

		if (GetOwnerRole() == ROLE_Authority) {
			pushback::pushback(attackerPushback, *attacker, *actor, pushbackMultiplier);
		}

		//Set any pushback to be applied to ragdolls (modified with a bonus amount and extra strength)
		effects::StorePushbackInNormal(spec, pushback::getLaunchVector(attackerPushback, *attacker, *actor, 1.5f * pushbackMultiplier * variant.EffectsPushbackMultiplier, 1.0f));
		context->StunMultiplier = GetStunMultiplier(variant.StunMultiplier, primaryDamageActor, actor);

		FGameplayTagContainer AssetTags;
		spec.GetAllAssetTags(AssetTags);

		if (!AssetTags.HasTag(FGameplayTag::RequestGameplayTag(TEXT("Damage")))) {
			spec.DynamicAssetTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Damage.Melee")));
			if (variant.DamageType.IsValid()) {
				spec.DynamicAssetTags.AddTag(variant.DamageType);
			}
			context->AdditionalCues.Add(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Hit.Melee")));
		}

		if (targetIsVoided)
		{
			spec.DynamicAssetTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Damage.Elemental.Void")));
		}

		const auto* baseCharHC = baseCharacter->FindComponentByClass<UHealthComponent>();
		const auto preDamageHealth = baseCharHC ? baseCharHC->GetCurrentHealth() : 0.0f;

		// We don't have immunity to melee attack that are source dependent
		spec.CapturedSourceTags = FTagContainerAggregator();
		const auto missedAttack = !abilitySystem->ApplyGameplayEffectSpecToTarget(spec, targetAbilitySystem, predictionContext.GetKey()).WasSuccessfullyApplied();

		const auto postDamageHealth = baseCharHC ? baseCharHC->GetCurrentHealth() : 0.0f;

		if (postDamageHealth <= 0) {
			if (AMobCharacter* mobCharacter = Cast<AMobCharacter>(actor)) {
				killedMobs.Emplace(mobCharacter->EntityType);
			}
		}

		if (auto gameInstance = Cast<UDungeonsGameInstance>(GetWorld()->GetGameInstance())) {
			if (auto playerOwner = Cast<APlayerCharacter>(GetOwner())) {
				auto equipment = playerOwner->GetEquipmentComponent();
				auto slots = equipment ? equipment->GetSlotsOfType(ESlotType::MeleeWeapon) : TArray<UItemSlot*>();
				if (slots.Num() > 0) {
					analytics::Analytics::GetInstance().FireWeaponUsed(*playerOwner, *slots[0], baseCharacter, AdjustedDamage, preDamageHealth, postDamageHealth);
				}
			}
		}

		const auto actorLocation = actor->GetActorLocation();
		DungeonsGearUtilLibrary::OnAfterDealtMeleeDamage(attacker, AdjustedDamage, missedAttack, actor, actor, attackerLocation, actorLocation, &stream, predictionContext);

		const bool successfulHitTarget = !missedAttack;
		if(successfulHitTarget) {
			successfulAttack = !missedAttack;
		}

		OnMeleeDamageDealt.Broadcast(successfulHitTarget, attacker, baseCharacter, predictionContext);
	}

	//D11.RR - Achievements
	OnMeleeAttack(killedMobs);

	if (successfulAttack) {
		SuccessfulAttackCounter++;
	}

	OnAfterMeleeDamageDealt.Broadcast(successfulAttack, attacker, predictionContext);

	if (hasAuthority) {
		Cast<ADungeonsGameMode>(GetWorld()->GetAuthGameMode())->OnActorAttackFinished(attacker, actualHitCount, totalDamage);
		if (bRemoveInvisibilityAfterAttack)
		{
			attacker->RemoveInvisibility();
		}
	}

	if (const auto mob = Cast<AMobCharacter>(attacker)) {
		mob->MobParams.totalAttacks.all++;
		mob->MobParams.totalAttacks.melee++;
		if (successfulAttack) {
			mob->MobParams.successfulAttacks.all++;
			mob->MobParams.successfulAttacks.melee++;
		}
	}

	// jryden: @marten @Brynolf and @niklas/BarneyBear should probably sort out when ::OnAfterMeleeAttack should be called, there is a sandwiched callback now also in OnAttackVariantEnd() which is on a timer on this branch: futurework/netherdlc
	if (variant.EnchantmentTriggerSettings.bTriggerEnchantmentsAfterDamage) {
		DungeonsGearUtilLibrary::OnAfterMeleeAttack(attacker, actualHitCount, attackTarget.Get(), attackVector, index, &stream, predictionContext);
	}
	
} 

AActor* UMeleeAttackComponent::FindPrimaryDamageActor(TWeakObjectPtr<AActor> attackTarget, ABaseCharacter* attacker, TArray<TArray<ABaseCharacter*>::ElementType> filteredTargets) {
	AActor* primaryDamageActor = nullptr;

	if (attackTarget.IsValid() && filteredTargets.Contains(attackTarget.Get())) {		
		//Our intended target is being hit
		primaryDamageActor = attackTarget.Get();
	} else {		
		//Otherwise select the actor with the shortest distance between our capsule and it.
		auto bestDist = FLT_MAX;
		for (auto actor : filteredTargets) {
			const auto distance = Get2DDistanceToTarget(attacker, actor);
			if (distance < bestDist) {
				bestDist = distance;
				primaryDamageActor = actor;
			}
		}
	}

	return primaryDamageActor;
}

float UMeleeAttackComponent::GetSplashAdjustedDamage(const float attackDamage, const float damageSplashMultiplier, AActor* primaryDamageTarget, ABaseCharacter* currentTarget) {
	//Full damage against primary target - splash multiplier against all others.
	return primaryDamageTarget && primaryDamageTarget != currentTarget ? attackDamage * damageSplashMultiplier : attackDamage;
}

float UMeleeAttackComponent::GetDistanceBasedDamage(float attackDamage, float attackRange, FVector attackLocation, ABaseCharacter* currentTarget)
{
	float distance = FVector::Dist2D(attackLocation, currentTarget->GetActorLocation());
	return UKismetMathLibrary::MapRangeClamped(distance, 0, attackRange, attackDamage, 1);
}

AActor* UMeleeAttackComponent::FindDamageSource(ABaseCharacter* attacker) const {
	auto damageSource = weapon.IsValid() ? weapon.Get() : attacker;
	if (const auto mob = Cast<AMobCharacter>(attacker)) {
		if (const auto master = mob->GetMaster()) {
			damageSource = master;
		}
	}
	return damageSource;
}

float UMeleeAttackComponent::GetStunMultiplier(const float stunMultiplier, const AActor* primaryDamageTarget, ABaseCharacter* currentTarget) {
	return primaryDamageTarget && primaryDamageTarget != currentTarget ? stunMultiplier * effects::SECONDARY_TARGET_STUN_MULTIPLIER : stunMultiplier;
}

float UMeleeAttackComponent::Get2DDistanceToTarget(AActor* attacker, AActor * attackTarget) {
	float minDist = std::numeric_limits<float>::max();
	const auto attackerCapsule = attacker ? attacker->FindComponentByClass<UCapsuleComponent>() : nullptr;
	
	if (attackerCapsule && attackTarget) {
		if (auto baseChar = Cast<ABaseCharacter>(attackTarget)) {
			minDist = actorquery::getCapsuleDistance2D(attackerCapsule, baseChar->GetClosestTargetableCapsule(attackerCapsule->GetComponentLocation()));
		}
		else if (auto targetCapsule = attackTarget->FindComponentByClass<UCapsuleComponent>()) {
			minDist = actorquery::getCapsuleDistance2D(attackerCapsule, targetCapsule);
		}
	}

	return FMath::IsNearlyEqual(minDist, std::numeric_limits<float>::max()) ? 0.f : minDist;
}

void UMeleeAttackComponent::OnAttackVariantEnd(AActor* attackTarget, FVector attackVector, int32 index, int32 seed, FSharedPredictionContext predictionContext, bool wouldLoopNext) {
	const auto& endedVariant = ActiveAttackVariants[index];
	FRandomStream stream(seed);
	if ((endedVariant.EnchantmentTriggerSettings.bTriggerEnchantmentsOnRelease && !bHoldingAttack) ||
		(endedVariant.EnchantmentTriggerSettings.bTriggerEnchantmentsOnVariantChange && !wouldLoopNext)) {
		DungeonsGearUtilLibrary::OnAfterMeleeAttack(Cast<ABaseCharacter>(GetOwner()), 1, attackTarget, attackVector, index, &stream, predictionContext);
	}
	GetWorld()->GetTimerManager().ClearTimer(continousDamageTimerHandle);

	auto* abilitySystem = Cast<ABaseCharacter>(GetOwner())->GetAbilitySystemComponent();

	if (!wouldLoopNext) {
		loopCount = 0;
		if (GetOwner()->HasAuthority()) {
			if (endedVariant.VariantChangeEffect) {
				const auto changeSpec = effects::CreateGameplayEffectSpecFromSubClass(abilitySystem, endedVariant.VariantChangeEffect);
				changeSpec.GetContext().AddInstigator(GetOwner(), GetOwner());
				VariantChangeEffectHandle = abilitySystem->ApplyGameplayEffectSpecToSelf(changeSpec);
			}
		}
	}
	if (!wouldLoopNext || !bHoldingAttack) {
		if (GetOwner()->HasAuthority()) {
			if (VariantActiveEffectHandle.IsValid()) {
				if (abilitySystem->RemoveActiveGameplayEffect(VariantActiveEffectHandle)) {
					VariantActiveEffectHandle.Invalidate();
				}
			}
		}
	}
	OnVariantEnd.Broadcast(attackTarget, attackVector, index, &stream, predictionContext);
}

void UMeleeAttackComponent::OnVariantCooldownDone(int32 index) {
	if (auto* abilitySystem = Cast<ABaseCharacter>(GetOwner())->GetAbilitySystemComponent()) {
		if (VariantChangeEffectHandle.IsValid()) {
			if (abilitySystem->RemoveActiveGameplayEffect(VariantChangeEffectHandle)) {
				VariantChangeEffectHandle.Invalidate();
			}
		}
	}
}

void UMeleeAttackComponent::OnDeath() {
	RemoveVariantEffects();
}

void UMeleeAttackComponent::RemoveVariantEffects() {
	if (auto* abilitySystem = Cast<ABaseCharacter>(GetOwner())->GetAbilitySystemComponent()) {
		if (VariantChangeEffectHandle.IsValid()) {
			if (abilitySystem->RemoveActiveGameplayEffect(VariantChangeEffectHandle)) {
				VariantChangeEffectHandle.Invalidate();
			}
		}
		if (VariantActiveEffectHandle.IsValid()) {
			if (abilitySystem->RemoveActiveGameplayEffect(VariantActiveEffectHandle)) {
				VariantActiveEffectHandle.Invalidate();
			}
		}
	}
}

void UMeleeAttackComponent::OnMeleeAttack(const TArray<EntityType>& killedMobs)
{
	if (APlayerCharacter* character = Cast<APlayerCharacter>(GetOwner())) {
		if (UStatTrackerComponent* tracker = character->GetStatTracker()) {
			tracker->OnMeleeAttack(killedMobs);
		}
		else {
			Client_OnMeleeAttack(killedMobs);
		}
	}
}

void UMeleeAttackComponent::Client_OnMeleeAttack_Implementation(const TArray<EntityType>& killedMobs)
{
	if (APlayerCharacter* character = Cast<APlayerCharacter>(GetOwner())) {
		if (UStatTrackerComponent* tracker = character->GetStatTracker()) {
			tracker->OnMeleeAttack(killedMobs);
		}		
	}
}

void UMeleeAttackComponent::OnWeaponSlotUpdated(UItemSlot* slot) {
	if (auto item = Cast<AMeleeWeaponGearItemInstance>(slot->GetItem())) {
		weapon = item;
		SetAttackVariants(item->GetEditableAttackVariants());
		ActiveAttackVariantResetSeconds = item->GetAttackVariantResetSeconds();
	}
	else {
		SetAttackVariants(ConfiguredAttackVariants);
		ActiveAttackVariantResetSeconds = AttackVariantResetSeconds;
	}
	if (auto playerCharacter = Cast<APlayerCharacter>(GetOwner())) {
		playerCharacter->OnAttackVariantsUpdated();		
	}
	RemoveVariantEffects();
}

void UMeleeAttackComponent::SetAttackVariants(TArray<FMeleeAttackComponentAttackVariant>& variants) {
	int32 index = 0;
	int32 num = variants.Num();
	for (auto& variant : variants) {
		if(!variant.DynamicMontageCache){
			if (variant.AttackSequence){

				//Pre-create montages
				const float montageBasePlayRate = 1.0f;
				const float blendTime = FMath::Min(0.2f, variant.AttackAnimationTimeDurationSeconds*0.5f);
				UAnimMontage* montage = UAnimMontage::CreateSlotAnimationAsDynamicMontage(variant.AttackSequence, variant.Slot, 0.0f, blendTime, montageBasePlayRate, 1, 0, 0.0f);				
				
				/*
				//Moj.DB: Experimental Feature
				if (index < num - 1) {
					auto sectionindex = montage->AddAnimCompositeSection(FName(TEXT("ComboLoop")), 1.0f);
					auto& section = montage->GetAnimCompositeSection(sectionindex);
					section.NextSectionName = FName(TEXT("ComboLoop"));
					montage->BlendOut.SetBlendOption(EAlphaBlendOption::ExpOut);
				}
				*/

				variant.DynamicMontageCache = montage;
			}
		}
		index++;
	}
	ActiveAttackVariants = variants;
	SetAttackVariantIndex(0);
}

DECLARE_CYCLE_STAT(TEXT("UMeleeAttackComponent::InAttackOrientation"), STAT_UMeleeAttackComponent_InAttackOrientation, STATGROUP_AI);

bool UMeleeAttackComponent::InAttackOrientation(AActor* attackTarget) const {
	SCOPE_CYCLE_COUNTER(STAT_UMeleeAttackComponent_InAttackOrientation);
	return IsWithinFuzzyArc(attackTarget->GetActorLocation());
}

bool UMeleeAttackComponent::IsWithinFuzzyArc(FVector target) const {

	auto variant = GetCurrentAttackVariant();

	auto attacker = Cast<ABaseCharacter>(GetOwner());
	check(attacker && "Melee attack component requires BaseCharacter owner.");

	UAbilitySystemComponent* abilitySystem = attacker->GetAbilitySystemComponent();
	check(abilitySystem && "Owning actor must have dungeons ability system component.");
	const MeleeAttackParametersCapture capture(abilitySystem);
	
	const float attackConeAngle = variant.ConeAngleDegrees * capture.RangeMultiplier;	

	auto attackOrigin{ attacker->GetActorLocation() };
	auto attackForward{ attacker->GetActorForwardVector() };

	return actorquery::isInArc(attackOrigin, attackForward, target, FMath::DegreesToRadians(attackConeAngle));	
}

void UMeleeAttackComponent::FindActorsInAttackWedge(const MeleeAttackParametersCapture capture, const FMeleeAttackComponentAttackVariant variant, TWeakObjectPtr<AActor> attackTarget, TArray<ABaseCharacter*>& outList) const {
	outList.Empty();
	
	auto attacker = Cast<ABaseCharacter>(GetOwner());
	check(attacker && "Melee attack component requires BaseCharacter owner.");

	//Final modified attributes of attack
	const float attackConeAngle = variant.ConeAngleDegrees * capture.RangeMultiplier;
	const float attackRange = variant.AttackRange * capture.AngleMultiplier;	

	auto attackOrigin{ attacker->GetActorLocation() };
	const auto attackForward{ attacker->GetActorForwardVector() };

	const auto attackerCapsule = attacker->FindComponentByClass<UCapsuleComponent>();
	const auto attackerCapsuleRadius = attackerCapsule != nullptr ? attackerCapsule->GetScaledCapsuleRadius() : 0.0f;

	// move origin to edge of attackers capsule and offset with the attack variants cone offset
	attackOrigin += attackForward * (variant.ConeOffsetUnits + attackerCapsuleRadius);

	auto attackConeAngleRadians = FMath::DegreesToRadians(attackConeAngle);
	actorquery::getInstanceTrackedActorsInWedge(attacker->GetWorld(), attackOrigin, attackForward, variant.MinRange, attackRange, attackConeAngleRadians, variant.MaxZDiff, outList, true);

	if (CVarDebugDrawMeleeAttack.GetValueOnGameThread()) {
		DungeonsDebug::drawDebugWedge(attacker->GetWorld(), attackOrigin, attackForward, variant.MinRange, attackRange, attackConeAngle, variant.MaxZDiff, variant.ConeOffsetUnits, FColor::Emerald);
	}

	if (attackTarget.IsValid()){
		//Forcibly add attack target if it was not too far from the player, even if target was not caught in wedge		
		if (auto attackCharacter = Cast<ABaseCharacter>(attackTarget.Get())) {
			if (targetDamageCharacters.Find(attackCharacter) == INDEX_NONE) {
				if (actorquery::isActorInExtraMargin(*attackTarget.Get(), attackOrigin, attackForward, attackRange + TargetAttackRangeMargin, attackConeAngleRadians, variant.MaxZDiff)) {
					outList.Add(attackCharacter);
				}
			}
		}		
	}
}

bool UMeleeAttackComponent::IsTargetInAttackWedge(AActor* attackTarget) {
	if (attackTarget) {
		auto variant = GetCurrentAttackVariant();
		
		auto attacker = Cast<ABaseCharacter>(GetOwner());
		check(attacker && "Melee attack component requires BaseCharacter owner.");

		UAbilitySystemComponent* abilitySystem = attacker->GetAbilitySystemComponent();
		check(abilitySystem && "Owning actor must have dungeons ability system component.");

		const float AbilityRangeMultiplier(abilitySystem->GetNumericAttribute(UMeleeAttributeSet::MeleeAttackRangeMultiplierAttribute()));
		const float AbilityAngleMultiplier(abilitySystem->GetNumericAttribute(UMeleeAttributeSet::MeleeAttackAngleMultiplierAttribute()));
		
		//Final modified attributes of attack
		const float attackConeAngle = variant.ConeAngleDegrees * AbilityRangeMultiplier;
		const float attackRange = variant.AttackRange * AbilityAngleMultiplier;

		auto attackOrigin{ attacker->GetActorLocation() };
		const auto attackForward{ attacker->GetActorForwardVector() };

		const auto attackerCapsule = attacker->FindComponentByClass<UCapsuleComponent>();
		const auto attackerCapsuleRadius = attackerCapsule != nullptr ? attackerCapsule->GetScaledCapsuleRadius() : 0.0f;

		// move origin to edge of attackers capsule and offset with the attack variants cone offset
		attackOrigin += attackForward * (variant.ConeOffsetUnits + attackerCapsuleRadius);

		auto attackConeAngleRadians = FMath::DegreesToRadians(attackConeAngle);

		return actorquery::isActorInWedge(attackOrigin, attackForward, attackTarget, variant.MinRange, attackRange, attackConeAngleRadians, variant.MaxZDiff);
	}
	return true;
}

bool UMeleeAttackComponent::CanAttack(AActor* attackTarget) const {
	return Super::CanAttack(attackTarget) && IsWithinAttackRange(attackTarget) && !IsMeleeDisabled();
}

bool UMeleeAttackComponent::IsWithinAttackRange(AActor* attackTarget) const {
	if (attackTarget) {
		const auto attackRange = GetAttackActivationRange() + ( IsSameTargetWithTimeThreshold(attackTarget, SameTargetAttackRangeThresholdSeconds) ? SameTargetAttackRangeBonus : 0.0f );
		const auto distance = Get2DDistanceToTarget(GetOwner(), attackTarget);
		return distance <= attackRange;
	}
	return true;
}

void UMeleeAttackComponent::StopAttack() {
	if (delayTimerHandle.IsValid()) {
		GetOwner()->GetWorld()->GetTimerManager().ClearTimer(delayTimerHandle);
		delayTimerHandle.Invalidate();
	}
}

void UMeleeAttackComponent::CancelActions()
{
	StopAttack();
	CancelCooldown();
}

USoundCue* UMeleeAttackComponent::GetImpactSound_Implementation(int index) const {
	if (ConfiguredAttackVariants.Num() == 0) {
		return nullptr;
	}
	return ConfiguredAttackVariants[FMath::Clamp(index, 0, ConfiguredAttackVariants.Num() - 1)].HitSoundCue;
}

bool UMeleeAttackComponent::IsMeleeDisabled() const {
	if (auto player = Cast<APlayerCharacter>(cachedOwner)) {
		return player->IsMeleeDisabled();
	}
	return false;
}









//Stats
float UMeleeAttackComponent::GetAttackVariantTotalTimeContributionFactor(const TArray<FMeleeAttackComponentAttackVariant>& attackVariants, const FMeleeAttackComponentAttackVariant& askedVar) {
	float seconds = 0.0f;
	float attacks = 0.0f;
	for (auto var : attackVariants) {
		seconds += (var.CooldownSeconds + var.AttackAnimationTimeDurationSeconds);
		attacks++;
	}

	const float averageDuration = seconds / attacks;
	const float duration = (askedVar.CooldownSeconds + askedVar.AttackAnimationTimeDurationSeconds);

	return duration / averageDuration;
}


float UMeleeAttackComponent::GetAttackVariantIndexRelevanceWeight(const FMeleeAttackComponentAttackVariant& askedVar, int numAttacks, int attackVariantIndex, float VariantResetSeconds) {	
	const float resetTimer = VariantResetSeconds * askedVar.AttackVariantResetMultiplier;
	return resetTimer + FMath::Pow(static_cast<float>(numAttacks - attackVariantIndex) / numAttacks, 1.0f / resetTimer);
}

float UMeleeAttackComponent::GetAttackVariantIndexRelevanceWeightFactor(const TArray<FMeleeAttackComponentAttackVariant>& attacks, int attackVariantIndex, float VariantResetSeconds) {
	float totalWeight = 0.0f;
	const int numAttacks = attacks.Num();
	for (int index = 0; index < attacks.Num(); index++) {
		totalWeight += GetAttackVariantIndexRelevanceWeight(attacks[index], numAttacks, index, VariantResetSeconds);
	}
	
	return GetAttackVariantIndexRelevanceWeight(attacks[attackVariantIndex], numAttacks, attackVariantIndex, VariantResetSeconds) / totalWeight;
}

float UMeleeAttackComponent::GetAttackVariantsStat(const TArray<FMeleeAttackComponentAttackVariant>& attackVariants, EItemStats stat, float VariantResetSeconds) {
	switch (stat) {
	case EItemStats::DamagePerSec:
	{
		float damage = 0.0f;
		float seconds = 0.0f;
		int index = 0;
		for (auto var : attackVariants) {
			const float relevance = GetAttackVariantIndexRelevanceWeightFactor(attackVariants, index, VariantResetSeconds);
			seconds += (var.CooldownSeconds + var.AttackAnimationTimeDurationSeconds) * relevance;
			damage += var.Damage * relevance;
			index++;
		}

		return damage / seconds;
	}
	case EItemStats::AverageDamage:
	{
		float damage = 0.0f;
		float attacks = 0.0f;
		int index = 0;
		for (auto var : attackVariants) {
			const float relevance = GetAttackVariantIndexRelevanceWeightFactor(attackVariants, index, VariantResetSeconds);
			const float factor = GetAttackVariantTotalTimeContributionFactor(attackVariants, var);
			attacks += 1.0f / factor * relevance;
			damage += var.Damage / factor * relevance;
			index++;
		}

		return damage / attacks;
	}
	case EItemStats::AttackSpeed:
	{
		float attacks = 0.0f;
		float seconds = 0.0f;
		int index = 0;
		for (auto var : attackVariants) {
			const float relevance = GetAttackVariantIndexRelevanceWeightFactor(attackVariants, index, VariantResetSeconds);
			attacks += relevance;
			seconds += (var.CooldownSeconds + var.AttackAnimationTimeDurationSeconds) * relevance;
			index++;
		}

		return attacks / seconds;
	}
	case EItemStats::AverageArea:
	{
		float area = 0.0f;
		float attacks = 0.0f;
		int index = 0;
		for (auto var : attackVariants) {
			const float relevance = GetAttackVariantIndexRelevanceWeightFactor(attackVariants, index, VariantResetSeconds);
			const float factor = GetAttackVariantTotalTimeContributionFactor(attackVariants, var);
			attacks += 1.0f / factor * relevance;
			const float TotalArcRad = FMath::DegreesToRadians(var.ConeAngleDegrees * 2.0f);
			const float CircleFraction = TotalArcRad / (PI*2.0f);
			float attackAreaMax = FMath::Pow(var.AttackRange, 2.0f) * PI * CircleFraction;
			float attackAreaMin = FMath::Pow(var.MinRange, 2.0f) * PI * CircleFraction;
			area += (attackAreaMax - attackAreaMin) / factor * relevance;
			index++;
		}
		return area / attacks;
	}
	case EItemStats::AreaDamage:
	{
		float splash = 0.0f;
		float attacks = 0.0f;
		int index = 0;
		for (auto var : attackVariants) {
			const float relevance = GetAttackVariantIndexRelevanceWeightFactor(attackVariants, index, VariantResetSeconds);
			const float factor = GetAttackVariantTotalTimeContributionFactor(attackVariants, var);
			attacks += 1.0f / factor * relevance;

			const float TotalArcRad = FMath::DegreesToRadians(var.ConeAngleDegrees * 2.0f);
			const float CircleFraction = TotalArcRad / (PI*2.0f);
			float attackAreaMax = FMath::Pow(var.AttackRange, 2.0f) * PI * CircleFraction;
			float attackAreaMin = FMath::Pow(var.MinRange, 2.0f) * PI * CircleFraction;
			const float attackArea = (attackAreaMax - attackAreaMin);

			splash += (attackArea * var.DamageSplashMultiplier) / factor * relevance;
			index++;
		}
		return splash / attacks;
	}
	case EItemStats::PushbackStr:
	{
		float pushback = 0.0f;
		float attacks = 0.0f;
		int index = 0;
		for (auto var : attackVariants) {
			const float relevance = GetAttackVariantIndexRelevanceWeightFactor(attackVariants, index, VariantResetSeconds);
			const float factor = GetAttackVariantTotalTimeContributionFactor(attackVariants, var);
			attacks += 1.0f / factor * relevance;
			const float linearPushback = var.pushback.enablePushback ? var.pushback.pushbackStrength : 0.0f;
			pushback += FMath::Pow(linearPushback, 2.0f) / factor * relevance;
			index++;
		}

		return pushback / attacks;
	}
	case EItemStats::HitSnappiness:
	{
		float attacks = 0.0f;
		float snappyness = 0.0f;
		int index = 0;
		for (auto var : attackVariants) {
			const float relevance = GetAttackVariantIndexRelevanceWeightFactor(attackVariants, index, VariantResetSeconds);
			attacks += relevance;
			const float dur = (var.CooldownSeconds + var.AttackAnimationTimeDurationSeconds);
			const float snapFactor = (dur - (var.DamageDelaySeconds)) / dur;
			snappyness += snapFactor / dur * relevance; //Fast attacks are snappier
			index++;
		}

		return snappyness / attacks;
	}
	case EItemStats::LowestDamage:
	{
		float lowest = FLT_MAX;
		for (const FMeleeAttackComponentAttackVariant& var : attackVariants) {
			lowest = FMath::Min(lowest, var.Damage);
		}
		return lowest;
	}
	case EItemStats::HighestDamage:
	{
		float highest = -FLT_MAX;
		for (const FMeleeAttackComponentAttackVariant& var : attackVariants) {
			highest = FMath::Max(highest, var.Damage);
		}
		return highest;
	}
	default:
		return -1.0f;
	}
}

float UMeleeAttackComponent::GetAttackVariantsStat(const TArray<FMeleeAttackComponentAttackVariant>& attacks, EItemStats stat) {
	return GetAttackVariantsStat(attacks, stat, AttackVariantResetSeconds);
}