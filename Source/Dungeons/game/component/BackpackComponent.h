#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "world/level/block/Block.h"
#include "GameplayEffect.h"
#include "BackpackComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONS_API UMochilaComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMochilaComponent();

	void BeginPlay() override;

	void Drop();

	void DropAtStoredLocation();
			
	UFUNCTION()
	void ApplyDamage(const FGameplayEffectSpec& spec);

	void Store(TSubclassOf<class AActor>, EntityType, const FVector& location, const FVector& firstLocation);

	bool IsEmpty() const;

	bool Has(EntityType) const;

	EntityType Get() const;

	void Clear();
	
	UFUNCTION()
	void OnRep_ActorClass();
	
	void EndPlay(EEndPlayReason::Type reason) override;
private:

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	TOptional<EntityType> StoredEntityType;
	TOptional<FVector> StoredEntityLocation;
	TOptional<FVector> StoredEntityFirstLocation;

	UPROPERTY(ReplicatedUsing = OnRep_ActorClass)
	TSubclassOf<AActor> ActorClass;

	UPROPERTY(EditAnywhere, Category = "Dungeons|BackpackComponent")
	class UParticleSystem* DropParticleSystem;
};
