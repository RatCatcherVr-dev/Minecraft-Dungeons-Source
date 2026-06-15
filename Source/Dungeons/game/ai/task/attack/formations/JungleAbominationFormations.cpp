#include "Dungeons.h"
#include "WhispererFormations.h"
#include "game/Conversion.h"
#include "game/actor/character/mob/MobParams.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/ai/bt/BtEvalState.h"
#include "game/util/ActorQuery.h"
#include "game/util/LocationQuery.h"
#include "util/Random.h"

#include "DrawDebugHelpers.h"

namespace bt { namespace formation {

	std::function<void(bt::StateRef, USummonFormationTask::AddEntryRef)>
		JungleAbominationRadial( 
			EntityType entityType,
			int count,
			float radius
	){
		return [=](bt::StateRef state, USummonFormationTask::AddEntryRef add) {
			FVector centre = state.owner->GetActorLocation();
			if (const auto maybeGround = locationquery::findGround(state.world(), centre)) {
				//centre.Z = maybeGround.GetValue() + 100.0f;
			}
			static Random rnd;
			const float twoPi = PI * 2.0f;
			const float fCount = static_cast<float>(count);

			for( int i = 0; i < count; ++i ) {
				float distance = rnd.nextFloat( 700.0f, radius );
				float angle = (twoPi/fCount) * static_cast<float>(i);
				FTransform transform {
					FRotator{ 0.0f, 0.0f, 0.0f },
					FVector{ cos(angle), sin(angle), 0.0f } * distance + centre,
					FVector::OneVector
				};
				add( bt::Duration(bt::Seconds(rnd.nextFloat(0.0f, 0.2f))), 
					entityType, nullptr, transform, false, false );
			}

		};
	}

	std::function<void(bt::StateRef, USummonFormationTask::AddEntryRef)>
		JungleAbominationPlayer( 
			EntityType entityType,
			int count,
			float radius
	){
		return [=](bt::StateRef state, USummonFormationTask::AddEntryRef add) {
			TArray< APlayerCharacter* > actors;
			UActorQuery::GetPlayerCharactersInRange( &state.world(), state.owner->GetActorLocation(), 2500.0f, actors, true);

			for( int i = 0; i < FMath::Min( count, actors.Num() ); ++i ) {
				FTransform transform{
					FRotator{ 0.0f, 0.0f, 0.0f },
					actors[i]->GetActorLocation(),
					FVector::OneVector
				};
				add( 0.0s, entityType, nullptr, transform, true, false );
			}

			int random = count - actors.Num();
			if( random > 0 ) {
				JungleAbominationRadial( entityType, random, radius )( state, add );
			}
		};
	}

}}
