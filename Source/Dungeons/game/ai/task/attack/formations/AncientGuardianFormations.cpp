#include "Dungeons.h"
#include "GeomancerFormations.h"
#include "game/Conversion.h"
#include "game/actor/character/mob/MobParams.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/ai/bt/BtEvalState.h"
#include "util/Random.h"

namespace bt { namespace formation {
	std::function<void(bt::StateRef, USummonFormationTask::AddEntryRef)> AncientGuardianMineFormation(std::function<TArray<FVector>()> GetFormationPositions) {
		static Random rnd;

		return [=](bt::StateRef state, USummonFormationTask::AddEntryRef add) {
			auto formationPositions = GetFormationPositions();

			for (auto i = 0; i < formationPositions.Num(); i++) {

				const FTransform transform{
					FRotator { 0.0f, rnd.nextFloat(PI * 2), 0.f },
					formationPositions[i],
					FVector::OneVector
				};

				add(0.06s, EntityType::Biomine, nullptr, transform, true, false);
			}
		};
	}
}}
