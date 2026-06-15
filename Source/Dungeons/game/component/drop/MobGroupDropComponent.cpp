#include "Dungeons.h"
#include "MobGroupDropComponent.h"

UMobGroupDropComponent::UMobGroupDropComponent() {
	//This component is spawned dynamically on the server and must be replicated for the RPCs to work.
	bReplicates = true;
}

FMobDropGroup::FMobDropGroup(const TArray<AMobCharacter*>& mobs) {
	Mobs.Append(mobs);
}

bool FMobDropGroup::IsMobGroupDead() const {
	const auto isDead = [](TWeakObjectPtr<AMobCharacter> mob) {
		return !mob.IsValid() || mob->IsNotAlive();
	};
	return algo::all_of(Mobs, isDead);
}


void UMobGroupDropComponent::SetMobGroup(TSharedPtr<FMobDropGroup> group) {
	MobGroup = group;
}

void UMobGroupDropComponent::OnMobKilled(const FOnAttributeChangeData& data) {
	if (MobGroup->IsMobGroupDead()) {
		UItemDropComponent::OnMobKilled(data);
	}
}

UMobGroupDropComponent* UMobGroupDropComponent::Create(UObject* outer, EItemRarityChanceCategory rarity, EDropCategory category, float probability) {
	UMobGroupDropComponent* object = NewObject<UMobGroupDropComponent>(outer);
	object->RarityChanceCategory = rarity;
	object->DropData.Category = category;
	object->DropData.Probability = probability;

	return object;
}