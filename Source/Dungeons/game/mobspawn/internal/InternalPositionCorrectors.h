#pragma once

#include <Optional.h>

class UWorld;
enum class EntityType : unsigned int;
namespace game { class Game; }

namespace game { namespace mobspawn { namespace positioncorrectors { namespace internal {

TOptional<FTransform> correctPositionByLevelTrace(UWorld&, FTransform, bool ignorePlayers);
TOptional<FTransform> correctPositionByTileData(const Game&, EntityType, FTransform);

}}}}
