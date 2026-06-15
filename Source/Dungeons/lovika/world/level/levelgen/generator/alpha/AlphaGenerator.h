#pragma once

#include "lovika/world/level/levelgen/generator/Generator.h"

struct FLevelSettings;
namespace io { struct HyperLevel; }

namespace generator { namespace alpha {

const GeneratorFunc& DefaultGenerator();
      GeneratorFunc  HyperGenerator(const io::HyperLevel&, const FLevelSettings&);

}}
