#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BackpackComponent.h"
#include "PickupComponent.generated.h"

class AMobCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONS_API UPickupComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPickupComponent();

	void BeginPlay() override;	
	void OnAttributeHealthChange(const FOnAttributeChangeData& data) const;	

	TOptional<FVector> StoredLocation;

	FVector StoredFirstLocation;

private:
	bool AttemptPickUp(const FOnAttributeChangeData& data) const;

	void PickUp(UMochilaComponent* backpackComponent) const;

	AMobCharacter* GetOwnerAsMob() const;

	UPROPERTY(EditAnywhere, Category = "Dungeons|PickupComponent")
	TSubclassOf<class AActor> MountedBlueprint;

	UPROPERTY(EditAnywhere, Category = "Dungeons|PickupComponent")
	class UParticleSystem* PickupParticleSystem;

	UPROPERTY(EditAnywhere, Category = "Dungeons|PickupComponent")
	bool bIsFakePickUp = false;
};
