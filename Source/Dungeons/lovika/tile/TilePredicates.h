#pragma once

#include "TileDef.h"

namespace tilepredicates {

      TilePredicate  id(const std::string&);
      TilePredicate  hasTag(const std::string&);
      MetaTilePredicate unlockedBy(const TArray<FString>&);

      TilePredicate  isDeadEndFromObjectGroups(const std::vector<std::string>&);

}

extern const std::string DeadEndTag;
