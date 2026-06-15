// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "EnchantersTome.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/item/power/ItemPowerEffectDefs.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/util/ActorQuery.h"
#include "game/Enchantments/EnchantmentUtil.h"
#include "world/entity/MobTags.h"

AEnchantersTome::AEnchantersTome() {
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	PrimaryActorTick.TickInterval = 0.1f;
	Effect = UGrowAttackBuffGameplayEffect::StaticClass();
	bCanBeInterruptedWithUserAction = false;

	PowerEffects = { UDurationIncrease::StaticClass() };
}

float AEnchantersTome::GetDuration() const {
	return GetItemType().getDurationSeconds() * GetPowerEffect()->GetMultiplier(ItemPower);
}

void AEnchantersTome::Toggle(const FPredictionKey& predictionKey, bool Enabled) {
	Super::Toggle(predictionKey, Enabled);

	if (Enabled) {
		OnToggleStart(predictionKey);
	}
	else {
		OnToggleEnd(predictionKey);
	}

	if (Role == ROLE_Authority) {
		SetActorTickEnabled(Enabled);
	}
}

void AEnchantersTome::OnToggleStart(const FPredictionKey& predictionKey) {
	if (Role == ROLE_Authority) {
		if (auto world = GetWorld()) {
			world->GetTimerManager().SetTimer(InterruptTimer, this, &AEnchantersTome::InterruptServer, GetDuration());
		}

		if (auto owner = GetPlayerOwner()) {
			owner->GetHealthComponent()->OnDeath.AddUObject(this, &AEnchantersTome::InterruptServer);
		}

		SpawnTomeMulticast();
	}
}

void AEnchantersTome::OnToggleEnd(const FPredictionKey& predictionKey) {
	for(const auto target : CurrentTargets) {
		DisenchantTarget(target);
	}

	if (Role == ROLE_Authority) {
		if (const auto world = GetWorld()) {
			world->GetTimerManager().ClearTimer(InterruptTimer);
		}

		if (auto owner = GetPlayerOwner()) {
			owner->GetHealthComponent()->OnDeath.RemoveAll(this);
		}

		DestroyTomeMulticast();
	}

	CurrentTargets.Empty();
	CachedCharacterMysteryEnchants.Empty();
}

void AEnchantersTome::DisenchantTarget(TWeakObjectPtr<ABaseCharacter> target) {
	if (!target.IsValid()) return;

	auto abilitySystem = target->GetAbilitySystemComponent();

	target->GetEnchantmentComponent()->RemoveEnchantments(GetPlayerOwner());

	FGameplayEffectQuery query;
	query.EffectDefinition = Effect;
	target->GetAbilitySystemComponent()->RemoveActiveEffects(query);
}

void AEnchantersTome::SpawnTomeMulticast_Implementation(){
	FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	TomeActor = GetWorld()->SpawnActor<AActor>(TomeActorClass, params);
	TomeActor->AttachToActor(GetPlayerOwner(), FAttachmentTransformRules::KeepRelativeTransform);
}

void AEnchantersTome::DestroyTomeMulticast_Implementation() {
	if (TomeActor.IsValid()) {
		TomeActor->Destroy();
	}
}

void AEnchantersTome::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	//Clean up weak targets.
	CurrentTargets.RemoveAllSwap([](const auto& target) { return !target.IsValid(); });

	const auto outOfRangeTargets = CurrentTargets.FilterByPredicate([&](const TWeakObjectPtr<ABaseCharacter>& target) {
		return target.IsValid() && target->GetHorizontalDistanceTo(GetPlayerOwner()) > FMath::Max(TargetRange, TargetBreakRange);
	});

	for (const auto& target : outOfRangeTargets) {
		DisenchantTarget(target);
		CurrentTargets.RemoveSwap(target);
	}

	if(CurrentTargets.Num() < MaxTargetCount) {
		TArray<ABaseCharacter*> applicableTargets;

		if(GetApplicableCharacters(applicableTargets)) {
			for (ABaseCharacter* target : applicableTargets) {
				EnchantTarget(target);
			}
		}
	}
}

void AEnchantersTome::EnchantTarget(ABaseCharacter* target){
	APlayerCharacter* owner = GetPlayerOwner();
	UAbilitySystemComponent* abilitySystem = owner->GetAbilitySystemComponent();
	UAbilitySystemComponent* targetAbilitySystem = target->GetAbilitySystemComponent();

	FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::Instigator);
	FGameplayEffectSpec spec = effects::CreateGameplayEffectSpec<UGrowAttackBuffGameplayEffect>(abilitySystem);
	spec.GetContext().AddInstigator(owner, this);

	if (abilitySystem->ApplyGameplayEffectSpecToTarget(spec, targetAbilitySystem).IsValid()) {
		CurrentTargets.Push(target);
		EEnchantmentTypeID randomEnchantment = EEnchantmentTypeID::Unset;

		TArray<FEnchantmentData> enchantments;

		if (TArray<FEnchantmentData>* foundEnchantments = CachedCharacterMysteryEnchants.Find(target)) {
			enchantments = *foundEnchantments;
		}
		else {
			enchantments.Add(Cast<APlayerCharacter>(target) ? GetRandomPlayerCapableEnchantment() : GetRandomPetCapableEnchantment());
			enchantments.Add(GetRandomPetCapableEnchantment());
			CachedCharacterMysteryEnchants.Add(target) = enchantments;
		}

		AddAbsoluteEnchantments(enchantments);

		target->GetEnchantmentComponent()->AddEnchantments(enchantments, { owner });

		if (ABasePlayerController* targetPlayerController = target->GetController<ABasePlayerController>())
		{
			for (auto enchant : enchantments)
			{
				targetPlayerController->ReceiveOnEnchantmentTriggered(enchant.TypeID);
			}
		}
	}
}

bool AEnchantersTome::GetApplicableCharacters(TArray<ABaseCharacter*>& outCharacters) {
	const APlayerCharacter* owner = GetPlayerOwner();

	const auto isEnchantable = [&](ABaseCharacter* pActor) {
		if(!pActor->IsAlive()){
			return false;
		}

		if (CurrentTargets.Find(pActor) != INDEX_NONE) {
			return false;
		}

		if (Cast<APlayerCharacter>(pActor)) {
			return true;
		}

		if (auto mob = Cast<AMobCharacter>(pActor)) {
			return  !mob->HasTag(MobTags::HashTag_Unenchantable) && !mob->HasTag(MobTags::HashTag_Cosmetic) && mob->IsFriendlyTowardsPlayers();
		}

		return false;
	};

	outCharacters = actorquery::getNearbyActors<ABaseCharacter>(owner, TargetRange, isEnchantable);

	outCharacters.Sort([&owner](ABaseCharacter& a, ABaseCharacter& b) {
		if (Cast<APlayerCharacter>(&a) != Cast<APlayerCharacter>(&b)) {
			return Cast<APlayerCharacter>(&a) != nullptr;
		}

		return a.GetHorizontalDistanceTo(owner) < b.GetHorizontalDistanceTo(owner);
	});

	outCharacters.SetNum(FMath::Min(outCharacters.Num(), MaxTargetCount - CurrentTargets.Num()));

	return outCharacters.Num() > 0;
}

void AEnchantersTome::AddAbsoluteEnchantments(TArray<FEnchantmentData>& outEnchantments) {
	const TArray<EnchantmentType>& availableEnchantments = game::enchantment::type::getAvailableEnchantments();
	TArray<EnchantmentType> absoluteEnchants = availableEnchantments.FilterByPredicate([](const EnchantmentType& type) { return type.hasEnchantersTomeFlags(EEnchantersTomeFlags::Absolute); });

	if (TotalAbsoluteEnchantsToApply >= outEnchantments.Num()) {
		for (const auto enchant : absoluteEnchants) {
			outEnchantments.Add(FEnchantmentData(enchant.getEnchantmentTypeID(), 1));
		}
	}
	else {
		for (int i = 0; i < TotalAbsoluteEnchantsToApply; i++) {
			const int randomIndex = FMath::RandRange(0, absoluteEnchants.Num() - 1);
			outEnchantments.Add(FEnchantmentData(absoluteEnchants[randomIndex].getEnchantmentTypeID(), EnchantmentLevel));
			absoluteEnchants.RemoveAt(randomIndex);
		}
	}
}

FEnchantmentData AEnchantersTome::GetRandomPetCapableEnchantment() const {
	const TArray<EnchantmentType>& availableEnchantments = game::enchantment::type::getAvailableEnchantments();
	const TArray<EnchantmentType> petCapableEnchants = availableEnchantments.FilterByPredicate([](const EnchantmentType& type) { return type.hasEnchantersTomeFlags(EEnchantersTomeFlags::Pet); });
	return FEnchantmentData(petCapableEnchants[FMath::RandRange(0, petCapableEnchants.Num() - 1)].getEnchantmentTypeID(), EnchantmentLevel);
}

FEnchantmentData AEnchantersTome::GetRandomPlayerCapableEnchantment() const {
	const TArray<EnchantmentType>& availableEnchantments = game::enchantment::type::getAvailableEnchantments();
	const TArray<EnchantmentType> playerCapableEnchants = availableEnchantments.FilterByPredicate([](const EnchantmentType& type) { return type.hasEnchantersTomeFlags(EEnchantersTomeFlags::Player); });
	return FEnchantmentData(playerCapableEnchants[FMath::RandRange(0, playerCapableEnchants.Num() - 1)].getEnchantmentTypeID(), EnchantmentLevel);
}