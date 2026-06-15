#include "Dungeons.h"
#include "EnvironmentUtils.h"

namespace environment {

bool startedFromEditor() {
#if WITH_EDITOR
	return true;
#else
	return false;
#endif
}

}