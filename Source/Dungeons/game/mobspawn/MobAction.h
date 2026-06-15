#pragma once

#include <functional>

class Mob;
class ABaseCharacter;
class AMobCharacter;
enum class ETeamName : uint8;

namespace game { namespace mobspawn {

using MobAction = std::function<void(AMobCharacter&)>;

const MobAction& LongOffensiveRange();
const MobAction& AddGlobalHealthBarOnMiniBosses();
const MobAction& RequiredForObjective();

      template <typename T>
	  MobAction  AddComponent(TOptional<bool> setReplicates = {});

	  MobAction  AddTag(const FName& tag);
	  MobAction	 SpawnAsUnderling();
	  MobAction BlockMusicOverride(bool musicOverrideBlocked);
      MobAction  ChangeMaster(ABaseCharacter*);
	  MobAction	 ChangeTeam(ETeamName);
	  MobAction	 SetTeleportToOwner(ABaseCharacter*);
}}

#include "MobAction.hpp"
