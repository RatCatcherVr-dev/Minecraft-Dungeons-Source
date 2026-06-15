// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "EmeraldDivination.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "Async.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/actor/item/StorableItem.h"
#include "game/actor/item/BaseProjectile.h"
#include "game/item/ItemUtil.h"
#include "game/item/generator/ItemGenerator.h"
#include "../component/WalkPickupComponent.h"


UEmeraldDivination::UEmeraldDivination() {
	TypeId = EEnchantmentTypeID::EmeraldDivination;
	LevelMultiplier = [=](int level) -> float {
		return (EmeraldToSpawnBase + (((float)level - 1) * EmeraldToSpawnPerLevel));
	};
	MultiplierFormatter = valueformat::asConstant;
}

void UEmeraldDivination::BeginPlay()
{
	Super::BeginPlay();
	if (auto playerCharacter = Cast<APlayerCharacter>(GetOwner())) {
		playerCharacter->OnMapBlocksRevealed.AddUObject(this, &UEmeraldDivination::OnBlocksExplored);
	}
}

void UEmeraldDivination::OnBlocksExplored(int amount) {
	blockCounter += amount;

	if (blockCounter >= BlockThreshold) {
		blockCounter -= BlockThreshold;
		TrySpawn();
	}
}

void UEmeraldDivination::TrySpawn() {
	if (RandCheckCanTrigger())
	{
		for (int i = 0; i < LevelMultiplier(Level); ++i)
		{
			FVector SpawnLocation = GetOwner()->GetActorLocation();
			SpawnLocation.X += FMath::RandRange(-100, 100);
			SpawnLocation.Y += FMath::RandRange(-100, 100);
			//Emerald classes are always loaded so this is fine
			AStorableItem* item = game::item::util::spawnStorableItem(
				*GetWorld(),
				SpawnLocation,
				game::item::generator::generate(game::item::type::Emerald),
				1,
				GetOwner(),
				true);

			if (!item) {
				return;
			}

			if (UWalkPickupComponent* WalkPickUpComponent = item->FindComponentByClass<UWalkPickupComponent>())
			{
				// Just to guarantee that the emerald goes to the desired player
				WalkPickUpComponent->PullInThreshold = 100000.f;
			}
		}
		TryApplyEffect();
	}
}

void UEmeraldDivination::TryApplyEffect() {
	auto characterOwner = GetCharacterOwner();
	auto abilitySystem = characterOwner->GetAbilitySystemComponent();

	FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
	FGameplayEffectSpec spec = effects::CreateGameplayEffectSpec<UEmeraldDivinationGameplayEffect>(abilitySystem);
	spec.SetSetByCallerMagnitude("Duration", 2.0);
	EffectHandle = abilitySystem->ApplyGameplayEffectSpecToSelf(spec);

	BroadcastEnchantmentTriggeredEvent();
}

void UEmeraldDivination::OnResetEffect() {
	auto characterOwner = GetCharacterOwner();
	auto abilitySystem = characterOwner->GetAbilitySystemComponent();
	abilitySystem->RemoveActiveGameplayEffect(EffectHandle);
}

bool UEmeraldDivination::RandCheckCanTrigger()
{
	return FMath::FRand() < PercentageToSpawn;
}

FText UEmeraldDivination::CreateDescription() const
{
	return FText::Format(Super::CreateDescription(), FText::FromString(valueformat::asPercentageChance(PercentageToSpawn)));
}

UEmeraldDivinationGameplayEffect::UEmeraldDivinationGameplayEffect(const FObjectInitializer& ObjectInitializer) {
	DurationPolicy = EGameplayEffectDurationType::HasDuration;

	FSetByCallerFloat durationMagnitude;
	durationMagnitude.DataName = FName("Duration");

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;
	DurationMagnitude = durationMagnitude;

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Enchantment.EmeraldDivination"), 0, 1);
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Enchantment.EmeraldDivination"), 0, 1);
}
