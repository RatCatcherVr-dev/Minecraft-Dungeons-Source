#pragma once

#include "Containers/Array.h"
#include "Containers/Map.h"

class UWorld;

template<typename T, bool AllowDataListShrink = true>
class DataTracker
{
public:

	void AddInstance(UWorld* pWorld, T data) { TrackingLists.FindOrAdd(pWorld).Push(std::move(data)); };
	void RemoveInstance(UWorld* pWorld, const T& data)
	{
		// Try find the actor in it's current UWorld (succeeds ~99.9% of the times)
		if (auto* WorldList = TrackingLists.Find(pWorld)) {
			if (WorldList->RemoveSingleSwap(data, AllowDataListShrink) > 0) {
				if (WorldList->Num() == 0) {
					TrackingLists.Remove(pWorld);
				}
				return;
			}
		}

		// Fallback to iterating (the other) worlds in case the actor has changed world
		for (auto it = TrackingLists.CreateIterator(); it; ++it) {
			auto& weakWorld = it.Key();
			if (!weakWorld.IsValid()) {
				it.RemoveCurrent();
				continue;
			}
			if (weakWorld == pWorld) {
				continue;
			}
			TArray<T>& WorldList = it.Value();
			if (WorldList.RemoveSingleSwap(data, AllowDataListShrink) > 0) {
				if (WorldList.Num() == 0) {
					it.RemoveCurrent();
				}
				return;
			}
		}
	};

	TArray<T>& GetList(UWorld* pWorld) { return TrackingLists.FindOrAdd(pWorld); };

private:
	TMap< TWeakObjectPtr<UWorld>, TArray<T> > TrackingLists;
};

