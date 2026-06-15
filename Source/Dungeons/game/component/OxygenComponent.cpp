#include "OxygenComponent.h"
#include "Dungeons.h"

#include "AbilitySystemComponent.h"
#include "WorldspaceHUDComponent.h"
#include "game/GamplayEffects/Drowning.h"
#include "game/GamplayEffects/WaterBreathing.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/attributes/OxygenAttributeSet.h"
#include "game/util/DungeonsEffectLibrary.h"
#include "game/actor/character/DungeonsAbilitySystemComponent.h"
#include "UnrealNetwork.h"

DECLARE_STATS_GROUP(TEXT("OxygenComponent"), STATGROUP_OxygenComponent, STATCAT_Advanced);
DECLARE_CYCLE_STAT(TEXT("OxygenChange"), STAT_OxygenChange, STATGROUP_OxygenComponent);

UOxygenComponent::UOxygenComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicated(true);
	LowOxygenNotifyEffect = FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.OxygenLowNotify");
	DrowningUIEffect = UDrowningUIGameplayEffect::StaticClass();
}

void UOxygenComponent::BeginPlay()
{
	Super::BeginPlay();

	GameState = Cast<ADungeonsGameState>(GetWorld()->GetGameState());
	GameInstance = GetWorld()->GetGameInstance<UDungeonsGameInstance>();
	APlayerCharacter* player = Cast<APlayerCharacter>(GetOwner());
	player->OnPlayerDown.AddUObject(this, &UOxygenComponent::OnDownOrDeath);
	player->OnPlayerDeath.AddUObject(this, &UOxygenComponent::OnDownOrDeath);
	player->OnPlayerRevive.AddUObject(this, &UOxygenComponent::OnReviveOrRespawn);

	if (UAbilitySystemComponent* abilitySystem = GetAbilitySystem())
	{
		check(abilitySystem->GetSet<UOxygenAttributeSet>() != nullptr && "All actors with a UOxygenComponent should also have the UOxygenAttributeSet");

		if (GetOwnerRole() == ROLE_Authority)
		{
			abilitySystem->SetNumericAttributeBase(UOxygenAttributeSet::MaxOxygenAttribute(), MaxOxygen);
			abilitySystem->SetNumericAttributeBase(UOxygenAttributeSet::OxygenAttribute(), Oxygen == MaxOxygen ? abilitySystem->GetNumericAttribute(UOxygenAttributeSet::MaxOxygenAttribute()) : Oxygen);

			auto underwaterTagDelegate = FOnGameplayEffectTagCountChanged::FDelegate::CreateUObject(this, &UOxygenComponent::OnUnderwaterTagChange);
			abilitySystem->RegisterAndCallGameplayTagEvent(FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.Underwater")), underwaterTagDelegate, EGameplayTagEventType::NewOrRemoved);
			auto waterBreathingTagDelegate = FOnGameplayEffectTagCountChanged::FDelegate::CreateUObject(this, &UOxygenComponent::OnWaterBreathingTagChange);
			abilitySystem->RegisterAndCallGameplayTagEvent(FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.WaterBreathing")), waterBreathingTagDelegate, EGameplayTagEventType::NewOrRemoved);
		}

		PreviouslySeenOxygen = abilitySystem->GetNumericAttributeBase(UOxygenAttributeSet::OxygenAttribute());
		abilitySystem->GetGameplayAttributeValueChangeDelegate(UOxygenAttributeSet::OxygenAttribute()).AddUObject(this, &UOxygenComponent::OnAttributeOxygenChange);
	}

	OnOxygenChanged.Broadcast(Oxygen, Oxygen, MaxOxygen);
}

void UOxygenComponent::OnAttributeOxygenChange(const FOnAttributeChangeData& data) {
	SCOPE_CYCLE_COUNTER(STAT_OxygenChange)

	auto abilitySystem = Cast<ABaseCharacter>(GetOwner())->GetAbilitySystemComponent();

	const float oldValue = PreviouslySeenOxygen.Get(0);
	const float newValue = GetOwnerRole() == ROLE_Authority ? data.NewValue : abilitySystem->GetNumericAttributeBase(UOxygenAttributeSet::OxygenAttribute());
	PreviouslySeenOxygen = newValue;

	UE_LOG(LogDungeonsNetwork, Verbose, TEXT("UOxygenComponent::OnAttributeOxygenChange GEMod(%p), Authority(%d). new %f, old %f"), data.GEModData, GetOwner()->HasAuthority(), newValue, oldValue);
	if (ShowDebugMessage)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::White, FString::Printf(TEXT("Remaining Oxygen: %f"), newValue));
	}

	const auto oxygenChange = newValue - oldValue;
	if (FMath::IsNearlyZero(oxygenChange)) {
		return;
	}

	const float newFraction = GetCurrentOxygenPercentage();
	const float oldFraction = CachedOxygenFraction.Get(1.f);
	const bool oxygenFractionChanged = !FMath::IsNearlyEqual(newFraction, oldFraction);

	if (oxygenFractionChanged) {
		OnOxygenFractionChanged.Broadcast(oldFraction, newFraction);

	}
	OnOxygenChanged.Broadcast(oldValue, newValue, MaxOxygen);
	if (newFraction < LowOxygenFractionThreshold) {
		auto& timerManager = GetWorld()->GetTimerManager();
		if (!timerManager.IsTimerActive(OxygenLowNotifyTimerHandle)) {
			timerManager.SetTimer(OxygenLowNotifyTimerHandle, this, &UOxygenComponent::OnNotifyOxygenLow, 1.f, true);
		}
		if (GetOwnerRole() == ROLE_Authority && !OxygenLowEffectHandle.IsValid()) {
			auto spec = effects::CreateGameplayEffectSpec<UOxygenLowEffect>(abilitySystem);
			OxygenLowEffectHandle = abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
		}
	}
	else {
		GetWorld()->GetTimerManager().ClearTimer(OxygenLowNotifyTimerHandle);
		if (GetOwnerRole() == ROLE_Authority && OxygenLowEffectHandle.IsValid()) {
			abilitySystem->RemoveActiveGameplayEffect(OxygenLowEffectHandle);
			OxygenLowEffectHandle.Invalidate();
		}
	}

	// Handle drowning UI effect
	if (GetOwnerRole() == ROLE_Authority) {
		if (newValue <= 0) {
			if (!DrowningUIEffectHandle.IsValid()) {
				auto spec = effects::CreateGameplayEffectSpecFromSubClass(abilitySystem, DrowningUIEffect);
				DrowningUIEffectHandle = abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
			}
			OnDrowning(true);
		}
		else {
			if (DrowningUIEffectHandle.IsValid()) {
				abilitySystem->RemoveActiveGameplayEffect(DrowningUIEffectHandle);
				DrowningUIEffectHandle.Invalidate();
			}
			OnDrowning(false);
		}
	}

	CachedOxygenFraction = newFraction;
}

float UOxygenComponent::GetCurrentOxygen() {
	if (PreviouslySeenOxygen) {
		return PreviouslySeenOxygen.GetValue();
	}
	if (UAbilitySystemComponent* abilitySystem = GetAbilitySystem()) {
		return abilitySystem->GetNumericAttributeBase(UOxygenAttributeSet::OxygenAttribute());
	}
	return 0.0f;
}

float UOxygenComponent::GetCurrentOxygenPercentage() {
	return FMath::Clamp(GetCurrentOxygen() / GetMaximumOxygen(), 0.0f, 1.0f);
}

void UOxygenComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(UOxygenComponent, IsUnderwater, COND_None, REPNOTIFY_Always);
}

float UOxygenComponent::GetMaximumOxygen() {
	if (UAbilitySystemComponent* abilitySystem = GetAbilitySystem()) {
		return abilitySystem->GetNumericAttribute(UOxygenAttributeSet::MaxOxygenAttribute());
	}
	return 0.0f;
}

void UOxygenComponent::FillOxygenWithoutEffects() {
	if (UAbilitySystemComponent* abilitySystem = GetAbilitySystem()) {
		if (GetOwnerRole() == ROLE_Authority) {
			abilitySystem->ApplyGameplayEffectSpecToSelf(effects::CreateGameplayEffectSpec<UOxygenReplenishOnlyEffect>(abilitySystem));
		}
	}
}

void UOxygenComponent::ApplyOxygenEffects()
{
	if (UAbilitySystemComponent* abilitySystem = GetAbilitySystem())
	{
		if (GetOwnerRole() == ROLE_Authority)
		{
			abilitySystem->ApplyGameplayEffectSpecToSelf(effects::CreateGameplayEffectSpec<UHoldingBreathGameplayEffect>(abilitySystem));
		}
	}
}

void UOxygenComponent::RemoveOxygenEffects()
{
	if (UAbilitySystemComponent* abilitySystem = GetAbilitySystem())
	{
		if (GetOwnerRole() == ROLE_Authority)
		{
			FGameplayTagContainer tags;
			tags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.Oxygen")));
			abilitySystem->RemoveActiveEffectsWithTags(tags);
		}
	}
}

void UOxygenComponent::OnUnderwaterTagChange(const FGameplayTag Tag, int32 Count)
{
	SetIsUnderwater(Count > 0);
}

void UOxygenComponent::OnWaterBreathingTagChange(const FGameplayTag Tag, int32 Count)
{
	if (!IsUnderwater)
	{
		FillOxygen();
		RemoveOxygenEffects();
	}

	// #D11.CM - If we gain water breathing, play the oxygen replenished cue.
	if (Count) {
		FGameplayCueParameters cueParams = FGameplayCueParameters();
		cueParams.Location = GetOwner()->GetActorLocation() + FVector(0,0, 250);
		GetAbilitySystem()->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Oxygen.Replenish"), cueParams);
	}
}

void UOxygenComponent::SetIsUnderwater(bool Underwater)
{
	if (IsUnderwater == Underwater)
	{
		return;
	}

	IsUnderwater = Underwater;
	if (Underwater)
	{
		FillOxygenWithoutEffects();
		OnInstantFillOxygen.Broadcast();
	}
	else
	{
		FillOxygenWithoutEffects();
		RemoveOxygenEffects();
	}

	BroadcastUnderwaterDelegate(IsUnderwater);
}

void UOxygenComponent::BroadcastUnderwaterDelegate_Implementation(bool Underwater)
{
	OnIsUnderwaterChanged.Broadcast(Underwater);
}

void UOxygenComponent::EnableWaterBreathing(bool enabled)
{
	if (enabled)
	{
		FillOxygen();
		RemoveOxygenEffects();
		if (UAbilitySystemComponent* abilitySystem = GetAbilitySystem())
		{
			if (GetOwnerRole() == ROLE_Authority)
			{
				WaterBreathingHandle = abilitySystem->ApplyGameplayEffectSpecToSelf(effects::CreateGameplayEffectSpec<UWaterBreathingGameplayEffect>(abilitySystem));
			}
		}
	}
	else
	{
		FillOxygen();
		if (UAbilitySystemComponent* abilitySystem = GetAbilitySystem())
		{
			if (GetOwnerRole() == ROLE_Authority)
			{
				abilitySystem->RemoveActiveGameplayEffect(WaterBreathingHandle);
			}
		}
	}
}

void UOxygenComponent::FillOxygen()
{
	if (UAbilitySystemComponent* abilitySystem = GetAbilitySystem())
	{
		if (GetOwnerRole() == ROLE_Authority)
		{
			abilitySystem->ApplyGameplayEffectSpecToSelf(effects::CreateGameplayEffectSpec<UOxygenReplenishEffect>(abilitySystem));
		}
	}
}

void UOxygenComponent::OnDownOrDeath() {
	if (IsUnderwater) {
		FillOxygen();
		RemoveOxygenEffects();
	}
}

void UOxygenComponent::OnReviveOrRespawn() {
	if (IsUnderwater) {
		FillOxygen();
		OnInstantFillOxygen.Broadcast();
	}
}

UAbilitySystemComponent* UOxygenComponent::GetAbilitySystem() {
	if (auto* character = Cast<ABaseCharacter>(GetOwner())) {
		return character->AbilitySystem;
	}

	check(false && "No AbilitySystemComponent available.");
	return nullptr;
}

bool UOxygenComponent::GetIsOxygenLow() {
	return GetCurrentOxygenPercentage() <= LowOxygenFractionThreshold;
}

void UOxygenComponent::OnNotifyOxygenLow() {
	auto abilitySystem = Cast<ABaseCharacter>(GetOwner())->GetAbilitySystemComponent();
	FGameplayCueParameters params;
	params.Instigator = GetOwner();
	params.EffectCauser = GetOwner();
	abilitySystem->ExecuteGameplayCue(LowOxygenNotifyEffect, params);
	OnNotifyLowOxygen.Broadcast(GetCurrentOxygenPercentage());
}

void UOxygenComponent::OnDrowning(bool isDrowning) {
	if (APlayerCharacter* player = Cast<APlayerCharacter>(GetOwner())) {
		if (UStatTrackerComponent* tracker = player->GetStatTracker()) {
			tracker->Drowning(isDrowning);
		}
		else {
			Client_OnDrowning(isDrowning);
		}
	}
}

void UOxygenComponent::Client_OnDrowning_Implementation(bool isDrowning) {
	if (APlayerCharacter* player = Cast<APlayerCharacter>(GetOwner())) {
		if (UStatTrackerComponent* tracker = player->GetStatTracker()) {
			tracker->Drowning(isDrowning);
		}
	}
}