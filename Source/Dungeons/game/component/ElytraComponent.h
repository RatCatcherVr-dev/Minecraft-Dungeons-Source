#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "PlayerCharacterMovementComponent.h"
#include "Particles/ParticleSystem.h"
#include "game/item/instance/AItemInstance.h"
#include "game/abilities/effects/WorldDamageGameplayEffect.h"
#include "ElytraComponent.generated.h"

UENUM(BlueprintType, meta = (Bitflags))
enum class EElytraAudioState : uint8 
{
	None,
	DiveImpact,
	Launch,
	Land,
	Dash,
	Dive,
	DiveRecovery,
	Glide		
};
ENUM_NAME(EElytraAudioState);

USTRUCT(BlueprintType)
struct DUNGEONS_API FElytraSoundCue {
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundMix* soundMix;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* sound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float fadeInDuration = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float fadeInVolumeLevel = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float fadeInStartTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float fadeOutDuration = 0.25f;
	 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float fadeOutVolumeLevel = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool allowSpatialization = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool onlyPlayLocally = true;

	TWeakObjectPtr<UAudioComponent> audioComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Bitmask, BitmaskEnum = "EElytraAudioState"))
	uint8 stopState = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Bitmask, BitmaskEnum = "EElytraAudioState"))
	uint8 soundMixPopState = 0;
};

UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent), DisplayName = "ElytraComponent")
class DUNGEONS_API UElytraComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()

public:
	UElytraComponent();

public:
	void SetIsFlying(bool flying);
	float GetDistanceToLandMarker() const;
	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	void DoDiveImpact();

	void Equip();
	bool IsEquipped() const { return bIsEquipped; };
	void PerformRollAnimation();

	UFUNCTION(BlueprintCallable)
	void RefreshVisibility(bool IsCharacterVisible);

protected:
	UFUNCTION()
	void OnRep_IsFlying();

	UFUNCTION()
	void OnRep_IsEquipped();

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;
	void BeginPlay() override;

public:
	void TriggerAudioStateChange(EElytraAudioState newState);

private:
	void UpdateLandMarkerLocation();

	void OnLandMarkerTrace(const FTraceHandle& handle, FTraceDatum& data);

	void DealDamageToMob(const class AMobCharacter* mob, const FVector& impactLocation, const float scaledDamage);
	FPushback CreatePushback(const AActor* entity, const FVector& impactLocation) const;
	float CalculateLaunchMagnitude(const AActor* entity, const FVector& impactLocation) const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons|Elytra")
	UStaticMeshComponent* elytraLandComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons|Elytra")
	UParticleSystemComponent* leftStreamParticleSystemComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons|Elytra")
	UParticleSystemComponent* RightStreamParticleSystemComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons|Elytra")
	FRotator ElytraCameraRotation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons|Elytra")
	float ElytraEnableCameraRotationLagSpeed = 2.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons|Elytra")
	float ElytraDisableCameraRotationLagSpeed = 5.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons|Elytra|Dive Impact")
	float DiveImpactScaleMultiplier = 0.0003f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons|Elytra|Dive Impact")
	float DiveImpactPushbackMultiplier = 15.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons|Elytra|Dive Impact")
	float DiveImpactMinimumPushbackStrength = 2.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons|Elytra|Dive Impact")
	float DiveImpactRadius = 350.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons|Elytra|Dive Impact")
	float DiveImpactDamage = 400.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons|Elytra|Dive Impact")
	float DiveImpactUpwardsPushback = 3.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons|Elytra|Dive Impact")
	float DiveImpactMinimumRagdollPushbackStrength = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons|Elytra|Dive Impact")
	float DiveImpactRagdollPushbackMultiplier = 1.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons|Elytra|Sound")
	TMap<EElytraAudioState, FElytraSoundCue> SoundCues;

	UPROPERTY(ReplicatedUsing = OnRep_IsFlying)
	bool bIsFlying = false;

	UPROPERTY(ReplicatedUsing = OnRep_IsEquipped)
	bool bIsEquipped = false;

	bool bPerformedDiveImpact = false;

	EElytraAudioState currentState;
	FRotator cachedCameraRotation;
	FRotator cachedLMPCameraRotation;
	UAnimMontage* launchMontage;
	UParticleSystem* launchParticleSystem;
	UParticleSystem* landParticleSystem;
	UParticleSystem* diveImpactParticleSystem;
	UAnimSequenceBase* elytraWingDashAnimation;
	TOptional<FTraceHandle> traceHandle;
	FTraceDelegate traceDelegate;
};

UCLASS()
class DUNGEONS_API AElytraInstance : public AItemInstance {
	GENERATED_BODY()
public:
	bool CanActivate() const override;

	void Activate(const FPredictionKey& predictionKey) override;
};


UCLASS()
class DUNGEONS_API UElytaDiveImpactDamageGameplayEffect : public UWorldDamageGameplayEffect {
	GENERATED_BODY()
public:
	UElytaDiveImpactDamageGameplayEffect();
};