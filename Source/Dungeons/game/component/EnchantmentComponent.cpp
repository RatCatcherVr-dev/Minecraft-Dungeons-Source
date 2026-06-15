#include "Dungeons.h"
#include "EnchantmentComponent.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/Enchantments/Enchantment.h"
#include "game/Enchantments/EnchantmentUtil.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include <Runtime/Engine/Classes/Engine/ActorChannel.h>
#include <Net/UnrealNetwork.h>
#include "ReviveComponent.h"
#include "util/EnumUtil.h"
#include "util/Algo.hpp"
#include "game/abilities/attributes/HealthAttributeSet.h"

#ifndef SHIPPING
static void validate(const TArray<UEnchantment*>& Enchantments, EEnchantmentTypeID typeId, const AActor* source) {
	auto existing = Enchantments.FindByPredicate([&](const UEnchantment* v) { return v->TypeId == typeId; });
	const auto enchType = game::enchantment::type::getEnchantmentType(typeId);
	const FString sourceName = source->GetName();
	const FString enchantmentName = GetEnumValueToStringStripped(typeId);

	if (existing) {
		const FString existingSourceName = (*existing)->GetSource()->GetName();
		UE_LOG(LogEnchantments, Warning, TEXT("Enchantment %s added multiple times (existing source: %s, new source: %s). If this is a stacking enchantment, it has been added twice erronously."), *enchantmentName, *existingSourceName, *sourceName);
	}
}

#else 

#define validate(x, y, z) do {} while (0);

#endif

EnchantmentSpawnConfig& EnchantmentSpawnConfig::SetOverriding() {
	overriding = true;
	return *this;
}

EnchantmentSpawnConfig& EnchantmentSpawnConfig::SetTreatAsInherited() {
	treatAsInherited = true;
	return *this;
}

EnchantmentSpawnConfig& EnchantmentSpawnConfig::SetOwnerItemRarity(EItemRarity rarity) {
	ownerItemRarity = rarity;
	return *this;
}

EnchantmentSpawnConfig createEnchantmentSpawnConfig(AActor* source) {
	return { source };
}

UEnchantmentComponent::UEnchantmentComponent() {
	PrimaryComponentTick.bCanEverTick = false;
	DefaultRandom.GenerateNewSeed();
	SetIsReplicated(true);
}

void UEnchantmentComponent::OnDeath() {
	for (auto enchantment : GetValidEnchantments()) {
		enchantment->OnEnd();
	}
}

void UEnchantmentComponent::OnRespawn() {
	for (auto enchantment : GetValidEnchantments()) {
		enchantment->OnStart();
	}
}

void UEnchantmentComponent::OnRevived(AActor* revivingActor) {
	OnRespawn();
}

void UEnchantmentComponent::AddEnchantments(const TArray<FEnchantmentData>& enchantmentData, EnchantmentSpawnConfig config) {
	if (!config.source) {
		config.source = GetOwner();
	}
	
	for (auto enchant : enchantmentData) {
		SpawnEnchantment(enchant, config);
	}

	OnRep_Enchantments();
}

UEnchantment* UEnchantmentComponent::SpawnEnchantment(const FEnchantmentData& data, EnchantmentSpawnConfig config) {
	if (data.Level == 0) {
		return nullptr;
	}

	if (Enchantments.ContainsByPredicate([&](const UEnchantment* it) {
		const auto newIsStrongerOverride = [&] { return config.overriding && data.Level > it->Level; };
		return it->TypeId == data.TypeID && it->bIsOverriding && !newIsStrongerOverride();
	})) {
		return nullptr;
	}

	// If we're overriding, remove all enchantments of the same type. We know
	// they are weaker or we'd have found a stronger and returned early above.
	if (config.overriding) {
		for (int i = Enchantments.Num() - 1; i >= 0; --i) {
			UEnchantment* removalCandidate = Enchantments[i];
			if (removalCandidate->TypeId != data.TypeID) {
				continue;
			}
			Enchantments.RemoveAtSwap(i);
			removalCandidate->End();
			removalCandidate->DestroyComponent();
		}
	}

	const auto existing = Enchantments.FindByPredicate([&](const UEnchantment* v) { return v->TypeId == data.TypeID && v->GetSource() == config.source; });
	ensure(!(config.overriding && existing));

	const auto& enchantmentType = game::enchantment::type::getEnchantmentType(data.TypeID);
	if (enchantmentType.hasTag(EEnchantmentTag::Stacking) && existing) {
		UE_LOG(LogEnchantments, Verbose, TEXT("Registered stacking enchantment for al ready existing enchantment %s from source %s."), *GetEnumValueToStringStripped(data.TypeID), *config.source->GetName());
		auto existingEnchantment = *existing;
		existingEnchantment->OnEnd();
		existingEnchantment->Level += data.Level;
		existingEnchantment->OnStart();
		return existingEnchantment;
	}
	
	validate(Enchantments, data.TypeID, config.source);

	const auto enchantmentClass = UEnchantment::GetEnchantmentTypeClass(data.TypeID);
	auto enchantmentEntity = NewObject<UEnchantment>(GetOwner(), enchantmentClass ? enchantmentClass : UEnchantment::StaticClass());
	Enchantments.Add(enchantmentEntity);
	enchantmentEntity->bIsOverriding = config.overriding;
	enchantmentEntity->bAlwaysTrigger = config.overriding;
	enchantmentEntity->TreatAsInherrent = config.treatAsInherited.Get(enchantmentEntity->TreatAsInherrent);
	enchantmentEntity->SetSource(config.source);
	enchantmentEntity->SetLevel(data.Level);
	enchantmentEntity->SetEnchantmentSource(data.Source);
	enchantmentEntity->SetOwnerItemRarity(config.ownerItemRarity.Get(enchantmentEntity->OwnerItemRarity));
	enchantmentEntity->RegisterComponent();
	enchantmentEntity->Start();

	OnEnchantmentRegistered(enchantmentEntity);

	UE_LOG(LogEnchantments, Verbose, TEXT("Spawned new enchantment %s from source %s."), *GetEnumValueToStringStripped(data.TypeID));

	return enchantmentEntity;
}

void UEnchantmentComponent::RemoveEnchantments(const AActor* source) {
	TArray<UEnchantment*> enchants;
	GetOwner()->GetComponents(enchants);

	if (!source) source = GetOwner();

	enchants = enchants.FilterByPredicate([&](const UEnchantment* v) {
		return v->GetSource() == source;
	});

	for (auto entry : enchants) {
		UE_LOG(LogEnchantments, Verbose, TEXT("Removed enchantment %s from source %s."), *GetEnumValueToStringStripped(entry->GetTypeId()), *source->GetName());
		entry->End();
		Enchantments.RemoveSwap(entry);
		entry->DestroyComponent();
	}

	OnRep_Enchantments();
}

void UEnchantmentComponent::OnRep_Enchantments() {
	// D11.SSN
	// cast check required as game equips armour during loading screens, before playerCharacter is initialised
	if (auto* playerCharacter = Cast<APlayerCharacter>(GetOwner())) {
		if (auto* tracker = playerCharacter->GetStatTracker()) {
			tracker->GearChange(GetEnchantmentsWithRarity());
		}
	}

	OnEnchantmentsUpdated.Broadcast();
}

void UEnchantmentComponent::TryRegisterEnchantment(UEnchantment* enchantment) {
	// Guard used for when quick-registering enchantments through AddEnchantment
	const auto index = Enchantments.IndexOfByKey(enchantment);
	if (index == INDEX_NONE) {
		validate(Enchantments, enchantment->GetTypeId(), GetOwner());
		Enchantments.Add(enchantment);
		enchantment->SetSource(GetOwner());
		enchantment->Start();
		OnRep_Enchantments();
		UE_LOG(LogEnchantments, Verbose, TEXT("Registered native enchantment %s"), *GetEnumValueToStringStripped(enchantment->GetTypeId()));
		OnEnchantmentRegistered(enchantment);
	}
}

void UEnchantmentComponent::TryUnregisterEnchantment(UEnchantment* enchantment) {
	// Guard used for when quick-unregistering enchantments through AddEnchantment
	const auto index = Enchantments.IndexOfByKey(enchantment);
	if (index != INDEX_NONE) {
		enchantment->End();
		Enchantments.RemoveAtSwap(index);
		OnRep_Enchantments();
	}

}

TArray<FEnchantmentData> UEnchantmentComponent::GetEnchantments() const {
	TArray<FEnchantmentData> enchantments;
	for (const UEnchantment* entry : Enchantments) {
		if (entry != nullptr) {
			enchantments.Emplace(entry->GetTypeId(), entry->GetLevel(), entry->GetEnchantmentTypeCategory(entry->GetTypeId()), entry->GetEnchantmentSource());
		}
	}
	return enchantments;
}

TArray<FEnchantmentDataWithRarity> UEnchantmentComponent::GetEnchantmentsWithRarity() const {
	TArray<FEnchantmentDataWithRarity> enchantments;
	for (const UEnchantment* entry : Enchantments) {
		if (entry != nullptr) {
			FEnchantmentData enchantment(entry->GetTypeId(), entry->GetLevel(), entry->GetEnchantmentTypeCategory(entry->GetTypeId()), entry->GetEnchantmentSource());
			enchantments.Emplace(enchantment, entry->GetOwnerItemRarity());
		}
	}
	return enchantments;
}

void UEnchantmentComponent::BeginPlay() {
	Super::BeginPlay();

	if (auto player = Cast<APlayerCharacter>(GetOwner())) {		
		player->OnPlayerDeath.AddUObject(this, &UEnchantmentComponent::OnDeath);
		player->OnPlayerDown.AddUObject(this, &UEnchantmentComponent::OnDeath);
		player->OnPlayerRevive.AddUObject(this, &UEnchantmentComponent::OnRespawn);		

		if (auto reviveComponent = player->FindComponentByClass<UReviveComponent>()) {
			//reviveComponent->OnReviveCompleted.AddUObject(this, &UEnchantmentComponent::OnRevived);
		}
	}
	else if (auto mob = Cast<AMobCharacter>(GetOwner())) {
		mob->OnDeath.AddUObject(this, &UEnchantmentComponent::OnDeath);
	}
}

void UEnchantmentComponent::GetLifetimeReplicatedProps(TArray < FLifetimeProperty > & OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UEnchantmentComponent, Enchantments);
}

int UEnchantmentComponent::HighestIndex(const TArray<FEnchantmentData>& enchantments) {
	int index = -1;
	for (int i = 0; i < enchantments.Num(); ++i) {
		if (enchantments[i].TypeID != EEnchantmentTypeID::Unset) {
			index = i;
		}
	}
	return index;
}

int UEnchantmentComponent::HighestAvailableIndex(const TArray<FEnchantmentData>& enchantments) {
	int sizeToCheck = std::min(enchantments.Num(), game::enchantment::MaxNumEnchantments - game::enchantment::RowSize);

	int rowScore, i;

	for (i = rowScore = 0; i < sizeToCheck; ++i) {
		//Reset when we begin a new row. If haven't accumulated enough score, break and return.
		if (i && !(i%game::enchantment::RowSize)) {
			if (rowScore < game::enchantment::RequiredPointsPerRow) break;
			rowScore = 0;
		}
		rowScore += enchantments[i].Level;
	}

	//We clamp result should the enchantments array be too ms
	return FMath::Max(0, rowScore < game::enchantment::RequiredPointsPerRow ? i - 1 : enchantments.Num() - 1);
}


TArray<UEnchantment*> UEnchantmentComponent::GetEnchantmentsBySource(const AActor* source) const {
	auto validEnchants = GetValidEnchantments();
	return validEnchants.FilterByPredicate([source](const UEnchantment* v) {return v->GetSource() == source; });	
}

UEnchantment* UEnchantmentComponent::GetEnchantment(EEnchantmentTypeID id) {
	auto validEnchants = GetValidEnchantments();
	auto entry = validEnchants.FindByPredicate([id](const UEnchantment* v) {return v->TypeId == id; });

	return entry ? *entry : nullptr;
}

TArray<UEnchantment*> UEnchantmentComponent::GetValidEnchantments() const {
	return algo::copy_if(Enchantments, RETLAMBDA(IsValid(it)));
}

TArray<UGearUtil*> UEnchantmentComponent::GetValidGearUtils() const {
	return algo::copy_if_map_tarray(Enchantments, RETLAMBDA(IsValid(it)), RETLAMBDA(Cast<UGearUtil>(it)));
}
