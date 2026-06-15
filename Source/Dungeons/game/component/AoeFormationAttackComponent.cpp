#include "Dungeons.h"
#include "AoeFormationAttackComponent.h"

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

UAoeFormationAttackComponent::UAoeFormationAttackComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

bool UAoeFormationAttackComponent::OnAttackBegan(AActor* Target)
{
	if (AttackFormations.Num() <= 0)
	{
		return false;
	}

	const FAttackFormationPositions& AttackFormation = AttackFormations[FMath::RandRange(0, AttackFormations.Num() - 1)];
	if (AttackFormation.positions.Num() <= 0)
	{
		return false;
	}

	if (TargetProvider->Marker)
	{
		if (GetOwnerRole() == ROLE_Authority)
		{
			while (MarkerActors.Num() < AttackFormation.positions.Num())
			{
				MarkerActors.Add(Cast<ADungeonsTargetMarker>(GetWorld()->SpawnActor<ADungeonsTargetMarker>(TargetProvider->Marker.Get(), FTransform())));
			}
		}

		FVector location = GetOwner()->GetActorLocation();
		for (int i = 0; i < AttackFormation.positions.Num(); ++i)
		{
			if (ADungeonsTargetMarker* marker = MarkerActors.IsValidIndex(i) ? MarkerActors[i] : nullptr)
			{
				marker->SetActorLocation(location + AttackFormation.positions[i]);
				marker->BeginAttack(this, Target, TargetProvider, FMath::FRandRange(TargetProvider->minMarkerTimeToLock, TargetProvider->maxMarkerTimeToLock));
			}
		}
	}

	return true;
}

