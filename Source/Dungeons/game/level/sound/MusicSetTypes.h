#pragma once

#include "CoreMinimal.h"
#include "Engine/StreamableManager.h"
#include <Sound/SoundCue.h>
#include "MusicSetTypes.generated.h"

class USoundCue;

UENUM(BlueprintType)
enum class EMusicSequenceState : uint8 {
	Intro = 0,
	Main,
	Outro
};

UENUM(BlueprintType)
enum class EDungeonsMusicPriority : uint8 {
	BackgroundMusic = 0, // level background, always play after all else. (outro?)
	SubDungeonsMusic, // subdungeon music, to allow main level background music to resume on time
	LevelIntroMusic, // level intro (needs to take precedence over background music)
	BossEncounterMusic, // boss fight + arena battle
	SideQuestMusic, // optional objectives (ChallengesSystem)
	EventMusic, // event mobs 
	UIMusic, // new tier for out of air/bubbles special music for Oceans
	Top
};
ENUM_NAME(EDungeonsMusicPriority);


typedef TMap<EMusicSequenceState, USoundCue*> DungeonsMusicGroup;

USTRUCT(BlueprintType)
struct DUNGEONS_API FDungeonsMusicSet {
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EMusicSequenceState, USoundCue*> MusicSet;

	bool Contains(EMusicSequenceState state) const { return MusicSet.Contains(state); }

	// make .hpp, also implement operator []
	USoundCue* at(EMusicSequenceState state) const {
		return MusicSet[state];
	}
};

// prefer DungeonsMusicGroup/DungeonsMusicSet
USTRUCT(BlueprintType)
struct DUNGEONS_API FMusicSet {
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<USoundCue> track;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<USoundCue> introduction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<USoundCue> exit;

	TSharedPtr< FStreamableHandle > StreamingHandle;

	bool AsyncLoadSet();
	bool SyncLoadSet();

	USoundCue* GetMusicTrack(EMusicSequenceState sequenceState);

	bool operator== (const FMusicSet& other) const {
		return track == other.track && introduction == other.introduction && exit == other.exit;
	}
	

#if WITH_EDITOR
	void CheckStreamingPtrs();
#endif

};

USTRUCT(BlueprintType)
struct FLevelAudio
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<USoundCue> LevelVOIntro;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<USoundCue> LevelIntroMusic;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<USoundCue> LevelVOOutro;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<USoundCue> LevelBackgroundMusic;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<USoundCue> EnvironmentSFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FMusicSet LevelSideQuestBattleMusic;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FMusicSet LevelEncounterMusic;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FMusicSet LevelBossMusic;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UReverbEffect* MainReverb;

	TSharedPtr< FStreamableHandle > StreamingHandle;

	bool AsyncLoadLevelAudio();
	bool SyncLoadLevelAudio();

#if WITH_EDITOR
	void CheckStreamingPtrs();
#endif

};

USTRUCT(BlueprintType)
struct DUNGEONS_API FDungeonsMusicTrack {
	GENERATED_USTRUCT_BODY()
public:
	FDungeonsMusicTrack() = default;
	FDungeonsMusicTrack(EDungeonsMusicPriority slotType, USoundCue* track, float fadeIn, float fadeOut, bool cacheWhenPaused, float elapsedSeconds = 0.0f)
		: SlotType(slotType)
		, Track(MakeWeakObjectPtr(track))
		, ElapsedSeconds(elapsedSeconds)
		, FadeIn(fadeIn)
		, FadeOut(fadeOut)
		, CacheWhenPaused(cacheWhenPaused)
	{}
	EDungeonsMusicPriority SlotType;
private:
	UPROPERTY()
	TWeakObjectPtr<USoundCue> Track;
public:

	float FadeIn = 0.0f;
	float FadeOut = 0.0f;

	float ElapsedSeconds = 0.0f;
	bool CacheWhenPaused = true;

	USoundCue *GetTrack() const { return Track.Get(); }
	bool IsValid() const { return Track.IsValid(); }

	bool operator! () const {
		return !IsValid();
	}

	bool operator== (const FDungeonsMusicTrack& other) const {
		return SlotType == other.SlotType && Track == other.Track;
	}

	bool operator!= (const FDungeonsMusicTrack& other) const {
		return !(*this == other);
	}

	bool operator< (const FDungeonsMusicTrack& other) const {
		return static_cast<uint8>(SlotType) < static_cast<uint8>(other.SlotType); // need typecast?
	}

};

