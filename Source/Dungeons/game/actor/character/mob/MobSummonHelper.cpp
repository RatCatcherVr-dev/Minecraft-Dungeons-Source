// © 2020 Mojang Synergies AB. TM Microsoft Corporation.


#include "MobSummonHelper.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/effects/calculations/ItemPowerModCalculations.h"
#include "game/abilities/attributes/DifficultyAttributeSet.h"
#include "game/mobspawn/MobSpawner.h"
#include <AbilitySystemInterface.h>
#include <AbilitySystemComponent.h>
#include "MobCharacter.h"
#include "util/Algo.h"
#include "game/abilities/effects/MobSummonGameplayEffect.h"
#include "Engine/NetDriver.h"
#include <UObjectBase.h>
#include "game/GameBP.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"

UMobSummonHelper::UMobSummonHelper() {
	SpawnCue = FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.MobSummon.Summon");
	ManuallyTriggerSpawnCue = false;
	Effect = UMobSummonGameplayEffect::StaticClass();
}


UAbilitySystemComponent* UMobSummonHelper::GetAbilitySystem() const {
	return AbilitySystemProvider->GetAbilitySystemComponent();
}

void UMobSummonHelper::PostInitProperties() {
	Super::PostInitProperties();
	AbilitySystemProvider = Cast<IAbilitySystemInterface>(GetOuter());
	
	UObject* Outer = this;

	//Walk up the chain and find our outer actor (if we have one)
	do {
		Outer = Outer->GetOuter();
		OuterActor = Cast<AActor>(Outer);
	} while (!OuterActor && Outer);
}

void UMobSummonHelper::PostRename(UObject* OldOuter, const FName OldName) {
	Super::PostRename(OldOuter, OldName);
	AbilitySystemProvider = Cast<IAbilitySystemInterface>(GetOuter());
}


void UMobSummonHelper::TrySummonMobs(game::mobspawn::TransformProvider location, const game::mobspawn::Config & config, float SourcePower, int mobsToSpawn, FOnMobSummonedDelegate delegate) const {
	using namespace game::mobspawn;
	UAbilitySystemComponent* abilitySystem = GetAbilitySystem();
	FGameplayEffectSpecHandle spec; 

	if (Effect) {
		spec = abilitySystem->MakeOutgoingSpec(Effect, SourcePower, abilitySystem->MakeEffectContext());
	}

	using namespace game::mobspawn;

	spawnGroupAsync(*GetWorld(), SpawnGroup(MobToSpawn, mobsToSpawn), location, config, [weakThis = TWeakObjectPtr<UMobSummonHelper>(this), SummonEvenIfExpired = bSummonEvenIfExpired, spec](AMobCharacter* mob) {
		if(mob) {
			if(!weakThis.IsValid() && !SummonEvenIfExpired) {
				mob->Destroy();
			} else {
				if(spec.IsValid()) mob->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*spec.Data);
			}

			if (spec.IsValid()) {
				mob->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*spec.Data);
			}
			
			if(weakThis.IsValid()) {
				weakThis->OnMobSummoned(mob);
			}	
		}
	}, [delegate](auto mobs){ delegate.ExecuteIfBound(mobs);}, MobSpawnPriority::ESpawnPriority_High);
}

void UMobSummonHelper::GrantSummons(const TArray<AMobCharacter *> summons) {
	for (const auto* mob : summons) {
		if (auto* hc = mob->GetHealthComponent()) {
			hc->OnDeath.AddUObject(this, &UMobSummonHelper::OnMobDeath, Cast<const AMobCharacter>(mob));
		}
		SummonedMobs.Add(mob);
	}
}

void UMobSummonHelper::OnMobSummoned(AMobCharacter* mob) {
	SummonedMobs.Add(mob);
	if (auto* hc = mob->GetHealthComponent()) {
		hc->OnDeath.AddUObject(this, &UMobSummonHelper::OnMobDeath, Cast<const AMobCharacter>(mob));
	}

	if(SpawnCue.IsValid() && !ManuallyTriggerSpawnCue) {
		auto* abilitySystem =GetAbilitySystem();
		FScopedPredictionWindow tmp(abilitySystem, FPredictionKey(), false);
		FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::Location);
		FGameplayCueParameters params;
		params.Location = mob->GetActorLocation();
		abilitySystem->ExecuteGameplayCue(SpawnCue, params);
	}
}

int UMobSummonHelper::CurrentNumberOfMobs() const {
	SummonedMobs.RemoveAllSwap([](const TWeakObjectPtr<AMobCharacter> v) {return !v.IsValid(); });
	return SummonedMobs.Num();
}

TArray<AMobCharacter*> UMobSummonHelper::GetSummonedMobs() const {
	SummonedMobs.RemoveAllSwap([](const TWeakObjectPtr<AMobCharacter> v) {return !v.IsValid(); });
	return algo::map_tarray(SummonedMobs, RETLAMBDA(it.Get()));
}

void UMobSummonHelper::OnMobDeath(const AMobCharacter * mob) {
	SummonedMobs.RemoveAllSwap([mob](const TWeakObjectPtr<AMobCharacter> v) {return !v.IsValid() || v == mob; });
}
