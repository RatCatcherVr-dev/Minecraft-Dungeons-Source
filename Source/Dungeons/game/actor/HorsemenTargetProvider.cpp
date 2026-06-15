#include "Dungeons.h"
#include "HorsemenTargetProvider.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "game/abilities/attributes/HealthAttributeSet.h"

AHorsemenTargetProvider::AHorsemenTargetProvider(const FObjectInitializer& objectInitializer)
	: Super(objectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = .5f;
}

void AHorsemenTargetProvider::BeginPlay() {
	Super::BeginPlay();
}

void AHorsemenTargetProvider::ApplyDamage(float damage, AActor* byWhom) {
	if (byWhom == nullptr) {
		return;
	}
		
	const auto id = byWhom->GetUniqueID();

	if (map.Contains(id)) {
		map.Emplace(id, map[id] + damage);
	} else {
		map.Add(id, damage);
	}
}

AActor* AHorsemenTargetProvider::GetMostAggresive() const {
	float maxDamage { 0 };
	uint32 mostAggresive { 0 };
	
	for (auto& entry : map) {
		if (entry.Value > maxDamage) {
			maxDamage = entry.Value;
			mostAggresive = entry.Key;
		}
	}

	if (maxDamage <= 0) {
		return nullptr;
	}

	//D11.PS switch does not like this, returning an address of a local variable.
	//for (auto&& actor : TActorRange<APlayerCharacter>(GetWorld())) {
	for (auto actor : TActorRange<APlayerCharacter>(GetWorld())) {
		if (actor->GetUniqueID() == mostAggresive) {
			return actor;
		}
	}

	return nullptr;
}

void AHorsemenTargetProvider::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	const auto reduceAmount = reducePerSecond * DeltaTime;

	TArray<int32> entriesToRemove;

	for (auto& entry : map) {
		if (entry.Value <= reduceAmount) {
			entriesToRemove.Add(entry.Key);
		} else {
			entry.Value -= reduceAmount;
		}
	}

	for (auto key : entriesToRemove) {
		map.Remove(key);
	}
}

void AHorsemenTargetProvider::OnAttributeHealthChange(const FOnAttributeChangeData& data) {
	if (data.OldValue - data.NewValue > SMALL_NUMBER && data.GEModData) {
		const FGameplayEffectSpec& effectSpec = data.GEModData->EffectSpec;
		const FGameplayEffectContextHandle& handle = effectSpec.GetEffectContext();

		ApplyDamage(data.OldValue - data.NewValue, handle.GetInstigator());
	}
}