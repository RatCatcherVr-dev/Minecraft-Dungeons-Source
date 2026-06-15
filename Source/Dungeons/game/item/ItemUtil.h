#pragma once
#include "game/component/ItemStashComponent.h"
#include "game/actor/item/StorableItem.h"
#include "Engine/StreamableManager.h"

class ItemType;
class AStorableItem;
class AItemInstance;
struct FInventoryItemData;


namespace game { namespace item { namespace util {

struct FSpecifiedStoreCountItemDrop {
	FItemId Id;
	TOptional<int> StoreCountOverride;

	FSpecifiedStoreCountItemDrop(FItemId Id, TOptional<int> storeCount = {})
		: Id(std::move(Id))
		, StoreCountOverride(storeCount) {}
};

struct FPlayerEquipmentStats {
public:
	static constexpr int MaxArrowsCount(void) { return 25; }
	float HealthPotionFillRate = 0.0f;
	float ArrowFillRate = 0.0f;
	float ConsumableFillRate = 0.0f;
	int NumPlayers = 0;
};

struct FLaunchRange {
public:
	FLaunchRange();
	FLaunchRange(float xmin, float xmax, float ymin, float ymax);

	FloatRange GetXRange() const;
	FloatRange GetYRange() const;
private:
	float Xmin, Xmax, Ymin, Ymax;
};

AStorableItem* spawnStorableItem(UWorld& world, const FVector& pos, const FInventoryItemData& itemData, TOptional<int> overrideStoreCount = {}, AActor* owner = nullptr, bool lockItemToOwner = false, float lifeSpan = 0.f);

AItemInstance* spawnItemInstance(const ItemType&, float power, const FVector& pos, AActor* owner);
AItemInstance* spawnItemInstance(const FInventoryItemData& itemData, float power, const FVector& pos, AActor* owner);

TSharedPtr<FStreamableHandle> loadItemInstance(const ItemType& itemType, FStreamableDelegate, TAsyncLoadPriority = FStreamableManager::AsyncLoadHighPriority);
FVector randItemDropVelocity(const float dropVelocityMin, const float dropVelocityMax, const float dropConeRadius);

FVector randItemDropVelocity(float velocityMin, float velocityMax, float coneHalfAngleDegrees, float yawDegrees);

bool ItemActorIsLocalOnly(AActor* actor);
	
void SpreadOutItemsInSquare(const TArray<  AStorableItem* >& items);
void SpreadOutItemsInSquareWithDirection(const TArray<  AStorableItem* >& items, const FVector& launchDirectionOffset, FloatRange launchSpan);

//private:
	void ApplyRandomImpulseToItem(AStorableItem* item, TArray<FLaunchRange>& avoidRanges, const FVector& launchDirectionOffset, FloatRange launchSpan);
	void ApplyImpulseToItem(AStorableItem* item, FVector launchDirection, TArray<FLaunchRange>& avoidRanges);
	FVector TryFindLaunchVectorOutsideRanges(const TArray<FLaunchRange>& avoidRanges, const FVector& launchDirectionOffset, FloatRange launchSpan);

}}}
