// © 2020 Mojang Synergies AB. TM Microsoft Corporation.

#pragma once

#include "CoreMinimal.h"
#include "repgraph/Actor_RepSpatializeStatic.h"
#include "save/CharacterSaveData.h"
#include "game/component/EyeOfEnderDropComponent.h"
#include "StrongholdFrame.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSubDungeonUnlocked, bool, isFirstUnlock);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEyesPlacedUpdated, int, lockedEyesLeft);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUnlockFailed);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnNoEffectEndPortalInteraction, const StrongholdProgressSaveData*);

/**
 *
 */
UCLASS(Abstract)
class DUNGEONS_API AStrongholdFrame : public APropActor_RepAlways
{
	GENERATED_BODY()
public:
	AStrongholdFrame();

	void BeginPlay() override;

	UPROPERTY(BlueprintAssignable)
	FOnSubDungeonUnlocked OnDeepwoodBrookUnlocked;
	UPROPERTY(BlueprintAssignable)
	FOnSubDungeonUnlocked OnOldTownHallUnlocked;
	UPROPERTY(BlueprintAssignable)
	FOnSubDungeonUnlocked OnSunkenWatchtowerUnlocked;
	UPROPERTY(BlueprintAssignable)
	FOnSubDungeonUnlocked OnTheUndercroftUnlocked;
	UPROPERTY(BlueprintAssignable)
	FOnSubDungeonUnlocked OnTheTrialOfTheNamelessUnlocked;
	UPROPERTY(BlueprintAssignable)
	FOnSubDungeonUnlocked OnHighblockHideawayUnlocked;
	UPROPERTY(BlueprintAssignable)
	FOnSubDungeonUnlocked OnFrameFilled;
	UPROPERTY(BlueprintAssignable)
	FOnUnlockFailed OnUnlockFailed;
	UPROPERTY(BlueprintAssignable)
	FOnEyesPlacedUpdated OnEyesPlacedUpdated;

	FOnNoEffectEndPortalInteraction OnNoEffectEndPortalInteraction;

	UFUNCTION()
	void InteractionBegin();

	UFUNCTION(NetMulticast, Reliable)
	void PlaceEyesInFrame(ACharacter* interactor);

	UFUNCTION()
	void TrySpendEyes(const TArray<EEyeOfEnderType>& eyeTypes, AActor* spender);

	UFUNCTION(NetMulticast, Reliable)
	void SpendEyesSuccess(const TArray<EEyeOfEnderType>& eyesSpent);

	UFUNCTION()
	void SpendEyesFailure();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsFirstPortalUse();

	// #D11.CM - Returns how many eyes are missing from this instance of the Stronghold Frame.
	UFUNCTION(BlueprintCallable, BlueprintPure)
	int EyeOfEnderRemaining() const { return TOTAL_EYES_OF_ENDER - EyesInFrameInstance; };

	// #D11.CM - Returns how many eyes have been placed in this instance of the Stronghold Frame.
	UFUNCTION(BlueprintCallable, BlueprintPure)
	int GetEyesInFrameInstance() const { return EyesInFrameInstance; };

	UFUNCTION()
	void OnRep_EyesInFrameInstance();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void UnlockCompletedMissions();
	
	bool IsFrameFilled();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	USceneComponent* DefaultSceneRootInternal;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UReplicatedInteractableComponent* Interactable;

protected:
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_EyesInFrameInstance)
	int EyesInFrameInstance = -1;

private:
	FOnSubDungeonUnlocked OnDummyUnlock;
	const FOnSubDungeonUnlocked& GetDelegateForEyeOfEnderType(EEyeOfEnderType) const;

	UPROPERTY(Replicated)
	ACharacter* LastInteractor;
};
