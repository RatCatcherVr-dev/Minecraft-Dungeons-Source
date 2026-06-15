#include "Dungeons.h"
#include "CharacterQuery.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/component/HealthComponent.h"
#include "game/Game.h"
#include "world/entity/MobTags.h"

namespace characterquery {
namespace is {

	CharacterPred hostile(const ABaseCharacter* character) {
		return [character](const ABaseCharacter* arg) {
			return character->IsHostileTowards(arg);
		};
	}

	bool hostile(const ABaseCharacter* firstCharacter, const ABaseCharacter* secondCharacter)
	{
		return firstCharacter->IsHostileTowards(secondCharacter);
	}

	CharacterPred friendly(const ABaseCharacter* character) {
		return [character](const ABaseCharacter* arg) {
			return character->IsFriendlyTowards(arg);
		};
	}

	bool friendly(const ABaseCharacter* firstCharacter, const ABaseCharacter* secondCharacter)
	{
		return firstCharacter->IsFriendlyTowards(secondCharacter);
	}

	bool targetable(const ABaseCharacter* character) {
		return character->IsTargetable();
	}

	bool movable(const ABaseCharacter* character) {
		bool isImmobile = false;
		if( auto mob = Cast<AMobCharacter>( character ) ) {
			isImmobile = hasMobTag( mob->EntityType, MobTags::HashTag_Immobile );
		}
		return characterquery::is::targetable(character) && actorquery::is::alive(character) && !isImmobile;
	}

	bool boss(const ABaseCharacter* character) {
		if (auto mob = Cast<AMobCharacter>(character)) {
			return hasMobTag(mob->EntityType, MobTags::HashTag_Miniboss);
		}

		return false;
	}
}
namespace can {
	CharacterPred heal(const ABaseCharacter* character) {
		return [character](const ABaseCharacter* arg) {
			return character->CanHealTarget(arg);
		};
	}

	CharacterPred damage(const ABaseCharacter* character) {
		return [character](const ABaseCharacter* arg) {
			return character->CanDamageTarget(arg);
		};
	}

}

}