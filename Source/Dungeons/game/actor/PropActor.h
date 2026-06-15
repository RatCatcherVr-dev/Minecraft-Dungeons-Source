#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "util/DataTracker.h"
#include "PropActor.generated.h"

UCLASS()
class DUNGEONS_API APropActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Put oftenly needed data in here (see where it's used) to increase cache locality
	class InstanceData {
	public:
		explicit InstanceData(APropActor*);

		APropActor* const Actor;
		const bool DoPlayerDistanceCheck;
		const float CheckDistance;

		FVector GetActorLocation() const;
		bool IsPlayerVisible() const {
			return PlayerVisible;
		}
		void SetPlayerVisible(bool isVisible);
		void VerifyCachedValues();
		void ResetToActorValues();

		bool operator==(const InstanceData& rhs) const {
			return Actor == rhs.Actor;
		}

	private:
		FVector ActorLocation;
		bool PlayerVisible;
		bool CanMove;
	};

	static DataTracker<InstanceData, false> Instances;

	// Sets default values for this actor's properties
	APropActor();
	APropActor(const class FObjectInitializer& OI);

	void BeginPlay() override;

	void AddDataInstanceTracking();

	void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	void RemoveDataInstanceTracking();

	bool IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const override;

	//reset instance tracked data to the current actor values
	void ResetInstanceData();

	//called when a player enters/leaves visible range check
	void SetPlayerInVisibleRange(bool Visible);

	UFUNCTION(BlueprintNativeEvent)
	void PlayerEnteredVisibleRange();
	virtual void PlayerEnteredVisibleRange_Implementation();

	UFUNCTION(BlueprintNativeEvent)
	void PlayerLeftVisibleRange();
	virtual void PlayerLeftVisibleRange_Implementation();

	UFUNCTION(BlueprintImplementableEvent, Category = "Dungeons")
	void PlayerInVisibleRangeChanged(bool Visible);
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	float mCheckDistance = 3500.0f; //standard (3500.0f) is equivalent to 35 tiles

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	bool mPlayerVisible = true;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool GetInPlayerVisibleRange() const;

	//Do player distance Check
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	bool mDoPlayerDistanceCheck = true;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	bool mCanMove = false;

protected:
	//C++ overridable function that can be used alongside Blueprint events that trigger when a player range chack changes value
	virtual void OnPlayerInVisibleRangeChange(bool isVisible){};

private:
	
	bool IsCinematicCurrentlyPlaying() const;
};

