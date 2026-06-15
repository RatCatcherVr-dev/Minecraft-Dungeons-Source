// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "Recycler.h"
#include "game/actor/item/BaseProjectile.h"
#include "game/actor/item/ItemEmitterActor.h"
#include "game/component/EquipmentComponent.h"
#include "game/item/ArrowItemSlot.h"
#include "game/item/ItemUtil.h"

URecycler::URecycler() {
	TypeId = EEnchantmentTypeID::Recycler;

	LevelMultiplier = [this](int level) -> float {
		return 40 - (10 * level);
	};
	MultiplierFormatter = valueformat::asEveryRoundedOrdinal;
}

void URecycler::OnBeforeReceivedDamage(bool& outAttackMissed, struct FGameplayEffectModCallbackData &data, UAbilitySystemComponent* targetComponent, FRandomStream& randStream, FPredictionKey key) {
	FGameplayTagContainer container;
	data.EffectSpec.GetAllAssetTags(container);
	auto effectCauser = data.EffectSpec.GetContext().GetEffectCauser();
	if (effectCauser && effectCauser->IsA(ABaseProjectile::StaticClass()) 
	 && container.HasTag(FGameplayTag::RequestGameplayTag("Damage.Ranged"))) {
		AbsorbedCount++;
		if (AbsorbedCount >= LevelMultiplier(Level)) {
			AbsorbedCount = 0;
			if (GetOwner()->HasAuthority()) {
				BroadcastEnchantmentTriggeredEvent();

				//D11.SC Send off to spawner to load & spawn this
				game::item::util::FSpecifiedStoreCountItemDrop ItemDrop(game::item::type::Arrow.getId(), 10);
				TArray<game::item::util::FSpecifiedStoreCountItemDrop>	GeneratedItemsData = { ItemDrop };
				AItemEmitterActor::SpawnSimpleItemEmitter(GetWorld(), GeneratedItemsData, GetOwner()->GetActorLocation(), GetOwner(), false, false, false, FItemEmitterActorCompleteDelegate::CreateLambda([](TArray< AStorableItem* > & SpawnedItems) {

					for (auto item : SpawnedItems)
					{
						item->SetReplicates(true);
					}
				}));
				
				outAttackMissed = true;
			}
		}
	}
}
