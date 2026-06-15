#pragma once

#include "CoreMinimal.h"

namespace classutil {
	
	template <class T>
	TSubclassOf<T> LoadClassAsRoot(const FString& name) {
		auto cls = StaticLoadClass(T::StaticClass(), nullptr, *name);
		if (cls) {
			cls->AddToRoot();	
		}			
		return cls;
	}
}