#include "OxygenAttributeSet.h"
#include "Dungeons.h"

#include "GameplayEffectExtension.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/component/OxygenComponent.h"
#include <AbilitySystemComponent.h>
#include <Net/UnrealNetwork.h>
#include "../effects/GameplayEffectUtil.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"

DEFINE_ATTRIBUTE_REPLICATION_FUNCTION(Oxygen, UOxygenAttributeSet)
DEFINE_ATTRIBUTE_REPLICATION_FUNCTION(MaxOxygen, UOxygenAttributeSet)

void UOxygenAttributeSet::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UOxygenAttributeSet, Oxygen, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UOxygenAttributeSet, MaxOxygen, COND_None, REPNOTIFY_Always);
}

void UOxygenAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	if (Attribute == OxygenAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, MaxOxygen);

		// due to the rounding of floats, Oxygen can actually reach zero one second later than supposed to,
		// checking if value under 0.001 to account for these rounding errors.
		if (NewValue <= 0.001f)
		{
			NewValue = 0.f;
		}
	}
}

void UOxygenAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == OxygenAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, MaxOxygen);

		// due to the rounding of floats, Oxygen can actually reach zero one second later than supposed to,
		// checking if value under 0.001 to account for these rounding errors.
		if (NewValue <= 0.001f)
		{
			NewValue = 0.f;
		}
	}
}

DEFINE_ATTRIBUTE_FUNCTION(Oxygen, UOxygenAttributeSet)
DEFINE_ATTRIBUTE_FUNCTION(MaxOxygen, UOxygenAttributeSet)