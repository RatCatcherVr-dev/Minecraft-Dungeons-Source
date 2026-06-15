#pragma once

#include <string>

namespace io {
	using RegionLocator = std::string;
}

namespace targetlocators {

io::RegionLocator allTilesInStretch(const std::string& stretch);
io::RegionLocator allRegionsInStretch(const std::string& stretch);

}

