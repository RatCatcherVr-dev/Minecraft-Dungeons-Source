#pragma once

#include "Components/ActorComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "game/component/HealthComponent.h"
#include "game/component/MissionProgressComponent.h"
#include "RagdollOnDeathComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnRagdollChanged);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DUNGEONS_API URagdollOnDeathComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URagdollOnDeathComponent();

	void BeginPlay() override;

	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
		
	void Ragdoll();		// Die
	UFUNCTION(BlueprintCallable, Category = "Dungeons|Ragdoll")
	void UnRagdoll();	// Undie
	
	bool IsRagdolling() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Ragdoll")
	void ApplyPhysicsBlendWithRadialImpulse(const FName bone, const FVector impactVector, float force);

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Ragdoll")
	void LaunchOrStoreRagdollImpulse(const FVector& impulse);


	void DissolveSkin(float duration);
	void UnDissolveSkin();

	void Desaturate(float duration);
	void Saturate();

	bool TriggersOnDeath() const { return TriggerOnDeath; }

	FOnRagdollChanged OnRagdollChanged;

protected:
	const FName PhysicsBlendRootBoneName = FName("J_Spine");
	const bool PhysicsBlendIncludeRootBone = true;
	


	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	bool TriggerOnDeath = true;
	const float PhysicsBlendTimeSeconds = 0.5f;

	UPROPERTY(EditAnywhere)
	float PhysicsBlendMultiplier = 1.f;
	bool IsDoingRagdolling = false;

	void LaunchRagdoll(const FVector& impactVector) const;

private:
	void HideParticles();
	void ShowParticles();

	UCapsuleComponent* CapsuleComponent() const;
	USkeletalMeshComponent* MeshComponent() const;
	UCharacterMovementComponent* CharacterMovementComponent() const;
	class UMassComponent* MassComponent() const;
	UHealthComponent* HealthComponent() const;

	float PhysicsBlendStartTimeSeconds = 0.f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float dissolveDuration = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float dissolveDelay = 4.f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	bool ragdollEnabled = true;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	TMap<EDLCName, float> DLCDissolveMultiplier;

	FTimerHandle visibilityHandle;
	FTimerHandle dissolveHandle;
	TArray<UActorComponent*> particleSystemsToActivate;
	TOptional<FVector> StoredRagdollImpulse;

	ECollisionEnabled::Type cachedMeshCollisionType = ECollisionEnabled::QueryOnly;
	FCollisionResponseContainer cachedMeshCollisionResponse;

	bool underwater = false;
	// affects how far/fast the mob will float
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float linearForceMultiplier = 0.16f;
	// affects how fast the mob will spin
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float angularForceMultiplier = 0.0064f;
};
