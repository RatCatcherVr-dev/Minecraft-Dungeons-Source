#pragma once
#include "Dungeons.h"
#include "CoreMinimal.h"
#include "game/util/SimpleMovementComponent.h"
#include "GameplayTagContainer.h"
#include "repgraph/Actor_RepSpatializeStatic.h"
#include "AbilitySystemComponent.h"

#include "PushVolumeReceiverComponent.generated.h"

UENUM(BlueprintType)
enum class EPushVolumeType : uint8 {
	None,
	Wind,
	Current
};

USTRUCT(BlueprintType)
struct FPushVolumeInfluenceInstance {
	GENERATED_USTRUCT_BODY()

		FPushVolumeInfluenceInstance(AActor* pushVolume, UPrimitiveComponent* influencedComponent, FVector vector, EPushVolumeType type = EPushVolumeType::None) {
		PushVolumeActor = pushVolume;
		InfluencedComponent = influencedComponent;
		PushVolumeVelocity = vector;
		PushVolumeType = type;
	}

	FPushVolumeInfluenceInstance() {
		PushVolumeActor = nullptr;
		InfluencedComponent = nullptr;
		PushVolumeVelocity = FVector::ZeroVector;
	}

	bool operator ==(const FPushVolumeInfluenceInstance& other) const {
		bool result = true;
		result &= PushVolumeActor == other.PushVolumeActor;
		result &= InfluencedComponent == other.InfluencedComponent;
		result &= PushVolumeType == other.PushVolumeType;

		return result;
	}

	UPROPERTY()
		AActor* PushVolumeActor;

	UPROPERTY()
		UPrimitiveComponent* InfluencedComponent;


	UPROPERTY()
		FVector PushVolumeVelocity;

	EPushVolumeType PushVolumeType = EPushVolumeType::None;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReceiverActivated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReceiverDeactivated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPushForceUpdated, FVector, CurrentPushForce);

UCLASS(Blueprintable)
class DUNGEONS_API UPushVolumeReactiveComponent : public UBoxComponent {
	GENERATED_BODY()
public:
	UPushVolumeReactiveComponent();

	void BeginPlay() override;

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override { Super::GetLifetimeReplicatedProps(OutLifetimeProps); };

	virtual void ProcessForces();

	bool HasInstanceOfInfluence(FPushVolumeInfluenceInstance pushVolume);

	virtual bool ApplyPushVolumeInfluence(FPushVolumeInfluenceInstance pushVolume);

	virtual bool RemovePushVolumeInfluence(FPushVolumeInfluenceInstance pushVolume);

	virtual void UpdateCumulativePushForce();

	void SetVolumeOverlapsEnabled(bool bEnable);

	UFUNCTION(BlueprintCallable)
	virtual bool CanVolumeInstanceBeAppliedToActor(FPushVolumeInfluenceInstance pushVolume) { return !bRestrictToPushVolumeType || pushVolume.PushVolumeType == RestrictedPushVolumeType; };

	UFUNCTION(BlueprintCallable)
	bool IsBeingEffectedByPushVolumes() { return CurrentPushVolumes.Num() > 0; };

	UFUNCTION(BlueprintCallable)
	int NumberOfPushVolumeInfluences() { return CurrentPushVolumes.Num(); };

	UFUNCTION(BlueprintCallable)
	FVector GetCumulativePushForce() { return CachedCumulativePushForce; };

	UFUNCTION(BlueprintCallable)
	FVector GetResistanceIgnoredCumulativePushForce() { return ResistanceIgnoredCachedCumulativePushForce; };

	UFUNCTION(BlueprintCallable)
	FVector GetCumulativePushDirection();

	UFUNCTION(BlueprintCallable)
	FVector GetResistanceIgnoredCumulativePushDirection();

	UPROPERTY(BlueprintAssignable)
	FOnReceiverActivated OnReceiverActivated;
	UPROPERTY(BlueprintAssignable)
	FOnReceiverDeactivated OnReceiverDeactivated;
	UPROPERTY(BlueprintAssignable)
	FOnPushForceUpdated OnPushForceUpdated;

protected:
	void OnResistancesChanged(FGameplayTag tag, int32 count);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Dungeons")
	bool bRestrictToPushVolumeType = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, meta = (EditCondition = "bRestrictToPushVolumeType"), Category = "Dungeons")
	EPushVolumeType RestrictedPushVolumeType;

	TArray<FPushVolumeInfluenceInstance> CurrentPushVolumes;
	FVector CachedCumulativePushForce;
	FVector ResistanceIgnoredCachedCumulativePushForce;

	UAbilitySystemComponent* CachedAbilitySystem;
};
