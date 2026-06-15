

#pragma once

#include "CoreMinimal.h"
#include "game/actor/item/InterruptableItemInstance.h"
#include "TogglableItemInstance.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONS_API ATogglableItemInstance : public AInterruptableItemInstance
{
	GENERATED_BODY()

	UPROPERTY(ReplicatedUsing=OnRep_BackingToggle)
	bool BackingToggle;
	
	bool Toggled = false;

	//If the server is interrupted, the client 
	UFUNCTION(Client, Reliable, WithValidation)
	void ClientToggle(bool Enabled);

	//If the client interrupts, we have to tell the server to Change toggle
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerToggle(FPredictionKey predictionKey, bool Enabled);

	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	//This function handles visualization for all non owning clients.
	UFUNCTION()
	void OnRep_BackingToggle();

	void StartAnimation() const;
	void StopAnimation() const;

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

	void HandleItemActorSpawn() override;
public:
	ATogglableItemInstance();
	
	bool IsBusy() const override;
	
	void Activate(const FPredictionKey& predictionKey) override;
	
	virtual void Toggle(const FPredictionKey& predictionKey, bool Enabled);

	//This will only be called on owning client, i.e an interrupt caused by user input
	void InterruptLocal();
	
	//This will only happen on server
	void InterruptServer();
	
	//This will happen on BOTH client and Server
	void Interrupt() override;

	bool CanActivate() const override;
protected:

	void ForceActivate() override;
	
	bool bCanBeInterruptedWithUserAction = true;
	
	bool IsToggled() const { return Toggled; }
};
