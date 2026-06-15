#include "Dungeons.h"
#include "LuckOfTheSea.h"

#include "AbilitySystemComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include <GameplayEffect.h>
#include "game/actor/character/mob/MobCharacter.h"
#include "game/component/HealthComponent.h"
#include "game/abilities/attributes/MovementAttributeSet.h"
#include "game/abilities/attributes/MeleeAttributeSet.h"
#include "game/abilities/attributes/RangedAttributeSet.h"
#include "game/abilities/effects/calculations/ResistanceModCalculations.h"
#include "util/CharacterQuery.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/GameSettings.h"
#include "game/item/drop/ItemDropGenerator.h"
#include "game/actor/item/ItemDropActor.h"
#include "game/util/ActorQuery.h"

namespace {
	FName RareToUniqueFraction(TEXT("RareToUniqueFraction"));
}

ULuckOfTheSea::ULuckOfTheSea() {
	TypeId = EEnchantmentTypeID::LuckOfTheSea;
	PredictiveExecution = true;

	LevelMultiplier = [this](int level) -> float {
		switch (level)
		{
		case 1:
			return luck_level_1;
		case 2:
			return luck_level_2;
		case 3:
			return luck_level_3;
		default:
			return 0;
		}
	};
	MultiplierFormatter = valueformat::asPercentage;
}

void ULuckOfTheSea::OnStart()
{
	Super::OnStart();
}

void ULuckOfTheSea::OnEnd()
{
	Super::OnEnd();
}

void ULuckOfTheSea::OnLuckChanged(FPredictionKey)
{
	//Uncomment if we end up wanting to show that this enchantment changed player's luck on the UI
	BroadcastEnchantmentTriggeredEvent();

	FGameplayCueParameters parameters;
	parameters.Location = GetCharacterOwner()->GetActorLocation();
	GetCharacterOwner()->GetAbilitySystemComponent()->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Enchantment.LuckOfTheSea"), parameters);
}

FText ULuckOfTheSea::CreateDescription() const
{
	return FText::Format(Super::CreateDescription(), FText::FromString(valueformat::asPercentage(LevelMultiplier(Level))));
}

TOptional<UGearUtil*> ULuckOfTheSea::ChangeItemRarity(EItemRarity& rarity)
{
	if(rarity == EItemRarity::Rare && FMath::RandRange(0.f, 1.f) < LevelMultiplier(Level))
	{ 
		rarity = EItemRarity::Unique;
		return this;
	}
	return TOptional<UGearUtil*>();
}
