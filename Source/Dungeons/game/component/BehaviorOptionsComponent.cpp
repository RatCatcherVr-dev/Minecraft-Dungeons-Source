#include "Dungeons.h"
#include "BehaviorOptionsComponent.h"

UBehaviorOptionsComponent::UBehaviorOptionsComponent() {
	PrimaryComponentTick.bCanEverTick = false;
}

float UBehaviorOptionsComponent::Get(const FString& name, float defaultValue) const {
	return Options.Contains(name) ? *Options.Find(name) : defaultValue;
}

bt::Duration UBehaviorOptionsComponent::Get(const FString& name, bt::Duration defaultValue) const {
	return Options.Contains(name) ?
		bt::Duration { bt::Seconds { *Options.Find(name) } } :
		defaultValue;
}

UAnimSequenceBase* UBehaviorOptionsComponent::Get(const FString& name) const {
	return Sequences.Contains(name) ? *Sequences.Find(name) : nullptr;
}

TSubclassOf<UGameplayEffect> UBehaviorOptionsComponent::GetEffect(const FString& name) const {
	return GameplayEffects.Contains(name) ? *GameplayEffects.Find(name) : nullptr;
}

TSubclassOf<AActor> UBehaviorOptionsComponent::GetClass(const FString& name) const {
	return Classes.Contains(name) ? *Classes.Find(name) : nullptr;
}

EntityType UBehaviorOptionsComponent::Get(const FString& name, EntityType defaultValue) const {
	return Entities.Contains(name) ? *Entities.Find(name) : defaultValue;
}