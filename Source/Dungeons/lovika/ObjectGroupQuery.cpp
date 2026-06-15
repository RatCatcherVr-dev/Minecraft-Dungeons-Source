#include "Dungeons.h"
#include "ObjectGroupQuery.h"
#include "io/ObjectGroupFile.h"

namespace objectgroupquery {

const io::Object* getObject(const io::ObjectGroup& group, BlockPos pos) {
	for (auto&& object : group.objects) {
		if (object.bounds.containsXZ(pos)) {
			return &object;
		}
	}
	return nullptr;
}

}
