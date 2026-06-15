#include "Dungeons.h"
#include "GrowComponent.h"
#include "HealthComponent.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "MobGroupBehaviorComponent.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"

UMobGroupBehaviorComponent::UMobGroupBehaviorComponent() {
	PrimaryComponentTick.bCanEverTick = false;
	bReplicates = true;
}

void UMobGroupBehaviorComponent::BeginPlay() {
	Super::BeginPlay();

	if (auto* mob = Cast<AMobCharacter>(GetOwner())) {
		if (auto* abilitySystem = mob->GetAbilitySystemComponent()) {
			abilitySystem->GetGameplayAttributeValueChangeDelegate(
				UHealthAttributeSet::HealthAttribute()).AddUObject(this, &UMobGroupBehaviorComponent::OnAttributeHealthChange);
		}
	}
	GetWorld()->GetTimerManager().SetTimer(LastAttackerAliveQueryHandle, this, &UMobGroupBehaviorComponent::CheckAttackerHealth, 1.0f, true);
}

void UMobGroupBehaviorComponent::SetState(EMobGroupAttackedState newState)
{
	MobState = newState;
	if (MobState != EMobGroupAttackedState::Normal && BackToNormalStateTime > 0)
	{
		GetWorld()->GetTimerManager().SetTimer(BackToNormalHandle, FTimerDelegate::CreateUObject(this, &UMobGroupBehaviorComponent::SetState, EMobGroupAttackedState::Normal), BackToNormalStateTime, false);
	}
}

EMobGroupAttackedState UMobGroupBehaviorComponent::GetState()
{
	return MobState;
}

void UMobGroupBehaviorComponent::OnAttributeHealthChange(const FOnAttributeChangeData& data) {
	if (data.OldValue - data.NewValue > SMALL_NUMBER && data.NewValue < data.OldValue) {
		if (data.GEModData) {
			const FGameplayEffectSpec& effectSpec = data.GEModData->EffectSpec;
			const FGameplayEffectContextHandle& handle = effectSpec.GetEffectContext();
			
			auto instigator = handle.GetInstigator();
			OnMobWasAttacked(instigator);
		}
	}
}

void UMobGroupBehaviorComponent::OnMobWasAttacked(AActor* Attacker)
{
	if (AMobCharacter* MobOwner = Cast<AMobCharacter>(GetOwner()))
	{
		TArray<AMobCharacter*> mobs = actorquery::getNearbyActors<AMobCharacter>(GetWorld(), GetOwner()->GetActorLocation(), MobsCheckRange);
		for (AMobCharacter* mob : mobs)
		{
			if (UMobGroupBehaviorComponent* MobGroupComponent = mob->FindComponentByClass<UMobGroupBehaviorComponent>())
			{
				if (MobOwner->EntityType == mob->EntityType)
				{
					MobGroupComponent->SetState(MobGroupOnAttackedState);
					MobGroupComponent->SetLastAttacker(Attacker);
				}
			}
		}
	}
}

AActor* UMobGroupBehaviorComponent::GetLastAttacker()
{
	return LastAttacker;
}

void UMobGroupBehaviorComponent::SetLastAttacker(AActor* newLastAttacker)
{
	LastAttacker = newLastAttacker;
}

void UMobGroupBehaviorComponent::ResetState()
{
	SetLastAttacker(nullptr);
	SetState(EMobGroupAttackedState::Normal);
}

bool UMobGroupBehaviorComponent::IsLastAttackerAlive()
{
	if (LastAttacker)
	{
		if (ABaseCharacter* Character = Cast<ABaseCharacter>(LastAttacker))
		{
			return Character->IsAlive();
		}
	}
	return false;
}

void UMobGroupBehaviorComponent::CheckAttackerHealth()
{
	if (!IsLastAttackerAlive())
	{
		ResetState();
	}
}
