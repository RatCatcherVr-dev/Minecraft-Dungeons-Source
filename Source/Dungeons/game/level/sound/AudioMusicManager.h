#pragma once

#include "CoreMinimal.h"
#include "AudioManagerBase.h"
#include "game/component/MusicOverrideComponent.h"
#include "game/level/sound/MusicSetTypes.h"
#include "game/level/GameTile.h"
#include "game/objective/EventTypes.h"

#include "AudioMusicManager.generated.h"

USTRUCT()
struct FMobMusicItem {
	GENERATED_USTRUCT_BODY()
public:
	FMobMusicItem() = default;
	FMobMusicItem(const class AMobCharacter* mob, float distance2);

	UPROPERTY()
	const class AMobCharacter* mob;
	
	float distance2; // distance to camera look-at-point squared

	inline bool operator==(const FMobMusicItem& other) const {
		return distance2 == other.distance2;
	}
	inline bool operator< (const FMobMusicItem& other) const {
		return distance2 < other.distance2;
	}

};

enum class DungeonsFadeType : uint8 {
	FadeIn,
	FadeOut
};

USTRUCT()
struct FDungeonsSoundFade {
	GENERATED_USTRUCT_BODY()
public:
	FDungeonsSoundFade() = default;
	FDungeonsSoundFade(DungeonsFadeType fadeType, float currentVolume, float currentFadeTime, float targetVolume, UAudioComponent* audioComponent);

	DungeonsFadeType mFadeType;
	float mCurrentVolume;
	float mCurrentFadeTime;
	float mTargetVolume;
	
	float mRateOfChange;

	UPROPERTY()
	UAudioComponent* mAudioComponent;

	FName mTrackName;

	bool isFading() const;
	bool stale() const { return !isFading(); }

	void update(float DeltaSeconds);
	bool operator== (const FDungeonsSoundFade& other) const {
		return other.mAudioComponent->Sound == mAudioComponent->Sound;
	}
};

enum class ELevelNames : uint8;

UCLASS(BlueprintType, Blueprintable)
class DUNGEONS_API AAudioMusicManager : public AAudioManagerBase {
	GENERATED_BODY()
	
public:
	AAudioMusicManager(const FObjectInitializer& ObjectInitializer);
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaSeconds) override;

public:
	/// EventMob callers
	void OnEventMobEngaged(const class AMobCharacter* mob);
	void OnEventMobUpdate(const class AMobCharacter* mob);
	void OnEventMobChanged(const class AMobCharacter* newMob);
	void OnEventMobDisengaged(const class AMobCharacter* mob);
	class UAudioCollectionComponent* collectionComponent() const;

	UAudioComponent* PlayLoadingScreenSound();
	void StopLoadingScreenSound();
	
	/* Current level context, to allow reuse of Tick/update in AudioMusicManager */
	ELevelNames GetCurrentLevelContext() const;

	/* Pause current music, for e.g. silent stretch before event music */
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void Pause(float fadeOut = 0.0f);

	//Persistent Audio Components for keeping track of the memory for playback
	UPROPERTY(BlueprintReadWrite)
	UAudioComponent* LoadingScreenAudioComponent;

	// convenience getter
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	FLevelAudio GetCurrentLevelAudioCollection() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	USoundCue* GetEncounterMusicTrack(EMusicSequenceState sequenceState, EEventType eventType) const;
	const float MusicOverrideTriggerRadius = 2100;
	const float MusicOverrideResetRadius = 3100;

protected:
	void OnFadeComplete(FDungeonsSoundFade audioComponentWrapper);
	void SoundFade(DungeonsFadeType fadeType, UAudioComponent* audioComponent, float fadeTime, float targetVolume);

	virtual void CleanupAudioComponents() override;
#if !UE_BUILD_SHIPPING
	virtual void PrintState(TSet<UAudioComponent*> handled) const override;
#endif
private:
	float MusicOverrideTriggerRadius2;
	float MusicOverrideResetRadius2;

	virtual void UpdateAndPlayInternal() override;
	virtual void SuspendCurrentTrack(float fadeOut) override;

	void ProcessBackgroundMusicOverride(AActor* target);
	void ProcessMobMusicOverrides(AActor* musicTarget);
	void OnTileChanged(game::TilePtr newTile);
	void OnLevelContextChanged(ELevelNames newLevelName);
	void StartEventMobMusic(const UMusicOverrideComponent* mobMusicOverride, EMusicSequenceState state);
	void ResetEventMobMusicState();

	UPROPERTY()
	mutable class UAudioCollectionComponent* mAudioCollectionComponent = nullptr;

	UPROPERTY()
	TArray<FDungeonsSoundFade> ActiveFades;

	UPROPERTY()
	TArray<FMobMusicItem> MusicOverrideSet;

	UPROPERTY()
	const AMobCharacter* mLastEngagedMob;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	TSubclassOf<class UAudioCollectionComponent> AudioCollectionBP;

	// keep a reasonable copy of the event mob tracks;	
	USoundMix* mLastTriggeredEventMobMix;	
	EMusicSequenceState mLastMusicSequenceState;
	ELevelNames mLastLevelName;
	game::TilePtr mCurrentTile = nullptr;

	TWeakObjectPtr<UAudioComponent> CurrentEventMobAudio;

};
