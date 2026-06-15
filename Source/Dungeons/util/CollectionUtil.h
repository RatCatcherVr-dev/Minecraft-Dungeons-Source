#pragma once

#include "CoreMinimal.h"

namespace util { namespace collection {

	template <class T>
	void clearAndDestroy(TArray<T*> actors) {
		for (auto* actor : actors) {
			if (actor->IsValidLowLevel()) {
				actor->Destroy();
			}		
		}
		actors.Empty();
	}
}}
