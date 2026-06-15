#include "Dungeons.h"
#include "GrowComponent.h"
#include "HealthComponent.h"
#include "DolphinBehaviorComponent.h"
#include "../item/ItemTypeDefs.h"
#include <GameplayEffect.h>


namespace {
	FName DolphinsGraceEffectDuration(TEXT("DolphinsGraceDuration"));
	FName DolphinsGraceEffectMagnitude(TEXT("DolphinsGraceMagnitude"));
}


UDolphinsGraceGameplayEffect::UDolphinsGraceGameplayEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	FSetByCallerFloat durationMagnitude;
	durationMagnitude.DataName = ::DolphinsGraceEffectDuration;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;

	DurationMagnitude = durationMagnitude;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	FGameplayModifierInfo info;
	info.Attribute = UMovementAttributeSet::SpeedMultiplierAttribute();

	FSetByCallerFloat powerMagnitude;
	powerMagnitude.DataName = ::DolphinsGraceEffectMagnitude;

	info.ModifierMagnitude = powerMagnitude;
	info.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(info);

	ApplicationRequirements.Emplace(UBaseGameplayEffectApplicationRequirement::StaticClass());

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.DolphinsGrace"), 0, 1);
}

UDolphinBehaviorComponent::UDolphinBehaviorComponent() {
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.1f;
	bReplicates = true;
}


void UDolphinBehaviorComponent::BeginPlay() {
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(LootActorQueryHandle, this, &UDolphinBehaviorComponent::DoLootActorQuery, 1.0f, true);
}

ALootActor* UDolphinBehaviorComponent::GetNearestTreasureChest()
{
	return NearestTresureChest;
}

void UDolphinBehaviorComponent::DoLootActorQuery()
{
	const TArray<ALootActor*>& LootActors = InstanceTracker< ALootActor >::GetList(GetWorld());
	float closestDistanceSq = FMath::Square(ChestMaximumDistance);
	FVector DolphinLocation = GetOwner()->GetActorLocation();
	NearestTresureChest = nullptr;
	bIsNearChest = false;
	for (ALootActor* actor : LootActors)
	{
		if (!actor->IsOpen())
		{
			float chestDistanceSq = FVector::DistSquared(DolphinLocation, actor->GetActorLocation());
			if (chestDistanceSq <= closestDistanceSq)
			{
				NearestTresureChest = actor;
				closestDistanceSq = chestDistanceSq;
			}
		}
	}

	if (closestDistanceSq <= FMath::Square(85))
	{
		bIsNearChest = true;
	}
}

bool UDolphinBehaviorComponent::IsNearChest()
{
	return bIsNearChest;
}

bool UDolphinBehaviorComponent::IsNearPlayer()
{
	return NearbyPlayers.Num() > 0;
}

void UDolphinBehaviorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime,  TickType, ThisTickFunction);

	if (ABaseCharacter* pDolphinChar = Cast<ABaseCharacter>(GetOwner()))
	{
		TArray< TWeakObjectPtr<APlayerCharacter> > CurrentPlayerList;
		CurrentPlayerList.Reserve(8);

		UActorQuery::GetPlayerCharactersInRange(GetWorld(), pDolphinChar->GetActorLocation(), 215.f, CurrentPlayerList, false);

		if (CurrentPlayerList != NearbyPlayers) //this should be fine, as the list is pulled from a statically stored base and its order/composition rarely changes
		{
			for (auto& OldPlayer : NearbyPlayers)
			{
				if (OldPlayer.IsValid() && !CurrentPlayerList.Contains(OldPlayer))
				{
					// Give the Dolphin's grace to the player
					if (UAbilitySystemComponent* DolphinsAbilitySystem = pDolphinChar->GetAbilitySystemComponent())
					{
						FGameplayEffectSpec spec(Cast<UDolphinsGraceGameplayEffect>(Effect->GetDefaultObject()), DolphinsAbilitySystem->MakeEffectContext(), 1.0f);
						spec.SetSetByCallerMagnitude(::DolphinsGraceEffectDuration, DolphinGraceDuration);
						spec.SetSetByCallerMagnitude(::DolphinsGraceEffectMagnitude, DolphinGraceSpeedBoostAmount);
						DolphinsAbilitySystem->ApplyGameplayEffectSpecToTarget(spec, OldPlayer->GetAbilitySystemComponent());
					}
				}
			}

			NearbyPlayers = CurrentPlayerList;
		}
	}
}
