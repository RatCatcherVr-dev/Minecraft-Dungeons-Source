// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "MobSummonItem.h"
#include "game/GameTypes.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/attributes/DifficultyAttributeSet.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/actor/character/DungeonsAbilitySystemComponent.h"
#include "game/affector/Affectors.h"
#include "game/component/HealthComponent.h"
#include "game/component/RangedAttackComponent.h"
#include "game/item/power/ItemPowerStats.h"
#include "game/item/power/ItemPowerEffectDefs.h"
#include "game/mobspawn/MobSpawner.h"
#include "game/mobspawn/MobAction.h"
#include "game/mobspawn/MobSpawnProviders.h"
#include "game/mobspawn/MobSpawnConfigs.h"
#include "game/util/ComponentUtils.h"
#include "game/util/LocationQuery.h"
#include "game/util/ActorQuery.h"
#include "world/entity/EntityTypes.h"
#include <GameplayEffect.h>
#include <AbilitySystemComponent.h>
#include <UnrealNetwork.h>
#include "game/mobspawn/SpawnLocationUtil.h"
#include "util/StringUtil.h"
#include "game/component/SoulComponent.h"
#include "util/Algo.h"
#include "game/component/TeleportToOwnerComponent.h"
#include "game/GameBP.h"

const FName UMobSummonItemGameplayEffect::MobDealDamageEffectMagnitude(TEXT("MobDealDamageEffectMagnitude"));
const FName UMobSummonItemGameplayEffect::MobIncreasedMaxHealthMagnitude(TEXT("MobIncreasedMaxHealthMagnitude"));

AMobSummonItem::AMobSummonItem() {
	Effect = UMobSummonItemGameplayEffect::StaticClass();
	PowerEffects = { USummonDamageIncrease::StaticClass() };
	bHasManualCooldownActivation = true;
	bHasManualSoulConsumption = true;
}

float AMobSummonItem::GetStatsForEntityType(EItemStats stat, EntityType type) {
	const auto mobClass = game::TypeMap::singleton().mobClass(type);

	switch (stat) {
	case EItemStats::HighestDamage:
	{
		float highest = -FLT_MAX;
		if (mobClass) {
			if (auto MeleeAttack = componentutils::GetDefaultComponentByClass<UMeleeAttackComponent>(mobClass)) {
				highest = FMath::Max(highest, MeleeAttack->GetAttackVariantsStat(MeleeAttack->GetConfiguredAttackVariants(), stat));
			}
			if (auto RangedAttack = componentutils::GetDefaultComponentByClass<URangedAttackComponent>(mobClass)) {
				highest = FMath::Max(highest, URangedAttackComponent::GetRangedAttackStat(RangedAttack->GetConfiguredAttackDefinition(), nullptr, stat));
			}
		}
		return highest > -FLT_MAX ? highest : 0.0f;
	}
	case EItemStats::LowestDamage:
	{
		float lowest = FLT_MAX;
		if (mobClass) {
			if (auto MeleeAttack = componentutils::GetDefaultComponentByClass<UMeleeAttackComponent>(mobClass)) {
				lowest = FMath::Min(lowest, MeleeAttack->GetAttackVariantsStat(MeleeAttack->GetConfiguredAttackVariants(), stat));
			}
			if (auto RangedAttack = componentutils::GetDefaultComponentByClass<URangedAttackComponent>(mobClass)) {
				lowest = FMath::Min(lowest, URangedAttackComponent::GetRangedAttackStat(RangedAttack->GetConfiguredAttackDefinition(), nullptr, stat));
			}
		}
		return lowest < FLT_MAX ? lowest : 0.0f;
	}
	}
	return -1;
}

void AMobSummonItem::OnMobSummoned(AMobCharacter* mob, const FVector& location, const FRotator& rotation) {
	if(mob) {
		auto playerOwner = Cast<APlayerCharacter>(GetOwner());
		
		const auto correctedPosition = teleport::AdjustToGround(*GetWorld(), location, *mob);
		mob->SetActorLocation(correctedPosition);
		mob->SetActorRotation(rotation);
		mob->OnDeath.AddUObject(this, &AMobSummonItem::OnPetDied);

		AddPetMob(*mob);
		const auto duration = GetItemType().getDurationSeconds();
		if (duration > 0.f) {
			FTimerHandle handle;
			GetWorld()->GetTimerManager().SetTimer(handle, FTimerDelegate::CreateUObject(this, &AMobSummonItem::KillMob, PetMobs.Last()), duration, false);
		}

		UAbilitySystemComponent* abilitySystem = playerOwner->GetAbilitySystemComponent();

		{
			FScopedPredictionWindow tmp(abilitySystem, FPredictionKey(), false);
			FGameplayCueParameters params;
			params.Location = correctedPosition;
			params.NormalizedMagnitude = 3.f / ItemPower;
			params.Instigator = playerOwner;
			params.SourceObject = mob;

			abilitySystem->ExecuteGameplayCue(GetSummonCueForType(mob->EntityType), params);
		}

		FGameplayEffectSpec spec(Cast<UMobSummonItemGameplayEffect>(Effect->GetDefaultObject()), abilitySystem->MakeEffectContext(), ItemPower);

		const float DealDamageItemPowerMultiplier = GetPowerEffect()->GetMultiplier(ItemPower);
		const float IncreasedMaxHealthItemPowerMultiplier = game::item::power::MaxHealthMultiplier(ItemPower);

		spec.SetSetByCallerMagnitude(UMobSummonItemGameplayEffect::MobDealDamageEffectMagnitude, DealDamageMultiplier * DealDamageItemPowerMultiplier);
		spec.SetSetByCallerMagnitude(UMobSummonItemGameplayEffect::MobIncreasedMaxHealthMagnitude, IncreasedMaxHealthMultiplier * IncreasedMaxHealthItemPowerMultiplier);
		abilitySystem->ApplyGameplayEffectSpecToTarget(spec, mob->GetAbilitySystemComponent());

		CleanupDeadPetMobs();

		if (PetMobs.Num() > PetAmountCap) {
			const auto mobToKill = FindWeakestMob();
			KillMob(mobToKill);
		}

		//Have to call super here as our implementation checks if we need to summon (which we just did)
		if (const auto soulCoust = Super::GetSoulActivationCost()) {
			GetSoulComponent()->ServerAddSouls(-soulCoust);
		}
	} else if (!Cooldown().IsOnCooldown()) {
		//Failed to summon mob - start a small cooldown.
		Cooldown().TriggerCooldown(0.5f);
	}
}

float AMobSummonItem::GetStats(EItemStats stat) const {
	return GetStatsForEntityType(stat, GetMobType());
}

void AMobSummonItem::BeginPlay() {
	Super::BeginPlay();

	if (!HasAuthority()) {
		return;
	}

	// in the menu, we cant die, and we cant bind
	if (const auto playerCharacter = Cast<APlayerCharacter>(GetOwner())) {
		playerCharacter->OnPlayerDeath.AddUObject(this, &AMobSummonItem::OnPlayerDeath);
		playerCharacter->OnPlayerDown.AddUObject(this, &AMobSummonItem::OnPlayerDeath);
		playerCharacter->OnPlayerTeleportedInternal.AddUObject(this, &AMobSummonItem::OnPlayerTeleported);
	}
}

bool AMobSummonItem::WillSummon() const {
	const auto petCount = affector::get(GetWorld()).GetPetSpawnCount();
	const int petAmountCap = FMath::Max(petCount, PetAmountCap);

	if (PetMobs.Num() < petAmountCap) return true;

	return algo::count_if(PetMobs, [](const auto& p) { return p.IsValid() && p->IsAlive(); }) < petAmountCap;
}

float AMobSummonItem::GetSoulActivationCost() const {
	return WillSummon() ? Super::GetSoulActivationCost() : 0;
}

int AMobSummonItem::GetAlivePetCount() const
{
	return algo::count_if(PetMobs, [](const auto& p) { return p.IsValid() && p->IsAlive(); });
}

int AMobSummonItem::GetDisplayCount() const {
	return 0;
}

void AMobSummonItem::Activate(const FPredictionKey& predictionKey) {
	const auto owner = GetCharacterOwner();
	using namespace game::mobspawn;

	if (!TeleportToOwner) {
		TeleportToOwner = NewObject<UTeleportToOwnerComponent>(GetOwner(), FName("TeleportToOwner"));
		TeleportToOwner->TeleportSpec = [&]() { return GenerateTeleportEffectSpec(); };
		const auto teleportCue = GetTeleportCueForType(GetMobType());
		TeleportToOwner->TeleportOutCue = teleportCue;
		TeleportToOwner->TeleportInCue = teleportCue;
		TeleportToOwner->RegisterComponent();
	}

	//Spawns actor so should only ever happen on server.
	if (owner->HasAuthority()) {

		const auto petCount = affector::get(GetWorld()).GetPetSpawnCount();
		PetAmountCap = FMath::Max(petCount, PetAmountCap);
		auto dir(owner->GetActorForwardVector());
		const auto degrees = 360.f / PetAmountCap;

		//Ensure our only represents valid alive pets.
		CleanupDeadPetMobs();

		for (auto pet : PetMobs) {
			if (auto maybeLocation = teleport::TryFindSpawnLocation(*owner, MobSummonOffset, dir)) {
				//Move pet

				//Remove levitation to avoid getting stuck
				const auto leviTag = FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.Levitation"));			
				pet->AbilitySystem->RemoveActiveEffectsWithTags(FGameplayTagContainer(leviTag));

				check(pet.IsValid() && "All valid indexes in the pet array should be valid pets");
				const auto teleportCue = GetTeleportCueForType(pet->EntityType);
				teleport::TeleportCharacter(*owner, *pet.Get(), maybeLocation.GetValue(), dir.Rotation(), teleportCue, teleportCue, GenerateTeleportEffectSpec());
				if (!Cooldown().IsOnCooldown()) {
					//Teleported mob, start a fraction of the full cooldown.
					Cooldown().TriggerCooldown(CalculateCooldown() * TeleportCooldownFactor);
				}
			}
			dir = dir.RotateAngleAxis(degrees, FVector::UpVector);
		}

		//Yes, this could be more optimal, but it ensures the same code path for checking if we should summon.
		if (WillSummon()) {
			//Has to happen here since 
			
			for(int summonCount = PetAmountCap - PetMobs.Num(); summonCount > 0; --summonCount) {
				if (auto maybeLocation = teleport::TryFindSpawnLocation(*owner, MobSummonOffset, dir)) {
					//Summon pet				
					SummonMob(predictionKey, maybeLocation.GetValue(), dir.Rotation());
				}
				dir = dir.RotateAngleAxis(degrees, FVector::UpVector);
			}
		}
	}

	Super::Activate(predictionKey);
}

bool AMobSummonItem::IsBusy() const {
	return PetMobs.Num() > 0;
}

void AMobSummonItem::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMobSummonItem, PetMobs);
}

void AMobSummonItem::SummonMob(const FPredictionKey& predictionKey, const FVector& location, const FRotator& rotation) {
	auto playerOwner = Cast<APlayerCharacter>(GetOwner());
	using namespace game::mobspawn;
	const auto spawnConfig = configs::PlayerPets().Action(ChangeMaster(playerOwner));
	const auto entityType = GetMobType();
	
	spawnAsync(*GetWorld(),entityType, providers::Location(location), spawnConfig, [weakThis = TWeakObjectPtr<AMobSummonItem>(this), loc = location, rot = rotation](auto mob) {
		if(weakThis.IsValid()) {
			weakThis->OnMobSummoned(mob, loc, rot);
		} else if(mob) {
			mob->Destroy();
		}
	}, MobSpawnPriority::ESpawnPriority_High);
}

void AMobSummonItem::EndPlay(EEndPlayReason::Type reason) {
	if (HasAuthority()) {
		while (PetMobs.Num()) {
			KillMob(PetMobs.Last());
		}
	}

	if (TeleportToOwner) {
		TeleportToOwner->DestroyComponent();
	}

	// in the menu, we cant die, and we cant bind
	if (const auto playerCharacter = Cast<APlayerCharacter>(GetOwner())) {
		playerCharacter->OnPlayerDeath.RemoveAll(this);
		playerCharacter->OnPlayerDown.RemoveAll(this);
		playerCharacter->OnPlayerTeleportedInternal.RemoveAll(this);
	}

	Super::EndPlay(reason);
}

EntityType AMobSummonItem::GetMobType() const
{
	return EntityTypeFromString(mobType);
}

FGameplayTag AMobSummonItem::GetSummonCueForType(EntityType type) const {
	FString cueString("GameplayCue.Trigger.MobSummon.Summon.");
	cueString += stringutil::toFString(EntityTypeToString(type));

	const auto specificTag = FGameplayTag::RequestGameplayTag(*cueString, false);

	if (specificTag.IsValid()) {
		return specificTag;
	}

	return FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.MobSummon.Summon");
}

FGameplayTag AMobSummonItem::GetTeleportCueForType(EntityType type) const {
	FString cueString("GameplayCue.Trigger.MobSummon.Teleport.");
	cueString += stringutil::toFString(EntityTypeToString(type));

	const auto specificTag = FGameplayTag::RequestGameplayTag(*cueString, false);

	if (specificTag.IsValid()) {
		return specificTag;
	}

	return FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.MobSummon.Teleport");
}

void AMobSummonItem::OnPlayerDeath() {
	while (PetMobs.Num() > 0) {
		KillMob(PetMobs.Last());
	}
}

void AMobSummonItem::OnPlayerTeleported() {
	using namespace game::mobspawn;

	if (HasAuthority()) {
		CleanupDeadPetMobs();

		const auto characterOwner = GetCharacterOwner();
		const auto ownerLocation = characterOwner->GetActorLocation();
		const auto ownerCapsuleComponent = characterOwner->GetCapsuleComponent();
		const auto ownerCapsuleHalfHeight = ownerCapsuleComponent != nullptr ? ownerCapsuleComponent->GetScaledCapsuleHalfHeight() : 0.f;

		auto dir = characterOwner->GetActorForwardVector();
		const auto degrees = 360.f / PetMobs.Num();

		for (auto petMob : PetMobs) {
			if (petMob.IsValid()) {
				TOptional<FVector> maybeLocation = teleport::TryFindSpawnLocation(*characterOwner, MobSummonOffset, dir);

				const auto adjustedLocation = [&] {
					const auto groundedLocation = maybeLocation.Get(ownerLocation - FVector{ 0.f, 0.f, ownerCapsuleHalfHeight });

					const auto mobCapsuleComponent = petMob->GetCapsuleComponent();
					const auto mobCapsuleHalfHeight = mobCapsuleComponent != nullptr ? mobCapsuleComponent->GetScaledCapsuleHalfHeight() : 0.f;
					return groundedLocation + FVector{ 0.f, 0.f, mobCapsuleHalfHeight };
				}();

				petMob->SetActorLocation(adjustedLocation, false, nullptr, ETeleportType::ResetPhysics);
				petMob->SetActorRotation(dir.Rotation());
			}

			dir = dir.RotateAngleAxis(degrees, FVector::UpVector);
		}
	}
}

void AMobSummonItem::OnPetDied() {
	CleanupDeadPetMobs();

	if (CoolDownForEachPetDeath || PetMobs.Num() == 0)
		Cooldown().TriggerCooldown(CalculateCooldown());
}

void AMobSummonItem::AddPetMob(AMobCharacter& petMob) {
	PetMobs.Emplace(&petMob);
	TeleportToOwner->AddCharacter(petMob);
}


void AMobSummonItem::CleanupDeadPetMobs() {
	PetMobs.RemoveAllSwap([](TWeakObjectPtr<AMobCharacter>& v) {
		if (v.IsValid()) {
			const auto mobHC = v->FindComponentByClass<UHealthComponent>();
			return !mobHC || mobHC->IsNotAlive();
		}
		else {
			return true;
		}
	});
}


TWeakObjectPtr<AMobCharacter> AMobSummonItem::FindWeakestMob() {
	TWeakObjectPtr<AMobCharacter> candidate = nullptr;
	for (auto mob : PetMobs) {
		if (!mob.IsValid()) {
			continue;
		}
		if (candidate == nullptr) {
			candidate = mob;
		}

		if (const auto mobHC = mob->FindComponentByClass<UHealthComponent>()) {
			const auto mobHealth = mobHC->GetCurrentHealth();
			if (const auto currentCandidateHC = candidate->FindComponentByClass<UHealthComponent>()) {
				const auto currentCandidateHealth = currentCandidateHC->GetCurrentHealth();

				if (mobHealth < currentCandidateHealth) {
					candidate = mob;
				}
			}
		}
	}

	return candidate;
}


void AMobSummonItem::KillMob(TWeakObjectPtr<AMobCharacter> mobToKill) {
	PetMobs.Remove(mobToKill);
	if (mobToKill.IsValid()) {
		mobToKill->Kill();
	}
}

TOptional<FGameplayEffectSpec> AMobSummonItem::GenerateTeleportEffectSpec() const
{
	if (TeleportEffect) {
		return FGameplayEffectSpec(TeleportEffect.GetDefaultObject(), GetCharacterOwner()->GetAbilitySystemComponent()->MakeEffectContext(), ItemPower);
	}
	return {};
}

UMobSummonItemGameplayEffect::UMobSummonItemGameplayEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;

	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	FGameplayModifierInfo maxHealthInfo;
	maxHealthInfo.Attribute = UHealthAttributeSet::MaxHealthAttribute();

	FSetByCallerFloat increasedMaxHealthMagnitude;
	increasedMaxHealthMagnitude.DataName = MobIncreasedMaxHealthMagnitude;

	maxHealthInfo.ModifierMagnitude = increasedMaxHealthMagnitude;
	maxHealthInfo.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(maxHealthInfo);

	FGameplayModifierInfo dealDamageInfo;
	dealDamageInfo.Attribute = UDifficultyAttributeSet::DifficultyDealDamageMultiplierAttribute();

	FSetByCallerFloat dealDamageMagnitude;
	dealDamageMagnitude.DataName = MobDealDamageEffectMagnitude;

	dealDamageInfo.ModifierMagnitude = dealDamageMagnitude;
	dealDamageInfo.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(dealDamageInfo);
}

