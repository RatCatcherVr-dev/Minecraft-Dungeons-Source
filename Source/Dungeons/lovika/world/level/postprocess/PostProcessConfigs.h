#pragma once

#include "PostProcessTypes.h"

namespace postprocess {

namespace worldfill { namespace providers {

const BlockProvider& None();
const BlockProvider& Overworld();
const BlockProvider& NewCave();

      BlockProvider  Uniform(FullBlock);
      BlockProvider  Gradient(float minDistance, float slope, float add, BlockID);

	  BlockProvider  BlockForDistance(const std::vector<FullBlock>&, float ditheringAmount = 1.0f);
	  BlockProvider  FillGradient(const std::vector<int>& rowsPerDataId);
	  BlockProvider  FillGradient(const std::vector<Pair<int, DataID>>& rowsOfIds);

}}


namespace door { namespace providers {

const BlockProvider& RandomDoorFrameBlock();
      BlockProvider  FromWorldFillProvider(const worldfill::BlockProvider&);

}}


namespace configs {

const Config& NewCave();
const Config& Overworld();
const Config& OnlyDoors();

      Config  FromWorldFillProvider(const worldfill::BlockProvider&);
	  Config  RandomDoorFrameBlock(const worldfill::BlockProvider& = {});

}

}
