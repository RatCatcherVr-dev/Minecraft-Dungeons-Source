#include "Dungeons.h"
#include "SpawnItemWhenPlayerRunsOutOf.h"
#include "game/component/EquipmentComponent.h"
#include "game/item/ItemTypeDefs.h"
#include "util/CollectionUtils.h"
#include "util/RandomUtil.h"
#include "game/item/ItemUtil.h"
#include "game/objective/ObjectiveAction.h"
#include "game/Game.h"
#include "game/item/instance/AItemInstance.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/actor/item/StorableItem.h"
#include "Engine/AssetManager.h"
#include "Assets/ItemAssetFinder.h"

namespace game { namespace objective {

bool has(UEquipmentComponent* equipmentComponent, const ItemType& item) {
	if (equipmentComponent) {
		auto& slot = equipmentComponent->GetSlot(item.slotType());
		if (slot.GetCount() && slot.GetItem() && slot.GetItem()->GetItemId() == item.getId()) {
			return true;
		}
	}
	return false;
}

bool allPlayersHave(UWorld& world, const ItemType& item) {
	const auto& players = InstanceTracker<APlayerCharacter>::GetList(&world);
	return algo::all_of(players, [&](APlayerCharacter* player) { return has(player->template FindComponentByClass<UEquipmentComponent>(), item); });
}

SpawnItemWhenPlayerRunsOutOf::SpawnItemWhenPlayerRunsOutOf(const io::RegionLocator& locator, const FItemId& type)
	: mLocator(locator)
	, mItem(GetItemRegistry().Get(type)) {
}

SpawnItemWhenPlayerRunsOutOf::~SpawnItemWhenPlayerRunsOutOf()
{
	if (ItemClassStreamingHandle.IsValid())
	{
		ItemClassStreamingHandle->ReleaseHandle();
		ItemClassStreamingHandle.Reset();
	}
}

void SpawnItemWhenPlayerRunsOutOf::onInit() {
	mRegions = targetFinder().get(mLocator);
}

void SpawnItemWhenPlayerRunsOutOf::onTick() {
	Util::removeNullsSwap(mSpawnedItems);

	if (mRegions.empty() || mSpawnedItems.Num()) {
		return;
	}
	if (allPlayersHave(game().world(), mItem)) {
		return;
	}
	
	if (!ItemClassStreamingHandle.IsValid()) //stream item class
	{
		const FSoftObjectPath TargetToStream =IDungeonsModule::Get().GetItemAssetFinder()->StorablePath(mItem.getId()).Get(FSoftObjectPath());		
		ItemClassStreamingHandle = UAssetManager::GetStreamableManager().RequestAsyncLoad(TargetToStream, []() {}, FStreamableManager::AsyncLoadHighPriority, true, false, TEXT("SpawnItemWhenPlayerRunsOutOf::onTick"));
	}
	else if(ItemClassStreamingHandle->HasLoadCompleted())
	{
		//spawn item, Ok to use spawnStorableItem here as it preloads the classes above
		if (auto item = item::util::spawnStorableItem(game().world(), centerFloor(*Util::randomChoice(mRegions)).toUeSpace(), mItem.getId())) {
			mSpawnedItems.Add(item);
			item->SetReplicates(true);
		}

		ItemClassStreamingHandle->ReleaseHandle();
		ItemClassStreamingHandle.Reset();
	}
	
}

}}
