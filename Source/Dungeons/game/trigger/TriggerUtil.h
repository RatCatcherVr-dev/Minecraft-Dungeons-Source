#pragma once

class AActor;

namespace lovika {
	class Region;
}

namespace game { namespace trigger {

enum class ExpandType { Both, Down };

UBoxComponent* createCollisionBox(AActor&, const lovika::Region&, ExpandType);

}}
