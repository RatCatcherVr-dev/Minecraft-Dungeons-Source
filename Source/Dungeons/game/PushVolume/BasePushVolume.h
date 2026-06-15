#pragma once
#include "Dungeons.h"
#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "Components/ArrowComponent.h"
#include "game/component/PushVolumeReceiverComponent.h"
#include "repgraph/Actor_RepSpatializeStatic.h"

#include "BasePushVolume.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPushVolumeActivated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPushVolumeDeactivated);

UCLASS()
class DUNGEONS_API ABasePushVolume : public APropActor_RepSpatializeDynamic{
	GENERATED_BODY()
public:
	ABasePushVolume();

	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable)
	void SetPushVolumeExtent(FVector newExtent);

	UFUNCTION(BlueprintCallable)
	void SetPushVolumeActive(bool active);

	UFUNCTION(BlueprintCallable)
	virtual void SetPushVolumeTimerActive(bool active);

	UFUNCTION(BlueprintCallable)
	bool IsPushVolumeActive() { return VolumeIsEnabled; }

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Dungeons")
	UBoxComponent* AffectorArea;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Dungeons")
	UArrowComponent* AffectorDirection;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dungeons")
	float MovementSpeed = 300;

	UFUNCTION(BlueprintCallable)
	virtual FVector GetForce() { return AffectorDirection->GetForwardVector() * MovementSpeed; };

	UPROPERTY(BlueprintAssignable)
	FOnPushVolumeActivated OnPushVolumeActivated;

	UPROPERTY(BlueprintAssignable)
	FOnPushVolumeDeactivated OnPushVolumeDeactivated;

protected:
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag ImmunityTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool VolumeIsEnabled = true;

	UPROPERTY(BlueprintReadWrite, ReplicatedUsing = OnRep_VolumeIsEnabled, Category = "Dungeons")
	bool ReplicatedIsEnabled = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	bool bIsTimeActivated = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons", meta=(EditCondition = "bIsTimeActivated"))
	float TimeBetweenOnOff = 4.0f;

	FTimerHandle ToggleTimerHandle;

	void OnActorImmunityChanged(const FGameplayTag tag, const int32 tagCount, UPushVolumeReactiveComponent* recevier);
	
	UFUNCTION()
	void OnWindAreaBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void OnWindAreaEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual void AddInfluence(UPushVolumeReactiveComponent* receiverToAdd);
	virtual void RemoveInfluence(UPushVolumeReactiveComponent* ReceiverToRemove);

	virtual void OnInfluenceSuccessfullyAddedToActor(UPushVolumeReactiveComponent* reciever) {};
	virtual void OnInfluenceSuccessfullyRemovedFromActor(UPushVolumeReactiveComponent* reciever) {};

	void ToggleActive();

	virtual void OnPlayerInVisibleRangeChange(bool isVisible) override;

	TArray<FPushVolumeInfluenceInstance> ActiveInfluences;

private:

	UFUNCTION()
	void OnRep_VolumeIsEnabled();
};