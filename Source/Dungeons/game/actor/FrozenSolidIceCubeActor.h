#pragma once
#include "repgraph/Actor_RepSpatializeStatic.h"
#include "game/component/ReplicatedInteractableComponent.h"
#include "game/actor/PropActor.h"
#include "FrozenSolidIceCubeActor.generated.h"

// #D11.CM

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerInteracted);

UCLASS()
class DUNGEONS_API AFrozenSolidIceCubeActor :
	public AActor_RepSpatializeStatic
{
	GENERATED_BODY()
public:
	AFrozenSolidIceCubeActor();

	void BeginPlay();

	void EndPlay(const EEndPlayReason::Type EndPlayReason);

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable)
	int GetCurrentInteractCount () const { return interactCount ; }

	UFUNCTION(BlueprintCallable)
	void ApplyBounceToOwner() const;

	UFUNCTION(BlueprintCallable)
	void EnableBounce() { BounceEnabled = true; };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Dungeons")
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Dungeons")
	UReplicatedInteractableComponent* ReplicatedInteractable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	float InteractionCountdown = 1.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	int InteractionLimit = 20;

	// #D11.CM - How many interact "points" does the owner of the icecube bestow on interaction
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	int OwnerInteractionContribution = 2;

	// #D11.CM - How many interact "points" do other players bestow on interaction
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	int OtherInteractionContribution = 1;

	// #D11.CM- How heigh to we "bounce" when interacted with.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	float InteractionLaunchHeight = 750;

private:
	void DestroyIcecube();

	void OnPlayerInteraction(AActor* interactor);

	bool RemoveFrozenSolidEffectFromActor(AActor* effected);

	bool BounceEnabled = false;

	UPROPERTY(Replicated)
	int interactCount = 0;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerInteracted OnPlayerInteracted;

	FTimerHandle BounceBlockerTimerHandle;
};