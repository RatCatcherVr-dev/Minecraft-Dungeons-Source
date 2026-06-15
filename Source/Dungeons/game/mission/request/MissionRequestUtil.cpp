#include "Dungeons.h"
#include "MissionRequestUtil.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/mission/offerings/MissionOfferingsUtil.h"
#include "game/mission/state/MissionState.h"
#include "game/mission/state/MissionStateUtil.h"
#include "game/component/MissionProgressComponent.h"
#include "game/component/CharacterSerializeComponent.h"

TOptional<FMissionRequest> UMissionRequestUtil::MakeOptionalRequest(EMissionRequestType requestType, TOptional<FMissionState> state) {
	if (state) {
		return FMissionRequest{
			requestType,
			state.GetValue()
		};
	}
	return {};
}

FMissionRequest UMissionRequestUtil::CreateMissionRequest(const APlayerCharacter* player, EMissionRequestType requestType, const FMissionSelection& selection, int32 seed, const FMissionOfferings& offerings) {
	FMissionRequest missionRequest = FMissionRequest{
		requestType,
		FMissionState(FMissionDifficulty::from(selection), seed, offerings, player->GetMissionProgressComponent()->GetOwnedDLCs())
	};
	missionRequest.state.bCompletedOnce = player->GetCharacterSerializeComponent()->IsLevelCompleted(missionRequest.state.missionDifficulty);
	return missionRequest;
}

TOptional<FMissionRequest> UMissionRequestUtil::InspectPlayerStartMissionRequest(const APlayerCharacter* progressOwner, const FMissionSelection& selection) {
	return MakeOptionalRequest(EMissionRequestType::Start, UMissionStateUtil::InspectProducedMissionState(progressOwner, selection));
}

TOptional<FMissionRequest> UMissionRequestUtil::ProducePlayerStartMissionRequest(APlayerCharacter* progressOwner, const FMissionSelection& selection) {
	return MakeOptionalRequest(EMissionRequestType::Start, UMissionStateUtil::ProduceMissionState(progressOwner, selection));
}

TOptional<FMissionRequest> UMissionRequestUtil::InspectPlayerContinueMissionRequest(const APlayerCharacter* progressOwner, ELevelNames mission) {
	return MakeOptionalRequest(EMissionRequestType::Continue, UMissionStateUtil::GetMissionState(progressOwner, mission));
}

const FMissionState& UMissionRequestUtil::GetMissionState(const FMissionRequest& request) {
	return request.getMissionState();
}

EMissionEditor UMissionRequestUtil::GetMissionRequestDesiredEditor(const FMissionRequest& request) {
	if (UMissionOfferingsUtil::QueryMissionOfferingsProblem(request.getMissionState()).IsSet()) {
		return EMissionEditor::Offerings;
	}
	return EMissionEditor::Difficulty;
}
