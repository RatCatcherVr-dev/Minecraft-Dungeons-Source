#pragma once

#include "MobSpawnTypes.h"

struct BlockCuboid;

namespace game { namespace mobspawn {

class Regions;
	
namespace providers {

namespace position {

UePositionProvider Location(const FVector&);
BlockPositionProvider Region(const BlockCuboid&, Random* = nullptr);
BlockPositionProvider Regions(const mobspawn::Regions&, Random* = nullptr); //@note @attn: ONLY STORING REFERENCE
BlockPositionProvider Regions(const mobspawn::Regions&&, Random* = nullptr) = delete;
BlockPositionProvider CopiedRegions(mobspawn::Regions, Random* = nullptr);

}

namespace yaw {

const YawProvider& Default(Random* = nullptr);
      YawProvider  OneOf(std::vector<float> degrees, Random* = nullptr);

}

namespace scale {

const ScaleProvider& Identity();

}

TransformProvider FromProviders(BlockPositionProvider, YawProvider = {}, ScaleProvider = {});
TransformProvider FromProviders(UePositionProvider, YawProvider = {}, ScaleProvider = {});
TransformProvider Location(const FVector&, YawProvider = {}, ScaleProvider = {});

}}}
