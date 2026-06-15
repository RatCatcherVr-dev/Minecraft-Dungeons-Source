#pragma once

#include "CoreMinimal.h"
#include "Tickable.h"
#include "Engine/StreamableManager.h"
#include "MusicSetTypes.h"

#include "AudioManagerBase.generated.h"

namespace audiomanager {
	UAudioComponent* createSound(UWorld* world, USoundBase* sound, bool PersistMusicOnLevelLoad, bool activate = false);
}

enum class EEventType : uint8;

UCLASS(BlueprintType, Blueprintable)
class DUNGEONS_API AAudioManagerBase : public AInfo {
	GENERATED_BODY()
public:
	AAudioManagerBase(const FObjectInitializer& ObjectInitializer);
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
protected:

	virtual void Tick(float DeltaSeconds) override;
	virtual void CleanupAudioComponents();

public:
	virtual void SetupAudioComponents();
	void ToggleShowDebugOutput() { mPrintDebug = !mPrintDebug; }
	virtual void ClearAllTracks();

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	class USoundCue* EmptySoundCue;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons|AudioComponent")
	bool PersistMusicOnLevelLoad = false;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons|AudioComponent")
	bool mAutoActivateNew = false;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float FadeInFromSilent = 0.f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons|AudioComponent")
	float mVolumeAtSilent = 0.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons|AudioComponent")
	bool mDoPause = true;

	// soft ptr ref of push music track:
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void PushMusicTrackSoft(TSoftObjectPtr<USoundCue> Sound, EDungeonsMusicPriority Priority, float CrossfadeIn = 1.0f, float CrossfadeOut = 1.0f, bool AllowResume = true);

	// music slots queue:	
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void PushMusicTrack(USoundCue* SoundCue, EDungeonsMusicPriority Priority, float CrossfadeIn = 1.0f, float CrossfadeOut = 1.0f, bool AllowResume = true);

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void Pop(EDungeonsMusicPriority slot, float CrossfadeIn = 1.0f, float CrossfadeOut = 1.0f, bool DestroyResume = false);

	bool IsPlaying(EDungeonsMusicPriority slot) const;
	void ForceResetResumableTrack(EDungeonsMusicPriority slot);

	// Debug tools:
	void TriggerPlayActiveTrack();
	void TriggerPlayAllTracks();

private:
	void UpdateAndPlay();
	virtual void UpdateAndPlayInternal();
	virtual void SuspendCurrentTrack(float fadeOut);

	bool mPrintDebug = false; 

	float mBackgroundMusicElapsedSeconds;
	float mLevelBackgroundMusicDuration = 0.0f;
	float mLastPoppedFadeout = 0.0f;

	int32 mActiveAudioCompIndex = 0;

	TMap<FSoftObjectPath, TSharedPtr< FStreamableHandle > > TrackStreamingCollection;
	
	bool AsyncLoadTrack(TSoftObjectPtr<USoundCue> Sound);
	bool SyncLoadTrack(TSoftObjectPtr<USoundCue> Sound);

protected:
	bool InitOK() const;
	void EndMusicTrack(UAudioComponent* audioComponent);
	int ActiveAudioCompIndex() const { return mActiveAudioCompIndex; }
	
	FString mDisplayName;
	FDungeonsMusicTrack mCurrent;
	FDungeonsMusicTrack mPrevious;

	UAudioComponent* ActiveAudioComponent() const;
	UPROPERTY()
	TArray<FDungeonsMusicTrack> mMusicQueue;

	UPROPERTY()
	TMap<int32, UAudioComponent*> AudioTracks;

	// keep paused tracks; streaming tracks cannot resume but must restart on Play
	UPROPERTY()
	TMap<FName, UAudioComponent*> PausedSet;

	bool printDebug() const { return mPrintDebug; }
#if !UE_BUILD_SHIPPING
	virtual void PrintState(TSet<UAudioComponent*> handled) const;
	float mTrackFadeDebug = 0.f;
	float debugFrequency = 0.0f;
#endif

};
