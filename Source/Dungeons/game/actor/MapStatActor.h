#pragma once


#include <CoreMinimal.h>
#include <UnrealNetwork.h>
#include "MapStatActor.generated.h"


UENUM(BlueprintType)
enum class EMapTrackingTypes : uint8 {
	ChestsOpened = 0,
	ChestsSpawned,
	SecretsFound,
	SecretsSpawned,
	Size
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMapTrackingTypeCountChanged, EMapTrackingTypes, TrackingType);

USTRUCT(BlueprintType)
struct FTrackedCount {
	GENERATED_BODY()

	UPROPERTY()
	EMapTrackingTypes TrackingType;

	UPROPERTY()
	int Count;
};

UCLASS()
class DUNGEONS_API AMapStatActor : public AActor {
	GENERATED_BODY()

public:	

	AMapStatActor(const FObjectInitializer& objectInitializer);

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	void TrackScalar(EMapTrackingTypes TrackingType, int Count);

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	int GetTrackingTypeCount(EMapTrackingTypes TrackingType) const;


	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnMapTrackingTypeCountChanged OnMapTrackingTypeCountChanged;

private:

	void SetTrackingTypeCount(EMapTrackingTypes TrackingType, int Count);
	
	

	UFUNCTION()
	void OnRep_TrackedCounts();

	TMap<EMapTrackingTypes,int> LastTrackedCount;

	void OnTrackingTypeCountChanged(EMapTrackingTypes TrackingType) const;

	UPROPERTY(ReplicatedUsing = OnRep_TrackedCounts)
	TArray<FTrackedCount> TrackedCounts;
};