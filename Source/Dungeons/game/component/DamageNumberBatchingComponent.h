

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayEffectTypes.h"
#include "DamageNumberBatchingComponent.generated.h"


USTRUCT()
struct FDamageNumberInstance {
	GENERATED_BODY()

	FDamageNumberInstance() {};
	FDamageNumberInstance(float damage, AActor* target, const FVector& location, const FGameplayTagContainer& tags);
	
	UPROPERTY()
	float Damage;

	UPROPERTY()
	AActor* Target;

	UPROPERTY()
	FVector_NetQuantize10 Location;

	UPROPERTY()
	FGameplayTagContainer Tags;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONS_API UDamageNumberBatchingComponent : public UActorComponent
{
	GENERATED_BODY()

	TArray<FDamageNumberInstance> DamageInstances;

	void InvokeCue(float damage, AActor* target, const FVector& location, const FGameplayTagContainer& tags);
	
public:	
	// Sets default values for this component's properties
	UDamageNumberBatchingComponent();
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void AddDamageInstance(float damage, AActor* target, const FVector& Location, const FGameplayTagContainer& tags);

	UFUNCTION(Client, Reliable)
	void ClientExecuteDamageNumbers(const TArray<FDamageNumberInstance>& damage);
	
};
