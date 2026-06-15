#include "Dungeons.h"
#include "ItemEmitterActor.h"
#include "game/item/generator/ItemGenerator.h"
#include "game/item/drop/ItemDropGenerator.h"
#include "game/item/instance/AItemInstance.h"
#include "Engine/AssetManager.h"
#include "Assets/ItemAssetFinder.h"
#include "game/actor/ProjectileManager.h"


float NewDeltaTime(const float lifeTime, const float deltaTime) {
	const auto k = HALF_PI / deltaTime / 10;
	return FMath::Clamp(deltaTime * FMath::Cos(k * lifeTime), 0.001f, deltaTime);
}

AItemEmitterActor::AItemEmitterActor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	SetReplicates(false);
}


AItemEmitterActor* AItemEmitterActor::SpawnElongatedItemEmitter(UWorld* world, const TArray<FInventoryItemData>& sourceItemList, const FVector& location, AActor* targetPlayer, AActor* sourceActor, float deltaTime, bool bReplicateItems)
{
	TArray<FGeneratedItem> StoreList;
	StoreList.Reserve(sourceItemList.Num());
	for (auto& ItemData : sourceItemList)
	{
		StoreList.Push({ ItemData , ItemData.OverrideStoreCount });
	}

	FItemEmitterActorCompleteDelegate CompletionDelegate = FItemEmitterActorCompleteDelegate::CreateLambda([sourceActor, bReplicateItems](TArray< AStorableItem* > & SpawnedItems) {

		if (bReplicateItems)
		{
			for (auto item : SpawnedItems)
			{
				item->SetReplicates(true);
				item->ApplyDropEffect(sourceActor, true);
			}
		}
	});

	return SpawnSimpleItemEmitter(world, StoreList, location, targetPlayer, sourceActor, false, false, false, deltaTime, bReplicateItems ? CompletionDelegate : FItemEmitterActorCompleteDelegate());
}

AItemEmitterActor* AItemEmitterActor::SpawnSimpleItemEmitter(UWorld* world, const TArray<FInventoryItemData>& sourceItemList, const FVector& location, AActor* targetPlayer, bool lockItemsToOwner, bool singleFrameSpawn /*= false*/, bool spawnWithTickDisabled /*= false*/, FItemEmitterActorCompleteDelegate CompletionDelegate)
{	
	TArray<FGeneratedItem> StoreList;
	StoreList.Reserve(sourceItemList.Num());
	for (auto& ItemData : sourceItemList)
	{
		StoreList.Push({ ItemData , ItemData.OverrideStoreCount });
	}
	
	return SpawnSimpleItemEmitter(world, StoreList, location, targetPlayer, nullptr, lockItemsToOwner, singleFrameSpawn, spawnWithTickDisabled, -1.0f, CompletionDelegate);
}

AItemEmitterActor* AItemEmitterActor::SpawnSimpleItemEmitter(UWorld* world, const TArray<game::item::util::FSpecifiedStoreCountItemDrop>& sourceItemList, const FVector& location, AActor* targetPlayer, bool lockItemsToOwner, bool singleFrameSpawn /*= false*/, bool spawnWithTickDisabled /*= false*/, FItemEmitterActorCompleteDelegate CompletionDelegate)
{

	TArray<FGeneratedItem> StoreList;
	StoreList.Reserve(sourceItemList.Num());
	for (auto& ItemData : sourceItemList)
	{
		StoreList.Push({ game::item::generator::generate(ItemData.Id) ,ItemData.StoreCountOverride });
	}

	return SpawnSimpleItemEmitter(world, StoreList, location, targetPlayer,nullptr, lockItemsToOwner, singleFrameSpawn, spawnWithTickDisabled,-1.0f, CompletionDelegate);
}

AItemEmitterActor* AItemEmitterActor::SpawnSimpleItemEmitter(UWorld* world, const TArray<FGeneratedItem>& sourceItemList, const FVector& location, AActor* targetPlayer, AActor* sourceActor, bool lockItemsToOwner, bool singleFrameSpawn /*= false*/, bool spawnWithTickDisabled /*= false*/, float deltaTime /*= -1.0f*/, FItemEmitterActorCompleteDelegate CompletionDelegate)
{
	
	if (world) {
		auto itemEmitterActor = AProjectileActorManager::Pop_ItemEmitterActor(AItemEmitterActor::StaticClass(), world);//world->SpawnActor<AItemEmitterActor>();
		if (itemEmitterActor) {
			itemEmitterActor->SetActorTickEnabled(false); //this is enabled by default from the pool, we dont want it yet
			itemEmitterActor->GeneratedItems = sourceItemList;
			itemEmitterActor->Location = location;
			itemEmitterActor->Player = targetPlayer;
			itemEmitterActor->Source = sourceActor;
			itemEmitterActor->DropCount = sourceItemList.Num();
			itemEmitterActor->LockItemsToOwner = lockItemsToOwner;
			itemEmitterActor->SingleFrameSpawn = singleFrameSpawn;
			itemEmitterActor->SpawnWithTickDisabled = spawnWithTickDisabled;
			itemEmitterActor->PostSpawnDelegate = CompletionDelegate;
			itemEmitterActor->DeltaTime = deltaTime;
			itemEmitterActor->SleepTime = deltaTime;
			itemEmitterActor->LifeTime = 0.0f;

			if(itemEmitterActor->LoadItemClasses() == EStreamResult::E_Error)
			{
				//The stream handle failed, just finish
				itemEmitterActor->FinishedSpawning();
				return nullptr;
			}
			
			return itemEmitterActor;
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("Could not spawn ASimpleItemEmitterActor."));
	return nullptr;
}

void AItemEmitterActor::Tick(float deltaTime)
{
	Super::Tick(deltaTime);	
	if (SingleFrameSpawn)
	{
		SingleFrameSpawnItems();

	}
	else
	{
		DeferredSpawning(deltaTime);
	}
}

void AItemEmitterActor::SingleFrameSpawnItems()
{
	//all spawns and finalize in single tick
	while (DropCount--)
	{
		SpawnSingleEmittedItem();
	}
	FinishedSpawning();
}

void AItemEmitterActor::DeferredSpawning(float deltaTime)
{
	if (DropCount < 1) {
		FinishedSpawning();
	}
	else
	{
		if (DeltaTime < 0.0f)
		{
			//Spawn 1 per frame (spawning items is expensive)
			--DropCount;
			SpawnSingleEmittedItem();
		}
		else
		{
			//Time Elongated Spawns
			LifeTime += deltaTime;
			if (SleepTime > 0) 
			{
				SleepTime -= deltaTime;
			}
			else 
			{
				--DropCount;
				AStorableItem* pItem = SpawnSingleEmittedItem();
				if(pItem)
					pItem->ApplyDropEffect(Source.Get());
				SleepTime = NewDeltaTime(LifeTime, DeltaTime);
			}
		}
	
	}
}

void AItemEmitterActor::FinishedSpawning()
{
	//callback
	
	if (PostSpawnDelegate.IsBound())
	{
		TArray< AStorableItem* > FinalList;
		FinalList.Reserve(SpawnedItemList.Num());

		for (auto pItem : SpawnedItemList)
		{
			if (pItem.IsValid())
				FinalList.Push(pItem.Get());
		}
		if (FinalList.Num() > 0)
			PostSpawnDelegate.ExecuteIfBound(FinalList);
	}

	SetActorTickEnabled(false);
	if (ItemClassStreamingHandle.IsValid())
	{
		ItemClassStreamingHandle->ReleaseHandle();
		ItemClassStreamingHandle.Reset();
	}	

	Reset();
	AProjectileActorManager::Push_ItemEmitterActor(this);

}

void AItemEmitterActor::Reset()
{
	Location = FVector::ZeroVector;

	if (ItemClassStreamingHandle.IsValid())
	{
		ItemClassStreamingHandle->ReleaseHandle();
		ItemClassStreamingHandle.Reset();
	}	
	Player.Reset();
	Source.Reset();
	GeneratedItems.Reset();
	SpawnedItemList.Reset();
	PostSpawnDelegate.Unbind();
	DropCount = 0;
	LockItemsToOwner = false;
	SingleFrameSpawn = false;
	SpawnWithTickDisabled = false;

	//Elongated Spawn Delay
	DeltaTime = -1.0f;
	SleepTime = -1.0f;
	LifeTime = 0;
}

AStorableItem* AItemEmitterActor::SpawnSingleEmittedItem()
{
	auto spawnedItem = game::item::util::spawnStorableItem(*GetWorld(), Location, GeneratedItems[DropCount].InvData, GeneratedItems[DropCount].StoreCount, Player.Get(), LockItemsToOwner);
	if (spawnedItem)
	{
		const auto& itemType = GeneratedItems[DropCount].InvData.GetItemType();
		if(itemType.isInstant() || itemType.isConsumable()) {
			//Sets up a UPROPERTY reference to the UClass to avoid the instance class getting GCd between load and pickup.
			spawnedItem->SetInstanceClass(itemType.getInstanceClass().Get());
		}
 		
		if(SpawnWithTickDisabled)
			spawnedItem->SetActorTickEnabled(false);

		SpawnedItemList.Push(spawnedItem);
	}
	return spawnedItem;
}

AItemEmitterActor::EStreamResult AItemEmitterActor::LoadItemClasses()
{
	if (!ItemClassStreamingHandle.IsValid())
	{
		TArray<FSoftObjectPath> TargetsToStream;
		for (auto&& generatedItem : GeneratedItems)
		{
			const auto& itemType = generatedItem.InvData.GetItemType();
			TargetsToStream.Push(IDungeonsModule::Get().GetItemAssetFinder()->StorablePath(generatedItem.InvData.GetItemId()).Get(FSoftObjectPath()));
			//If this is an instant use or consumable item we want to load the item instance class as well
			if(generatedItem.InvData.GetItemType().isInstant() || generatedItem.InvData.GetItemType().isConsumable()) {
				FSoftObjectPath InstPath = itemType.getInstancePath();
				if(InstPath.IsValid())
					TargetsToStream.Push(InstPath);
			}
		}

		ItemClassStreamingHandle = UAssetManager::GetStreamableManager().RequestAsyncLoad(TargetsToStream, FStreamableDelegate::CreateUObject(this, &AItemEmitterActor::ItemClassesLoaded), FStreamableManager::AsyncLoadHighPriority, true, false, TEXT("ASimpleItemEmitterActor::AsyncLoadItemClasses"));
		check(ItemClassStreamingHandle.IsValid() && "Failed to stream These Assets in!");
	}

	if (!ItemClassStreamingHandle.IsValid())
	{
		return EStreamResult::E_Error;
	}

	return ItemClassStreamingHandle->HasLoadCompleted() ? EStreamResult::E_Complete : EStreamResult::E_Loading;
}

void AItemEmitterActor::ItemClassesLoaded()
{
	SetActorTickEnabled(true);
}

