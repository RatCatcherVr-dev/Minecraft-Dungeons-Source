#pragma once

#include "GameFramework/Actor.h"
#include "ObjectMacros.h"
#include "Components/SceneComponent.h"
#include "InstantTravelActor.generated.h"

class APlayerCharacter;

UCLASS()
class DUNGEONS_API AInstantTravelActor : public AActor {
	GENERATED_BODY()

public:
	explicit AInstantTravelActor(const FObjectInitializer& objectInitializer);

	FVector GetTravelToMeLocation() const;

	FRotator GetTravelToMeRotation() const;

	void SetDestinationLocation(FVector partnerDoorLocation) const;

	void SetDestinationRotation(const FRotator partnerDoorRotation) const;

	void BeginPlay() override;

	class UReplicatedInteractableComponent* GetInteractableComponent() {return InteractableComponent;}

	UFUNCTION(BlueprintCallable)
	void InstantTravelToDestination(AActor* traveler);
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	USceneComponent* Destination;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	USceneComponent* TravelToMeLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, /*Replicated,*/ Category = "Dungeons")
	class UMapPinComponent* MapPinComponent;

	UFUNCTION(BlueprintImplementableEvent)
	void OnInstantTravel(AActor* traveler);

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, Category = "Dungeons")
	class UReplicatedInteractableComponent* InteractableComponent;

	// #D11.CM
	bool InstantTravelLocalPlayer(ULocalPlayer* player, APlayerCharacter* leader);
};
