// © 2020 Mojang Synergies AB. TM Microsoft Corporation.

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

#include "MobSummonRandomChanceEnchantment.h"
#include "util/Algo.h"

UMobSummonRandomChanceEnchantment::UMobSummonRandomChanceEnchantment() {
	SummonHelper = CreateDefaultSubobject<UMobSummonHelper>(TEXT("SummonHelper"));
	
	LevelMultiplier = [this](int level) -> float {
		return BaseTriggerChance + (level - 1) * TriggerChanceIncreasePerLevel;
	};
	MultiplierFormatter = valueformat::asPercentageChance;

}

void UMobSummonRandomChanceEnchantment::AttemptMobSummon(game::mobspawn::TransformProvider transform, FRandomStream& randStream) {
	if (NumMobsAllowedToSummon() > 0 && (randStream.FRand() <= LevelMultiplier(Level) || bAlwaysTrigger)) {
		if (SpawnDelaySeconds > 0.f) {
			GetWorld()->GetTimerManager().SetTimer(SpawnHandle, FTimerDelegate::CreateUObject(this, &UMobSummonRandomChanceEnchantment::SummonMob, transform), SpawnDelaySeconds, false);
		} else {
			SummonMob(transform);
		}
	}
}

void UMobSummonRandomChanceEnchantment::SummonMob(game::mobspawn::TransformProvider provider) {
	using namespace game::mobspawn;
	const int spawnCount = NumMobsAllowedToSummon();
	if (spawnCount > 0) {
		auto config = configs::PlayerPets().Action(ChangeMaster(GetCharacterOwner()));
		SummonHelper->TrySummonMobs(provider, config, GetSourceItemPower(), spawnCount, FOnMobSummonedDelegate::CreateUObject(this, &UMobSummonRandomChanceEnchantment::OnMobsSummoned));
	}
}


void UMobSummonRandomChanceEnchantment::EndPlay(const EEndPlayReason::Type reason) {
	Super::EndPlay(reason);

	if (SpawnHandle.IsValid()) {
		GetWorld()->GetTimerManager().ClearTimer(SpawnHandle);
	}

	if (DestroySummonsOnDestruction) {
		for (auto& mob : SummonHelper->GetSummonedMobs()) {
			if (auto hc = mob->GetHealthComponent()) {
				hc->OnDeath.RemoveAll(this);
				hc->Kill();
			}
		}
	}
}

int UMobSummonRandomChanceEnchantment::NumMobsAllowedToSummon() const {
	return FMath::Min(affector::get(GetWorld()).GetPetSpawnCount(), MaxNumMobs - SummonHelper->CurrentNumberOfMobs());
}

FText UMobSummonRandomChanceEnchantment::CreateDescription() const {
	return FText::Format(Super::CreateDescription(), FText::FromString(valueformat::asConstant(MaxNumMobs)));
}

void UMobSummonRandomChanceEnchantment::OnMobsSummoned(const TArray<AMobCharacter*>& mobs) {
	if(mobs.Num() > 0) {
		BroadcastEnchantmentTriggeredEvent();
	}
}
