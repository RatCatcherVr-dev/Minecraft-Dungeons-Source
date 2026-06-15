#pragma once

#include "Containers/Array.h"
#include "Components/ActorComponent.h"
#include "TileDecoratorFilter.h"
#include <vector> // @todo: shouldn't sit here, but a workaround for ResourceHelper missing an #include, and I don't wanna rebuild

class UObject;
class AActor;
class USelection;
class ULevel;
class ADecorationLevelActor;

namespace util {
namespace actor { // @todo: move to ActorQuery.h
	TArray<AActor*> filter(const filter::ActorPredicate&, UWorld* = nullptr);
	bool isAttachedParentOf(const AActor& parent, const AActor& descendant);
	bool isAttachedTo(const AActor& descendant, class UClass*);
	void removeAllComponentsOfType(AActor&, const TSubclassOf<UActorComponent>&);
	void removeMatchingComponentsOfType(AActor&, const TSubclassOf<UActorComponent>&, const filter::ComponentPredicate&);
}

namespace editor {
UWorld* getWorld();
	TArray<AActor*> getSelectedActors(USelection* selection = nullptr);
	void setSelectedActors(const TArray<AActor*>&);
	void selectActors(const TArray<AActor*>&);

	void deleteActors(const TArray<AActor*>&);
	void closeAllEditorsForAsset(UObject*);
}

namespace deco {
	ADecorationLevelActor* getLevel();
}

namespace save {
	void saveObject(UObject*);
	void saveObjects(const TArray<UObject*>&);
}
}
