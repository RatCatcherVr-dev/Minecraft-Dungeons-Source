#include "Dungeons.h"
#include "ItemUtil.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "game/actor/item/StorableItem.h"
#include "game/item/ItemType.h"
#include "game/item/instance/AItemInstance.h"
#include <GameFramework/GameStateBase.h>
#include <DungeonsGameInstance.h>
#include "game/actor/ProjectileManager.h"
#include "Engine/AssetManager.h"

namespace game { namespace item { namespace util {

FLaunchRange::FLaunchRange() {
}

FLaunchRange::FLaunchRange(float xmin, float xmax, float ymin, float ymax) : Xmin(xmin), Xmax(xmax), Ymin(ymin), Ymax(ymax) {
}

FloatRange FLaunchRange::GetXRange() const {
	return { Xmin, Xmax };
}

FloatRange FLaunchRange::GetYRange() const {
	return { Ymin, Ymax };
}

//D11.SC Try not to use this directly unless the AStorableItem classes being used have been loaded/streamed in first (try using SpawnSimpleItemEmitter)
AStorableItem* spawnStorableItem(UWorld& world, const FVector& pos, const FInventoryItemData& itemData, TOptional<int> overrideStoreCount, AActor* Owner, bool lockItemToOwner, float lifeSpan) {
	static const ESpawnActorCollisionHandlingMethod handlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
	if (auto item = AProjectileActorManager::TryPop_Storable(&world, itemData.GetItemType()))
	{
		//Pop An ItemStorable from pre-cached storage
		const FTransform transform(pos);
		
		if (!item) {
			return nullptr;
		}

		item->SetOwner(Owner);
		item->lockItemToOwner = lockItemToOwner;
		item->ItemData = itemData;
		item->SetActorTransform(transform, false, nullptr, ETeleportType::ResetPhysics);
		item->ReInitialiseItem();
		item->ItemData.OverrideStoreCount = overrideStoreCount;
		return item;
	}


	auto pStorableClass = GetItemRegistry().Get(itemData.GetItemId()).getStorableClass();

	if (pStorableClass)
	{
		FPlatformMisc::LowLevelOutputDebugStringf(TEXT("### spawnStorableItem : %s\n"), *pStorableClass->GetName());
	}


	const FTransform transform(pos);
	auto item = world.SpawnActorDeferred<AStorableItem>(
		pStorableClass,
		transform,
		Owner,
		nullptr,
		handlingMethod);

	if (!item) {
		return nullptr;
	}

	if(lockItemToOwner)
	{
		if(auto gi = Cast<UDungeonsGameInstance>(item->GetGameInstance()))
		{
			gi->OnLocalPlayerLeave.AddUObject(item, &AStorableItem::HandleLockedOwnerDropOut);
		}
	}
	
	item->lockItemToOwner = lockItemToOwner;
	item->ItemData = itemData;
	item->ItemData.OverrideStoreCount = overrideStoreCount;
	item->SetLifeSpan(lifeSpan);
	UGameplayStatics::FinishSpawningActor(item, transform);
	return item;
}



AItemInstance* spawnItemInstance(const ItemType& itemType, float power, const FVector& pos, AActor* owner) {
	UClass* it = itemType.getInstanceClass();
	ensure(it);
	auto instance = owner->GetWorld()->SpawnActorDeferred<AItemInstance>(it ? it : AItemInstance::StaticClass(), FTransform(), owner);	
	instance->SetItemPower(power);
	UGameplayStatics::FinishSpawningActor(instance, FTransform());
	return instance;
}

AItemInstance* spawnItemInstance(const FInventoryItemData& itemData, float power, const FVector& pos, AActor* owner) {
	auto instance = spawnItemInstance(itemData.GetItemType(), power, pos, owner);
	instance->OptionalItemDataSource = itemData;
	return instance;
}

TSharedPtr<FStreamableHandle> loadItemInstance(const ItemType& itemType, FStreamableDelegate delegate,  TAsyncLoadPriority priority) {
	return UAssetManager::GetStreamableManager().RequestAsyncLoad(itemType.getInstancePath(), delegate, priority);
}

FVector randItemDropVelocity(const float dropVelocityMin, const float dropVelocityMax, const float dropConeRadius)
{
	const auto randVelocity = FMath::RandRange(dropVelocityMin, dropVelocityMax);
	constexpr auto pi = 3.14f;
	const FVector upVector(0, 0, 1);
	return FMath::VRandCone(upVector, pi * dropConeRadius) * randVelocity;
}

FVector randItemDropVelocity(float velocityMin, float velocityMax, float coneHalfAngleDegrees, float yawDegrees) {
	const auto pitchDegrees { -30.f };

	const auto direction = FMath::VRandCone(
		FVector::UpVector.RotateAngleAxis(pitchDegrees, FVector::ForwardVector).RotateAngleAxis(yawDegrees, FVector::UpVector),
		FMath::DegreesToRadians(coneHalfAngleDegrees)
	);

	const auto velocity = FMath::RandRange(velocityMin, velocityMax);

	return direction * velocity;
}

bool ItemActorIsLocalOnly(AActor* actor) {
	bool isServerAndLocallyOwned = actor->GetWorld()->IsServer() && !actor->GetIsReplicated();
	bool isClientAndLocallyOwned = !actor->GetWorld()->IsServer() && actor->HasAuthority();
	return isServerAndLocallyOwned || isClientAndLocallyOwned;
}

void SpreadOutItemsInSquare(const TArray<  AStorableItem* >& items) {
	TArray<FLaunchRange> avoidRanges;
	for (auto item : items) {
		ApplyRandomImpulseToItem(item, avoidRanges, FVector::ZeroVector, FloatRange(0.3f, 1.0f));
	}
}

void SpreadOutItemsInSquareWithDirection(const TArray<  AStorableItem* >& items, const FVector& launchDirectionOffset, FloatRange launchSpan) {
	TArray<FLaunchRange> avoidRanges;
	for (auto item : items) {
		ApplyRandomImpulseToItem(item, avoidRanges, launchDirectionOffset, launchSpan);
	}
}

void ApplyRandomImpulseToItem(AStorableItem* item, TArray<FLaunchRange>& avoidRanges, const FVector& launchDirectionOffset, FloatRange launchSpan) {
	FVector launchDirection = TryFindLaunchVectorOutsideRanges(avoidRanges, launchDirectionOffset, launchSpan);
	ApplyImpulseToItem(item, launchDirection, avoidRanges);
}

void ApplyImpulseToItem(AStorableItem* item, FVector launchDirection, TArray<FLaunchRange>& avoidRanges) {
	if (USphereComponent* sphere = Cast<USphereComponent>(item->GetRootComponent())) {
		const float rangeOffset = 0.2f;
		FLaunchRange launchRange(launchDirection.X - rangeOffset, launchDirection.X + rangeOffset, launchDirection.Y - rangeOffset, launchDirection.Y + rangeOffset);
		avoidRanges.Emplace(launchRange);

		const float power = 1500.0f;
		//launchDirection.Normalize();

		sphere->AddImpulse(launchDirection * power, NAME_None, true);
	}
}

FVector TryFindLaunchVectorOutsideRanges(const TArray<FLaunchRange>& avoidRanges, const FVector& launchDirectionOffset, FloatRange launchSpan) {
	float X = 0.0f;
	float Y = 0.0f;
	FVector launch;
	
	for (int i = 0; i < 20; ++i) {
		X = FMath::RandRange(launchSpan.min(), launchSpan.max());
		Y = FMath::RandRange(launchSpan.min(), launchSpan.max());
		X *= FMath::RandBool() ? 1.0f : -1.0f;
		Y *= FMath::RandBool() ? 1.0f : -1.0f;

		launch = launchDirectionOffset + FVector(X, Y, 1.0f);
		launch.Normalize();
		
		if (!avoidRanges.ContainsByPredicate([launch](const FLaunchRange& range) { return range.GetXRange().inRange(launch.X) && range.GetYRange().inRange(launch.Y); })) {
			break;
		}
	}

	return launch;
}

}}}
