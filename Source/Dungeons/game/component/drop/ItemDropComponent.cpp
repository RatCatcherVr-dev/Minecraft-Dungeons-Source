#include "Dungeons.h"
#include "ItemDropComponent.h"

#include "AbilitySystemGlobals.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "GameplayEffectTypes.h"
#include "Engine/LocalPlayer.h"
#include "game/Game.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/actor/character/loot/LootActor.h"
#include "game/actor/item/ItemEmitterActor.h"
#include "game/item/drop/ItemDropGenerator.h"
#include "game/item/drop/ItemDropUtil.h"
#include "game/util/Tags.h"
#include "game/actor/character/player/PlayerCharacter.h"

#if WITH_EDITOR
static TAutoConsoleVariable<int32> CVarDumpItemDropsOnDeath( TEXT("Dungeons.Items.DumpItemDropsOnDeath"), 0, 
	TEXT("Arg1: Number of iterations. Performs a number of fake item drops and writes the results into the Saved directory."), ECVF_Cheat);
#endif

UItemDropComponent::UItemDropComponent(): bEnableDrops(true),
                                          bElongatedDrops(false),
                                          ElongatedDropDelay(0.2),
                                          ElongatedDropDelayCountSpeedUpFactor(0.4),
                                          bApplyAutomatically(true), 
										  RarityChanceCategory(EItemRarityChanceCategory::Default) {
	PrimaryComponentTick.bCanEverTick = false;
}

void UItemDropComponent::BeginPlay() {
	Super::BeginPlay();

	const auto owner = GetOwner();	
	if (auto lootActor = Cast<ALootActor>(owner)) {
		if (bApplyAutomatically) {
			lootActor->OnOpenLoot.AddUObject(this, &UItemDropComponent::ServerDropItems);
		}
		lootActor->OnActivatedLoot.AddUObject(this, &UItemDropComponent::TearDownComponent);
	}


	if (bApplyAutomatically && GetOwnerRole() == ROLE_Authority && GetOwner()->IsA<ABaseCharacter>()) {
		auto HC = GetOwner()->FindComponentByClass<UHealthComponent>();
		if (HC)
		{
			HC->OnChanged.AddUObject(this, &UItemDropComponent::OnAttributeHealthChange);
		}
	}

	//RarityChanceProbabilityMultiplier is used to drop fewer items in sessions where we have more mobs, to keep loot distribution uniform
	//in multiplayer. Caching it here should work ok, but premultiplying this value into the drop data would have been slightly cleaner
	//(as it also allows us to inspect the value in the editor, as opposed to this approach which is a bit secret).
	if (auto game = actorquery::getGame(GetWorld())) {
		if (RarityChanceCategory == EItemRarityChanceCategory::Mob) {
			float mobCountMultiplier = game->settings().difficultyStats.GetPlayerCountMobMultiplier();
			// D11.DB - Needed to take endless struggle into account here.
			mobCountMultiplier *= game->settings().difficultyStats.GetEndlessStruggleMobMultiplier();
			RarityChanceProbabilityMultiplier = 1.0f / mobCountMultiplier;
		}
	}
}

void UItemDropComponent::TearDownComponent() {
	RemoveFromRoot();
	DestroyComponent();
}

void UItemDropComponent::OnAttributeHealthChange(const FOnAttributeChangeData& data) {
	if (data.NewValue <= 0.f && data.OldValue > 0.f) {
		OnMobKilled(data);
	} else if (data.OldValue > data.NewValue) {
		OnMobDamaged(data);
	}
}

AActor* UItemDropComponent::GetActorCreditedForKill(AActor* instigator) {
	if (auto character = Cast<ABaseCharacter>(instigator)) {
		if (auto master = character->GetMaster()) {
			return master;
		}
	}
	return instigator;
}

void UItemDropComponent::OnMobKilled(const FOnAttributeChangeData& data) {
	auto instigator = GetOwner();

	if (data.GEModData) {
		const auto& handle = data.GEModData->EffectSpec.GetEffectContext();
		instigator = handle.GetInstigator();
	}

	ServerDropItems(GetOwner()->GetActorLocation(), GetActorCreditedForKill(instigator), GetOwner());
}

void UItemDropComponent::OnMobDamaged(const FOnAttributeChangeData& data) {	
}

TArray<FNetworkedItemDropData> UItemDropComponent::GatherItemDropData(const FItemDropSource& dropSource) {
	return {};
}

bool UItemDropComponent::ServerDropItems_Validate(const FVector& spawnLocation, AActor* triggeringPlayer, AActor* source) {
	return true;
}

bool isHyperMission(UWorld* world) {
	if (const auto* game = actorquery::getGame(world)) {
		return game->missionDef().isHyperMission();
	}
	return false;
}

void UItemDropComponent::ServerDropItems_Implementation(const FVector& spawnLocation, AActor* triggeringPlayer, AActor* source) {
	if (bEnableDrops && IsDropAllowedFrom(source)) {
		const FItemDropSource dropSource(spawnLocation, source, Cast<APlayerCharacter>(triggeringPlayer));
		auto dropData = GatherItemDropData(dropSource);

		if (isHyperMission(GetWorld())) {
			dropData = game::item::drop::toHyperMissionDrops(dropData, RarityChanceCategory, source);
		}

		bool gearDrop = false;
		for (const auto& itemDrop : dropData) {
			DropItems(dropSource, itemDrop, source);
			if (game::item::drop::isGearOrPermanent(itemDrop.GetDropData().GeneratorCategory)) {
				gearDrop = true;
			}
		}

#if WITH_EDITOR
		// D11.DB - Handy debug tool for checking item drop probabilities
		int dumpCount = CVarDumpItemDropsOnDeath.GetValueOnGameThread();
		if (dumpCount != 0) {
			DebugDumpItemDrops(dumpCount, dropSource);
		}
#endif

		// D11.SSN - cast check required as NPCs can trigger item drops
		if (auto* playerCharacter = Cast<APlayerCharacter>(triggeringPlayer)) {
			if (RarityChanceCategory == EItemRarityChanceCategory::FancyChest && gearDrop) {
				playerCharacter->LastOpenedChestRarity = RarityChanceCategory;
				playerCharacter->OnRep_OpenedChestRarity();
			}
		}
	}
}

void UItemDropComponent::DropItems(const FItemDropSource& dropSource, const FNetworkedItemDropData& dropData, AActor* source) {


	switch (dropData.GetSpawnType())
	{
	case EDropSpawnType::All:
	{
		if (!dropData.ShouldDropRandomized(RarityChanceProbabilityMultiplier)) {
			return;
		}
		for (auto playerCharacter : InstanceTracker<APlayerCharacter>::GetList(GetWorld())) {
			if (auto pc = Cast<ABasePlayerController>(playerCharacter->GetController())) {
				pc->ClientDropItems(dropSource, dropData, source, false, bElongatedDrops, ElongatedDropDelay, ElongatedDropDelayCountSpeedUpFactor, RarityChanceCategory);
			}
		}
		break;
	}
	case EDropSpawnType::Individual:
	{
		auto& allPlayerCharacters = InstanceTracker<APlayerCharacter>::GetList(GetWorld());
		for (auto playerCharacter : allPlayerCharacters) {
			if (!dropData.ShouldDropRandomized(RarityChanceProbabilityMultiplier)) {
				continue;
			}

			// Drops are individual. Generate separate drop for each client.
			if (auto pc = Cast<ABasePlayerController>(playerCharacter->GetController())) {
				pc->ClientDropItems(dropSource, dropData, source, false, bElongatedDrops, ElongatedDropDelay, ElongatedDropDelayCountSpeedUpFactor, RarityChanceCategory);
			}
		}
		break;
	}
	case EDropSpawnType::Shared:
	{
			if (!dropData.ShouldDropRandomized(RarityChanceProbabilityMultiplier)) {
				return;
			}
			auto player = GetWorld()->GetGameInstance()->GetFirstLocalPlayerController()->GetPawn();
			// Drops are shared. Generate on server and replicate to clients.
			GenerateDropsForPlayer(GetWorld(), player, source, dropSource, dropData, true, bElongatedDrops, ElongatedDropDelay, ElongatedDropDelayCountSpeedUpFactor, RarityChanceCategory);
		}
	}
}

void UItemDropComponent::GenerateDropsForPlayer(UWorld* world, AActor* targetPlayer, AActor* sourceActor, const FItemDropSource& dropSource, const FNetworkedItemDropData& dropData, bool replicateItems, bool elongatedDrops, float dropDelay, float dropDelayCountSpeedUpFactor, EItemRarityChanceCategory rarityChanceCategory) {
	auto localPlayers = world->GetGameInstance()->GetLocalPlayers();
	const auto hasExtraLocalPlayers = localPlayers.Num() > 1;

	if (elongatedDrops) 
	{
		replicateItems = replicateItems && (dropData.GetDropData().GeneratorCategory != EDropGeneratorCategory::Emerald);
		GenerateElongatedDrops(world, dropDelay, dropDelayCountSpeedUpFactor, dropSource, dropData, targetPlayer, sourceActor, replicateItems);
	}
	else
	{

		TArray<FInventoryItemData> generatedItems = GenerateDropData(world, dropSource, dropData, targetPlayer, rarityChanceCategory);

		if (generatedItems.Num() > 0)
		{
			bool CustomImpulse = (!replicateItems && hasExtraLocalPlayers && (dropData.GetDropData().GeneratorCategory != EDropGeneratorCategory::Emerald));
			TWeakObjectPtr<AActor> WeakSourceActor = sourceActor;

			AItemEmitterActor::SpawnSimpleItemEmitter(world, generatedItems, dropSource.DropLocation, targetPlayer, !replicateItems, true, false, FItemEmitterActorCompleteDelegate::CreateLambda([WeakSourceActor, CustomImpulse, replicateItems](TArray< AStorableItem* > & SpawnedItems) {
				
				if (replicateItems)
				{
					for (auto item : SpawnedItems)
					{
						item->SetReplicates(true);
						item->MulticastApplyDropEffect(WeakSourceActor.Get());
					}
				}
				else
				{
					for (auto item : SpawnedItems)
					{
						item->SetReplicates(false);
						//D11.KS - Do a special impulse for co-op on multicasted loot that aren't emeralds.
						item->ApplyDropEffect(WeakSourceActor.Get(), CustomImpulse);
					}
				}
				
			}));

		}
	}
}

TArray<FInventoryItemData> UItemDropComponent::GenerateDropData(UWorld* world, const FItemDropSource& dropSource, const FNetworkedItemDropData& dropData, AActor* targetPlayer, EItemRarityChanceCategory rarityChanceCategory)
{
	const auto dropCount = dropData.GetDropCount(world);
	if(dropCount)
	{
		const game::item::drop::DropGenerationInput rewardData(dropSource.MobType, targetPlayer,
			dropData.GetDropData().pred(), FRareItemChance::GetChanceFromCategory(rarityChanceCategory), itemgen::ItemSource::Drop);
			
		return  (dropData.GetDropData().GeneratorCategory == EDropGeneratorCategory::Emerald) ? generateDroppedEmeralds(world, rewardData, dropCount) : generateDroppedItems(world, rewardData, dropCount);
	}
	return {};
}

void UItemDropComponent::GenerateElongatedDrops(UWorld* world, float dropDelay, float dropDelayCountSpeedUpFactor, const FItemDropSource& dropSource, const FNetworkedItemDropData& dropData, AActor* targetPlayer, AActor* sourceActor, bool bReplicateItems)
{
	const auto dropCount = dropData.GetDropCount(world);
	if (dropCount > 0) {
		const float scaledDropDelay = dropDelay / (1.0 + dropCount * dropDelayCountSpeedUpFactor);
		const auto chanceFromCategory = FRareItemChance::GetChanceFromCategory(EItemRarityChanceCategory::Default);
		const game::item::drop::DropGenerationInput generationInput(dropSource.MobType, targetPlayer, dropData.GetDropData().pred(), chanceFromCategory, itemgen::ItemSource::Drop);
		
		auto generatedItems = (dropData.GetDropData().GeneratorCategory == EDropGeneratorCategory::Emerald) ? generateDroppedEmeralds(world, generationInput, dropCount) : generateDroppedItems(world, generationInput, dropCount);

		AItemEmitterActor::SpawnElongatedItemEmitter(world, generatedItems, dropSource.DropLocation, targetPlayer, sourceActor, scaledDropDelay, bReplicateItems);
	}	
}

bool UItemDropComponent::IsDropAllowedFrom(AActor* source) {
	return source ? !source->Tags.Contains(tags::noDrop) : true;
}

#if WITH_EDITOR
void UItemDropComponent::DebugDumpItemDrops( int count, const FItemDropSource& dropSource ) {
	TMap<EDropGeneratorCategory, int> categoryCounters;
	TMap<FItemId, int> itemTypeCounters;
	int categoryCount = 0;
	int itemCount = 0;
	for( int i = 0; i < count; ++i ) {
		auto dropDataList = GatherItemDropData(dropSource);
		//for( auto dropCategoriesIndex = 0; dropCategoriesIndex < dropData.Num(); dropCategoriesIndex++ ) {
		for( auto& itemDrop : dropDataList ) {
			if( !itemDrop.ShouldDropRandomized( RarityChanceProbabilityMultiplier ) ) {
				continue;
			}
			categoryCount++;
			if( categoryCounters.Contains( itemDrop.GetDropData().GeneratorCategory) ) {
				categoryCounters[itemDrop.GetDropData().GeneratorCategory] += 1;
			}
			else {
				categoryCounters.Add( itemDrop.GetDropData().GeneratorCategory, 1 );
			}

			int dropCount = itemDrop.GetDropCount(GetWorld());
			const game::item::drop::DropGenerationInput rewardData(dropSource.MobType, dropSource.TriggeringPlayer,
				itemDrop.GetDropData().pred(), FRareItemChance::GetChanceFromCategory(RarityChanceCategory), itemgen::ItemSource::Drop);

			
			auto generatedItems = (itemDrop.GetDropData().GeneratorCategory == EDropGeneratorCategory::Emerald) ? generateDroppedEmeralds(GetWorld(), rewardData, dropCount) : generateDroppedItems(GetWorld(), rewardData, dropCount);

			//const auto droppedItems = GenerateDrops( dropSource, itemDrop, dropSource.TriggeringPlayer, true );
			//for( auto item : droppedItems ) {
			for( auto item : generatedItems ) {
				const auto& itemType = item.GetItemId();
				itemCount++;
				//auto itemType = item->GetItemType().getId();
				if( itemTypeCounters.Contains( itemType ) ) {
					itemTypeCounters[itemType]+= 1;
				}
				else {
					itemTypeCounters.Add( itemType, 1 );
				}
			}
		}
	}

	#define GETENUMSTRING(etype, evalue) ( (FindObject<UEnum>(ANY_PACKAGE, TEXT(etype), true) != nullptr) ? FindObject<UEnum>(ANY_PACKAGE, TEXT(etype), true)->GetEnumName((int32)evalue) : FString("Invalid - are you sure enum uses UENUM() macro?") )
	FString strDropType; GetName( strDropType );
	FString strMobType = GETENUMSTRING("EntityType", dropSource.MobType);
	FString strCategories;
	for( auto It = categoryCounters.CreateConstIterator(); It; ++It ) {
		float percentage = static_cast<float>( It.Value() ) / static_cast<float>( categoryCount ) * 100.0f;
		float percentageAll = static_cast<float>( It.Value() ) / static_cast<float>( count ) * 100.0f;
		strCategories += FString::Printf( TEXT( "%s, Count = %d, PercentageOfDrops = %f, PercentageAll = %f\n" ),
			*GETENUMSTRING( "EDropCategory", It.Key() ), It.Value(), percentage, percentageAll
		);
	}

	FString strItems;
	for( auto It = itemTypeCounters.CreateConstIterator(); It; ++It ) {
		float percentage = static_cast<float>( It.Value() ) / static_cast<float>( itemCount ) * 100.0f;
		float percentageAll = static_cast<float>( It.Value() ) / static_cast<float>( count ) * 100.0f;
		strItems += FString::Printf( TEXT( "%s, Count = %d, PercentageOfDrops = %f, PercentageAll = %f\n" ),
			*It.Key().GetBackingType().ToString(), It.Value(), percentage, percentageAll
		);
	}

	FString strOut = FString::Printf(
		TEXT( "--- ITEM DROP DUMP ---\n\nITERATIONS: %d\n\nCLASS: %s\n\nCATEGORIES:\n%s\nITEMS:\n%s\n\n--- END ---\n" ),
		count, *strDropType, *strCategories, *strItems
	);

	FString strFilepath = FPaths::ConvertRelativePathToFull( FPaths::GameSavedDir() );
	FString strFilename = strFilepath + TEXT("/ItemDropDump/") + strMobType + TEXT( "_" ) + strDropType + TEXT(".txt");

	FFileHelper::SaveStringToFile( strOut, *strFilename );
}
#endif
