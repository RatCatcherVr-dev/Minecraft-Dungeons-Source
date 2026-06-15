#pragma once

#include <Array.h>
#include "CommonTypes.h"
#include "Objective.h"

namespace io {
struct Objective;
}

namespace game { namespace objective {

Unique<Objective> create(const io::Objective&);
TArray<Unique<Objective>> create(const std::vector<io::Objective>&);

}}
