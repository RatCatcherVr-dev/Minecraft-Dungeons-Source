


#include "DamageNumberBatchingComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"

FDamageNumberInstance::FDamageNumberInstance(float damage, AActor* target, const FVector& location, const FGameplayTagContainer& tags)
: Damage(damage), Target(target), Location(location), Tags(tags) {}

// Sets default values for this component's properties
UDamageNumberBatchingComponent::UDamageNumberBatchingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	// ...
}



// Called every frame
void UDamageNumberBatchingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ClientExecuteDamageNumbers(DamageInstances);
	DamageInstances.Empty();
	SetComponentTickEnabled(false);
}

void UDamageNumberBatchingComponent::AddDamageInstance(float damage, AActor* target, const FVector& location, const FGameplayTagContainer& tags) {
	auto pawn = Cast<APawn>(GetOwner());
	
	if(pawn && !pawn->IsLocallyControlled()) {
		DamageInstances.Emplace(damage, target, location, tags);
		SetComponentTickEnabled(true);
	} else {
		InvokeCue(damage, target, location, tags);
	}
}

void UDamageNumberBatchingComponent::InvokeCue(float damage, AActor* target, const FVector& location, const FGameplayTagContainer& tags)
{
	if (auto targetAbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(target)) {
		static FName GameplayCue_DamageNumber = TEXT("GameplayCue.DamageNumber");
		FGameplayCueParameters params;
		params.RawMagnitude = damage;
		params.Location = location;
		params.Instigator = GetOwner();
		params.AggregatedTargetTags = tags;
		targetAbilitySystem->InvokeGameplayCueEvent(FGameplayTag::RequestGameplayTag(GameplayCue_DamageNumber), EGameplayCueEvent::Executed, params);
	}
}
 
void UDamageNumberBatchingComponent::ClientExecuteDamageNumbers_Implementation(const TArray<FDamageNumberInstance>& damage) {
	for(auto instance : damage) {
		InvokeCue(instance.Damage, instance.Target, instance.Location, instance.Tags);
	}
}

