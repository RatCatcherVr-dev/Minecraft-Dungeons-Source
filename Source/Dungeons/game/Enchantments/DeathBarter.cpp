#include "Dungeons.h"
#include "DeathBarter.h"
#include <AbilitySystemComponent.h>
#include <AbilitySystemGlobals.h>
#include <GameplayEffect.h>
#include <GameplayEffectExtension.h>
#include "game/actor/character/mob/MobCharacter.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "util/CharacterQuery.h"
#include "game/abilities/attributes/MeleeAttributeSet.h"
#include "game/abilities/attributes/RangedAttributeSet.h"
#include "../abilities/ui/DungeonsGameplayEffectUIData.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "game/actor/item/StorableItem.h"

namespace deathbarter {
	int EmeraldsRequiredPerLevel(int Level) {
		return 150 - (Level - 1) * 50;
	}
}

UDeathBarterChargingGameplayEffect::UDeathBarterChargingGameplayEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;
	bClearStackOnOverflow = true;
	bDenyOverflowApplication = true;

	//OverflowEffects.Add(UDeathBarterActiveGameplayEffect::StaticClass());

	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("PersistAfterDeath"));
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.DeathBarter.Charging"));
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.DeathBarter.Charging"));
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.StatusEffect.Enchantment.DeathBarter")), 0, 1);
}

UDeathBarterActiveGameplayEffect::UDeathBarterActiveGameplayEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.DeathBarter.Active"));
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.DeathBarter.Active"));

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.StatusEffect.Enchantment.DeathBarter")), 0, 1);
}

UDeathBarterChargingLevelOneGameplayEffect::UDeathBarterChargingLevelOneGameplayEffect(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer) {
	StackLimitCount = deathbarter::EmeraldsRequiredPerLevel(1);
}
UDeathBarterChargingLevelTwoGameplayEffect::UDeathBarterChargingLevelTwoGameplayEffect(const FObjectInitializer& ObjectInitializer) 
: Super(ObjectInitializer) {
	StackLimitCount = deathbarter::EmeraldsRequiredPerLevel(2);
}
UDeathBarterChargingLevelThreeGameplayEffect::UDeathBarterChargingLevelThreeGameplayEffect(const FObjectInitializer& ObjectInitializer) 
: Super(ObjectInitializer) {
	StackLimitCount = deathbarter::EmeraldsRequiredPerLevel(3);
}

UDeathBarterReviveGameplayEffect::UDeathBarterReviveGameplayEffect(const FObjectInitializer& ObjectInitializer) {
	
	FAttributeBasedFloat reviveData;
	reviveData.BackingAttribute.AttributeToCapture = UHealthAttributeSet::MaxHealthAttribute();
	reviveData.BackingAttribute.bSnapshot = true;
	reviveData.Coefficient = 0.3f;

	FGameplayModifierInfo healthInfo;
	healthInfo.Attribute = UHealthAttributeSet::HealthAttribute();
	healthInfo.ModifierOp = EGameplayModOp::Type::Override;
	healthInfo.ModifierMagnitude = reviveData;

	Modifiers.Add(healthInfo);

	RemoveGameplayEffectsWithTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.DeathBarter.Active"));

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Revive")));
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Trigger.Enchantment.DeathBarter")), 0, 1);
}

UDeathBarterInvulnerabilityGameplayEffect::UDeathBarterInvulnerabilityGameplayEffect(const FObjectInitializer& ObjectInitializer) {}

UDeathBarter::UDeathBarter()
{
	TypeId = EEnchantmentTypeID::DeathBarter;

	LevelMultiplier = [this](int level) -> float {
		return deathbarter::EmeraldsRequiredPerLevel(level);
	};
}

void UDeathBarter::BeginPlay() {
	Super::BeginPlay();
	auto abilitySystem = GetAbilitySystemComponent();
	auto tag = FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.DeathBarter.Active"));
	GetAbilitySystemComponent()->RegisterGameplayTagEvent(tag).AddUObject(this, &UDeathBarter::OnActiveChanged);
	LocalCounter = LevelMultiplier(Level);

	OnActiveChanged(FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.DeathBarter.Active")), abilitySystem->GetTagCount(tag));
} 

void UDeathBarter::EndPlay(const EEndPlayReason::Type reason) {
	Super::EndPlay(reason);

	auto owner = GetCharacterOwner();
	if (GetOwnerRole() == ROLE_Authority) {
		GetAbilitySystemComponent()->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.DeathBarter.Active"))).RemoveAll(this);
		GetAbilitySystemComponent()->RemoveActiveEffects(FGameplayEffectQuery::MakeQuery_MatchAnyEffectTags(FGameplayTagContainer(FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.DeathBarter")))));
		auto hc = owner ? owner->GetHealthComponent() : nullptr;
		if (hc) hc->OnBeforeDeath.RemoveAll(this);
	}

	auto wallet = owner ? owner->FindComponentByClass<UWalletComponent>() : nullptr;

	if (owner->IsLocallyControlled() && wallet) {
		wallet->RemoveAllConsumers(this);
	}
}

void UDeathBarter::OnEmeraldsEarned(int32 amount, bool TriggerCue /*= true*/) {
	auto abilitySystem = GetAbilitySystemComponent();

	if (GetOwnerRole() == ROLE_Authority) {
		const auto chargingEffect = GetChargingEffect();
		auto spec = effects::CreateGameplayEffectSpecFromSubClass(abilitySystem, chargingEffect, Level);

		const auto currentStacks = abilitySystem->GetAggregatedStackCount(FGameplayEffectQuery::MakeQuery_MatchAnyEffectTags(FGameplayTagContainer(FGameplayTag::RequestGameplayTag("StatusEffect.DeathBarter.Charging"))));
		const auto limit = chargingEffect.GetDefaultObject()->StackLimitCount;

		const auto stackRemainder = limit - currentStacks;

		if (amount > stackRemainder && stackRemainder > 0) {
			//if stack count is not at maximum when we overflow stacks, it is instead clamped rather than triggering an overflow, so we must always first reach the stack limit, then overflow
			spec.StackCount = stackRemainder;
			abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
			spec.StackCount = 1;
			abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
		}
		else {
			spec.StackCount = amount;
			abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
		}
	}
	else {
		BatchCount += amount;
		if (!ReplicationTimer.IsValid()) {
			ReplicationTimer = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UDeathBarter::ReplicateCountToServer));
		}
	}

	auto character = GetCharacterOwner();

	if (character->IsLocallyControlled() && TriggerCue) {
		FGameplayCueParameters params;
		params.Location = character->GetActorLocation();
		abilitySystem->InvokeGameplayCueEvent(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Trigger.Enchantment.DeathBarterCollect")), EGameplayCueEvent::Executed, params);
	}
}

void UDeathBarter::OnBeforeDeath() {
	auto abilitySystem = GetAbilitySystemComponent();
	//This clears the active effect
	abilitySystem->ApplyGameplayEffectToSelf(UDeathBarterReviveGameplayEffect::StaticClass()->GetDefaultObject<UDeathBarterReviveGameplayEffect>(), 1, abilitySystem->MakeEffectContext());
	auto spec = effects::CreateGameplayEffectSpec<UDeathBarterInvulnerabilityGameplayEffect>(abilitySystem);
	spec.SetSetByCallerMagnitude(TEXT("Duration"), InvulnerabilityDuration);
	abilitySystem->ApplyGameplayEffectSpecToSelf(spec);

	Client_OnBeforeDeath();
}

void UDeathBarter::Client_OnBeforeDeath_Implementation() {
	if (APlayerCharacter* player = Cast<APlayerCharacter>(GetCharacterOwner())) {
		if (UStatTrackerComponent* tracker = player->GetStatTracker()) {
			tracker->DeathBarter();
		}
	}
}

TSubclassOf<UDeathBarterChargingGameplayEffect> UDeathBarter::GetChargingEffect() const {
	return ChargingEffects[FMath::Clamp(Level - 1, 0, ChargingEffects.Num() - 1)];
}

int32 UDeathBarter::AttemptConsumeCurrency(const FItemId & type, int32 amount, ECurrencyObtainReason reason)
{
	if (type == game::item::type::Emerald.getId() && amount > 0 && reason == ECurrencyObtainReason::Pickup) {
		int32 amountToConsume = FMath::Min(LocalCounter, amount);
		LocalCounter -= amountToConsume;

		OnEmeraldsEarned(amountToConsume);

		PlaySoundCount += amountToConsume;

		return amountToConsume;
	}

	return 0;
}

void UDeathBarter::OnActiveChanged(const FGameplayTag, int32 count) {
	auto owner = GetCharacterOwner();

	auto hc = owner->GetHealthComponent();
	auto wallet = owner->FindComponentByClass<UWalletComponent>();
	if (count > 0) {
		if (GetOwnerRole() == ROLE_Authority) {
			if (hc) hc->OnBeforeDeath.AddUObject(this, &UDeathBarter::OnBeforeDeath);
		}
		
		if (owner->IsLocallyControlled() && wallet) {
			wallet->RemoveAllConsumers(this);
		}
	}
	else {
		if (GetOwnerRole() == ROLE_Authority) {
			if (hc) hc->OnBeforeDeath.RemoveAll(this);
			OnEmeraldsEarned(1, false);
		}

		if (owner->IsLocallyControlled() && wallet) {
			wallet->AddConsumer(FCurrencyConsumer::CreateUObject(this, &UDeathBarter::AttemptConsumeCurrency));
		}

		LocalCounter = LevelMultiplier(Level);
	}
}

void UDeathBarter::ReplicateCountToServer() {
	ServerSendCount(BatchCount);
	BatchCount = 0;
	ReplicationTimer.Invalidate();
}


void UDeathBarter::ServerSendCount_Implementation(int32 count) {
	OnEmeraldsEarned(count);
}

bool UDeathBarter::ServerSendCount_Validate(int32 count) {
	return true;
}