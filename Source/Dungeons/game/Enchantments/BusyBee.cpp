// © 2020 Mojang Synergies AB. TM Microsoft Corporation.


#include "BusyBee.h"
#include "game/actor/character/mob/MobSummonHelper.h"
#include <Engine/ActorChannel.h>
#include "game/component/HealthComponent.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/mobspawn/MobAction.h"
#include "game/mobspawn/MobSpawnConfigs.h"
#include "game/mobspawn/MobSpawnProviders.h"
#include "game/affector/Affectors.h"
#include "game/item/power/ItemPowerStats.h"
#include "game/component/HealthComponent.h"

UBusyBee::UBusyBee() {
	TypeId = EEnchantmentTypeID::BusyBee;
	SummonHelper->MobToSpawn = EntityType::Bee;
	SummonHelper->SpawnCue = FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.MobSummon.Summon.Bee");
	MaxNumMobs = 3;
	BaseTriggerChance = 0.2f;
	TriggerChanceIncreasePerLevel = 0.1f;
}

void UBusyBee::OnAfterDealtMeleeDamage(float Damage, bool missedAttack, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream, FSharedPredictionContext window) {
	auto characterOwner = GetCharacterOwner();
	if (characterOwner->HasAuthority()) {
		UHealthComponent* targetHealthComponent = nullptr;
		auto* character = Cast<ABaseCharacter>(toWhat);
		if (character) {
			targetHealthComponent = character->GetHealthComponent();
		}
		else {
			targetHealthComponent = toWhat->FindComponentByClass<UHealthComponent>();
		}

		if (targetHealthComponent && targetHealthComponent->IsNotAlive()) {
			TWeakObjectPtr<ABaseCharacter> target = character;
			game::mobspawn::TransformProvider location = [target, atLocation]() { return FTransform(target.IsValid() ? target->GetMesh()->GetComponentLocation() : atLocation); };
			AttemptMobSummon(location, randStream);
		}
	}
}