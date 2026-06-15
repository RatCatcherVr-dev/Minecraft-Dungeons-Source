#include "Dungeons.h"
#include "AoeMarkerComponent.h"

#include "game/component/HealthComponent.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include <AbilitySystemComponent.h>
#include <DrawDebugHelpers.h>
#include "game/abilities/effects/executions/DamageExecutionCalculation.h"
#include "game/abilities/effects/calculations/DamageModCalculations.h"
#include "Assets/DungeonsAssetManager.h"
#include "PlayerExperienceComponent.h"
#include "DungeonsGameInstance.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "AoeAttackComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "NavigationSystem.h"

UAoeMarkerComponent::UAoeMarkerComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

FVector UAoeMarkerComponent::GetAttackLocation() const
{
	return EarliestLockedMarker ? EarliestLockedMarker->GetActorLocation() : FVector::ZeroVector;
}

void UAoeMarkerComponent::EndAttack(AActor* attackTarget /*= nullptr*/)
{
	Super::EndAttack(attackTarget);

	for (ADungeonsTargetMarker* marker : MarkerActors)
	{
		marker->OnAttackEnded();
	}
}

void UAoeMarkerComponent::BeginPlay()
{
	Super::BeginPlay();

	if (UHealthComponent* OwnerHealthComponent = GetOwner()->FindComponentByClass<UHealthComponent>())
	{
		OwnerHealthComponent->OnDeath.AddUObject(this, &UAoeMarkerComponent::OnOwnerDeath);
	}
}

void UAoeMarkerComponent::AttackCpp(AActor* attackTarget, int32 seed /*= 0*/, FSharedPredictionContext predictionContext /*= FSharedPredictionContext()*/)
{
	Super::AttackCpp(attackTarget, seed, predictionContext);

	OnAttackBegan(attackTarget);
}

bool UAoeMarkerComponent::IsAttackInProgress() const
{
	auto& timerManager = GetOwner()->GetWorld()->GetTimerManager();
	return delayTimerHandle.IsValid() && timerManager.IsTimerActive(delayTimerHandle);
}

void UAoeMarkerComponent::OnOwnerDeath()
{
	for (ADungeonsTargetMarker* marker : MarkerActors)
	{
		marker->OnCharacterDeath();
	}
}

void UAoeMarkerComponent::StopAttack()
{
	Super::StopAttack();

	for (ADungeonsTargetMarker* marker : MarkerActors)
	{
		marker->OnAttackEnded();
	}
}

void UAoeMarkerComponent::SetLockedMarker(ADungeonsTargetMarker* TargetMarker)
{
	EarliestLockedMarker = TargetMarker;
}
