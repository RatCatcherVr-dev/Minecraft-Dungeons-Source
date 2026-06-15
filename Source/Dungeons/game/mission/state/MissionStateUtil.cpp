#include "Dungeons.h"
#include <GenericPlatformMath.h>
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/component/MissionProgressComponent.h"
#include "game/component/ItemStashComponent.h"
#include "game/mission/difficulty/MissionDifficulty.h"
#include "game/mission/hyper/HyperMissions.h"
#include "util/telemetry/Analytics.h"
#include "MissionStateUtil.h"


TOptional<FMissionState> UMissionStateUtil::InspectProducedMissionState(const APlayerCharacter* progressOwner, const FMissionSelection& selection) {
	const auto stash = progressOwner->GetItemStashComponent(); //const to ensure we don't change anything when inspecting
	const RandomSeed seed = 0; //Don't actually generate any random-seed when inspecting.
	const auto& missionDef = missions::get(selection.levelName);
	if (missionDef.requiresOfferings()) {
		if (auto currentOffering = stash->GetMaybeMissionOfferings(selection.levelName)) {
			//Create state based on current offerings
			auto missionState = FMissionState(FMissionDifficulty::from(selection), seed, currentOffering.GetValue(), progressOwner->GetMissionProgressComponent()->GetOwnedDLCs());
			missionState.bCompletedOnce = progressOwner->GetCharacterSerializeComponent()->IsLevelCompleted(missionState.missionDifficulty);
			return missionState;
		}
	}
	//Default mission state
	FMissionState missionState = FMissionState(FMissionDifficulty::from(selection), seed);
	missionState.bCompletedOnce = progressOwner->GetCharacterSerializeComponent()->IsLevelCompleted(missionState.missionDifficulty);
	return missionState;
}

TOptional<FMissionState> UMissionStateUtil::ProduceMissionState(APlayerCharacter* progressOwner, const FMissionSelection& selection) {
	const auto& missionDef = missions::get(selection.levelName);
	if (missionDef.requiresOfferings()) {
		if (auto offeringSuccessfullyConsumed = progressOwner->GetItemStashComponent()->TryConsumeMissionOfferings(selection.levelName)) {
			analytics::Analytics::GetInstance().FireEventItemsSacrificed(progressOwner, offeringSuccessfullyConsumed.GetValue());
			auto missionState = FMissionState(FMissionDifficulty::from(selection), missionDef.provideRandomSeed(offeringSuccessfullyConsumed), offeringSuccessfullyConsumed.GetValue(), progressOwner->GetMissionProgressComponent()->GetOwnedDLCs());
			missionState.bCompletedOnce = progressOwner->GetCharacterSerializeComponent()->IsLevelCompleted(missionState.missionDifficulty);
			progressOwner->GetMissionProgressComponent()->SetMissionState(selection.levelName, missionState);
			return missionState;
		} else {
			checkf(false, TEXT("mission requires offerings, offerings could not be consumed"));
			return TOptional<FMissionState>{};
		}
	}
	//Default mission state
	FMissionState missionState = FMissionState(FMissionDifficulty::from(selection), missionDef.provideRandomSeed({}));
	missionState.bCompletedOnce = progressOwner->GetCharacterSerializeComponent()->IsLevelCompleted(missionState.missionDifficulty);
	return missionState;
}

TOptional<FMissionState> UMissionStateUtil::GetMissionState(const APlayerCharacter* progressOwner, ELevelNames mission) {
	return progressOwner->GetMissionProgressComponent()->GetMissionState(mission);
}

EGameDifficulty UMissionStateUtil::GetDifficulty(const FMissionState& missionState) {
	return missionState.missionDifficulty.difficulty;
}

EThreatLevel UMissionStateUtil::GetThreatLevel(const FMissionState& missionState) {
	return missionState.missionDifficulty.threatLevel;
}

ELevelNames UMissionStateUtil::GetLevelName(const FMissionState& missionState) {
	return missionState.missionDifficulty.mission;
}

FEndlessStruggle UMissionStateUtil::GetEndlessStruggle(const FMissionState& missionState) {
	return missionState.missionDifficulty.endlessStruggle;
}

TArray<FItemBulletPoint> UMissionStateUtil::GetBulletPoints(const FMissionState& missionState) {
	TArray<FItemBulletPoint> bulletPoints = {};
	if (missions::get(missionState.getLevelName()).isHyperMission()) {
		missions::hajper::populateMissionBulletPoints(missionState, bulletPoints);
	}	
	return bulletPoints;
}

int UMissionStateUtil::GetTotalDisplayItemPower(const FMissionState& missionState) {
	return missionState.getTotalDisplayItemPower();
}

const FMissionDifficulty& UMissionStateUtil::getMissionDifficulty(const FMissionState& missionState) {
	return missionState.missionDifficulty;
}
