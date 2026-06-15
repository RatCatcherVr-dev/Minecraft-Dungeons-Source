// © 2020 Mojang Synergies AB. TM Microsoft Corporation.

#include "Dungeons.h"
#include "MobMasherInstance.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "util/CharacterQuery.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/item/power/ItemPowerEffectDefs.h"
#include "game/util/ComponentUtils.h"

extern TAutoConsoleVariable<int32> CVarDebugDrawItems;

AMobMashShockWave::AMobMashShockWave() {
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void AMobMashShockWave::BeginPlay() {
	Super::BeginPlay();

	Sphere = FindComponentByClass<USphereComponent>();
	if (Sphere) {
		Sphere->OnComponentBeginOverlap.AddDynamic(this, &AMobMashShockWave::OnOverlapBegin);
	}
	SetLifeSpan(LifeTime);
}

void AMobMashShockWave::Tick(float deltaTime) {
	Super::Tick(deltaTime);
	if (mExpandedTime <= ExpandDurationSeconds) {
		mExpandedTime += deltaTime;
		const float radius = FMath::Pow(FMath::Min(1.0f, mExpandedTime / ExpandDurationSeconds), ExpandRadiusExponent) * ExpandTargetRadius;
		Sphere->SetSphereRadius(radius);
		if (CVarDebugDrawItems.GetValueOnGameThread()) {
			DrawDebugSphere(GetWorld(), GetActorLocation(), Sphere->GetScaledSphereRadius(), 100, FColor::Green);
		}
	} else {
		Sphere->OnComponentBeginOverlap.RemoveAll(this);
		SetActorTickEnabled(false);
	}
}

void AMobMashShockWave::SetDamage(float damage) {
	mDamage = damage;
}

void AMobMashShockWave::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (!HasAuthority()) return;

	if (const auto target = Cast<ABaseCharacter>(OtherActor)) {
		ABaseCharacter* owner = Cast<ABaseCharacter>(GetOwner());
		if (!owner) return;
		if (mHitTargets.Contains(target)) {
			return;
		}
		if (characterquery::is::targetable(target) && !owner->IsFriendlyTowards(target)) {

			if (target->HasAuthority()) {
				pushback::pushback(HitPushback, *this, *target);
			}

			UAbilitySystemComponent* ownerAbilitySystem = owner->GetAbilitySystemComponent();
			UAbilitySystemComponent* targetAbilitySystem = target->GetAbilitySystemComponent();
			UAbilitySystemComponent* abilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
			FGameplayEffectSpec spec = effects::CreateGameplayEffectSpec<UMobMashShockwaveDamageGameplayEffect>(abilitySystem, effects::HealthName, -mDamage, GetOwner(), this, GetActorLocation(), 1.f);			
			FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::Normal);
			effects::StorePushbackInNormal(spec, pushback::getLaunchVector(HitPushback, *this, *target, 1.0f, 0.0f));

			abilitySystem->ApplyGameplayEffectSpecToTarget(spec, targetAbilitySystem);
			mHitTargets.Add(target);
		}
	}
}

AMobMasherInstance::AMobMasherInstance() {
	PowerEffects = { UItemArtifactDamageIncrease::StaticClass() };
	bHasManualCooldownActivation = true;
	Effect = UMobMashGameplayEffect ::StaticClass();
	bCanFail = true;
}

float AMobMasherInstance::GetStats(EItemStats stat) const {
	switch (stat) {
	case EItemStats::HighestDamage:
	case EItemStats::LowestDamage:
		return ShockwaveDamage;
	}
	return -1;
}

void AMobMasherInstance::BeginPlay() {
	Super::BeginPlay();
	InstanceTracker<AMobMasherInstance>::AddInstance(GetWorld(), this);

	auto owner = GetCharacterOwner();
	
	//Slightly hacky way to get around server forced activation and busy status
	if(owner && owner->IsLocallyControlled() && !HasAuthority()) {
		auto abilitySystem = GetCharacterOwner()->GetAbilitySystemComponent();
		abilitySystem->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag("StatusEffect.MobMash")).AddUObject(this, &AMobMasherInstance::OnMobMasherGainedLocally);
	}
}

void AMobMasherInstance::EndPlay(EEndPlayReason::Type Reason) {
	Super::EndPlay(Reason);
	InstanceTracker<AMobMasherInstance>::RemoveInstance(GetWorld(), this);
	if (ActiveStatusEffectHandle.IsValid()) {
		auto abilitySystem = GetCharacterOwner()->GetAbilitySystemComponent();
		abilitySystem->RemoveActiveGameplayEffect(ActiveStatusEffectHandle);
	}
}

void AMobMasherInstance::Activate(const FPredictionKey& predictionKey) {
	auto abilitySystem = GetCharacterOwner()->GetAbilitySystemComponent();

	if(CanActivate()) {
		FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
		FGameplayEffectSpec spec(Cast<UMobMashGameplayEffect>(Effect->GetDefaultObject()), abilitySystem->MakeEffectContext(), ItemPower);	
		float duration = GetItemType().getDurationSeconds();
		spec.SetSetByCallerMagnitude(TEXT("Duration"), duration);
		spec.StackCount = AttackStacks;
		spec.GetContext().AddInstigator(GetOwner(), this);
		delegateHandle = abilitySystem->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag("StatusEffect.MobMash")).AddUObject(this, &AMobMasherInstance::OnMobMasherStatusChanged);
		ActiveStatusEffectHandle = abilitySystem->ApplyGameplayEffectSpecToSelf(spec, predictionKey);
		if(ActiveStatusEffectHandle.IsValid()){
			ActivationSucceeded(predictionKey);
		} else {
			Cooldown().TriggerCooldown(0.3f, predictionKey);
		}
	}	
	Super::Activate(predictionKey);
}

bool AMobMasherInstance::CanActivate() const {
	auto abilitySystem = GetCharacterOwner()->GetAbilitySystemComponent();
	return Super::CanActivate() && !abilitySystem->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("StatusEffect.MobMash"));
}

bool AMobMasherInstance::IsBusy() const {
	return ActiveStatusEffectHandle.IsValid() || bLocallyBusy;
}

void AMobMasherInstance::OnMobMasherStatusChanged(const FGameplayTag tag, const int32 tagCount) {
	auto meleeAttack = GetOwner()->FindComponentByClass<UMeleeAttackComponent>();

	if(tagCount > 0 && meleeAttack) {
		meleeAttack->OnMeleeDamageDealt.AddUObject(this, &AMobMasherInstance::OnMeleeDamageDealt);
		meleeAttack->OnAfterMeleeDamageDealt.AddUObject(this, &AMobMasherInstance::OnAfterMeleeDamageDealt);
	} else if(tagCount <= 0) {
		if(HasAuthority()) {
			AttackedMobsPerMash.Empty();	
			Cooldown().TriggerCooldown(CalculateCooldown());
		}

		if(meleeAttack) {
			meleeAttack->OnMeleeDamageDealt.RemoveAll(this);
			meleeAttack->OnAfterMeleeDamageDealt.RemoveAll(this);	
		}
		
		auto characterOwner = GetCharacterOwner();
		characterOwner->GetAbilitySystemComponent()->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag("StatusEffect.MobMash")).Remove(delegateHandle);
		ActiveStatusEffectHandle.Invalidate();
	}
}

void AMobMasherInstance::OnMobMasherGainedLocally(const FGameplayTag tag, const int32 tagCount) {
	static FGameplayEffectQuery query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(FGameplayTagContainer(FGameplayTag::RequestGameplayTag("StatusEffect.MobMash")));

	auto abilitySystem = GetCharacterOwner()->GetAbilitySystemComponent();
	if(tagCount > 0) {
		bLocallyBusy = algo::any_of(abilitySystem->GetActiveEffects(query), RETLAMBDA(abilitySystem->GetActiveGameplayEffect(it)->Spec.GetContext().GetEffectCauser() == this));
	} else {
		bLocallyBusy = false;
	}
}

void AMobMasherInstance::SpawnShockWave(FMobToMash mob) {
	if (mob.Mob) {
		FActorSpawnParameters params;
		params.Owner = GetOwner();
		params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		auto location = mob.Mob->GetActorLocation();
		if (const auto actorCapsule = mob.Mob->GetCapsuleComponent()) {
			location.Z -= actorCapsule->GetScaledCapsuleHalfHeight_WithoutHemisphere();
		}
		auto transform = FTransform(location);
		auto actor = GetWorld()->SpawnActorDeferred<AMobMashShockWave>(mob.Primary ? MobMashShockWavePrimaryClass : MobMashShockWaveSecondaryClass, transform, GetOwner(), nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		const float scaledItemPower = GetPowerEffect()->GetMultiplier(ItemPower);
		const float scaledDamage = ShockwaveDamage * scaledItemPower;
		actor->SetDamage(scaledDamage);
		actor->FinishSpawning(transform);
	}
}

void AMobMasherInstance::SpawnShockwaveDelayed(const FMobToMash& mob, float delay) {
	FTimerHandle handle;
	GetWorld()->GetTimerManager().SetTimer(handle, FTimerDelegate::CreateUObject(this, &AMobMasherInstance::SpawnShockWave, mob), delay, false);
}

void AMobMasherInstance::MulticastSpawnShockwave_Implementation(const FMobToMash& mob, float delay, FPredictionKey key) {
	if (!key.IsLocalClientKey() || !ActiveStatusEffectHandle.IsValid() || HasAuthority()) {
		if (delay > 0.f) {
			SpawnShockwaveDelayed(mob, delay);
		}
		else {
			SpawnShockWave(mob);
		}
	}
}

void AMobMasherInstance::OnMeleeDamageDealt(bool successfulAttack, const ABaseCharacter* fromCharacter, const ABaseCharacter* toCharacter, const FSharedPredictionContext& context) {
	auto owner = GetCharacterOwner();
	auto abilitySystem = owner->GetAbilitySystemComponent();
	if (successfulAttack && !IsMaxCountReached() && !IsMobMaxStrikeCountReached(toCharacter)) {
		auto pc = Cast<ABasePlayerController>(owner->GetController());
		MobsToMash.Emplace(toCharacter,false); 
		AttackedMobsPerMash.Add(toCharacter);
	}
}

void AMobMasherInstance::OnAfterMeleeDamageDealt(bool successfulAttack, const ABaseCharacter* fromCharacter, const FSharedPredictionContext& context) {
	if(MobsToMash.Num() > 0){
		ExecuteMobMash(context);
		MobsToMash.Empty();
		if(HasAuthority()) {

			auto abilitySystem = GetCharacterOwner()->GetAbilitySystemComponent();
			abilitySystem->RemoveActiveGameplayEffect(ActiveStatusEffectHandle, 1);
		}
	}
}

void AMobMasherInstance::ExecuteMobMash(const FSharedPredictionContext& context) {
	auto owner = Cast<ABaseCharacter>(GetOwner());
	MobsToMash.Sort([owner](const FMobToMash& a, const FMobToMash& b) { return owner->GetDistanceTo(a.Mob) < owner->GetDistanceTo(b.Mob); });
	MobsToMash[0].Primary = true;
	
	auto delay = SpawnDelayInitial;
	const int count = MobsToMash.Num();
	const float SpawnDelayPerTargetScaled = SpawnDelayPerTarget / (1.0f + static_cast<float>(count)*SpawnDelayNumTargetsSpeedUpFactor);
	for (auto mob : MobsToMash) {
		if (mob.Mob) {

			FGameplayCueParameters params;
			params.Location = mob.Mob->GetActorLocation();			
			params.Instigator = GetOwner();
			params.RawMagnitude = delay;
			mob.Mob->GetAbilitySystemComponent()->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.MobMash.WillExplode"), params);
			
			if (HasAuthority()) {
				MulticastSpawnShockwave(mob, delay, context.GetKey());
			}
			else {
				SpawnShockwaveDelayed(mob, delay);
			}
			delay += SpawnDelayPerTargetScaled;			
		}
	}
}

bool AMobMasherInstance::IsMaxCountReached() const {
	return MaxTimesPerAttack > -1 ? MobsToMash.Num() >= MaxTimesPerAttack : false;
}

bool AMobMasherInstance::IsMobMaxStrikeCountReached(const ABaseCharacter* mob) const {
	return MaxTimesPerMob > -1 ? (algo::count_if(AttackedMobsPerMash, RETLAMBDA(it == mob)) >= MaxTimesPerMob) : false;
}

UMobMashGameplayEffect::UMobMashGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	FSetByCallerFloat durationMagnitude;
	durationMagnitude.DataName = TEXT("Duration");
	
	StackLimitCount = 4;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;	
	DurationMagnitude = durationMagnitude;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.MobMash"));
	
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.MobMash.PoweredUp"), 0, 1);
	GameplayCues.Last().GameplayCueTags.AddTag(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.MobMash"));
}


UMobMashShockwaveDamageGameplayEffect::UMobMashShockwaveDamageGameplayEffect() {
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Damage.Item"), 0, 1);
	GameplayCues.Last().MagnitudeAttribute = UHealthAttributeSet::HealthAttribute();
}

FMobToMash::FMobToMash(const ABaseCharacter* mob, bool primary) 
	: Mob(mob)
	, Primary(primary) {
}
