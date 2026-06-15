#pragma once

#include "CoreMinimal.h"
#include "game/item/instance/AItemInstance.h"
#include "game/actor/BeeNest.h"
#include "BeeNestItemInstance.generated.h"

UCLASS()
class DUNGEONS_API ABeeNestItemInstance : public AItemInstance
{
	GENERATED_BODY()

public:
	ABeeNestItemInstance();

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ABeeNest> BeesNestClass;

	void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	void Activate(const FPredictionKey& predictionKey) override;

	bool IsBusy() const override;

	float GetStats(EItemStats) const override;

	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "0.1", ClampMax = "5"))
	float SpawnDelayRangeMin = 0.5f;

	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "0.5", ClampMax = "10"))
	float SpawnDelayRangeMax = 2.f;

	UPROPERTY(EditDefaultsOnly)
	int MaxNumBees = 3;

	bool CanActivate() const override;

private:
	void OnBeenestExpired();
	void PredictionCaughtUp();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_BeeNest();

	UPROPERTY(ReplicatedUsing=OnRep_BeeNest, Transient)
	ABeeNest* BeeNest = nullptr;

	bool OutStandingPrediction = false;

	TArray<TWeakObjectPtr<AMobCharacter>> SummonCarryOver;
};
