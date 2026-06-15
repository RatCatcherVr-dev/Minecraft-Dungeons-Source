#pragma once

#include "CoreMinimal.h"
#include "game/item/instance/AItemInstance.h"
#include "TotemBaseItemInstance.generated.h"

USTRUCT(BlueprintType)
struct DUNGEONS_API FVitalityThreshold {
	GENERATED_BODY()

	FVitalityThreshold();
	FVitalityThreshold(float threshold, FString name);

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	float Threshold;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FString Name;
};

UCLASS()
class DUNGEONS_API ATotemBaseActor : public AActor {
	GENERATED_BODY()
public:
	ATotemBaseActor();

	virtual void TryStartDestroyCountdown();

	void BeginPlay() override;
protected:
	void TryNotifyWeakenedVitality();

	virtual void OnDestroyCountdownStarted_Internal();

	UFUNCTION(BlueprintCallable)
	virtual float GetHealthFraction() { return 1.0f; }

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float DestroyTime = 2.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	TArray<FVitalityThreshold> WeakenedVitalityThresholds; 

	UFUNCTION(BlueprintImplementableEvent)
	void OnWeakenedVitalityReached(const FVitalityThreshold& vitality);

	UFUNCTION(BlueprintImplementableEvent)
	void OnDestroyCountdownStarted();

	UFUNCTION(BlueprintImplementableEvent)
	void OnPlayerEnter(APlayerCharacter* player, bool local);
	UFUNCTION(BlueprintImplementableEvent)
	void OnPlayerExit(APlayerCharacter* player, bool local);
private:
	bool IsDestroying() const;

	void SwitchNextVitalityThreshold();

	void DestroyTotem();

	TOptional<FVitalityThreshold> CurrentVitalityThreshold;

	bool bDidPassWeakenedVitality = false;

	FTimerHandle DestructionTimerHandle;

	UFUNCTION()
	void OnBeginSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};


UCLASS()
class DUNGEONS_API ATotemBaseItemInstance : public AItemInstance
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ATotemBaseActor> TotemClass;

	void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	virtual void PreTotemBeginPlay(ATotemBaseActor* totemActor) {}

	void Activate(const FPredictionKey& predictionKey) override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void OnSetupWithValidOwner() override;
private:
	void SpawnTotem();

	void OnPlayerDeath();

	UPROPERTY(Replicated)
	TWeakObjectPtr<ATotemBaseActor> Totem;
};
