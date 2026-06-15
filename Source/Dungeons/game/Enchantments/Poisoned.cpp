#include "Dungeons.h"
#include "Poisoned.h"

#include "AbilitySystemComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include <GameplayEffect.h>
#include "game/actor/character/mob/MobCharacter.h"
#include <GameplayTagContainer.h>
#include "game/util/DungeonsEffectLibrary.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/effects/calculations/DamageModCalculations.h"
#include "game/abilities/effects/calculations/StatusModCalculations.h"
#include "game/abilities/effects/executions/DamageExecutionCalculation.h"
#include <AbilitySystemGlobals.h>
#include "util/CharacterQuery.h"
#include "game/item/power/ItemPowerStats.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/actor/EffectGroup/EffectGroupContainer.h"
#include "game/actor/ProjectileManager.h"

extern TAutoConsoleVariable<int32> CVarDebugDrawEnchantments;


TAutoConsoleVariable<int32> CVarCloudPoisonedPerTick(
	TEXT("Dungeons.Perf.CloudPoisonedPerTick"),
	4,
	TEXT("The number of Characters per tick that will be poisoned by cloud (higher numbers could spike when they generate more effects/actors ect)"),
	ECVF_Cheat
);

UPoisonCloudDamageGameplayEffect::UPoisonCloudDamageGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	Period = 0.25f;

	FCustomCalculationBasedFloat durationMagnitude;
	durationMagnitude.CalculationClassMagnitude = UStatusDurationModCalculation::StaticClass();
	DurationMagnitude = durationMagnitude;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;

	bExecutePeriodicEffectOnApplication = true;
	bRequireModifierSuccessToTriggerCues = true;
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Damage.Medium.Poison")));
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Damage.Elemental.Poison")));

	FGameplayEffectExecutionDefinition damage;
	damage.CalculationClass = UDamageExecutionCalculation::StaticClass();
	Executions.Add(std::move(damage));

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.Negative")));

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Damage"), 0, 1);
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Damage.Medium"), 0, 1);
}

void UPoisonCloudDamageGameplayEffect::AddHealthModifier(UClass* modDamageCalculationClass) {
	FCustomCalculationBasedFloat healthMagnitude;	
	healthMagnitude.CalculationClassMagnitude = modDamageCalculationClass;

	FGameplayModifierInfo healthInfo;
	healthInfo.Attribute = UHealthAttributeSet::HealthAttribute();
	healthInfo.ModifierMagnitude = healthMagnitude;
	healthInfo.ModifierOp = EGameplayModOp::Type::Additive;
	Modifiers.Add(healthInfo);
}

UPoisonRangedCloudDamageGameplayEffect::UPoisonRangedCloudDamageGameplayEffect() {
	AddHealthModifier(URangedItemPowerOnlyModDamageCalculation::StaticClass());
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Enchantment.Poisoned"), 0, 1);
	GameplayCues.Last().MagnitudeAttribute = UHealthAttributeSet::HealthAttribute();
}

UPoisonMeleeCloudDamageGameplayEffect::UPoisonMeleeCloudDamageGameplayEffect() {
	AddHealthModifier(UMeleeItemPowerOnlyModDamageCalculation::StaticClass());
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Enchantment.Poisoned"), 0, 1);
	GameplayCues.Last().MagnitudeAttribute = UHealthAttributeSet::HealthAttribute();
}

UPoisonItemCloudDamageGameplayEffect::UPoisonItemCloudDamageGameplayEffect() {
	AddHealthModifier(UItemDamageModCalculation::StaticClass());
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Enchantment.Poisoned"), 0, 1);
	GameplayCues.Last().MagnitudeAttribute = UHealthAttributeSet::HealthAttribute();
}

UPoisonedRanged::UPoisonedRanged() {
	TypeId = EEnchantmentTypeID::PoisonedRanged;
	PredictiveExecution = false;

	LevelMultiplier = [this](int level) -> float {
		return BaseDamagePerSecond * level;
	};
	ItemPowerMultiplier = game::item::power::DamageIncreaseMultiplier;
	MultiplierFormatter = valueformat::asDamage;
}

FText UPoisonedRanged::CreateDescription() const {
	return FText::Format(Super::CreateDescription(), FText::FromString(valueformat::asPercentageChance(TriggerChance)), FText::FromString(valueformat::asForRoundedWordSecond(CloudDuration)));
}


void UPoisonedRanged::OnAfterDealtRangedDamage(float Damage, ABaseProjectile* fromProjectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream) {
	if (!GetOwner()->HasAuthority()) {
		return;
	}
	if ((randStream.FRandRange(0.0f, 1.0f) <= TriggerChance) || bAlwaysTrigger) {
		MulticastSpawnCloudEffect(FTransform(atLocation));
		UPoisonCloudSpawnHelper::SpawnPoisonCloud(PosionCloudClass, UPoisonRangedCloudDamageGameplayEffect::StaticClass(), GetOwner(), atLocation, Level, IsOwnerMob() ? MobDamagePerSecond : LevelMultiplier(Level), IsOwnerMob() ? MobCloudDuration : CloudDuration);
		BroadcastEnchantmentTriggeredEvent();
	}
}

UPoisonedMelee::UPoisonedMelee() {
	TypeId = EEnchantmentTypeID::PoisonedMelee;
	PredictiveExecution = false;

	LevelMultiplier = [this](int level) -> float {
		return BaseDamagePerSecond * level;
	};
	ItemPowerMultiplier = game::item::power::DamageIncreaseMultiplier;
	MultiplierFormatter = valueformat::asDamage;
}

FText UPoisonedMelee::CreateDescription() const {
	return FText::Format(Super::CreateDescription(), FText::FromString(valueformat::asPercentageChance(TriggerChance)), FText::FromString(valueformat::asDurationRoundedWordSecond(CloudDuration)));
}


void UPoisonedMelee::OnAfterDealtMeleeDamage(float Damage, bool missedAttack, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream, FSharedPredictionContext window) {
	if (!GetOwner()->HasAuthority()) {
		return;
	}
	if ((randStream.FRandRange(0.0f, 1.0f) < TriggerChance) || bAlwaysTrigger) {
		MulticastSpawnCloudEffect(FTransform(atLocation));
		UPoisonCloudSpawnHelper::SpawnPoisonCloud(PosionCloudClass, UPoisonMeleeCloudDamageGameplayEffect::StaticClass(), GetOwner(), atLocation, Level, IsOwnerMob() ? MobDamagePerSecond : LevelMultiplier(Level), IsOwnerMob() ? MobCloudDuration : CloudDuration);
		BroadcastEnchantmentTriggeredEvent();
	}
}

APoisonCloud::APoisonCloud() {
	bReplicates = false;
	LifeTime = 2.5f;
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Sphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(static_cast<ECollisionChannel>(ECustomTraceChannels::PlayerPawn), ECollisionResponse::ECR_Overlap);
}

void APoisonCloud::Tick(float DeltaSecs)
{
	if (PoisonList.Num() > 0)
	{
		int32 iNumToPoison = FMath::Min(PoisonList.Num(), CVarCloudPoisonedPerTick.GetValueOnGameThread());
		
		while (iNumToPoison--)
		{
			auto TargetChar = PoisonList.Pop(false);
			if (TargetChar.IsValid())
			{
				FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);

				UAbilitySystemComponent* abilitySystem = TargetChar->GetAbilitySystemComponent();
				if (abilitySystem)
				{
					abilitySystem->ApplyGameplayEffectSpecToSelf(Spec);
				}
			}
		}
	}

	Super::Tick(DeltaSecs);
}

void APoisonCloud::BeginPlay() {
	Super::BeginPlay();
	InitialLifeSpan = 0.0f;
		
	if (HasAuthority()) {
		Sphere->SetSphereRadius(AreaOfEffect);
		PoisonList.Reserve(32);
	}
}

void APoisonCloud::NotifyActorBeginOverlap(AActor* OtherActor) {
	if (HasAuthority() && bDealingDamage) {
		ABaseCharacter* target = Cast<ABaseCharacter>(OtherActor);
		ABaseCharacter* owner = GetCharacterOwner();
		if (target && owner && actorquery::is::alive(target) && characterquery::is::targetable(target) && !owner->IsFriendlyTowards(target)) {
			if (!characterClassesToExclude.FindByPredicate([&target](const auto& excludeClass) { return target->GetClass()->IsChildOf(excludeClass); }))
			{
				PoisonList.Push(target);
			}
		}
	}
}

void APoisonCloud::NotifyActorEndOverlap(AActor* OtherActor) {
	if (HasAuthority()) {
		if (ABaseCharacter* target = Cast<ABaseCharacter>(OtherActor)) {
			if (PoisonList.RemoveSwap(target) == 0) //check if we have got round to poisoning it yet first
			{
				RemoveEffectOnCharacter(target);
			}
		}
	}
}

void APoisonCloud::Reset()
{
	Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void APoisonCloud::SetGameplaySpec(FGameplayEffectSpec&& spec) {
	Spec = MoveTemp(spec);
}

void APoisonCloud::StartCloud(float lifeTime)
{
	bDealingDamage = true;
	LifeTime = lifeTime;
	if (HasAuthority())
	{
		Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		if (CVarDebugDrawEnchantments.GetValueOnGameThread()) DrawDebugSphere(GetWorld(), GetActorLocation(), AreaOfEffect, 50, FColor::Green, false, lifeTime);
	}
	GetWorld()->GetTimerManager().SetTimer(DamageDisableTimerHandle, this, &APoisonCloud::OnDamageDisabled, lifeTime - DamageDisableTimeBeforeDestroy);
	SetActorTickEnabled(true);

	OnCloudStarted();
}

class ABaseCharacter* APoisonCloud::GetCharacterOwner() const {
	return Cast<ABaseCharacter>(GetOwner());
}

void APoisonCloud::OnDamageDisabled_Implementation() {
	bDealingDamage = false;
	TArray<AActor*> overlappingactors;
	GetOverlappingActors(overlappingactors);

	for (auto actor : overlappingactors) {
		if (auto character = Cast<ABaseCharacter>(actor)) {
			RemoveEffectOnCharacter(character);
		}
	}
}



void APoisonCloud::RemoveEffectOnCharacter(ABaseCharacter* character) {
	auto abilitySystem = character->GetAbilitySystemComponent();
	FGameplayEffectQuery query;
	query.EffectDefinition = Spec.Def->GetClass();
	abilitySystem->RemoveActiveEffects(query);
}

APoisonCloud* UPoisonCloudSpawnHelper::SpawnPoisonCloud(const TSubclassOf<APoisonCloud> poisonCloudClass, const TSubclassOf<UPoisonCloudDamageGameplayEffect> gameplayEffect, AActor* owner, const FVector& location, float level, float damagePerSecond, float duration) {
	FTransform transform;
	transform.SetLocation(location);

	auto world = owner->GetWorld();
	if (APoisonCloud* poisonCloud = Cast<APoisonCloud>(AProjectileActorManager::Pop_ProjectileProp(poisonCloudClass, transform, world)))
	{
		poisonCloud->SetOwner(owner);

		auto spec = CreatePoisonCloudDamageEffectSpec(gameplayEffect, owner, location, level, damagePerSecond, poisonCloud, duration);
		poisonCloud->SetGameplaySpec(MoveTemp(spec));
		
		poisonCloud->StartCloud(duration);

		return poisonCloud;
	}

	return nullptr;

}

FGameplayEffectSpec UPoisonCloudSpawnHelper::CreatePoisonCloudDamageEffectSpec(const TSubclassOf<UPoisonCloudDamageGameplayEffect> gameplayEffect, AActor* owner, const FVector& location, const float level, float damagePerSecond, APoisonCloud* poisonCloud, float duration) {
	const auto abilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(owner);
	
	auto spec = FGameplayEffectSpec(gameplayEffect->GetDefaultObject<UGameplayEffect>(), abilitySystem->MakeEffectContext(), level);
	
	spec.GetContext().AddInstigator(owner, owner);
	spec.GetContext().AddOrigin(location);
	effects::SetStunMultiplier(spec, effects::ENCHANTMENT_STUN_MULTIPLIER);
		
	spec.SetSetByCallerMagnitude(effects::HealthName, -damagePerSecond * spec.Period);
	spec.SetSetByCallerMagnitude(effects::DurationName, duration);

	return spec;
}

void UPoisoned::BeginPlay() {
	Super::BeginPlay();
}

void UPoisoned::MulticastSpawnCloudEffect_Implementation(const FTransform& transform) {
	UEffectGroupContainer::SpawnEffectInGroup(this, GetEffectClass(), transform, EffectGroupParameters);
}

TSubclassOf<AReusedEffectActor> UPoisoned::GetEffectClass() {
	for (auto effect : EffectMap) {
		if (GetOwner()->IsA(effect.Key)) {
			return effect.Value;
		}
	}
	return AActor::StaticClass();
}

void UPoisoned::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void UPoisoned::OnStart()
{
	if (PosionCloudClass)
	{
		PreCachePoisonClass();
	}
}

void UPoisoned::PreCachePoisonClass()
{
	if(auto* pWorld = GetWorld())
	{
		if (InstanceTracker< AProjectileActorManager >::GetList(pWorld).Num() > 0)
		{
			AProjectileActorManager::PreCache_ProjectilePropClass(PosionCloudClass, pWorld, 32);
		}
		else
		{
			//If we havent set up the projectile manager yet, try again shortly
			FTimerHandle TimerHandle;
			pWorld->GetTimerManager().SetTimer(TimerHandle, this, &UPoisoned::PreCachePoisonClass, 0.5f, false);
		}
	}
}
