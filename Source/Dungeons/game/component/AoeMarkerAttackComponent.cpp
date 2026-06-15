#include "Dungeons.h"
#include "AoeMarkerAttackComponent.h"

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

UAoeMarkerAttackComponent::UAoeMarkerAttackComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

bool UAoeMarkerAttackComponent::OnAttackBegan(AActor* Target)
{
	if (TargetProvider->Marker)
	{
		if (GetOwnerRole() == ROLE_Authority)
		{
			while (MarkerActors.Num() < markersToSpawn)
			{
				MarkerActors.Add(Cast<ADungeonsTargetMarker>(GetWorld()->SpawnActor<ADungeonsTargetMarker>(TargetProvider->Marker.Get(), FTransform())));
			}
		}

		for (ADungeonsTargetMarker* marker : MarkerActors)
		{
			FNavLocation Destination;
			FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld())->GetRandomReachablePointInRadius(GetOwner()->GetActorLocation(), markersRadiusToSpawn, Destination);
			marker->SetActorLocation(Destination.Location);
			marker->BeginAttack(this, Target, TargetProvider, FMath::RandRange(TargetProvider->minMarkerTimeToLock, TargetProvider->maxMarkerTimeToLock));
		}
	}

	return true;
}

