#include "TileDecorator.h"
#include "TileDecoratorFilter.h"
#include "TileDecoratorUtils.h"
#include "editor/decoration/DecorationAnchor.h"
#include "editor/decoration/DecorationLevelActor.h"
#include <Engine/DirectionalLight.h>
#include <Engine/ExponentialHeightFog.h>
#include <Engine/SkyLight.h>
#include <Engine/PostProcessVolume.h>
#include <LevelSequenceActor.h>

namespace filter {

ActorPredicate All() {
	return [](const AActor*) { return true; };
}

ActorPredicate HasTag(const FName& tag) {
	return[tag](const AActor* actor) { return actor->ActorHasTag(tag); };
}

ActorPredicate HasAttachedParent(const AActor& parent) {
	return[&parent](const AActor* actor) {
		return util::actor::isAttachedParentOf(parent, *actor);
	};
}

ActorPredicate IsAttachedTo(UClass* cls) {
	return[cls](const AActor* actor) {
		return util::actor::isAttachedTo(*actor, cls);
	};
}

ActorPredicate IsA(UClass* cls) {
	return[cls](const AActor* actor) {
		return actor->IsA(cls);
	};
}

ActorPredicate IsInside(const FBox& box) {
	return [box](const AActor* actor) {
		return box.IsInsideOrOn(actor->GetActorLocation());
	};
}

ActorPredicate IsInsideXy(const FBox& box) {
	return [box](const AActor* actor) {
		const auto pos = actor->GetActorLocation();
		return (pos.X >= box.Min.X) && (pos.X <= box.Max.X) && (pos.Y >= box.Min.Y) && (pos.Y <= box.Max.Y);
	};
}

ActorPredicate InLevel(const class ULevel& level) {
	return [&level](const AActor* actor) {
		return &level == actor->GetLevel();
	};
}

ActorPredicate InPersistentLevel() {
	return [](const AActor* actor) {
		return actor->GetLevel() == actor->GetWorld()->GetLevel(0);
	};
}

ActorPredicate InNullableLevel(const class ULevel* level) {
	return level ? InLevel(*level) : All();
}


//
// Lovika editor specific
//

ActorPredicate HasDecorActorTag() {
	static const FName lovikaBPActorTag = "LovikaDecorActor";
	return HasTag(lovikaBPActorTag);
}

ActorPredicate IsDecorComponent() {
	return
		!HasAttachedParent(*util::deco::getLevel()) &&
		!HasDecorActorTag();
}

ActorPredicate NeedsDecorActor() {
	return
		!HasAttachedParent(*util::deco::getLevel()) &&
		HasDecorActorTag();
}

filter::ActorPredicate IsGlobal() {
	return
		IsA<ADirectionalLight>() ||
		IsA<ASkyLight>() ||
		IsA<APostProcessVolume>() ||
		IsA<AExponentialHeightFog>() ||
		IsA<ADecorationAnchor>() ||
		IsA<ADecorationLevelActor>() ||
		IsA<ALevelSequenceActor>();
}

}

//
// Overloaded boolean operators
//
filter::ActorPredicate operator!(const filter::ActorPredicate& pred) {
	return filter::Not(pred);
}

filter::ActorPredicate operator&&(const filter::ActorPredicate& pred1, const filter::ActorPredicate& pred2) {
	return filter::And(pred1, pred2);
}

filter::ActorPredicate operator||(const filter::ActorPredicate& pred1, const filter::ActorPredicate& pred2) {
	return filter::Or(pred1, pred2);
}
