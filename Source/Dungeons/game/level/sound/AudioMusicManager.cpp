#include "Dungeons.h"
#include "AudioMusicManager.h"
#include "game/levels.h"

#include "DungeonsGameInstance.h"
#include "DungeonsGameState.h"
#include "AudioCollectionComponent.h"
#include "DungeonsAudioUtils.h"

#include "Engine/Classes/Kismet/GameplayStatics.h"
#include "Engine/Classes/Sound/SoundCue.h"
#include "Engine/Public/AudioDevice.h"
#include "game/util/ActorQuery.h"
#include "util/StringUtil.h"

#include "game/GameBP.h"
#include "game/LobbyBP.h"

#include "game/actor/character/mob/MobCharacter.h"

namespace DungeonsQA {
	extern TAutoConsoleVariable<int32> CVShowMobAudioTriggers;
};

FMobMusicItem::FMobMusicItem(const AMobCharacter* mob, float distance2) {
	this->mob = mob;
	this->distance2 = distance2;
}


FDungeonsSoundFade::FDungeonsSoundFade(DungeonsFadeType fadeType, float currentVolume, float currentFadeTime, float targetVolume, UAudioComponent* audioComponent)
: mFadeType(fadeType)
, mCurrentVolume(currentVolume)
, mCurrentFadeTime(currentFadeTime)
, mTargetVolume(targetVolume)
, mRateOfChange((mTargetVolume - mCurrentVolume) / mCurrentFadeTime)
, mAudioComponent(audioComponent)
, mTrackName(audioComponent->GetFName())
{
	// Fix AB#303180, forcing linear fade- some other fade curve would need 
	if (mCurrentFadeTime < .001) {
		UE_LOG(LogDungeonsAudio, Warning, TEXT("Asking too short fade time: %f2, Rate of Change=dY/dX will fall back to '1'.\n Did you intend instant playback? Don't use DungeonsSoundFade"), mCurrentFadeTime);
		mRateOfChange = 1.0f;
	}

}

bool FDungeonsSoundFade::isFading() const {
	return mCurrentVolume != mTargetVolume;
}

void FDungeonsSoundFade::update(float DeltaSeconds) {
	if (!isFading())
		return;

	// fix AB#290148 self-destruct if AudioComponent was lost for some reason
	if (!mAudioComponent) {
		UE_LOG(LogDungeonsAudio, Error, TEXT("no audio component for %s, Resume immediately at Vol=100"), *mTrackName.ToString());
		mCurrentVolume = mTargetVolume;
		return;
	}

	float MinValue = 0.0f;
	float MaxValue = 0.0f;

	if (mCurrentVolume < mTargetVolume)
	{
		MinValue = mCurrentVolume;
		MaxValue = mTargetVolume;
	}
	else
	{
		MinValue = mTargetVolume;
		MaxValue = mCurrentVolume;
	}

	float DeltaTimeValue = FMath::Min(DeltaSeconds, 0.5f);
	mCurrentFadeTime = mCurrentFadeTime - DeltaTimeValue;

	if (mCurrentFadeTime > 0) {
		mCurrentVolume = mCurrentVolume + DeltaTimeValue * mRateOfChange;
	}
	else {
		mCurrentVolume = mTargetVolume;
	}

	mCurrentVolume = FMath::Clamp(mCurrentVolume, MinValue, MaxValue);
	mAudioComponent->AdjustVolume(0.f, mCurrentVolume);

}

AAudioMusicManager::AAudioMusicManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, MusicOverrideResetRadius2(FMath::Square(MusicOverrideResetRadius))
	, MusicOverrideTriggerRadius2(FMath::Square(MusicOverrideTriggerRadius))
	, mLastLevelName(ELevelNames::Invalid)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	bAllowTickBeforeBeginPlay = false;

	PrimaryActorTick.TickInterval = 0.1f;
	mDisplayName = "Music.Manager";
}

void AAudioMusicManager::BeginPlay() {
	Super::BeginPlay();
	checkf(AudioCollectionBP->IsValidLowLevel(), TEXT("MusicManager must have an associated AudioCollectionBP or sound playback will fail!"));

	UAudioCollectionComponent* createdComp = NewObject<UAudioCollectionComponent>(this, AudioCollectionBP, "Audio Collection");
	if (createdComp)
	{
		createdComp->RegisterComponent();
		mAudioCollectionComponent = createdComp;
	}

	if (EmptySoundCue)
	{
		bool PersistOnLevelLoad = true;
		LoadingScreenAudioComponent = audiomanager::createSound(GetWorld(), EmptySoundCue, PersistOnLevelLoad, false);
	}
}

void AAudioMusicManager::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);
	
	auto* target = audioutils::CameraViewTargetPosition(GetWorld());
	ProcessMobMusicOverrides(target);
	ProcessBackgroundMusicOverride(target);

	for (auto& track : ActiveFades)
	{
		track.update(DeltaSeconds);
		if (track.stale())
			OnFadeComplete(track);
	}

	ActiveFades.RemoveAll([](const FDungeonsSoundFade& track) {
		return track.stale();
	});
}

#if !UE_BUILD_SHIPPING
void AAudioMusicManager::PrintState(TSet<UAudioComponent*> handled) const {
	int i = 0;
	TSet<UAudioComponent*> handledTracks;
	for (auto f : ActiveFades) {
		if (f.mAudioComponent) {
			if(f.mAudioComponent->IsPlaying()) {
			FString playingLabel = FString::Printf(TEXT("P -%f.2"), f.mCurrentVolume);

			GEngine->AddOnScreenDebugMessage(int32(GetClass()->GetUniqueID()) + ++i, 1.f, FColor::Green, FString::Printf(TEXT("%s %s (%s)"), *f.mAudioComponent->GetFName().ToString(), *f.mAudioComponent->Sound->GetFName().ToString(), *playingLabel));
			}

			handledTracks.Add(f.mAudioComponent);
		}
	}

	Super::PrintState(handledTracks);
}
#endif

UAudioCollectionComponent* AAudioMusicManager::collectionComponent() const {
	if (!mAudioCollectionComponent) 
	{
		mAudioCollectionComponent = FindComponentByClass<UAudioCollectionComponent>();
	}
	return mAudioCollectionComponent;
}

ELevelNames AAudioMusicManager::GetCurrentLevelContext() const {
	return mLastLevelName;
}

FLevelAudio AAudioMusicManager::GetCurrentLevelAudioCollection() const {
	auto* gameInstance = GetGameInstance<UDungeonsGameInstance>();
	if (collectionComponent() && gameInstance) {
		if (ULevelSettingsUtil::IsLobby(gameInstance->GetLevelSettingsLastStarted())) {
			return collectionComponent()->GetLobbyAudio();
		}
		
		ELevelNames currentLevelContext = gameInstance->GetLevelSettingsLastStarted().getLevelName();
		if (GetCurrentLevelContext() != ELevelNames::Invalid)
			currentLevelContext = GetCurrentLevelContext();
		return collectionComponent()->GetMusicTracksForLevel(currentLevelContext);
	}
	return FLevelAudio();
}

USoundCue* AAudioMusicManager::GetEncounterMusicTrack(EMusicSequenceState sequenceState, EEventType eventType) const {
	if (collectionComponent()) {
		auto audioCollection = collectionComponent()->GetMusicTracksForLevel(UDungeonsAudioUtils::GetLevelName(GetWorld()));
		switch (eventType)
		{
		case EEventType::SideQuestBattle:
			return audioCollection.LevelSideQuestBattleMusic.GetMusicTrack(sequenceState);
		case EEventType::ArenaBattle:
			return audioCollection.LevelEncounterMusic.GetMusicTrack(sequenceState);
		case EEventType::Boss:
			return audioCollection.LevelBossMusic.GetMusicTrack(sequenceState);

		}
		
	}
	return nullptr;
}

namespace musicmanager {
	static TArray<FMobMusicItem> makelist(const TArray<TTuple<AMobCharacter*, float>>& data) {
		TArray<FMobMusicItem> mobMusicList;

		for (const auto& ttp : data) {
			mobMusicList.Emplace(ttp.Key, ttp.Value);
		}

		return mobMusicList;
	}
}

// similar to the MobMusicOverride checking with BlockPos for 'Actor' CameraTargetPosition.
void AAudioMusicManager::ProcessBackgroundMusicOverride(AActor* target) {
	if (!target)
		return;

	if (auto game = actorquery::getGame(GetWorld()))
	{
		if (game->IsLobbyLevel())
			return;

		const auto* closestTile = game->tiles().getClosestTile(*target);
		if (closestTile != mCurrentTile) 
		{
			OnTileChanged(closestTile);
		}
	}
}

void AAudioMusicManager::ProcessMobMusicOverrides(AActor* musicTarget) {
	float search_radius = MusicOverrideResetRadius;
	TArray<TTuple<AMobCharacter*, float>> closeMobs;
	if (musicTarget) {
		closeMobs = actorquery::getNearbyInstanceTrackedActorsDistanceSquared<AMobCharacter>(musicTarget, search_radius, actorquery::is::mobShouldOverrideMusicTrack());
	}
	
	// update mob list/arrange
	MusicOverrideSet = musicmanager::makelist(closeMobs);
	MusicOverrideSet.Sort();
	
#if !UE_BUILD_SHIPPING
	// debug: show mob queue
	if (DungeonsQA::CVShowMobAudioTriggers.GetValueOnGameThread() != 0) {
		GEngine->AddOnScreenDebugMessage(int32(GetClass()->GetUniqueID()), 1.f, FColor::Purple, TEXT("::Mob Music Override QUEUE::"));
		int i = 1;

		FString closeMobsDisp = "fnd: ";
		for (const auto& ttp : MusicOverrideSet)
		{
			closeMobsDisp += FString::Printf(TEXT(".%s (d:%f)"), *ttp.mob->GetFName().ToString(), ttp.distance2);
		}

		GEngine->AddOnScreenDebugMessage(int32(GetClass()->GetUniqueID()) + ++i, 1.2f, FColor::Purple, *closeMobsDisp);

		FString lastMobLabel = mLastEngagedMob ? mLastEngagedMob->GetFName().ToString() : TEXT("NONE");
		GEngine->AddOnScreenDebugMessage(int32(GetClass()->GetUniqueID()) + ++i, 1.2f, FColor::Green, FString::Printf(TEXT("nCandidates=%d (lastMob: %s)"), MusicOverrideSet.Num(), *lastMobLabel));
	}
#endif

	auto* mobInRange = mLastEngagedMob ? MusicOverrideSet.FindByPredicate([this](const FMobMusicItem& item) { return item.mob == this->mLastEngagedMob; }) : nullptr;
	const auto* activeMob = mobInRange ? mLastEngagedMob : nullptr;
	if (!activeMob) {
		MusicOverrideSet.FilterByPredicate([this](const FMobMusicItem& item) { return item.distance2 < MusicOverrideTriggerRadius2;  });
		if (MusicOverrideSet.Num() > 0) 
			activeMob = MusicOverrideSet[0].mob;
	}

	// handle event music updates:
	if (activeMob != mLastEngagedMob) {
		if (activeMob && mLastEngagedMob)
			OnEventMobChanged(activeMob);
		else if (activeMob)
			OnEventMobEngaged(activeMob);
		else if (mLastEngagedMob)
			OnEventMobDisengaged(mLastEngagedMob);
	} else if (activeMob) {
		OnEventMobUpdate(activeMob);
	}
	
}

void AAudioMusicManager::OnTileChanged(game::TilePtr newTile) {
	mCurrentTile = newTile;

	const auto levelFromString = [](const FString& s) -> TOptional<ELevelNames> {
		// @todo: add an enum redirect? how do we know if safe to add? (or to update level::fromString but same question there)
		if (s.Equals("redstonemines", ESearchCase::IgnoreCase)) {
			return ELevelNames::mooncorecaverns;
		}
		if (const auto ambienceLevelId = GetEnumValueFromStringT<ELevelNames>(s)) {
			return ambienceLevelId;
		}
		return {};
	};
	const auto musicLevelId = [&] () -> TOptional<ELevelNames> {
		if (newTile->musicLevelOverride()) {
			return newTile->musicLevelOverride().GetValue();
		}
		if (const auto ambienceLevelId = levelFromString(newTile->ambienceGroupName())) {
			return ambienceLevelId;
		}
		if (const auto resourcePackLevelId = levelFromString(stringutil::toFString(newTile->dungeon().def().resourcePack))) {
			return resourcePackLevelId;
		}
		return {};
	}();

	if (musicLevelId && musicLevelId.GetValue() != mLastLevelName) {
		OnLevelContextChanged(musicLevelId.GetValue());
	}
}

void AAudioMusicManager::OnLevelContextChanged(ELevelNames newLevelName) {
	mLastLevelName = newLevelName;
	auto trackSet = collectionComponent()->GetMusicTracksForLevel(newLevelName);
	trackSet.SyncLoadLevelAudio();//make sure we have it
	USoundCue* levelMusicTrack = trackSet.LevelBackgroundMusic.Get();
	if (levelMusicTrack && levelMusicTrack != mCurrent.GetTrack()) {
		PushMusicTrack(levelMusicTrack, EDungeonsMusicPriority::BackgroundMusic);

		// refactor to update Audio SFX _outside_ of MusicManger?
		if (auto gameInstance = GetGameInstance<UDungeonsGameInstance>())
		{
			if (auto sfx = gameInstance->GetAudioSFXManager())
				sfx->PushMusicTrack(trackSet.EnvironmentSFX.Get(), EDungeonsMusicPriority::BackgroundMusic);
		}
	}
}

void AAudioMusicManager::OnEventMobEngaged(const AMobCharacter* mob) {
	UE_LOG(LogDungeonsAudio, Verbose, TEXT("[MusicManager] OnEventMobEngaged (%s)"), *mob->GetName());
	if (auto* comp = mob->FindComponentByClass<UMusicOverrideComponent>()) {
		StartEventMobMusic(comp, mob->GetMusicSequenceState());
	}

	mLastEngagedMob = mob;
}

void AAudioMusicManager::OnEventMobUpdate(const AMobCharacter* mob) {
	if (auto* comp = mob->FindComponentByClass<UMusicOverrideComponent>()) {
		auto state = mob->GetMusicSequenceState();
		if (state != mLastMusicSequenceState) {
			StartEventMobMusic(comp, state);
		}
	}
}

void AAudioMusicManager::OnEventMobChanged(const AMobCharacter* newMob) {
	// new case/update mob if no change 'type'
	if (newMob->EntityType == mLastEngagedMob->EntityType) {
		mLastEngagedMob = newMob;
	}
	else {
		OnEventMobDisengaged(mLastEngagedMob);
		OnEventMobEngaged(newMob);
	}

}

void AAudioMusicManager::OnEventMobDisengaged(const AMobCharacter* mob)
{
	ensure(mob);
	if (!mob->IsAlive()) {
		UE_LOG(LogDungeonsAudio, Verbose, TEXT("[MusicManager] OnEventMob DEAD (%s)"), *mob->GetName());
		ForceResetResumableTrack(EDungeonsMusicPriority::EventMusic);

		if (auto* comp = mob->FindComponentByClass<UMusicOverrideComponent>()) {
			if (auto soundCue = comp->GetMusicOverrideTrack(EMusicSequenceState::Outro)) {
				// 'tail' music track, will pop on finished and fade with hard coded 2.0f s
				PushMusicTrack(soundCue, EDungeonsMusicPriority::EventMusic, 0.f, 1.f, false);
				ResetEventMobMusicState();
				return;
			}
		}
		else
			UE_LOG(LogDungeonsAudio, Warning, TEXT("No MusicOverrideComponent on 'mob', will not push the 'outro' track.."));

		/// fallback / fix AB#342328
		Pop(EDungeonsMusicPriority::EventMusic);
		ResetEventMobMusicState();
	}
	else { // disengaged
		ResetEventMobMusicState();
		Pop(EDungeonsMusicPriority::EventMusic, 4.0f, 4.0f);
	}

}

void AAudioMusicManager::StartEventMobMusic(const UMusicOverrideComponent* mobMusicOverride, EMusicSequenceState state) {		
	if (!mobMusicOverride->GetSoundMix()){
		UE_LOG(LogDungeonsAudio, Warning, TEXT("No sound mix for event mob music playback!!!"));
	}

	if(auto* soundCue = mobMusicOverride->GetMusicOverrideTrack(state)){
		mLastMusicSequenceState = state;
		mLastTriggeredEventMobMix = mobMusicOverride->GetSoundMix();
		GetGameInstance<UDungeonsGameInstance>()->GetSoundMixManager()->PushSoundMix(mobMusicOverride->GetSoundMix());

		// note although fade is set to 0.f here for instant mob action event; 
		// resume play music (when re-engaging a mob) will fade due to the track being in PausedSet'
		// AllowResume set to false for 'Intro' state
		PushMusicTrack(soundCue, EDungeonsMusicPriority::EventMusic, 0.f, 1.0f, state != EMusicSequenceState::Intro);
	}
}

void AAudioMusicManager::ResetEventMobMusicState() {
	GetGameInstance<UDungeonsGameInstance>()->GetSoundMixManager()->PopSoundMix(mLastTriggeredEventMobMix);
	mLastTriggeredEventMobMix = nullptr;	
	mLastMusicSequenceState = EMusicSequenceState::Main;
	mLastEngagedMob = nullptr;
}

void AAudioMusicManager::OnFadeComplete(FDungeonsSoundFade soundFadeItem) {
	if (soundFadeItem.mAudioComponent && soundFadeItem.mFadeType == DungeonsFadeType::FadeOut && mDoPause)
		soundFadeItem.mAudioComponent->SetPaused(true);
}

void AAudioMusicManager::SoundFade(DungeonsFadeType fadeType, UAudioComponent* audioComponent, float fadeTime, float targetVolume) {
	ensureMsgf(audioComponent, TEXT("No audiocomponent when calling SoundFade, errendous playback possible!"));
	if (!audioComponent)
		return;
	
	audioComponent->SetPaused(false);

	FString fadeTxt = fadeType == DungeonsFadeType::FadeIn ? "IN": "Out";

	// try to update existing fade
	if (auto e = ActiveFades.FindByPredicate([audioComponent](const FDungeonsSoundFade& ac) {return ac.mAudioComponent->Sound == audioComponent->Sound; })) {
		if (e->mAudioComponent != audioComponent) {
			e->mAudioComponent->Stop();
			EndMusicTrack(e->mAudioComponent);
		}

		UE_LOG(LogDungeonsAudio, Verbose, TEXT("Fade %s comp: %s UPDATE; vol: %f->%f, t:%f")
			, *fadeTxt, *audioComponent->GetFName().ToString(), e->mCurrentVolume, targetVolume, fadeTime
		);

		e->mFadeType = e->mCurrentVolume > targetVolume? DungeonsFadeType::FadeOut: DungeonsFadeType::FadeIn;

		e->mAudioComponent = audioComponent;
		float fadeTimeRescale = FMath::Clamp(targetVolume - e->mCurrentVolume, 0.1f, 1.0f);
		e->mCurrentFadeTime = fadeTime * fadeTimeRescale;
		e->mTargetVolume = targetVolume;
		//e->mCurrentVolume =  // No change
	}
	else {
		UE_LOG(LogDungeonsAudio, Verbose, TEXT("Fade %s comp: %s New! vol: %f, t:%f")
			, *fadeTxt, *audioComponent->GetFName().ToString(), targetVolume, fadeTime
		);
		ActiveFades.Emplace(
			fadeType
			, (fadeType == DungeonsFadeType::FadeIn)? 0.f: 1.f
			, fadeTime
			, targetVolume
			, audioComponent
		);
	}
}

void AAudioMusicManager::SuspendCurrentTrack(float fadeOut) {
	// early out if no mPrevious, since isPlaying()-check is not working with the volume-adjustment style track fadeout
	if (!mPrevious.IsValid())
		return;

	FString byAdjustVolume = mPrevious.CacheWhenPaused ? TEXT("-Adjust volume") : TEXT("");
	FString previousTrackName = mPrevious.GetTrack()->GetFName().ToString();
	UE_LOG(LogDungeonsAudio, Log, TEXT("%f.2 s Fade out track %s (mPrevious is %s) on %s (%d) %s"), fadeOut, *ActiveAudioComponent()->Sound->GetFName().ToString(), *previousTrackName , *ActiveAudioComponent()->GetFName().ToString(), ActiveAudioCompIndex(), *byAdjustVolume);
	
	if (mPrevious.CacheWhenPaused) {
		SoundFade(DungeonsFadeType::FadeOut, ActiveAudioComponent(), fadeOut, mVolumeAtSilent);
		PausedSet.Add(mPrevious.GetTrack()->GetFName(), ActiveAudioComponent());
			
	}
	else {
		if (auto* fadingTrack = ActiveFades.FindByPredicate([this](const FDungeonsSoundFade& F) {return F.mAudioComponent == this->ActiveAudioComponent(); })) {
			UE_LOG(LogDungeonsAudio, Warning, TEXT("Suspending track currently fading, assigning fade==complete: %s "), *fadingTrack->mAudioComponent->GetFName().ToString());
			fadingTrack->mCurrentVolume = fadingTrack->mTargetVolume;
			ActiveFades.Remove(*fadingTrack);
		}
		ActiveAudioComponent()->FadeOut(fadeOut, 0.f);
	}
}

void AAudioMusicManager::UpdateAndPlayInternal() {
	UAudioComponent* ac = nullptr;
	bool PlaybackNewAudioComponent = false;

	if (auto newTrack = PausedSet.Find(mCurrent.GetTrack()->GetFName()))
	{
		ac = *newTrack;
		if (!ac) // clear out stale component
			PausedSet.Remove(mCurrent.GetTrack()->GetFName());
	}
	
	if (!ac) { // replace current AudioComponent:
		ActiveAudioComponent()->bAutoDestroy = true;
		ac = audiomanager::createSound(GetWorld(), mCurrent.GetTrack(), PersistMusicOnLevelLoad, mAutoActivateNew);
		PlaybackNewAudioComponent = true;
	}

	auto* old_AudioComp = ActiveAudioComponent();
	AudioTracks[ActiveAudioCompIndex()] = ac;

	UE_LOG(LogDungeonsAudio, Verbose, TEXT("Updated AudioComponentSet %s->%s (%d)"), *old_AudioComp->GetFName().ToString(), *ac->GetFName().ToString(), ActiveAudioCompIndex());

	if (PlaybackNewAudioComponent) {
		ActiveAudioComponent()->SetSound(mCurrent.GetTrack()); // will reset playback position!
		UE_LOG(LogDungeonsAudio, Verbose, TEXT("Fade in track %s on %s (%d) 'actual (1st playback)'"), *ActiveAudioComponent()->Sound->GetFName().ToString(), *ActiveAudioComponent()->GetFName().ToString(), ActiveAudioCompIndex());
		ActiveAudioComponent()->FadeIn(mCurrent.FadeIn, 1.f, mCurrent.ElapsedSeconds);
	}
	else {
		UE_LOG(LogDungeonsAudio, Verbose, TEXT("Fade in track %s on %s (%d) -Adjust volume"), *ActiveAudioComponent()->Sound->GetFName().ToString(), *ActiveAudioComponent()->GetFName().ToString(), ActiveAudioCompIndex());
		if (!ActiveAudioComponent()->IsActive())
		{
			UE_LOG(LogDungeonsAudio, Warning, TEXT("%s: Playback was deactivated, cannot resume, re-trigger fade in from start!"), *ActiveAudioComponent()->GetFName().ToString());
			ActiveAudioComponent()->Play();
		}

		// always fade in resume music track. Fallback/mitigation AB#345029, AB#262302
		float fadeIn = mCurrent.FadeIn > 0.0f? mCurrent.FadeIn: 3.0f;
		SoundFade(DungeonsFadeType::FadeIn, ActiveAudioComponent(), fadeIn, 1.f);
	}
	
	ensure(ActiveAudioComponent()->IsActive());
}

UAudioComponent* AAudioMusicManager::PlayLoadingScreenSound() {
	if (collectionComponent() && LoadingScreenAudioComponent) {
		LoadingScreenAudioComponent->SetSound(collectionComponent()->GetLoadingScreenSound());
		LoadingScreenAudioComponent->FadeIn(collectionComponent()->LoadingScreenFadeIn());
	}
	return LoadingScreenAudioComponent;
}

void AAudioMusicManager::StopLoadingScreenSound() {
	if (collectionComponent())
		GetWorld()->GetAudioDevice()->StopSoundsUsingResource(collectionComponent()->GetLoadingScreenSound());
	LoadingScreenAudioComponent = nullptr;
}

void AAudioMusicManager::Pause(float fadeOut) {
	mPrevious = mCurrent;
	if(ActiveAudioComponent() && ActiveAudioComponent()->IsPlaying())
		SuspendCurrentTrack(fadeOut);
}

void AAudioMusicManager::CleanupAudioComponents() {
	Super::CleanupAudioComponents();
	// kill the paused music tracks
	for (auto paused : PausedSet)
		EndMusicTrack(paused.Value);

	PausedSet.Empty();
}

void AAudioMusicManager::EndPlay(EEndPlayReason::Type EndPlayReason) {
	// allow dangling pointer to cleanup once finished!
	if (LoadingScreenAudioComponent)
		LoadingScreenAudioComponent->bAutoDestroy = true;

	for (auto pausedTrack : PausedSet)
		EndMusicTrack(pausedTrack.Value);

	PausedSet.Empty();

	Super::EndPlay(EndPlayReason);
}
