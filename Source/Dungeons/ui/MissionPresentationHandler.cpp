#include "Dungeons.h"
#include "MissionPresentationHandler.h"

#include "DungeonsGameMode.h"
#include "game/objective/Objective.h"
#include "game/objective/objectives/SoundData.h"

#include "game/level/sound/AudioMusicManager.h"
#include "game/level/sound/DungeonsAudioUtils.h"
#include "game/objective/EventTypes.h"
#include "util/EnumUtil.h"

// FMissionPresentation no longer allows pointing to audio data in level .jsons!
FMissionPresentation::FMissionPresentation(const game::objective::Objective& obj, ELevelNames levelname)
{
	EventType = obj.eventType();
	UICountdown = obj.uiDelayTime();
	ObjectiveIndex = obj.info().objectiveIndex;
	Id = obj.info().id.Get("");
	TriggerMusicEvents = obj.triggerMusicPlaybackEvents();
}

void FMissionPresentation::write() const {
	FString eventTypeString = GetEnumValueToString(EventType);
	FString triggerMusic = TriggerMusicEvents ? "[YES]" : "[NO]";
	UE_LOG(LogDungeons, Verbose, TEXT("[%s][%s] (UI-countdown)%f.2, (obj-index)%d, (trigger-music)%s")
		, *eventTypeString, *Id, UICountdown, ObjectiveIndex, *triggerMusic);
}

UMissionPresentationHandler::UMissionPresentationHandler()
{
	SetIsReplicated(true);
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;

}

void UMissionPresentationHandler::SetOOBMissionBoundsLimit(FBox arenaTileBounds) {
	mMissionBoundingBox = arenaTileBounds.ExpandBy(5000.0f);
	if (GetOwner()->Role == ROLE_Authority) {
		OnRep_SetMissionBoundingBox();
	}
}

void UMissionPresentationHandler::ObjectiveStarted(const game::objective::Objective& obj) {
	FMissionPresentation NewPresentationData = FMissionPresentation(obj, audioutils::CurrentLevelContext(GetWorld()));
	if( mPresentationData != NewPresentationData && NewPresentationData.EventType != EEventType::None ) {// only trigger presentation events for new objective {
		UpdatePresentationData( NewPresentationData );
	}
}

void UMissionPresentationHandler::ObjectiveFinished(const game::objective::Objective& obj) {
	if (obj.eventType() != EEventType::None)
		EndCurrentObjective();
}

void UMissionPresentationHandler::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) 
{
	updateStateMusicPlayback();
};

void UMissionPresentationHandler::updateStateMusicPlayback() {
	// if user is outside bounding box for current Objective AND objective is not finished AND is playing music:
	if (auto gi = GetWorld()->GetGameInstance<UDungeonsGameInstance>())
	{
		if (auto* target = audioutils::CameraViewTargetPosition(GetWorld()))
		{
			if (!mMissionBoundingBox.IsInsideXY(target->GetActorLocation())
				&& (mPresentationData.EventType == EEventType::ArenaBattle
					|| mPresentationData.EventType == EEventType::SideQuestBattle)
				&& UMissionDefs::IsHyperMission(gi->GetLevelSettingsLastStarted().getLevelName())
				)
			{
				// kill the music
				auto* mm = gi->GetAudioMusicManager();
				if (mm->IsPlaying(EDungeonsMusicPriority::BossEncounterMusic)) {
					mm->Pop(EDungeonsMusicPriority::BossEncounterMusic, 2.0f, 2.0f);
				}
			}
		}
	}
}

void UMissionPresentationHandler::UpdatePresentationData(const FMissionPresentation& presentationData) {
	mPresentationData = presentationData;
	RepNotifyInternal();
}

void UMissionPresentationHandler::EndCurrentObjective_Implementation() {
	if (mPresentationData.TriggerMusicEvents) {
		auto* outro = audioutils::GetEncounterMusicTrack(
			GetWorld(), EMusicSequenceState::Outro, mPresentationData.EventType, audioutils::CurrentLevelContext(GetWorld()) );
		// also send nullptr, which is scripted to "halt" music playback (for e.g. ArenaBattle)
		OnTriggerOutroMusic.Broadcast( outro );
	}

	// Stop ticking (for ArenaBattle music trigger update)
	SetComponentTickEnabled(false);

	// AB#296327 connecting clients need to have cleared old presentation data
	UpdatePresentationData(FMissionPresentation());
}

void UMissionPresentationHandler::TriggerMusicLoop() {
	if( mPresentationData.TriggerMusicEvents ) {
		USoundCue* track = audioutils::GetEncounterMusicTrack(
			GetWorld(), EMusicSequenceState::Main, mPresentationData.EventType, audioutils::CurrentLevelContext(GetWorld()) );
		OnObjectiveMusicStart.Broadcast( track );
	}
}

void UMissionPresentationHandler::OnRep_PresentationData() {
	FString serverClient = GetOwner()->Role == ROLE_Authority ? "SERVER" : "CLIENT";
	UE_LOG(LogDungeons, Verbose, TEXT("[%s] got new presentation data"), *serverClient);
	mPresentationData.write();

	if( mPresentationData.TriggerMusicEvents ) {
		// trigger intro 
		USoundCue* Introtrack = audioutils::GetEncounterMusicTrack(
			GetWorld(), EMusicSequenceState::Intro, mPresentationData.EventType, audioutils::CurrentLevelContext(GetWorld()) );
		OnTriggerIntroMusic.Broadcast( Introtrack );

		FString name = Introtrack ? Introtrack->GetName() : "NULL";
		UE_LOG(LogDungeons, Verbose, TEXT("Triggering audio track '%s'/Encounter for (lvl)'%s'"), *name, *GetEnumValueToString(audioutils::CurrentLevelContext(GetWorld())));

		if( mPresentationData.UICountdown > 0 ) {
			FTimerHandle T1;
			GetWorld()->GetTimerManager().SetTimer( T1, this, &UMissionPresentationHandler::TriggerMusicLoop, mPresentationData.UICountdown, false );
		}
		else {
			TriggerMusicLoop();
		}
	}
}

// assumes mPresentationData was replicated BEFORE MissionBoundingBox.
void UMissionPresentationHandler::OnRep_SetMissionBoundingBox() {
	if (auto gi = GetWorld()->GetGameInstance<UDungeonsGameInstance>())
	{
		SetComponentTickEnabled(mPresentationData.TriggerMusicEvents
			&& (mPresentationData.EventType == EEventType::ArenaBattle
				|| mPresentationData.EventType == EEventType::SideQuestBattle)
			&& UMissionDefs::IsHyperMission(gi->GetLevelSettingsLastStarted().getLevelName()));
	}
}

void UMissionPresentationHandler::RepNotifyInternal() {
	if (GetOwner()->Role == ROLE_Authority) {
		OnRep_PresentationData();
	}
}

void UMissionPresentationHandler::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UMissionPresentationHandler, mPresentationData);
	DOREPLIFETIME(UMissionPresentationHandler, mMissionBoundingBox);
}
