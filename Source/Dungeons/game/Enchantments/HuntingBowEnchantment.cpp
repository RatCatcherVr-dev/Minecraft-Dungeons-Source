#include "Dungeons.h"
#include "HuntingBowEnchantment.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/component/HealthComponent.h"
#include "game/component/EnchantmentComponent.h"
#include "game/Enchantments/HuntingBowTaggedEnchantment.h"
#include "game/util/Tags.h"

UHuntingBowEnchantment::UHuntingBowEnchantment() {
	TypeId = EEnchantmentTypeID::HuntingBowEnchantment;
}

void UHuntingBowEnchantment::OnAfterDealtRangedDamage(float Damage, ABaseProjectile* fromProjectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream) {
	auto owner = GetOwner();
	if (owner->Role != ROLE_Authority)
		return;

	const auto mob = Cast<AMobCharacter>(toWhat);

	if (!mob->IsTargetable())
		return;

	if (auto health = mob->FindComponentByClass<UHealthComponent>()) {
		if (health->IsNotAlive())
			return;
	}

	if (const auto enchantmentComponent = mob->FindComponentByClass<UEnchantmentComponent>()) {
		bool shouldAddEnchantment = false;

		UHuntingBowTaggedEnchantment* enchant = mob->FindComponentByClass<UHuntingBowTaggedEnchantment>();
		if (!enchant) {
			enchantmentComponent->AddEnchantments({{ EEnchantmentTypeID::HuntingBowTaggedEnchantment, 1 }});
			enchant = mob->FindComponentByClass<UHuntingBowTaggedEnchantment>();
		}
		enchant->SetTaggedByCharacter(GetCharacterOwner());
	}

	FName tagName = tags::petTarget;
	if (!mob->ActorHasTag(tagName)) {
		mob->Tags.Add(tagName);
	}
}