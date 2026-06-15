#pragma once

class AActor;
class UPrimitiveComponent;

namespace game { namespace trigger { namespace internal {

struct OverlapState {
	UPrimitiveComponent& regionComponent;
	AActor& actor;
};

using BeginOverlapState = OverlapState;

}}}
