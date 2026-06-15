#pragma once
#include "CommonTypes.h"
#include <GameFramework/Actor.h>

class UClass;
class ULevel;

namespace filter {

using ActorPredicate = Pred<const AActor*>;
using ComponentPredicate = Pred<const UActorComponent*>;

template <class ElemType>
Pred<ElemType> And(const Pred<ElemType>&, const Pred<ElemType>&);
template <class ElemType>
Pred<ElemType> Or(const Pred<ElemType>&, const Pred<ElemType>&);
template <class ElemType>
Pred<ElemType> Not(const Pred<ElemType>&);

ActorPredicate All();

ActorPredicate HasAttachedParent(const AActor& parent);
ActorPredicate IsAttachedTo(UClass*);
template <typename T> ActorPredicate IsAttachedTo() { return IsAttachedTo(T::StaticClass()); }

ActorPredicate IsA(UClass*);
template <typename T> ActorPredicate IsA() { return IsA(T::StaticClass()); }

ActorPredicate IsInside(const FBox&);
ActorPredicate IsInsideXy(const FBox&);

ActorPredicate InLevel(const ULevel&);
ActorPredicate InPersistentLevel();
ActorPredicate InNullableLevel(const ULevel*);

// Lovika (editor) specific
ActorPredicate HasDecorActorTag();
ActorPredicate IsDecorComponent();
ActorPredicate NeedsDecorActor();

ActorPredicate IsGlobal();

}

filter::ActorPredicate operator!(const filter::ActorPredicate&);
filter::ActorPredicate operator&&(const filter::ActorPredicate&, const filter::ActorPredicate&);
filter::ActorPredicate operator||(const filter::ActorPredicate&, const filter::ActorPredicate&);

#include "TileDecoratorFilter.hpp"
