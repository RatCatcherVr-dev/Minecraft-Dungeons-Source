#include "Dungeons.h"
#include "LevelUpAction.h"
#include "DungeonsGameInstance.h"
#include "game/Game.h"
#include "game/component/PlayerExperienceComponent.h"
#include "game/actor/character/player/XpConsoleCommands.h"

namespace game { namespace objective {

LevelUpAction::LevelUpAction(int level)
	: mLevel(level) {
}

void LevelUpAction::onStop() {
	auto* gameInstance = game().world().GetGameInstance<UDungeonsGameInstance>();
	if (!gameInstance) {
		return;
	}
	const auto targetXp = gameInstance->createCharacterLevelForLevel(mLevel).currentXp;

	for (auto& playerPtr : game().getPlayers()) {
		auto* player = playerPtr.Get();
		if (!player) {
			continue;
		}

		if (auto* experience = player->FindComponentByClass<UPlayerExperienceComponent>()) {
			if (experience->CurrentXp() < targetXp) {
				SetXpFor(player, targetXp);
			}
		}
	}
}

}}
