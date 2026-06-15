#include "GearUtil.h"
#include "game/actor/character/BaseCharacter.h"

UGearUtil::UGearUtil()
{

}

UAbilitySystemComponent* UGearUtil::GetAbilitySystemComponent() const
{
	if (auto character = Cast<ABaseCharacter>(GetOwner())) {
		return character->GetAbilitySystemComponent();
	}

	return nullptr;
}

