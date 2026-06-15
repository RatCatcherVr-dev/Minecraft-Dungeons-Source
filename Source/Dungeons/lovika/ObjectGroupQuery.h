#pragma once
#include "world/level/BlockPos.h"

namespace io {
struct Object;
struct ObjectGroup;
}

namespace objectgroupquery {

const io::Object* getObject(const io::ObjectGroup&, BlockPos);

}
