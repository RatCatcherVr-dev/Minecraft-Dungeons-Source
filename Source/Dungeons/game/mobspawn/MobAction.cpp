#include "Dungeons.h"
#include "MobAction.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/component/GlobalHealthBarComponent.h"
#include "world/entity/MobTags.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/component/TeleportToOwnerComponent.h"


namespace game { namespace mobspawn {

const MobAction& LongOffensiveRange() {
	static const MobAction action = [](AMobCharacter& mob) {
		mob.OffensiveRange = FMath::Max(10000.0f, mob.OffensiveRange);
	};
	return action;
}

const MobAction& RequiredForObjective() {
	static const MobAction action = [](AMobCharacter& mob) {
		mob.SetIsRequiredForObjective(true);
	};

	return action;
}

using MobPredicate = ::Pred<AMobCharacter&>;

template <typename T>
MobAction AddComponent(TOptional<bool> shouldReplicate, FString path, MobPredicate predicate = {}) {
#if !PLATFORM_PS4 && !PLATFORM_SWITCH
	static_assert(std::is_base_of_v<UActorComponent, T>, "T needs to be a UActorComponent");
#endif
	return[shouldReplicate, pred = std::move(predicate), dir = std::move(path)](AMobCharacter& mob) {
		if (pred && !pred(mob)) {
			return;
		}
		const auto cls = StaticLoadClass(T::StaticClass(), &mob, *dir);

		T* component = NewObject<T>(&mob, cls? cls : T::StaticClass());
		component->RegisterComponent();
		component->SetIsReplicated(shouldReplicate.Get(component->GetIsReplicated()));
	};
}

const MobAction& AddGlobalHealthBarOnMiniBosses() {
	static const MobAction action = AddComponent<UGlobalHealthBarComponent>(
		true,
		"/Game/Components/BP_GlobalHealthBar.BP_GlobalHealthBar_C",
		RETLAMBDA(hasMobTag(it.EntityType, MobTags::HashTag_Miniboss)));

	return action;
}

MobAction ChangeMaster(ABaseCharacter* master) {
	return [master = TWeakObjectPtr<ABaseCharacter>(master)](AMobCharacter& mob) { if(master.IsValid()) mob.ChangeMaster(master.Get()); };
}

MobAction AddTag(const FName& tag) {
	return [tag](AMobCharacter& mob) { mob.Tags.AddUnique(tag); };
}

MobAction SpawnAsUnderling() {
	return [](AMobCharacter& mob) { mob.SetIsUnderlingMob(true); };
}

MobAction BlockMusicOverride(bool musicOverrideBlocked) {
	return [musicOverrideBlocked](AMobCharacter& mob) { mob.SetBlockMusicOverride(musicOverrideBlocked); };
}

MobAction ChangeTeam(ETeamName newTeam) {
	return [newTeam](AMobCharacter& mob) { mob.ChangeTeam(newTeam); };
}

MobAction SetTeleportToOwner(ABaseCharacter* owner) {
	return [owner = TWeakObjectPtr<ABaseCharacter>(owner)](AMobCharacter& mob) {
		if(owner.IsValid()) {
			auto teleport = owner->FindComponentByClass<UTeleportToOwnerComponent>();
			if (!teleport) {
				teleport = NewObject<UTeleportToOwnerComponent>(owner.Get(), "TeleportToOwner");
				teleport->RegisterComponent();
			}
			teleport->AddCharacter(mob);
		}
	};
}

}}
