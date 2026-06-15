#pragma once

#include "StorableItem.h"
#include "EmeraldStorableItem.generated.h"

class UAudioComponent;

UCLASS()
class DUNGEONS_API AEmeraldStorableItem : public AStorableItem {
	GENERATED_BODY()
public:
	AEmeraldStorableItem(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	void BeginPlay() override;
	void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	void Tick(float DeltaSeconds) override;

	virtual void ReInitialiseItem()override;

	UPROPERTY(EditAnywhere, Category = "Dungeons")
	class UInstancedStaticMeshComponent* pMainMeshes;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	class UAudioComponent* EmeraldBurst;
	

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	class UParticleSystemComponent* ParticleSystemShimmer;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	class UWalkPickupComponent* WalkPickup;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	class UParticleSystem*  InitParticleSystemTemplate;

	virtual void OnPlayerInVisibleRangeChange(bool isVisible) override;
	

protected:

	virtual void OnLandedInternal() override;

	float RotationTime;

	TArray< FVector > SubMeshOffsets;
	
};
