#pragma once

#include "MetaScoreTypes.h"

namespace levelgen { namespace score {

      Scorer  Constant(float);

      Scorer  Required(Scorer);
      Scorer  Required(std::initializer_list<Scorer>);

      Scorer  Equals(Scorer, float value, float epsilon = SMALL_NUMBER);
      Scorer  IsInRange(Scorer, float min, float max);

const Scorer& SubDungeons();
const Scorer& BlocksArea();

// Use the meta scorer code path, but don't use
// any additional meta capabilities
const MetaScorer& JustMetaCodePath();

namespace subrange {

      Scorer  MainDungeon(Scorer);
      Scorer  SubDungeons(Scorer);

}

}}
