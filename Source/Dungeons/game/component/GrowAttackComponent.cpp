#include "Dungeons.h"
#include "GrowAttackComponent.h"

#include "Kismet/KismetMathLibrary.h"
#include "game/component/HealthComponent.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "AbilitySystemComponent.h"
#include "game/actor/DynamicBeam.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/item/enchantment/EnchantmentData.h"
#include "AbilitySystemGlobals.h"
#include "Assets/DungeonsAssetManager.h"
#include "PlayerExperienceComponent.h"
#include "Dungeons/DungeonsGameInstance.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"

UGrowAttackComponent::UGrowAttackComponent() {
	PrimaryComponentTick.bCanEverTick = true;

	Effect = UGrowAttackBuffGameplayEffect::StaticClass();
}

void UGrowAttackComponent::BeginPlay() {
	Super::BeginPlay();
	static std::array<EEnchantmentTypeID, 3> possibleEnchants = { EEnchantmentTypeID::DoubleDamage , EEnchantmentTypeID::FastAttack, EEnchantmentTypeID::Quick };
	EnchantToGrant = possibleEnchants[FMath::FRandRange(0, possibleEnchants.size() - 1)];
}

void UGrowAttackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


	EnchantedTargets.RemoveAllSwap([](const auto& target) { return !target.IsValid(); });

	const auto ownerLocation = GetOwner()->GetActorLocation();
	auto outOfRangeTargets = EnchantedTargets.FilterByPredicate([&](const TWeakObjectPtr<ABaseCharacter>& target) {
		return target.IsValid() && FVector::DistSquared2D(ownerLocation, target->GetActorLocation()) > distanceMax * distanceMax; 
	});
	
	for (auto& enchantedTarget : outOfRangeTargets) {
		DisenchantTarget(enchantedTarget);
		EnchantedTargets.RemoveSwap(enchantedTarget);
	}
}

void UGrowAttackComponent::DisenchantTarget(TWeakObjectPtr<ABaseCharacter> target) {
	if (!target.IsValid()) return;

	auto abilitySystem = target->GetAbilitySystemComponent();
	auto enchantComponent = target->GetEnchantmentComponent();

	if(enchantComponent)
		enchantComponent->RemoveEnchantments(GetOwner());

	if (abilitySystem)
	{
		FGameplayEffectQuery query;
		query.EffectDefinition = UGrowAttackBuffGameplayEffect::StaticClass();
		abilitySystem->RemoveActiveEffects(query);
	}

}

bool UGrowAttackComponent::CanGrow(AActor* attackTarget) const {
	auto characterTarget = Cast<ABaseCharacter>(attackTarget);

	if (!characterTarget || !characterTarget->IsAlive()) return false;

	// is this necessary?	
	const auto attackerHealthComponent = GetOwner()->FindComponentByClass<UHealthComponent>();
	if (attackerHealthComponent && attackerHealthComponent->IsNotAlive()) {
		return false;
	}

	auto targetAbilitySystem = characterTarget->GetAbilitySystemComponent();
	auto enchantComponent = characterTarget->GetEnchantmentComponent();

	return attackTarget
		&& EnchantedTargets.Num() < MaxEnchantedTargets
		&& (enchantComponent && !enchantComponent->HasEnchantment(EnchantToGrant))
		&& (targetAbilitySystem && !targetAbilitySystem->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("StatusEffect.Enchanted")));
}

bool UGrowAttackComponent::CanGrow(const TArray<AActor*>& attackTargets) const {
	if (!CanAttack(nullptr)) return false;
	
	auto enchantedTargetsCount = EnchantedTargets.Num();
	for (auto target : attackTargets) {
		if (enchantedTargetsCount >= MaxEnchantedTargets) {
			break;
		}

		if (CanGrow(target)) {
			return true;
		}

		enchantedTargetsCount++;
	}
	return false;
}

void UGrowAttackComponent::Grow(const TArray<AActor*>& attackTargets) {
	AttackTargets.Empty();

	auto enchantedTargetsCount = EnchantedTargets.Num();
	for (auto target : attackTargets) {
		if (enchantedTargetsCount >= MaxEnchantedTargets) {
			break;
		}

		if (CanGrow(target)) {
			AttackTargets.Add(target);
		}

		enchantedTargetsCount++;
	}
	AttackLocal();
}

void UGrowAttackComponent::ApplyEnchant() {
	const auto ownerCharacter = Cast<ABaseCharacter>(GetOwner());

	if (!actorquery::is::alive(ownerCharacter)) {
		return;
	}
	
	for (auto target : AttackTargets) {
		if (target.IsValid()) {
			ApplyEnchant(Cast<ABaseCharacter>(target.Get()));
		}
	}
}

void UGrowAttackComponent::ApplyEnchant(ABaseCharacter* targetCharacter) {
	if (!actorquery::is::alive(targetCharacter)) {
		return;
	}

	const auto ownerCharacter = Cast<ABaseCharacter>(GetOwner());
	auto abilitySystem = ownerCharacter->GetAbilitySystemComponent();
	auto targetAbilitySystem = targetCharacter->GetAbilitySystemComponent();

	FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::Instigator);
	FGameplayEffectSpec spec = effects::CreateGameplayEffectSpec<UGrowAttackBuffGameplayEffect>(abilitySystem);
	spec.GetContext().AddInstigator(ownerCharacter, ownerCharacter);
	if (abilitySystem->ApplyGameplayEffectSpecToTarget(spec, targetAbilitySystem).IsValid()) {
		EnchantedTargets.Push(targetCharacter);
		auto enchantComponent = targetCharacter->GetEnchantmentComponent();
		enchantComponent->AddEnchantments({ FEnchantmentData(EnchantToGrant, 1) }, { ownerCharacter });
	}
}

void UGrowAttackComponent::CompleteAttack() {
	SuccessfulAttackCounter++;
}

void UGrowAttackComponent::AttackCpp(AActor* attackTarget, int32 seed /*= 0*/, FSharedPredictionContext predictionContext) {
	// needed as this gets invoked twice0
	if (!CanAttack(attackTarget) || !GetOwner()->HasAuthority()) {
		return;
	}

	Super::AttackCpp(attackTarget);

	AttackCounter++;	
	
	if (ABaseCharacter* owner = Cast<ABaseCharacter>(GetOwner())) 
	{
		if (owner->HasAuthority()) {
			owner->RemoveInvisibility();
			owner->MulticastPlayAnimationAsDynamicMontage(EnchantSequence, Slot, 0, 0.2f, 1.f, 1, 0, 0, predictionContext.GetKey());
		} else {
			owner->PlayAnimationAsDynamicMontage(EnchantSequence, Slot);
		}
	}

	auto world = GetWorld();
	world->GetTimerManager().ClearTimer(DelayTimerHandle);
	world->GetTimerManager().SetTimer(DelayTimerHandle, this, &UGrowAttackComponent::ApplyEnchant, enchantDelay, false);

	world->GetTimerManager().ClearTimer(CompleteTimerHandle);
	world->GetTimerManager().SetTimer(CompleteTimerHandle, this, &UGrowAttackComponent::CompleteAttack, totalTime, false);
}

void UGrowAttackComponent::Cancel() {
	auto world = GetWorld();
	
	world->GetTimerManager().ClearTimer(DelayTimerHandle);
	world->GetTimerManager().ClearTimer(CompleteTimerHandle);
	
	if (EnchantSequence != nullptr) {
		ABaseCharacter* owner = Cast<ABaseCharacter>(GetOwner());
		owner->MulticastStopSlotAnimation(Slot);
	}
}

void UGrowAttackComponent::Disenchant() {
	for (auto& enchantedTarget : EnchantedTargets) {
		DisenchantTarget(enchantedTarget);
	}

	EnchantedTargets.Empty();
}

void UGrowAttackComponent::HandleDamage(float) {
	if (deflateOnDamage) {
		Disenchant();
	}
}

UGrowAttackBuffGameplayEffect::UGrowAttackBuffGameplayEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.EnchantedEnchanter"), 0, 1);

	auto statusTag = FGameplayTag::RequestGameplayTag("StatusEffect.EnchantedEnchanter");
	
	InheritableOwnedTagsContainer.AddTag(statusTag);
	ApplicationTagRequirements.IgnoreTags.AddTag(statusTag);
}