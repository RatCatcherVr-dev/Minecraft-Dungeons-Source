#include "Dungeons.h"
#include "MobSpawnProviders.h"
#include "MobSpawnTypes.h"
#include "SpawnRegions.h"
#include "game/Conversion.h"
#include "util/SharedRandom.h"

namespace game { namespace mobspawn { namespace providers {

namespace position {

UePositionProvider Location(const FVector& location) {
	return [location] { return location; };
}

BlockPositionProvider Region(const BlockCuboid& floor, Random* rnd /*= nullptr*/) {
	return [floor, rnd = Util::thisOrSharedRandom(rnd)] { return randomPos(floor, *rnd, 0.3f); };
}

BlockPositionProvider Regions(const mobspawn::Regions& regions, Random* rnd /*= nullptr*/) {
	return [&regions, rnd = Util::thisOrSharedRandom(rnd)] { return regions.getRandomPosFromRandomRegion(0.3f, rnd); };
}

BlockPositionProvider CopiedRegions(mobspawn::Regions regions, Random* rnd /*= nullptr*/) {
	return[regions = std::move(regions), rnd = Util::thisOrSharedRandom(rnd)]{ return regions.getRandomPosFromRandomRegion(0.3f, rnd); };
}

}

namespace yaw {

const YawProvider& Default(Random* rnd /*= nullptr*/) {
	static const YawProvider provider = [] { return Util::sharedRandom().nextFloat(360.0f); };
	return provider;
}

YawProvider OneOf(std::vector<float> degrees, Random* rnd /*= nullptr*/) {
	return[degrees = std::move(degrees), rnd]{ return randomRotation(degrees, *Util::thisOrSharedRandom(rnd)); };
}

}

namespace scale {

const ScaleProvider& Identity() {
	static const ScaleProvider provider = [] { return FVector::OneVector; };
	return provider;
}

}

TransformProvider FromProviders(BlockPositionProvider position, YawProvider yaw, ScaleProvider scale) {
	if (!yaw) {
		yaw = yaw::Default();
	}
	if (!scale) {
		scale = scale::Identity();
	}
	return [pos = std::move(position), yaw = std::move(yaw), scale = std::move(scale)] {
		return FTransform{ FRotator { 0.f, yaw(), 0.f }, conversion::posToUe(pos()), scale() };
	};
}

TransformProvider FromProviders(UePositionProvider position, YawProvider yaw, ScaleProvider scale) {
	if (!yaw) {
		yaw = yaw::Default();
	}
	if (!scale) {
		scale = scale::Identity();
	}
	return[pos = std::move(position), yaw = std::move(yaw), scale = std::move(scale)]{
		return FTransform{ FRotator { 0.f, yaw(), 0.f }, pos(), scale() };
	};
}

TransformProvider Location(const FVector& location, YawProvider yaw, ScaleProvider scale) {
	return FromProviders(position::Location(location), std::move(yaw), std::move(scale));
}

}}}
