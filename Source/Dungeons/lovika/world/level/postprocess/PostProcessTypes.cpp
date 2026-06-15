#include "Dungeons.h"
#include "PostProcessTypes.h"
#include "PostProcessConfigs.h"

namespace postprocess {

Config::Config(worldfill::BlockProvider outside, door::BlockProvider door)
	: outsideBlockProvider(std::move(outside))
	, doorBlockProvider(std::move(door)) {
}

Config::Config(const worldfill::BlockProvider& outside)
	: outsideBlockProvider(outside)
	, doorBlockProvider(outside ? door::providers::FromWorldFillProvider(outside) : door::BlockProvider{}) {
}

}
