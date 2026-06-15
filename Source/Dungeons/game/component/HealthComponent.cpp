#include "Dungeons.h"
#include "HealthComponent.h"
#include "DungeonsGameMode.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "game/Game.h"
#include "AbilitySystemComponent.h"
#include "BackpackComponent.h"
#include "game/util/ValueFormat.h"
#include <GameplayTagContainer.h>
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "GameplayEffectExtension.h"
#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"
#include "game/abilities/effects/executions/DamageExecutionCalculation.h"
#include <Stats.h>
#include "game/abilities/effects/calculations/HealingModCalculations.h"
#include "DungeonsGameInstance.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/abilities/effects/executions/RespawnExecutionCalculation.h"
#include "online/sessions/OnlineUtil.h"
#include "online/seasons/LiveOps.h"

DECLARE_STATS_GROUP(TEXT("HealthComponent"), STATGROUP_HealthComponent, STATCAT_Advanced);
DECLARE_CYCLE_STAT(TEXT("HealthChange"), STAT_HealthChange, STATGROUP_HealthComponent);

UHealthRegenerationGameplayEffect::UHealthRegenerationGameplayEffect() : UHealSelfGameplayEffect()
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	FSetByCallerFloat durationMagnitude;
	durationMagnitude.DataName = FName(TEXT("Duration"));
	DurationMagnitude = durationMagnitude;
	Period = tickRate;
}

UHealSelfGameplayEffect::UHealSelfGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::Instant;

	Modifiers.Add(GetHealingInfo(EGameplayModOp::Type::Additive));
	
	FGameplayEffectExecutionDefinition damage;
	damage.CalculationClass = UDamageExecutionCalculation::StaticClass();
	Executions.Add(std::move(damage));
}

FGameplayModifierInfo UHealSelfGameplayEffect::GetHealingInfo(const EGameplayModOp::Type modifierOperation) {
	FCustomCalculationBasedFloat healthMagnitude;
	healthMagnitude.CalculationClassMagnitude = UHealingModCalculation::StaticClass();

	FGameplayModifierInfo healthInfo;
	healthInfo.Attribute = UHealthAttributeSet::HealthAttribute();
	healthInfo.ModifierMagnitude = healthMagnitude;	
	healthInfo.ModifierOp = modifierOperation;

	return healthInfo;
}

UReviveGameplayEffect::UReviveGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::Instant;

	Modifiers.Add(UHealSelfGameplayEffect::GetHealingInfo(EGameplayModOp::Type::Override));

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Revive")));
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Revive"), 0, 1);
	GameplayCues.Last().MagnitudeAttribute = UHealthAttributeSet::HealthAttribute();
}

UKillGameplayEffect::UKillGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::Instant;

	Modifiers.Add(UHealSelfGameplayEffect::GetHealingInfo(EGameplayModOp::Type::Override));

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Kill")));
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Kill"), 0, 1);
	GameplayCues.Last().MagnitudeAttribute = UHealthAttributeSet::HealthAttribute();
}

URespawnGameplayEffect::URespawnGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::Instant;
	FGameplayEffectExecutionDefinition respawnCall;
	respawnCall.CalculationClass = URespawnExecutionCalculation::StaticClass();

	Executions.Add(std::move(respawnCall));
}

UFreezeRespawnGameplayEffect::UFreezeRespawnGameplayEffect() : URespawnGameplayEffect() {
	Executions.Last().PassedInTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Respawn.Freeze")));
}

USetHealthThresholdGameplayEffect::USetHealthThresholdGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FSetByCallerFloat healthMagnitude;
	healthMagnitude.DataName = TEXT("HealthThreshold");

	FGameplayModifierInfo healthInfo;
	healthInfo.Attribute = UHealthAttributeSet::HealthThresholdAttribute();
	healthInfo.ModifierMagnitude = healthMagnitude;
	healthInfo.ModifierOp = EGameplayModOp::Type::Override;
	Modifiers.Add(healthInfo);
}

UHealthComponent::UHealthComponent() {
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicated(true);
}

void UHealthComponent::BeginPlay() {
	Super::BeginPlay();

	auto abilitySystem = GetAbilitySystem();
	check(abilitySystem->GetSet<UHealthAttributeSet>() != nullptr && "All actors with a UHealthComponent should also have the UHealthAttributeSet");

	if (GetOwnerRole() == ROLE_Authority) {
		abilitySystem->SetNumericAttributeBase(UHealthAttributeSet::MaxHealthAttribute(), MaxHealth);
		abilitySystem->SetNumericAttributeBase(UHealthAttributeSet::HealthAttribute(), Health == MaxHealth ? abilitySystem->GetNumericAttribute(UHealthAttributeSet::MaxHealthAttribute()) : Health);
		abilitySystem->SetNumericAttributeBase(UHealthAttributeSet::ShieldAttribute(), Shield ? 1.0f : 0.0f);
		abilitySystem->SetNumericAttributeBase(UHealthAttributeSet::ResistDeathAttribute(), ResistDeath ? 1.0f : 0.0f); // D11.DB
		if (HealthResistThresholds.Num()) {
			FGameplayEffectSpec spec = effects::CreateGameplayEffectSpec<USetHealthThresholdGameplayEffect>(GetAbilitySystem(), 1.0f);
			spec.SetSetByCallerMagnitude("HealthThreshold", HealthResistThresholds[0]);
			GetAbilitySystem()->ApplyGameplayEffectSpecToSelf(spec);
		}
	}

	PreviouslySeenHealth = abilitySystem->GetNumericAttributeBase(UHealthAttributeSet::HealthAttribute());

	abilitySystem->GetGameplayAttributeValueChangeDelegate(UHealthAttributeSet::HealthAttribute()).AddUObject(this, &UHealthComponent::OnAttributeHealthChange);
	abilitySystem->GetGameplayAttributeValueChangeDelegate(UHealthAttributeSet::ShieldAttribute()).AddUObject(this, &UHealthComponent::OnAttributeShieldChange);

	if (GetOwnerRole() == ROLE_Authority) {
		abilitySystem->GetGameplayAttributeValueChangeDelegate(UHealthAttributeSet::MaxHealthAttribute()).AddUObject(this, &UHealthComponent::OnAttributeMaxHealthChange);
	}

	OnChanged.Broadcast(FOnAttributeChangeData());
}

void UHealthComponent::ApplyHeal(const float heal) {

	if (IsNotAlive() || !GetOwner()->HasAuthority()) {
		return;
	}


	if (!IsInGameThread())
	{
		//Push the to the task graph to be executed from the game thread later
		TWeakObjectPtr<UHealthComponent> WeakThisComponent = this;
		FFunctionGraphTask::CreateAndDispatchWhenReady([WeakThisComponent, heal]()
		{
			if (WeakThisComponent.IsValid())
			{
				WeakThisComponent->ApplyHeal(heal);
			}
		}, TStatId(), nullptr, ENamedThreads::GameThread);

		return;
	}


	const auto healthIncrease = GetHealthIncrease(heal);

	auto spec = effects::CreateGameplayEffectSpec<UHealSelfGameplayEffect>(GetAbilitySystem(), 1.f);	
	spec.SetSetByCallerMagnitude(effects::HealthName, healthIncrease);
	spec.GetContext().AddOrigin(GetOwner()->GetActorLocation());
	GetAbilitySystem()->ApplyGameplayEffectSpecToSelf(spec);	

	auto gameMode = Cast<ADungeonsGameMode>(GetWorld()->GetAuthGameMode());
	gameMode->OnActorHeal(GetOwner(), healthIncrease);
}

void UHealthComponent::ServerApplyHeal_Implementation(float heal) {
	ApplyHeal(heal);
}

bool UHealthComponent::ServerApplyHeal_Validate(float heal) {
	return true;
}

void UHealthComponent::Revive(const float healthPercentage) {

	if (!IsInGameThread())
	{
		//Push the to the task graph to be executed from the game thread later
		TWeakObjectPtr<UHealthComponent> WeakThisComponent = this;
		FFunctionGraphTask::CreateAndDispatchWhenReady([WeakThisComponent, healthPercentage]()
		{
			if (WeakThisComponent.IsValid())
			{
				WeakThisComponent->Revive(healthPercentage);
			}
		}, TStatId(), nullptr, ENamedThreads::GameThread);

		return;
	}

	DeathDamageTags.Reset();

	auto abilitySystem = GetAbilitySystem();

	auto spec = effects::CreateGameplayEffectSpec<UReviveGameplayEffect>(abilitySystem, 1.f);
	spec.SetSetByCallerMagnitude(effects::HealthName, GetMaximumHealth() * healthPercentage);
	spec.GetContext().AddOrigin(GetOwner()->GetActorLocation());
	FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
	abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
}

void UHealthComponent::Kill() {

	if(GetOwnerRole() != ROLE_Authority) return;

	if (!IsInGameThread())
	{
		//Push the to the task graph to be executed from the game thread later
		TWeakObjectPtr<UHealthComponent> WeakThisComponent = this;
		FFunctionGraphTask::CreateAndDispatchWhenReady([WeakThisComponent]()
		{
			if (WeakThisComponent.IsValid())
			{
				WeakThisComponent->Kill();
			}
		}, TStatId(), nullptr, ENamedThreads::GameThread);

		return;
	}

	if (FMath::IsNearlyEqual(GetCurrentHealth(), 0.0f))
		return;

	if (auto owner = GetOwner()) {
		GetAbilitySystem()->SetNumericAttributeBase(UHealthAttributeSet::HealthAttribute(), 0.f);
	}
}

void UHealthComponent::KillWith(const FGameplayTag damageType) {

	if (!IsInGameThread())
	{
		//Push the to the task graph to be executed from the game thread later
		TWeakObjectPtr<UHealthComponent> WeakThisComponent = this;
		FFunctionGraphTask::CreateAndDispatchWhenReady([WeakThisComponent, damageType]()
		{
			if (WeakThisComponent.IsValid())
			{
				WeakThisComponent->KillWith(damageType);
			}
		}, TStatId(), nullptr, ENamedThreads::GameThread);

		return;
	}

	if (const auto owner = GetOwner()) {
		if (auto abilitySystem = GetAbilitySystem()) {
			abilitySystem->RemoveActiveEffectsWithTags(FGameplayTagContainer{ FGameplayTag::RequestGameplayTag("StatusEffect.Stunned") });

			auto spec = effects::CreateGameplayEffectSpec<UKillGameplayEffect>(abilitySystem, 1.f);
			spec.SetSetByCallerMagnitude(effects::HealthName, 0.0f);
			spec.GetContext().AddOrigin(owner->GetActorLocation());
			spec.DynamicAssetTags.AddTag(damageType);

			abilitySystem->ApplyGameplayEffectSpecToSelf(spec);

			if (const auto backpackComponent = owner->FindComponentByClass<UMochilaComponent>()) {
				backpackComponent->DropAtStoredLocation();
			}
		}
	}
}

bool UHealthComponent::IsNotAlive() const {
	return GetCurrentHealth() <= 0.f;
}

bool UHealthComponent::IsAlive() const {
	return GetCurrentHealth() > 0.f;
}

bool UHealthComponent::IsHealthMaxed() const {
	return GetCurrentHealth() >= GetMaximumHealth();
}

bool UHealthComponent::IsDeathPrevented() const {
	return IsMissionCompleted();
}

bool UHealthComponent::IsMissionCompleted() const {	
	if (const auto world = GetWorld()) {
		if (const auto game = actorquery::getGame(world)) {
			return !game->IsLobbyLevel() && game->isCompleted();
		}
	}	
	return false;
}

bool UHealthComponent::ShouldImpactProjectile(const FGameplayTag& damageType) const {
	return impactFilter::shouldImpact(ImpactFilter, damageType);
}

float UHealthComponent::GetCurrentHealthPercentage() const {
	return FMath::Clamp(GetCurrentHealth() / GetMaximumHealth(), 0.0f, 1.0f);	 
}

int UHealthComponent::GetHealthAsDisplayAmount(const float damage) {
	return valueformat::healthAsDisplayAmount(damage);
}

bool UHealthComponent::GetShield() const {
	if (const auto abilitySystem = GetAbilitySystem()) {
		const auto shieldValue = abilitySystem->GetNumericAttribute(UHealthAttributeSet::ShieldAttribute());
		return FMath::IsNearlyEqual(shieldValue, 1.0f);
	}

	return false;
}

float UHealthComponent::GetCurrentHealth() const {
	if (PreviouslySeenHealth) {
		return PreviouslySeenHealth.GetValue();
	}
	if (const auto abilitySystem = GetAbilitySystem()) {
		return abilitySystem->GetNumericAttributeBase(UHealthAttributeSet::HealthAttribute());
	}

	return 0.0f;
}

float UHealthComponent::GetMaximumHealth() const {
	if (const auto abilitySystem = GetAbilitySystem()) {
		return abilitySystem->GetNumericAttribute(UHealthAttributeSet::MaxHealthAttribute());
	}

	return 0.0f;
}

float UHealthComponent::GetMaximumHealthBase() const {
	if (const auto abilitySystem = GetAbilitySystem()) {
		return abilitySystem->GetNumericAttributeBase(UHealthAttributeSet::MaxHealthAttribute());
	}

	return 0.0f;
}

float UHealthComponent::GetConstantDesignedMaximumHealth() const {
	return MaxHealth;
}

float UHealthComponent::GetDamageMultiplier(const FGameplayTagContainer& damageTypes) const {
	return damageFilter::getMultiplier(DamageFilter, damageTypes);
}

void UHealthComponent::SetHealthThresholds(TArray<float> newHealthThresholds)
{
	if (HealthResistThresholds.Num() == 0) {
		newHealthThresholds.Sort([](const float& LHS, const float& RHS) { return LHS > RHS; });
		HealthResistThresholds.Append(newHealthThresholds);
		if (GetOwnerRole() == ROLE_Authority) {
			FGameplayEffectSpec spec = effects::CreateGameplayEffectSpec<USetHealthThresholdGameplayEffect>(GetAbilitySystem(), 1.0f);
			spec.SetSetByCallerMagnitude("HealthThreshold", HealthResistThresholds[0]);
			GetAbilitySystem()->ApplyGameplayEffectSpecToSelf(spec);
		}
	}
}

FGameplayTagContainer UHealthComponent::GetDeathDamageTags() const
{
	return DeathDamageTags;
}

void UHealthComponent::OnAttributeMaxHealthChange(const FOnAttributeChangeData& data) {
	auto abilitySystem = GetAbilitySystem();
	
	const auto currentHealth = abilitySystem->GetNumericAttribute(UHealthAttributeSet::HealthAttribute());
	const auto normalizedHealth = FMath::Clamp(currentHealth / data.OldValue, 0.f, 1.f);
	const auto newHealth = data.NewValue * normalizedHealth;

	abilitySystem->SetNumericAttributeBase(UHealthAttributeSet::HealthAttribute(), newHealth);
}

void UHealthComponent::OnAttributeHealthChange(const FOnAttributeChangeData& data) {
	SCOPE_CYCLE_COUNTER(STAT_HealthChange)

	const float oldValue = PreviouslySeenHealth.Get(0);
	const float newValue = GetOwnerRole() == ROLE_Authority ? data.NewValue : GetAbilitySystem()->GetNumericAttributeBase(UHealthAttributeSet::HealthAttribute());
	PreviouslySeenHealth = newValue;

	UE_LOG(LogDungeonsNetwork, Verbose, TEXT("UHealthComponent::OnAttributeHealthChange GEMod(%p), Authority(%d). new %f, old %f"), data.GEModData, GetOwner()->HasAuthority(), newValue, oldValue);

	const float healthChange = newValue - oldValue;
	if (FMath::IsNearlyZero(healthChange)) {
		return;
	}

	const float healthFraction = GetCurrentHealthPercentage();
	const float oldFraction = CachedHealthFraction.Get(1.f);
	const bool healthFractionChanged = !FMath::IsNearlyEqual(healthFraction, oldFraction);

	if (GetOwnerRole() == ROLE_Authority && healthFractionChanged) {
		if (HealthResistThresholds.Num() > CurrentHealthThreshold + 1) {
			if (FMath::IsNearlyEqual(HealthResistThresholds[CurrentHealthThreshold], healthFraction, 0.01f) && HealthResistThresholds[CurrentHealthThreshold] < oldFraction) {
				CurrentHealthThreshold++;
				FGameplayEffectSpec spec = effects::CreateGameplayEffectSpec<USetHealthThresholdGameplayEffect>(GetAbilitySystem(), 1.0f);
				spec.SetSetByCallerMagnitude("HealthThreshold", HealthResistThresholds[CurrentHealthThreshold]);
				GetAbilitySystem()->ApplyGameplayEffectSpecToSelf(spec);
			}
		}
	}

	if (newValue <= 0.f && oldValue > 0.f) {
		//Give On Before death a chance to save the player
		OnBeforeDeath.Broadcast();
		if (GetCurrentHealth() > 0) {
			OnDamageReceived.Broadcast(-healthChange);
			BroadcastDamageAmountAndType(-healthChange, data);
			return;
		}

		if (data.GEModData)
			data.GEModData->EffectSpec.GetAllGrantedTags(DeathDamageTags);

		// Character have died.
		OnDeath.Broadcast();
	}
	else if (newValue > 0.f && oldValue <= 0.f) {
		OnRevive.Broadcast();
	}
	else if (healthChange >= 0) {
		OnHeal.Broadcast(healthChange);

		// D11.SSN
 		if (auto* playerCharacter = Cast<APlayerCharacter>(GetOwner())) {
			if (UStatTrackerComponent* tracker = playerCharacter->GetStatTracker()) {
				tracker->Heal(valueformat::healthDisplayScaled(healthChange));
			}
			if (playerCharacter->IsLocallyControlled() && healthFractionChanged) {
				online::getLiveOps(GetWorld()->GetGameInstance())->OnHealed.Broadcast(valueformat::healthDisplayScaled(healthChange), data);
			}
		}
	}
	else {
		if (healthFractionChanged) {
			OnDamageReceived.Broadcast(-healthChange);
			BroadcastDamageAmountAndType(-healthChange, data);
		}
	}

	if (GetOwner()->HasAuthority()) {
		OnServerAttributeHealthChange(data);
	}
	
	if (healthFractionChanged) {
		OnHealthFractionChanged.Broadcast(healthFraction, oldFraction);
		OnHealthFraction.Broadcast(healthFraction, oldFraction);
	}
	OnChanged.Broadcast(data);
	CachedHealthFraction = healthFraction;
}

void UHealthComponent::OnServerAttributeHealthChange(const FOnAttributeChangeData& data) const {
	const auto changeCausedDeath = data.NewValue <= 0.f && data.OldValue > 0.f;
	if (changeCausedDeath) {
		const auto owner = GetOwner();
		if (data.GEModData) {
			const auto& effectContext = data.GEModData->EffectSpec.GetEffectContext();
			auto* effectCauser = effectContext.GetEffectCauser();
			auto* instigator = Cast<ABaseCharacter>(effectContext.GetInstigator());

			if (instigator) {
				if (auto* characterEffectCauser = Cast<ABaseCharacter>(effectCauser)) {
					characterEffectCauser->OnKilledOther.Broadcast(instigator, characterEffectCauser, owner);
				} else {
					instigator->OnKilledOther.Broadcast(effectCauser, instigator, owner);
				}
			}
			if (auto gameMode = Cast<ADungeonsGameMode>(GetWorld()->GetAuthGameMode())) {
				gameMode->OnActorDeath(owner, instigator, effectCauser);
			}
		}
		else if (auto gameMode = Cast<ADungeonsGameMode>(GetWorld()->GetAuthGameMode())) {			
			gameMode->OnActorDeath(owner, owner, owner);
		}
	}
}

void UHealthComponent::OnAttributeShieldChange(const FOnAttributeChangeData& data) const {
	const auto hasShield = FMath::IsNearlyEqual(data.NewValue, 1.0f);
	OnShieldChange.Broadcast(hasShield);
}

void UHealthComponent::BroadcastDamageAmountAndType(float amount, const FOnAttributeChangeData& data) const
{
	if (OnDamageReceivedWithType.IsBound()) {
		OnDamageReceivedWithType.Broadcast(amount, data.Attribute);
	}

	if (OnDamageTypeReceived.IsBound() && data.GEModData) {
		const auto damageType = effects::GetDungeonsDamageTypes(*data.GEModData);
		OnDamageTypeReceived.Broadcast(amount, damageType);
	}
}

float UHealthComponent::GetHealthIncrease(const float heal) const {
	const auto before = GetCurrentHealth();
	const auto after = FMath::Min(GetMaximumHealth(), GetCurrentHealth() + heal);
	return after - before;
}

UAbilitySystemComponent* UHealthComponent::GetAbilitySystem() {
	if (auto* character = Cast<ABaseCharacter>(GetOwner())) {
		return character->GetAbilitySystemComponent();
	}

	check(false && "No AbilitySystemComponent available.");
	return nullptr;
}

const UAbilitySystemComponent* UHealthComponent::GetAbilitySystem() const {
	if (const auto* character = Cast<const ABaseCharacter>(GetOwner())) {
		return character->GetAbilitySystemComponent();
	}

	check(false && "No AbilitySystemComponent available.");
	return nullptr;
}

namespace damageTag {
	FGameplayTag def() {
		static const auto tag = FGameplayTag::RequestGameplayTag(TEXT("Damage"));
		return tag;
	}

	FGameplayTag aoe() {
		static const auto tag = FGameplayTag::RequestGameplayTag(TEXT("Damage.Aoe"));
		return tag;
	}
	
	FGameplayTag explosion() {
		static const auto tag = FGameplayTag::RequestGameplayTag(TEXT("Damage.Explosion"));
		return tag;
	}

	FGameplayTag strongExplosion() {
		static const auto tag = FGameplayTag::RequestGameplayTag(TEXT("Damage.Explosion.Strong"));
		return tag;
	}

	FGameplayTag damageFriends() {
		static const auto tag = FGameplayTag::RequestGameplayTag(TEXT("Damage.Friends"));
		return tag;
	}

	FGameplayTag harvester() {
		static const auto tag = FGameplayTag::RequestGameplayTag(TEXT("Damage.Harvester"));
		return tag;
	}

	FGameplayTag killzone() {
		static const auto tag = FGameplayTag::RequestGameplayTag(TEXT("Damage.Killzone"));
		return tag;
	}

	FGameplayTag lava() {
		static const auto tag = FGameplayTag::RequestGameplayTag(TEXT("Damage.Lava"));
		return tag;
	}

	FGameplayTag trap() {
		static const auto tag = FGameplayTag::RequestGameplayTag(TEXT("Damage.Trap"));
		return tag;
	}

	FGameplayTag magic() {
		static const auto tag = FGameplayTag::RequestGameplayTag(TEXT("Damage.Magic"));
		return tag;
	}

	FGameplayTag wind() {
		static const auto tag = FGameplayTag::RequestGameplayTag(TEXT("Damage.Wind"));
		return tag;
	}

	FGameplayTag magicThorns() {
		static const auto tag = FGameplayTag::RequestGameplayTag(TEXT("Damage.Magic.Thorns"));
		return tag;
	}

	FGameplayTag poison() {
		static const auto tag = FGameplayTag::RequestGameplayTag(TEXT("Damage.Medium.Poison"));
		return tag;
	}

	FGameplayTag melee() {
		static const auto tag = FGameplayTag::RequestGameplayTag(TEXT("Damage.Melee"));
		return tag;
	}

	FGameplayTag ranged() {
		static const auto tag = FGameplayTag::RequestGameplayTag(TEXT("Damage.Ranged"));
		return tag;
	}

	FGameplayTag weak() {
		static const auto tag = FGameplayTag::RequestGameplayTag(TEXT("Damage.Weak"));
		return tag;
	}

	FGameplayTag fire() {
		static const auto tag = FGameplayTag::RequestGameplayTag(TEXT("Damage.Weak.Fire"));
		return tag;
	}

	FGameplayTag toxicWater() {
		static const auto tag = FGameplayTag::RequestGameplayTag(TEXT("Damage.Weak.ToxicWater"));
		return tag;
	}

	FGameplayTag drowning() {
		static const auto tag = FGameplayTag::RequestGameplayTag(TEXT("Damage.Medium.Drowning"));
		return tag;
	}

	FGameplayTag environmental() {
		static const auto tag = FGameplayTag::RequestGameplayTag(TEXT("Damage.Environmental"));
		return tag;
	}
}
