#pragma once

#include "GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Engine/BlueprintGeneratedClass.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Engine/SCS_Node.h"

namespace componentutils {

template <class T>
T* CreateComponent(AActor& actor) {
	T* component = NewObject<T>(&actor, T::StaticClass());
	actor.AddInstanceComponent(component);
	component->RegisterComponent();
	return component;
}

template <class T>
T* LookForComponentInChildren(USceneComponent* comp) {
	if (auto casted = Cast<T>(comp)) {
		return casted;
	}
	for (auto child : comp->GetAttachChildren()) {
		if (auto result = LookForComponentInChildren<T>(child)) {
			return result;
		}
	}
	return nullptr;
}

	//Searches a component tree for a specific component, starting at the "second level root".
	//This second level root represents an actor merged into another as part of level decor saving.
	template <class T>
	T* GetComponentInMergedActors(USceneComponent* comp) {
		while (true) {
			USceneComponent* parent = comp->GetAttachParent();
			if (parent) {
				USceneComponent* grandparent = parent->GetAttachParent();
				if (grandparent) {
					comp = parent;
				}
				else {
					break;
				}
			}
			else {
				return nullptr;
			}
		}
		return LookForComponentInChildren<T>(comp);
	}

	template <typename T>
	T* FindChildComponentByClass(const USceneComponent& comp) {
		static_assert(std::is_base_of<UActorComponent, T>::value, "Type must inherit from UActorComponent.");

		for (auto&& child : comp.GetAttachChildren()) {
			if (auto casted = Cast<T>(child)) {
				return casted;
			}
		}
		return nullptr;
	}

	template <typename T>
	int GetChildComponentsByClass(const USceneComponent& comp, TArray<T*>& outChildren) {
		static_assert(std::is_base_of<UActorComponent, T>::value, "Type must inherit from UActorComponent.");

		int num = 0;
		for (auto&& child : comp.GetAttachChildren()) {
			if (auto casted = Cast<T>(child)) {
				outChildren.Add(casted);
				++num;
			}
		}
		return num;
	}

	template <typename T>
	TArray<T*> GetChildComponentsByClass(const USceneComponent& comp) {
		TArray<T*> children;
		GetChildComponentsByClass(comp, children);
		return children;
	}

	template <class T>
	size_t AppendComponents(AActor& actor, TArray<T*>& dest, bool includeFromChildActors = false) {
		TInlineComponentArray<T*, 48> components;
		actor.GetComponents<T>(components, includeFromChildActors);
		dest.Append(components);
		return components.Num();
	}

	template <class T>
	size_t AppendComponents(const TArray<AActor*> actors, TArray<T*>& dest, bool includeFromChildActors = false) {
		size_t preSize = dest.Num();
		for (AActor* actor : actors) {
			AppendComponents<T>(*actor, dest, includeFromChildActors);
		}
		return dest.Num() - preSize;
	}

	template <class T>
	TArray<T*> GetComponents(const TArray<AActor*> actors, bool includeFromChildActors = false) {
		TArray<T*> out;
		AppendComponents(actors, out, includeFromChildActors);
		return out;
	}

	// Used to get components from a default object
	template <class T>
	void FindDefaultComponentsByClass(const UClass* InActorClass, UClass* InComponentClass, TArray<T*>& outArray)
	{
		// Cast the actor class to a UBlueprintGeneratedClass
		if (const UBlueprintGeneratedClass* ActorBlueprintGeneratedClass = Cast<UBlueprintGeneratedClass>(InActorClass)) {
			// Use UBrintGeneratedClass->SimpleConstructionScript->GetAllNodes() to get an array of USCS_Nodes
			if (ActorBlueprintGeneratedClass->SimpleConstructionScript) {
				// Iterate through the array looking for the USCS_Node whose ComponentClass matches the component you're looking for
				for (USCS_Node* Node : ActorBlueprintGeneratedClass->SimpleConstructionScript->GetAllNodes())
				{
					if (UClass::FindCommonBase(Node->ComponentClass, InComponentClass) == InComponentClass)
					{
						// Return cast USCS node's Template into your component class and return it, data's all there
						outArray.Emplace(Cast<T>(Node->ComponentTemplate));
					}
				}
			}
		}
		else {
			UE_LOG(LogDungeons, Warning, TEXT("ActorClass either null or not UBlueprintGeneratedClass"));
		}
	}

	template <class T>
	bool HasComponentOfClass(const UClass* InActorClass) {
		UClass* InComponentClass = T::StaticClass();
		// Cast the actor class to a UBlueprintGeneratedClass
		if (const UBlueprintGeneratedClass* ActorBlueprintGeneratedClass = Cast<UBlueprintGeneratedClass>(InActorClass)) {
			// Use UBrintGeneratedClass->SimpleConstructionScript->GetAllNodes() to get an array of USCS_Nodes
			if (ActorBlueprintGeneratedClass->SimpleConstructionScript) {
				// Iterate through the array looking for the USCS_Node whose ComponentClass matches the component you're looking for
				for (USCS_Node* Node : ActorBlueprintGeneratedClass->SimpleConstructionScript->GetAllNodes())
				{
					if (UClass::FindCommonBase(Node->ComponentClass, InComponentClass) == InComponentClass)
					{
						return true;
					}
				}
			}
		}
		else {
			UE_LOG(LogDungeons, Warning, TEXT("ActorClass either null or not UBlueprintGeneratedClass"));
		}

		return false;
	}

	template<class T>
	FORCEINLINE T* GetComponentOfName(const FString& name, AActor* owner) {
		auto components = owner->GetComponentsByClass(T::StaticClass());

		for (auto comp : components) {

			if (comp->GetName() == name) {
				if (auto casted = Cast<T>(comp)) {
					return casted;
				}
			}
		}

		return nullptr;
	}

	UActorComponent* FindDefaultComponentByClass(const TSubclassOf<AActor> InActorClass, const TSubclassOf<UActorComponent> InComponentClass);
	
	// Used to get components from a default object
	template <class T>
	T* GetDefaultComponentByClass(const TSubclassOf<AActor> InActorClass) {
		UClass* componentClass = T::StaticClass();
		if (auto comp = componentutils::FindDefaultComponentByClass(InActorClass, componentClass)) {
			return Cast<T>(comp);
		}
		return nullptr;
	}


	template <class T>
	T* GetComponentByTag(const AActor* object, FName tag) {
		UClass* componentClass = T::StaticClass();
		auto comps = object->GetComponentsByTag(componentClass, tag);
		if (comps.Num() > 0) {
			return Cast<T>(comps[0]);
		}
		return nullptr;
	}
}
