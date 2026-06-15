#pragma once

#include "util/Validation.h"

class TileGroup;

namespace io { struct Level; }

namespace levelgen {
namespace sourcedata { struct SourceData; }

Validation validate(const sourcedata::SourceData&);

}
