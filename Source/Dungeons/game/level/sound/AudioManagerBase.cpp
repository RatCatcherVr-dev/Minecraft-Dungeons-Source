 #include "Dungeons.h"
#include "AudioManagerBase.h"
#include "game/levels.h"

#include "DungeonsGameInstance.h"
#include "DungeonsGameState.h"
#include "DungeonsAudioUtils.h"

#include "AudioCollectionComponent.h"

#include "Engine/Public/AudioDevice.h"
#include "Engine/Classes/Kismet/GameplayStatics.h"
#include "Engine/Classes/Sound/SoundCue.h"
#include "Engine/Classes/Sound/SoundNodeWavePlayer.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"

#include "game/objective/EventTypes.h"


AAudioManagerBase::AAudioManagerBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) 
	, mBackgroundMusicElapsedSeconds(0.0f)
	, mCurrent(FDungeonsMusicTrack())
	, mDisplayName("AudioManager")
{}

void AAudioManagerBase::BeginPlay() {
	Super::BeginPlay();
	SetupAudioComponents();

}

bool AAudioManagerBase::InitOK() const {
	FString track1dbg = AudioTracks.Num() > 0 ? (AudioTracks[0]->IsValidLowLevel() ? TEXT("Y") : TEXT("N")) : TEXT("UNSET");
	FString track2dbg = AudioTracks.Num() > 1 ? (AudioTracks[1]->IsValidLowLevel() ? TEXT("Y") : TEXT("N")) : TEXT("UNSET");
	UE_LOG(LogDungeonsAudio, Verbose, TEXT("Track1 OK: %s, Track2 OK: %s"), *track1dbg, *track2dbg);
	return (AudioTracks.Num()>=2 && AudioTracks[0]->IsValidLowLevel() && AudioTracks[1]->IsValidLowLevel());
}

namespace audiomanager {
	UAudioComponent* createSound(UWorld* world, USoundBase* sound, bool PersistMusicOnLevelLoad, bool activate) {
		auto* audioComponent = UGameplayStatics::CreateSound2D(world, sound, 1.0f, 1.0f, 0.0f, nullptr, PersistMusicOnLevelLoad, false);
		if (audioComponent) {
			audioComponent->bAutoActivate = activate;
		}
		return audioComponent;

	}
}

void AAudioManagerBase::CleanupAudioComponents() {
	for (auto item : AudioTracks) {
		EndMusicTrack(item.Value);
	}

	AudioTracks.Empty();
}

void AAudioManagerBase::SetupAudioComponents()
{
	CleanupAudioComponents();

	if (EmptySoundCue)
	{
		if (auto sound = audiomanager::createSound(GetWorld(), EmptySoundCue, PersistMusicOnLevelLoad)) {
			AudioTracks.Emplace(0, sound);
		}
		if (auto sound = audiomanager::createSound(GetWorld(), EmptySoundCue, PersistMusicOnLevelLoad)) {
			AudioTracks.Emplace(1, sound);
		}

	}
	else
	{
		UE_LOG(LogDungeonsAudio, Warning, TEXT("EmptySoundCue is set to null reference on BP_AudioMusicManager, this breaks audio playback!"));
	}

	SetActorTickEnabled(AudioTracks.Num() > 0);
}

void AAudioManagerBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (mMusicQueue.Num() > 0) {
		if (mCurrent != mMusicQueue.Top())
		{
			if (mCurrent.IsValid()) {
				auto* lastPlayedTrack = mMusicQueue.FindByPredicate([this](const FDungeonsMusicTrack& t) {
					return t == mCurrent;
					});
				if (lastPlayedTrack) {
					lastPlayedTrack->ElapsedSeconds = mBackgroundMusicElapsedSeconds;
				}
			}
			mPrevious = mCurrent;
			mCurrent = mMusicQueue.Top();
			mBackgroundMusicElapsedSeconds = mCurrent.ElapsedSeconds;
			mLevelBackgroundMusicDuration = mCurrent.IsValid() ? mCurrent.GetTrack()->GetDuration(): 0.0f;

			UpdateAndPlay();
		}
	}
	else if (mCurrent.IsValid()) {
		mCurrent = FDungeonsMusicTrack();
		UpdateAndPlay();
	}


	if (mCurrent.IsValid())
		mBackgroundMusicElapsedSeconds += DeltaSeconds;

	if (mBackgroundMusicElapsedSeconds > mLevelBackgroundMusicDuration)
	{
		mBackgroundMusicElapsedSeconds = 0.0f;
		mCurrent.ElapsedSeconds = 0.0f;

		if (!mCurrent.IsValid() || !mCurrent.GetTrack()->IsLooping()) {
			Pop(mCurrent.SlotType, 2.0f);
		}
	}


#if !UE_BUILD_SHIPPING
	debugFrequency += DeltaSeconds;
	if (mPrintDebug && debugFrequency >= .5) {
		mTrackFadeDebug -= DeltaSeconds;
		PrintState(TSet<UAudioComponent*>());
	}
#endif
}

UAudioComponent* AAudioManagerBase::ActiveAudioComponent() const {
	if (mActiveAudioCompIndex < AudioTracks.Num())
		return AudioTracks[mActiveAudioCompIndex];

	UE_LOG(LogDungeonsAudio, Error, TEXT("%s: Can't get AudioComponent on index %d, was the component destroyed??"), *mDisplayName, mActiveAudioCompIndex);
	return nullptr;
}

void AAudioManagerBase::SuspendCurrentTrack(float fadeOut) {
	ActiveAudioComponent()->FadeOut(fadeOut, mVolumeAtSilent);
}

void AAudioManagerBase::UpdateAndPlayInternal() {
	ActiveAudioComponent()->SetSound(mCurrent.GetTrack());
	ActiveAudioComponent()->FadeIn(mCurrent.FadeIn, 1.f);
}

void AAudioManagerBase::UpdateAndPlay() {
	if (!InitOK()) {
		SetupAudioComponents();
	}

	if (ActiveAudioComponent()->IsPlaying())
	{
		SuspendCurrentTrack(mLastPoppedFadeout);
			
#if !UE_BUILD_SHIPPING
		mTrackFadeDebug = mLastPoppedFadeout;
#endif
		mLastPoppedFadeout = 0.0f;
	}
	else
	{
		mCurrent.FadeIn = FadeInFromSilent;
	}

	UE_LOG(LogDungeonsAudio, Verbose, TEXT("mCurrent: %s"), mCurrent.GetTrack() ? *mCurrent.GetTrack()->GetFName().ToString() : TEXT("NULL"));
	if (!mCurrent.IsValid()) {
		return;
	}


	mActiveAudioCompIndex = (mActiveAudioCompIndex + 1) % 2;

	UE_LOG(LogDungeonsAudio, Log, TEXT("Set ActiveAudioComponent to %s (%d)"), *ActiveAudioComponent()->GetFName().ToString(), mActiveAudioCompIndex);
	UpdateAndPlayInternal();

	FString logAudioComponent = ActiveAudioComponent() ? ActiveAudioComponent()->GetFName().ToString() : FString::Printf(TEXT("NULL AudioComponent at %d"), mActiveAudioCompIndex);
	UE_LOG(LogDungeonsAudio, Log, TEXT("Playing %s AT [%s], "), mCurrent.GetTrack() ? *mCurrent.GetTrack()->GetFName().ToString(): TEXT("UNKNOWN"), *logAudioComponent);

}

#if !UE_BUILD_SHIPPING
void AAudioManagerBase::PrintState(TSet<UAudioComponent*> handled) const {
	GEngine->AddOnScreenDebugMessage(int32(GetClass()->GetUniqueID()), 1.f, FColor::Magenta, FString::Printf(TEXT("%s:"), *mDisplayName));
	int i = 1;
	for (const auto& item : mMusicQueue)
	{
		GEngine->AddOnScreenDebugMessage(int32(GetClass()->GetUniqueID()) + ++i, 1.2f, FColor::Magenta, FString::Printf(TEXT(".%s (%f s) T_%s"), *item.GetTrack()->GetFName().ToString(), item.ElapsedSeconds, *GetEnumValueToString(item.SlotType)));
	}

	for (auto musicTrack : AudioTracks) {
		if (musicTrack.Value && !handled.Contains(musicTrack.Value)) {
			FString playingLabel = musicTrack.Value->IsPlaying() ? ( mTrackFadeDebug > 0.f ? FString::Printf(TEXT("P -%f.2"), mTrackFadeDebug) : TEXT("P") ) : TEXT("");
			GEngine->AddOnScreenDebugMessage(int32(GetClass()->GetUniqueID()) + ++i, 1.f, FColor::Green, FString::Printf(TEXT("%s %s (%s)"), *musicTrack.Value->GetFName().ToString(), *musicTrack.Value->Sound->GetFName().ToString(), *playingLabel));
		}
	}
}
#endif

void AAudioManagerBase::ClearAllTracks() {
	mMusicQueue.Empty();
}

bool AAudioManagerBase::IsPlaying(EDungeonsMusicPriority slot) const {
	return mCurrent.SlotType == slot;
}

void AAudioManagerBase::ForceResetResumableTrack(EDungeonsMusicPriority slot) {
	if (mPrevious.SlotType == slot)
		mPrevious.CacheWhenPaused = false;
	if (mCurrent.SlotType == slot)
		mCurrent.CacheWhenPaused = false;
}

bool AAudioManagerBase::SyncLoadTrack(TSoftObjectPtr<USoundCue> Sound)
{
	FSoftObjectPath id = Sound.ToSoftObjectPath();
	if (!AsyncLoadTrack(Sound))
	{
		if (TrackStreamingCollection[id].IsValid())
		{
			TrackStreamingCollection[id]->WaitUntilComplete(); //just wait in here until we are done (this can prevent an async flush)
		}
	}
	return true;

}

bool AAudioManagerBase::AsyncLoadTrack(TSoftObjectPtr<USoundCue> Sound)
{
	FSoftObjectPath id = Sound.GetUniqueID();
	if (TrackStreamingCollection.Find(id))
	{
		if (TrackStreamingCollection[id].IsValid())
		{
			return TrackStreamingCollection[id]->HasLoadCompleted();
		}
	}

	UE_LOG(LogDungeons, Log, TEXT("attempt load soft object track %s"), *Sound.ToSoftObjectPath().GetAssetPathString());

	TrackStreamingCollection.Add(TPair<FSoftObjectPath, TSharedPtr<FStreamableHandle>>(id, UAssetManager::GetStreamableManager().RequestAsyncLoad(Sound.ToSoftObjectPath(), []() {}, FStreamableManager::AsyncLoadHighPriority, false, false, TEXT("FDungeonsMusicTrack::AsyncLoadTrack"))));

	if (!TrackStreamingCollection[id].IsValid())
	{
		// no asset/streaming failed, return 'true' to not get stuck in infininte retry.
		return true;
	}

	return TrackStreamingCollection[id]->HasLoadCompleted();
}

void AAudioManagerBase::PushMusicTrackSoft(TSoftObjectPtr<USoundCue> Sound, EDungeonsMusicPriority Priority, float CrossfadeIn, float CrossfadeOut, bool AllowResume) {
	// can I fetch/loop all music of this kind on the level and load upfront?
	if (Sound.IsNull()) { return; }
	SyncLoadTrack(Sound);
	PushMusicTrack(Sound.Get(), Priority, CrossfadeIn, CrossfadeOut, AllowResume);

}

void AAudioManagerBase::PushMusicTrack(USoundCue* SoundCue, EDungeonsMusicPriority Priority, float CrossfadeIn, float CrossfadeOut, bool AllowResume) {
	if (!SoundCue)
		return;
	int removed = mMusicQueue.RemoveAll([&Priority](const FDungeonsMusicTrack& item) {
		return item.SlotType == Priority;
		});
	if (removed > 1)
	{
		UE_LOG(LogDungeonsAudio, Warning, TEXT("Removed > 1 item from music slot %s, should only ever contain 1"),
			*GetEnumValueToString(Priority));
	}

	mLastPoppedFadeout = CrossfadeOut;
	mMusicQueue.Emplace(Priority, SoundCue, CrossfadeIn, 0.0f, AllowResume);

	// sort by priority:
	mMusicQueue.Sort([](const FDungeonsMusicTrack& l, const FDungeonsMusicTrack& r) {
		return l < r;
		});
	UE_LOG(LogDungeonsAudio, Verbose, TEXT("pushed track %s (removed %d tracks, %s)"), *SoundCue->GetFName().ToString(), removed, *GetEnumValueToString(Priority));

}

void AAudioManagerBase::Pop(EDungeonsMusicPriority slot, float CrossfadeIn, float CrossfadeOut, bool DestroyResume) {
	if (mMusicQueue.Num() == 0)
		return;
	if (slot == EDungeonsMusicPriority::Top) {
		slot = mMusicQueue.Top().SlotType;
	}
	// update fade
	mLastPoppedFadeout = CrossfadeOut;
	mMusicQueue.Top().FadeIn = CrossfadeIn;

	// clean suspended tracks, if should not be able to resume (e.g. EventMob dead)
	if (DestroyResume) {
		ForceResetResumableTrack(slot);
	}

	int removed = mMusicQueue.RemoveAll([slot](const FDungeonsMusicTrack& item) {
		return item.SlotType == slot;
		});
	if (removed > 1) {
		UE_LOG(LogDungeonsAudio, Error, TEXT("current track is NULL, will not playback pushed NULL track"));
		return;
	}

	UE_LOG(LogDungeonsAudio, Verbose, TEXT("popped %d tracks,%s"), removed, *GetEnumValueToString(slot));
}

/// debug
void AAudioManagerBase::TriggerPlayActiveTrack() {
#if !UE_BUILD_SHIPPING
	ActiveAudioComponent()->Play();
	GEngine->AddOnScreenDebugMessage(int32(GetClass()->GetUniqueID()) + 150, 2.5f, FColor::Magenta, TEXT("Play current Track through DEBUG TriggerPlayActiveTrack, this will be disabled in Shipped game!"));
#endif
}

void AAudioManagerBase::TriggerPlayAllTracks() {
#if !UE_BUILD_SHIPPING
	for (const auto& p : AudioTracks) {
		if (p.Value)
			p.Value->Play();

	}
	GEngine->AddOnScreenDebugMessage(int32(GetClass()->GetUniqueID()) + 151, 2.5f, FColor::Magenta, TEXT("Playing all AutioTracks trough DEBUG TriggerPlayAllTracks, this will be disabled in Shipped game!"));

#endif

}

void AAudioManagerBase::EndMusicTrack(UAudioComponent* audioComponent) {
	if (audioComponent)
	{
		// kill all wave instances of the (Music) track being deleted
		audioComponent->bAutoDestroy = true;
		if (USoundCue* soundCue = Cast<USoundCue>(audioComponent->Sound)) {
			TArray<USoundNodeWavePlayer*> WavePlayers;
			soundCue->RecursiveFindNode<USoundNodeWavePlayer>(soundCue->FirstNode, WavePlayers);

			for (auto WavePlayer: WavePlayers)
			{
				if (USoundWave* SoundWave = WavePlayer->GetSoundWave())
				{
					if (!GetWorld()->GetAudioDevice())
						break;
					GetWorld()->GetAudioDevice()->StopSoundsUsingResource(SoundWave);
				}
			}
		}

	}
}

void AAudioManagerBase::EndPlay(EEndPlayReason::Type EndPlayReason) {
	UE_LOG(LogDungeonsAudio, Verbose, TEXT("%s EndPlay"), *mDisplayName);

	// allow dangling pointer to cleanup once finished!
	for (auto track : AudioTracks)
	{
		EndMusicTrack(track.Value);
	}

	Super::EndPlay(EndPlayReason);
}
