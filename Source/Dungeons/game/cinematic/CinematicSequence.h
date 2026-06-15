#pragma once

#include "CoreMinimal.h"
#include "lovika/tile/TilePlacement.h"
#include "LevelSequenceActor.h"
#include "CineCameraActor.h"
#include "CinematicSequence.generated.h"

class UWorld;
class UClass;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCinematicPlaying);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCinematicStopped);

UCLASS(BlueprintType)
class DUNGEONS_API UCinematicSequence : public UObject {
	GENERATED_BODY()
	
public:
	static UCinematicSequence* Construct(UWorld& world, UClass* sequenceClass, const Placement& placement, const BlockPos& originalPos);

	UCinematicSequence();	

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void stop();

	bool load();
	void play();

	void DelayedPlayCallback();

	void RemoveDelayedPlayTimer();

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool isPlaying() const;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnCinematicPlaying OnCinematicPlaying;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnCinematicStopped OnCinematicStopped;

	UFUNCTION()
	void OnStopped();

	virtual class UWorld* GetWorld() const override;

private:
	ULevelSequencePlayer* player() const;

	UPROPERTY()
	UWorld* mWorld;

	UPROPERTY()
	UClass* mSequenceClass;

	UPROPERTY()
	TWeakObjectPtr<ALevelSequenceActor> mLevelSequencePtr;
		
	Placement mPlacement;
	
	BlockPos mOriginalPos; // This tile's position in Unreal decor maps where the cinematics are made

	class UDefaultLevelSequenceInstanceData* InstanceData = nullptr;

	AActor* mTransformOriginActor = nullptr;

	FTimerHandle mDelayedPlayTimerHandle;

};
