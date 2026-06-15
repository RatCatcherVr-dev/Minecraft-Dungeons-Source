#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "game/component/drop/ItemDropComponent.h"
#include "game/item/itemutil.h"
#include "ItemEmitterActor.generated.h"

DECLARE_DELEGATE_OneParam(FItemEmitterActorCompleteDelegate, TArray< AStorableItem* >& );

UCLASS()
class DUNGEONS_API AItemEmitterActor : public AActor
{
	GENERATED_BODY()

public:
	AItemEmitterActor();
	static AItemEmitterActor* SpawnElongatedItemEmitter(UWorld* world, const TArray<FInventoryItemData>& sourceItemList, const FVector& location, AActor* targetPlayer, AActor* sourceActor, float deltaTime, bool bReplicateItems);
	static AItemEmitterActor* SpawnSimpleItemEmitter(UWorld* world, const TArray<FInventoryItemData>& sourceItemList,const FVector& location, AActor* targetPlayer, bool lockItemsToOwner, bool singleFrameSpawn = false, bool spawnWithTickDisabled = false, FItemEmitterActorCompleteDelegate CompletionDelegate = FItemEmitterActorCompleteDelegate());//std::function< void(TArray< AStorableItem* > &) > postSpawnFunc = default_PostSpawnFunc);
	static AItemEmitterActor* SpawnSimpleItemEmitter(UWorld* world, const TArray<game::item::util::FSpecifiedStoreCountItemDrop>& sourceItemList,const FVector& location, AActor* targetPlayer, bool lockItemsToOwner, bool singleFrameSpawn = false, bool spawnWithTickDisabled = false, FItemEmitterActorCompleteDelegate CompletionDelegate = FItemEmitterActorCompleteDelegate());//std::function< void(TArray< AStorableItem* > &) > postSpawnFunc = default_PostSpawnFunc);

	virtual void Tick(float deltaTime) override;

	void SingleFrameSpawnItems();

	virtual void DeferredSpawning(float deltaTime);

	void FinishedSpawning();

	void Reset();
	
private:

	struct FGeneratedItem
	{
		FInventoryItemData InvData;
		TOptional<int> StoreCount;
	};

	enum class EStreamResult : int8
	{
		E_Error		= -1,
		E_Loading	= 0,
		E_Complete = 1,
		E_Max
	};
	
	static AItemEmitterActor* SpawnSimpleItemEmitter(UWorld* world, const TArray<FGeneratedItem>& sourceItemList, const FVector& location, AActor* targetPlayer, AActor* sourceActor, bool lockItemsToOwner, bool singleFrameSpawn = false, bool spawnWithTickDisabled = false, float deltaTime = -1.0f, FItemEmitterActorCompleteDelegate CompletionDelegate = FItemEmitterActorCompleteDelegate());//std::function< void(TArray< AStorableItem* > &) > postSpawnFunc = default_PostSpawnFunc);
	
	AStorableItem* SpawnSingleEmittedItem();
	EStreamResult LoadItemClasses();
	void ItemClassesLoaded();

	FVector Location;
	TSharedPtr< FStreamableHandle > ItemClassStreamingHandle;
	TWeakObjectPtr < AActor > Player;
	TWeakObjectPtr < AActor > Source;
	TArray< FGeneratedItem >	GeneratedItems;
	TArray< TWeakObjectPtr < AStorableItem > > SpawnedItemList;
	FItemEmitterActorCompleteDelegate PostSpawnDelegate;
	int DropCount;		
	bool LockItemsToOwner;
	bool SingleFrameSpawn;
	bool SpawnWithTickDisabled;
	
	//Elongated Spawn Delay
	float DeltaTime;
	float SleepTime;
	float LifeTime = 0;

};
