#pragma once
#include <WeakObjectPtrTemplates.h>

struct FLocator {
	FLocator() : type(Type::Nothing) {}
	FLocator(const FVector& location) : location(location), type(Type::Vector) {}
	FLocator(TWeakObjectPtr<AActor> actor) : actor(actor), type(Type::Actor) {}
	FLocator(AActor* actor) : actor(actor), type(Type::Actor) {}

	bool IsLocation() const {
		return type == Type::Vector;
	}

	bool HasActor() const {
		return type == Type::Actor && actor != nullptr;
	}

	bool IsValid() const {
		return IsLocation() || HasActor();
	}

	TOptional<FVector> GetLocationLike() const {
		return IsLocation() ? location : (HasActor() ? actor->GetActorLocation() : TOptional<FVector>{});
	}

	operator AActor*() const {
		return actor.Get();
	}

	FVector location;
	TWeakObjectPtr<AActor> actor;

private:
	enum class Type { Nothing, Actor, Vector };
	Type type;
};
