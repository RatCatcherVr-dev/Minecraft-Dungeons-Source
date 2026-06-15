#include "Dungeons.h"
#include "game/ArmorProperties/ArmorPropertyType.h"
#include "MissChance.h"
#include <AbilitySystemGlobals.h>
#include "AbilitySystemComponent.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/Enchantments/DropIncreasingEnchantment.h"
#include "game/component/HealthComponent.h"

UMissChance::UMissChance() {
	TypeID = EArmorPropertyID::MissChance;
}

void UMissChance::OnBeforeReceivedDamage(bool& outAttackMissed, struct FGameplayEffectModCallbackData& data, UAbilitySystemComponent* targetComponent, FRandomStream& randStream, FPredictionKey key) {
	if (IsBlockableDamage(data) && randStream.FRandRange(0.0f, 1.0f) < Chance) {
		outAttackMissed = true;
		auto abilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
		abilitySystem->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.Damage.Melee.Miss"), FGameplayCueParameters());
	}
}

bool UMissChance::IsBlockableDamage(const FGameplayEffectModCallbackData& data) {
	auto damageTypes = effects::GetDungeonsDamageTypes(data);

	static FGameplayTagContainer blockableTypes = FGameplayTagContainer::CreateFromArray(TArray<FGameplayTag>({ damageTag::aoe(), damageTag::ranged(), damageTag::melee(), damageTag::explosion() }));
	
	return damageTypes.HasAny(blockableTypes);
}
