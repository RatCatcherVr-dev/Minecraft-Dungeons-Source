#include "Dungeons.h"
#include "TargetLocators.h"

namespace targetlocators {

io::RegionLocator allTilesInStretch(const std::string& stretch) {
	return stretch + ".*";
}

io::RegionLocator allRegionsInStretch(const std::string& stretch) {
	return stretch + ".*.*";
}

}
