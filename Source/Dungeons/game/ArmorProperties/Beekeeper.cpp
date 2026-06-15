#include "Beekeeper.h"
#include "game/mobspawn/MobSpawnConfigs.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/affector/Affectors.h"
#include "game/mobspawn/MobSpawnProviders.h"
#include "game/mobspawn/SpawnLocationUtil.h"
#include "game/util/LocationQuery.h"
#include "game/mobspawn/MobAction.h"
#include "game/actor/character/mob/MobSummonHelper.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include <Engine/ActorChannel.h>
#include "util/Algo.h"

UBeekeeperGameplayEffect::UBeekeeperGameplayEffect(const FObjectInitializer& ObjectInitializer) {
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("PersistAfterDeath"));
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.BeeMine"));
}

UBeekeeper::UBeekeeper() {
	TypeID = EArmorPropertyID::Beekeeper;
	SummonHelper = CreateDefaultSubobject<UMobSummonHelper>(TEXT("SummonHelper"));
	SummonHelper->MobToSpawn = EntityType::Bee;
	SummonHelper->SpawnCue = FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.MobSummon.Summon.Bee");
}


bool UBeekeeper::IsAllowedToSpawn() const {
	return GetWorld()->GetTimeSeconds() - spawnTimestamp > Cooldown && (MaxNumberOfBees == 0 || SummonHelper->CurrentNumberOfMobs() < MaxNumberOfBees);
}

void UBeekeeper::BeginPlay() {
	Super::BeginPlay();
	auto owner = GetOwnerCharacter();
	if (owner && owner->HasAuthority()) {
		auto abilitySystem = owner->GetAbilitySystemComponent();
		handle = abilitySystem->ApplyGameplayEffectSpecToSelf(effects::CreateGameplayEffectSpec<UBeekeeperGameplayEffect>(abilitySystem, 1.f));
	}
}

void UBeekeeper::EndPlay(const EEndPlayReason::Type reason) {
	Super::EndPlay(reason);

	auto owner = GetOwnerCharacter();
	if (owner && owner->HasAuthority()) {
		auto abilitySystem = owner->GetAbilitySystemComponent();
		abilitySystem->RemoveActiveGameplayEffect(handle);
	}

	for (const auto* bee : SummonHelper->GetSummonedMobs()) {
		if (auto hc = bee->GetHealthComponent()) {
			hc->Kill();
		}
	}
}

void UBeekeeper::OnAfterReceivedDamage(const struct FGameplayEffectModCallbackData &data, FRandomStream& randStream) {
	const int spawnCount = affector::get(GetWorld()).GetPetSpawnCount();

	if (IsAllowedToSpawn() && randStream.GetFraction() <= Chance) {
		using namespace game::mobspawn;
		auto ownerCharacter = GetOwnerCharacter();
		const auto spaws = FMath::Min(spawnCount, MaxNumberOfBees - SummonHelper->CurrentNumberOfMobs());

		const auto actorLocation = ownerCharacter->GetActorLocation();
		const auto locationFinder = [&]() { return RandomLocationAround(ownerCharacter, SpawnRadius); };

		auto config = configs::PlayerPets().Actions({ ChangeMaster(ownerCharacter),[&](AMobCharacter& mob) {
			spawnTimestamp = GetWorld()->GetTimeSeconds();
		} });

		SummonHelper->TrySummonMobs(providers::FromProviders(locationFinder), config, SourceItemPower, spaws);
	}
}

UAbilitySystemComponent * UBeekeeper::GetAbilitySystemComponent() const{
	return GetOwnerCharacter()->GetAbilitySystemComponent();
}

FFormatOrderedArguments UBeekeeper::createFormattedDisplayValueString() const
{
	FFormatOrderedArguments args;
	args.Emplace(FText::FromString(valueformat::asPercentageChance(Chance)));
	args.Emplace(FText::FromString(valueformat::asConstant(MaxNumberOfBees)));

	return args;
}
