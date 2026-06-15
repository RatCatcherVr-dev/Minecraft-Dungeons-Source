#include "TileDecorator.h"
#include "TileDecoratorUtils.h"
#include "UObject/Object.h"
#include "FileHelpers.h"
#include "Editor.h"
#include "Engine/Selection.h"
#include "EngineUtils.h"
#include "AssetEditorManager.h"
#include "ILayers.h"
#include "editor/decoration/DecorationLevelActor.h"
#include "game/level/ambience/AmbienceActor.h"
#include "game/util/ActorQuery.h"

namespace util {

TArray<AActor*> actor::filter(const filter::ActorPredicate& pred, UWorld* world /* = nullptr*/) {
	TArray<AActor*> out;

	for (auto* actor : TActorRange<AActor>(world? world : editor::getWorld())) {
		if (pred(actor)) {
			out.Add(actor);
		}
	}
	return out;
}

bool actor::isAttachedParentOf(const AActor& parent, const AActor& descendant) {
	auto child = &descendant;
	while (child) {
		if (&parent == child) {
			return true;
		}
		child = child->GetAttachParentActor();
	}
	return false;
}

bool actor::isAttachedTo(const AActor& descendant, UClass* cls) {
	auto parent = descendant.GetAttachParentActor();
	while (parent) {
		if (parent->IsA(cls)) {
			return true;
		}
		parent = parent->GetAttachParentActor();
	}
	return false;
}

void actor::removeAllComponentsOfType(AActor& actor, const TSubclassOf<UActorComponent>& componentClass) {
	return removeMatchingComponentsOfType(actor, componentClass, [](auto&&) { return true; });
}

void actor::removeMatchingComponentsOfType(AActor& actor, const TSubclassOf<UActorComponent>& componentClass, const filter::ComponentPredicate& predicate) {
	for (auto&& old : actor.GetComponentsByClass(componentClass)) {
		if (!predicate(old)) {
			continue;
		}
		actor.RemoveOwnedComponent(old);
		actor.RemoveInstanceComponent(old);
	}
}

UWorld* editor::getWorld() {
	return GEditor->GetEditorWorldContext().World();
}

TArray<AActor*> editor::getSelectedActors(USelection* selection /*= nullptr*/) {
	if (selection == nullptr) {
		selection = GEditor->GetSelectedActors();
	}
	TArray<AActor*> actors;
	for (FSelectionIterator Iter(*selection); Iter; ++Iter) {
		if (auto actor = Cast<AActor>(*Iter)) {
			actors.Add(actor);
		}
	}
	return actors;
}

void editor::setSelectedActors(const TArray<AActor*>& actors) {
	GEditor->SelectNone(true, true);
	selectActors(actors);
}

void editor::selectActors(const TArray<AActor*>& actors) {
	for (auto actor : actors) {
		GEditor->SelectActor(actor, true, false, true);
	}
}

void editor::deleteActors(const TArray<AActor*>& actors) {
	auto world = getWorld();

	GEditor->GetSelectedActors()->Modify();
	for (auto&& actor : actors) {
		// Remove from active selection in editor
		GEditor->SelectActor(actor, /*bSelected=*/ false, /*bNotify=*/ false);
		GEditor->Layers->DisassociateActorFromLayers(actor);
		world->EditorDestroyActor(actor, false);
	}
}

void editor::closeAllEditorsForAsset(UObject* object) {
	FAssetEditorManager::Get().CloseAllEditorsForAsset(object);
}

void save::saveObject(UObject* object) {
	if (!object) {
		return;
	}
	saveObjects({ object });
}

void save::saveObjects(const TArray<UObject*>& objects) {
	if (objects.Num() == 0) {
		return;
	}
	TArray<UPackage*> packages;
	for (auto object : objects) {
		check((object != nullptr) && object->IsAsset());
		packages.Add(object->GetOutermost());
	}
	FEditorFileUtils::PromptForCheckoutAndSave(packages, true, /*bPromptToSave=*/ false);
}

ADecorationLevelActor* deco::getLevel() {
	if (auto world = editor::getWorld()) {
		return actorquery::getFirstActor<ADecorationLevelActor>(world);
	}
	return nullptr;
}

}
