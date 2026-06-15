// © 2020 Mojang Synergies AB. TM Microsoft Corporation.


#include "BeeNest.h"
#include <GameplayTagContainer.h>
#include <AbilitySystemComponent.h>
#include <TimerManager.h>
#include "game/mobspawn/MobSpawner.h"
#include "game/mobspawn/MobSpawnConfigs.h"
#include "game/mobspawn/MobSpawnProviders.h"
#include "game/mobspawn/SpawnLocationUtil.h"
#include <AbilitySystemGlobals.h>
#include <UnrealNetwork.h>
#include "game/mobspawn/MobAction.h"
#include "game/affector/Affectors.h"
#include "character/mob/MobSummonHelper.h"
#include "util/Algo.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/effects/DifficultyGameplayEffect.h"

// Sets default values
ABeeNest::ABeeNest() {
	PrimaryActorTick.bCanEverTick = false;
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	bReplicates = true;

	SummonHelper = CreateDefaultSubobject<UMobSummonHelper>(TEXT("SummonHelper"));
	SummonHelper->MobToSpawn = EntityType::Bee;
	SummonHelper->ManuallyTriggerSpawnCue = true;
	SummonHelper->SpawnCue = FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.MobSummon.Summon.Bee");
}

void ABeeNest::InitFromItem(int maxNumBees, float sourceItemPower, float spawnDelayRangeMin, float spawnDelayRangeMax) {
	MaxNumBees = maxNumBees;
	BeesToSpawnAtAtime = affector::get(GetWorld()).GetPetSpawnCount();
	SpawnDelayRangeMin = FMath::Min(spawnDelayRangeMax, spawnDelayRangeMin);
	SpawnDelayRangeMax = FMath::Max(spawnDelayRangeMax, spawnDelayRangeMin);
	SourceItemPower = sourceItemPower;
}

void ABeeNest::SetLifeSpan(float timeToLiveSeconds) {
	auto& timerManager = GetWorld()->GetTimerManager();
	Super::SetLifeSpan(0.f);
;	if (TimeToLiveHandle.IsValid()) {
		timerManager.ClearTimer(TimeToLiveHandle);
	}

	if (timeToLiveSeconds > 0.f) {
		timerManager.SetTimer(TimeToLiveHandle, FTimerDelegate::CreateUObject(this, &ABeeNest::OnLifetimeExipired), timeToLiveSeconds, false);
	}
}

void ABeeNest::OnLifetimeExipired() {
	Super::SetLifeSpan(AutoDestroyDelay);
	if (EnqueuedSpawnHandle.IsValid()) {
		GetWorld()->GetTimerManager().ClearTimer(EnqueuedSpawnHandle);
	}
	for (auto* mob : SummonHelper->GetSummonedMobs()) {
		mob->OnDeath.RemoveAll(this);
	}

	Expired = true;
	ForceNetUpdate();
	OnRep_Expired();
	OnExpired.Broadcast();
}

void ABeeNest::OnMobsSummoned(const TArray<AMobCharacter*>&) {
	if (SummonHelper->CurrentNumberOfMobs() < MaxNumBees) {
		EnqueueBeeSummon();
	}
}

void ABeeNest::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABeeNest, Expired);
}

void ABeeNest::OnRep_Expired() {
	if (Expired) {
		OnDespawn();
	}
}

void ABeeNest::DestryoAllBees() {
	for (auto* bee : SummonHelper->GetSummonedMobs()) {
		bee->Kill();
	}
}

TArray<AMobCharacter*> ABeeNest::GetBees() const
{
	return SummonHelper->GetSummonedMobs();
}

// Called when the game starts or when spawned
void ABeeNest::BeginPlay() {
	Super::BeginPlay();
	InstanceTracker< ABeeNest >::AddInstance(GetWorld(), this);

	if (HasAuthority()) {
		EnqueueBeeSummon();
	}
}

void ABeeNest::EndPlay(EEndPlayReason::Type reason) {
	Super::EndPlay(reason);
	InstanceTracker< ABeeNest >::RemoveInstance(GetWorld(),this);
}

void ABeeNest::EnqueueBeeSummon() {
	if (SummonHelper->CurrentNumberOfMobs() < MaxNumBees) {
		GetWorld()->GetTimerManager().SetTimer(EnqueuedSpawnHandle, FTimerDelegate::CreateUObject(this, &ABeeNest::SummonBee), FMath::RandRange(SpawnDelayRangeMin, SpawnDelayRangeMax), false);
	}
}

void ABeeNest::SummonBee() {
	EnqueuedSpawnHandle.Invalidate();
	using namespace game::mobspawn;

	const auto spawns = FMath::Min(BeesToSpawnAtAtime, MaxNumBees - SummonHelper->CurrentNumberOfMobs());
	const auto locationFinder = [&]() { return RandomLocationAround(this, SpawnRadius); };

	auto config = configs::PlayerPets().PostSpawnAction([weakThis = TWeakObjectPtr<ABeeNest>(this)](AMobCharacter& mob) {
		if(weakThis.IsValid()) {
			mob.OnDeath.AddUObject(weakThis.Get(), &ABeeNest::OnBeeDeath);	
		}
	});

	if (auto characterOwner = Cast<ABaseCharacter>(GetOwner())) {
		config.Action(ChangeMaster(characterOwner));
	}

	SummonHelper->TrySummonMobs(providers::FromProviders(locationFinder), config, SourceItemPower.Get(1.f), spawns, FOnMobSummonedDelegate::CreateUObject(this, &ABeeNest::OnMobsSummoned));
}

void ABeeNest::OnBeeDeath() {
	if (!EnqueuedSpawnHandle.IsValid()) {
		EnqueueBeeSummon();
	}
}

void ABeeNest::Expire() {
	if (TimeToLiveHandle.IsValid()) {
		GetWorld()->GetTimerManager().ClearTimer(TimeToLiveHandle);
	}

	OnLifetimeExipired();
}

EntityType ABeeNest::GetBeeType() const {
	return SummonHelper->MobToSpawn;
}