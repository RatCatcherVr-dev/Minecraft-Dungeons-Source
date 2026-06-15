#include "Dungeons.h"
#include "WhispererFormations.h"
#include "game/Conversion.h"
#include "game/actor/character/mob/MobParams.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/ai/bt/BtEvalState.h"
#include "game/util/ActorQuery.h"
#include "util/Random.h"

namespace bt { namespace formation {

	static FTransform randomRotation(const FVector & translation) {
		static Random rnd;

		return {
			//FRotator { 0.f, rnd.nextInt(4) * 90.f, 0.f },
			FRotator { 0.f, 0.f, 0.f },
			translation,
			FVector::OneVector
		};
	}

	static void addEntry(
		bt::Duration delay,
		EntityType type,
		FVector location,
		float time,
		USummonFormationTask::AddEntryRef add,
		bool ignorePlayers, 
		bool transferEnchantments
	) {
		struct GridEntry { int x; int y; float time; };
		static TArray<GridEntry> entries;

		entries.RemoveAllSwap([&](const auto& candidate) { 
			return time - candidate.time > 5.f; 
		});

		const auto pos = conversion::ueToBlock(location);

		if (!entries.FindByPredicate([&](const auto& candidate) { 
				return candidate.x == pos.x && candidate.y == pos.z; 
			})) {
			entries.Add({ pos.x, pos.z, time });
			add(delay, type, nullptr, randomRotation(location), ignorePlayers, transferEnchantments);
		}
	}

	void Summon(
		bt::StateRef state,
		USummonFormationTask::AddEntryRef add, 
		EntityType entityType,
		bt::Duration delay,
		FVector location,
		bool ignorePlayers = false,
		bool transferEnchantments = false
		)
	{
		addEntry( delay, entityType, location, state.world().GetTimeSeconds(), add, ignorePlayers, transferEnchantments);
	}

	std::function<void(bt::StateRef, USummonFormationTask::AddEntryRef)> makeWhispererWall(EntityType type) {
		static Random rnd;
		
		static const TArray<TArray<FVector2D>> formations{
				{{0,2},{-1,2},{1,2},{-2,1},{2,1},{0,-2},{-1,-2},{1,-2},{-2,-1},{2,-1}},
				{{-1,-1},{0,-1},{1,-1},{-1,0},{1,0},{-1,1},{0,1},{1,1}},
				{{-2,0},{-2,1},{-2,2},{-1,2},{0,2},{1,2},{2,2},{2,1},{2,0}},
				{{-2,0},{-2,-1},{-2,-2},{-1,-2},{0,-2},{1,-2},{2,-2},{2,-1},{2,-0}},
				{{-2,2},{-1,2},{0,2},{1,2},{2,2},{-2,-2},{-1,-2},{0,-2},{1,-2},{2,-2}},
				{{-2,2},{-2,1},{-1,2},{1,2},{2,2},{2,1},{2,-1},{2,-2},{1,-2},{-1,-2},{-2,-2},{-2,-1}},
		};

		return [=](bt::StateRef state, USummonFormationTask::AddEntryRef add) {
			if (!state.params().target.HasActor()) {
				return;
			}

			AActor* target = state.params().target;
			for (auto& formation : formations[rnd.nextInt(0, formations.Num())]) {
				const auto worldLocation = target->GetActorLocation() +
					FVector{ formation, 0 } * Math::PE_TO_UE_UNITS;
				Summon(state, add, type, 0.0s, worldLocation, false, false);
			}
		};
	}
	
	std::function<void(bt::StateRef, USummonFormationTask::AddEntryRef)> makeWhispererPoison(EntityType type) {
		return [=](bt::StateRef state, USummonFormationTask::AddEntryRef add) {
			if (!state.params().target.HasActor()) {
				return;
			}

			AActor* target = state.params().target;

			const auto location = state.owner->GetActorLocation();
			const auto targetLocation = target->GetActorLocation();
			auto forwardVector = targetLocation - location;
			forwardVector.Normalize();
			const auto sideVector = forwardVector.RotateAngleAxis(90.0f, FVector(0.0f, 0.0f, 1.0f));

			const auto pos1 = location + sideVector * -200.f;
			const auto pos2 = location + sideVector * 200.f;

			Summon(state, add, type, 0.821s, pos1, false, false);
			Summon(state, add, type, 2.352s, pos2, false, false);
		};
	}
	
	std::function<void(bt::StateRef, USummonFormationTask::AddEntryRef)> makeWhispererWeb(EntityType type) {
		return [=](bt::StateRef state, USummonFormationTask::AddEntryRef add) {
			TArray<APlayerCharacter*> actors;
			UActorQuery::GetPlayerCharactersInRange(&state.world(), state.owner->GetActorLocation(), 1300.0f, actors, false);
			for (auto actor : actors) {
				auto location = actor->GetActorLocation();
				Summon(state, add, type, 0.0s, location, true, false);
			}
		};
	}

}}
