#pragma once

#include "Components/ActorComponent.h"
#include "InteractableComponent.h"
#include "ReplicatedInteractableComponent.generated.h"

class UInteractableComponent;
class ABaseCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteract);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFail);

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONS_API UReplicatedInteractableComponent : public UInteractableComponent
{
	GENERATED_BODY()
public:
	UReplicatedInteractableComponent();

	void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void Interact(AActor* Instigator = nullptr);

	// #D11.CM - Functionally the same as Interact, but makes sure the call is picked up by the server if you're a client
	UFUNCTION(BlueprintCallable)
	void ClientSafeInteract(ACharacter* InstigatingCharacter);

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>&) const override;

	void initActivatable();

	// This might be temporary. Used for a local C++-binding (currently only in Objectives)
	std::function<bool(UReplicatedInteractableComponent&, const AActor*)> ValidateCallback;
	std::function<void(UReplicatedInteractableComponent&, AActor*)> OnInteractCallback;
	std::function<void(UReplicatedInteractableComponent&, AActor*)> OnFailCallback;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnInteract OnReplicatedInteract;

	UFUNCTION(BlueprintCallable)
	void ServerMakeNonActive();

	void Interacted(ACharacter* instigator) override;
	void SetLastInstigator(ACharacter* instigator);
	ACharacter* GetLastInstigator() { return LastInstigator; };

	virtual void EnableInteraction() override;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnFail OnReplicatedFail;
protected:

	UFUNCTION(NetMulticast, Unreliable)
	void MulicastOnFail();

	UFUNCTION()
	void OnRep_IsActivated();

	UFUNCTION()
	void OnRep_ActivatableInitialized();

	UPROPERTY(Replicated, ReplicatedUsing = OnRep_IsActivated)
	bool bIsActivated;

	UPROPERTY(BlueprintReadOnly, Replicated)
	ACharacter* LastInstigator;

	UPROPERTY(Replicated, ReplicatedUsing = OnRep_ActivatableInitialized)
	bool bActivatableInitialized;
};
