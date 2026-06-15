#pragma once

#include "AmbienceActor.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include <CommonTypes.h>
#include <LogMacros.h>

namespace game { namespace ambience {

class AmbienceUtil {
public:
	template <class AmbienceType>
	static TArray<AmbienceType*>& GetAmbienceActors(UWorld* world) {
		return InstanceTracker<AmbienceType>::GetList(world);
	}

	template <class AmbienceType, typename EType>
	static AmbienceType* SetActiveAmbienceT(UWorld* world, const EType& ambience) {
		AmbienceType* ActorToActivate = nullptr;

		for (auto* ambienceActor : GetAmbienceActors<AmbienceType>(world)) {
			if (!ambienceActor->Matches(ambience)){
				ambienceActor->SetActive(false);				
			} else if (!ActorToActivate) {				
				ActorToActivate = ambienceActor;
			} else {
				//Only handle first matching actor
				// D11.DH 
				// made this not print in Test configuration as it was having a significant performance impact and was skewing profiler data in dingy jungle
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
				UE_LOG(LogDungeons, Error, TEXT("Too many ambience actors match the ambience Id/GroupName"), *GetEnumValueToStringStripped(ambience.Id), *ambience.GroupName);
#endif
			}
		}

		if (ActorToActivate) {
			ActorToActivate->SetActive(true);
			return ActorToActivate;
		}
		return nullptr;
	}

	template <class AmbienceType, typename EType>
	static void PlayerEnteredAmbienceT(APlayerCharacter* player, const EType& ambience) {
		for (auto* ambienceActor : GetAmbienceActors<AmbienceType>(player->GetWorld())) {
			if (ambienceActor->Matches(ambience)) {
				ambienceActor->PlayerEntered(player);
				break; //Only handle first matching actor
			}			
		}
	}


	template <class AmbienceType, typename EType>
	static void PlayerExitedAmbienceT(APlayerCharacter* player, const EType& ambience) {
		for (auto* ambienceActor : GetAmbienceActors<AmbienceType>(player->GetWorld())) {
			if (ambienceActor->Matches(ambience)) {
				ambienceActor->PlayerExited(player);
				break; //Only handle first matching actor
			}
		}
	}

};

}}
