

#include "game/component/PlayerCharacterMovementComponent.h"
#include "CustomMover.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "game/component/OxygenComponent.h"


void UCustomMover::ExecuteCustomMove_Implementation(UPlayerCharacterMovementComponent* Component) const {
	
}

void UPushbackCustomMover::ExecuteCustomMove_Implementation(UPlayerCharacterMovementComponent* Component) const {
	FPushback newPushback = Pushback;
	if (UAbilitySystemComponent* AbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Component->GetOwner()))
	{
		if (ABaseCharacter* baseCharacter = Cast<ABaseCharacter>(Component->GetOwner())) {
			if (baseCharacter->IsUnderwater())
			{
				newPushback.pushbackStrength *= UnderWaterMultiplier;
				newPushback.pushbackZFactor *= UnderWaterMultiplier;
			}
		}
	}
	pushback::pushback(newPushback, *Component->GetOwner(), *Component->GetOwner(), 1.0f, bApplyResistance);
}
