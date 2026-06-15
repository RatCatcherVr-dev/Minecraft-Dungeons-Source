#include "Dungeons.h"
#include "MissionSelectorComponent.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "game/actor/character/player/BasePlayerState.h"
#include "UserWidget.h"
#include "DungeonsGameInstance.h"
#include "DungeonsGameState.h"
#include "game/util/DungeonsTravelUtil.h"
#include "game/mission/MissionSelection.h"
#include "game/mission/state/MissionState.h"
#include "game/mission/request/MissionRequest.h"
#include "game/mission/request/MissionRequestUtil.h"
#include "LoadingScreen/LoadingScreenInitializer.h"


namespace missionSelector {

FLevelSettings makeMission(UWorld* world, const FMissionState& state) {

	if (missions::get(state.getLevelName()).isTutorial()) {
		//Ignore difficulty and threat level!
		return levelsettingsutil::generateTutorialSettings(world);
	} 	
	return levelsettingsutil::generateMissionSettings(world, state);
}

}

void UMissionSelectorComponent::BeginPlay() {
	Super::BeginPlay();
	if (IsInLobby()) {
		if (auto playerController = static_cast<ABasePlayerController*>(GetOwner())) {
			playerController->OnServerLogout.AddUniqueDynamic(this, &UMissionSelectorComponent::HandleLogout);
		}
		if (GetOwner()->HasAuthority()) {
			for (auto&& lobby : TActorRange<ALobbyActor>(GetWorld())) {
				mLobbyActor = lobby;
				mLobbyActor->OnMissionSelectionTimedOut.AddUObject(this, &UMissionSelectorComponent::SelectionTimedOut);
				break;
			}
		}
	}
}

void UMissionSelectorComponent::SelectionTimedOut() {
	InternalReject();
}

void UMissionSelectorComponent::HandleLogout() {
	if (GetWorld()->IsServer() && mLobbyActor && mLobbyActor->IsRequestingLevel()) {
		InternalReject();
	}
}

void UMissionSelectorComponent::TravelToMission() {
	if(GetWorld()->IsServer() && mLobbyActor){
		for (auto participant : GetMissionSelectors()) {
			if (auto PlayerState = Cast<ABasePlayerState>(Cast<APlayerController>(participant->GetOwner())->PlayerState)) {
				PlayerState->SetReady(false); //Reset ready for all
			}
		}

		auto gameInstance = GetGameInstance();

		const FLevelRequest& request = mLobbyActor->GetRequest();

		if (!gameInstance->IsDedicatedServerInstance()) {
			gameInstance->BeginLoadingScreenWithTravel(request.LevelSettings, EMapLoadType::TravelIngameServer, UDungeonsGameInstance::fDefaultFadeOutTime, UDungeonsGameInstance::fDefaultFadeInTime);
		} else {
			travelutil::ServerTravelToGameMap(GetWorld(), request.LevelSettings);
		}
	}

	GetWorld()->GetTimerManager().ClearTimer(TravelTimerHandle);
}

bool UMissionSelectorComponent::_ServerInternalAccept() {
	check(GetWorld()->IsServer());

	if (auto PlayerState = Cast<ABasePlayerState>(Cast<APlayerController>(GetOwner())->PlayerState)) {
		PlayerState->SetReady(true);
	}

	if (TravelTimerHandle.IsValid() && GetWorld()->GetTimerManager().IsTimerActive(TravelTimerHandle)) {
		return false;
	}

	if (mLobbyActor && mLobbyActor->TryAccept(GetNumPlayers())) {
		for (auto participant : GetMissionSelectors()) {
			participant->ClientRequestFinished();			
		}

		const FLevelRequest Request = mLobbyActor->GetRequest();
		for (auto participant : GetMissionSelectors()) {
			participant->ClientLevelStarting(Request.LevelSettings);
		}

		if (GetMissionSelectors().Num() <= 1) {
			//Single player - no need for any delays
			TravelToMission();
		} else {
			//Delay to allow all players to see that all players have accepted the mission before starting it.		
			const float DELAY = 1.0f; 
			GetWorld()->GetTimerManager().SetTimer(TravelTimerHandle, this, &UMissionSelectorComponent::TravelToMission, DELAY, false, DELAY);
		}
		return true;
	}
	return false;
}

void UMissionSelectorComponent::InternalReject() {
	if(GetWorld()->IsServer()){
		mLobbyActor->Reject();
		for (auto participant : GetMissionSelectors()) {
			participant->ClientRequestFinished();
			if (auto PlayerState = Cast<ABasePlayerState>(Cast<APlayerController>(participant->GetOwner())->PlayerState)) {
				PlayerState->SetReady(false); //Server resets ready state for all
			}
		}
	}
}

bool UMissionSelectorComponent::IsInLobby() {	
	if(auto state = Cast<ADungeonsGameState>(GetWorld()->GetGameState())){
		return state->IsLobby();
	}
	return false;
}

TArray<UMissionSelectorComponent*> UMissionSelectorComponent::GetMissionSelectors() const {
	TArray<UMissionSelectorComponent*> out;
	for (auto&& player : TActorRange<ABasePlayerController>(GetWorld())) {
		auto comp = player->FindComponentByClass<UMissionSelectorComponent>();
		if (comp) {
			out.Add(comp);
		}
	}
	return out;
}

int32 UMissionSelectorComponent::GetNumPlayers() {
	int numControllers = 0;
	for (auto&& player : TActorRange<ABasePlayerController>(GetWorld())) {
		numControllers++;
	}
	return numControllers;
}

bool UMissionSelectorComponent::CanRequest() const {
	if (GetGameInstance()->IsLoadingScreenActive()) {
		return false;
	}
	if (mLobbyActor && mLobbyActor->IsRequestingLevel()) {
		return false;
	}
	if (TravelTimerHandle.IsValid() && GetWorld()->GetTimerManager().IsTimerActive(TravelTimerHandle)) {
		return false;
	}
	return true;
}

bool UMissionSelectorComponent::TryStartMission(APlayerCharacter* RequestingPlayer, const UMissionSelectorComponent::MissionRequestProvider& requestProvider) {
	if (!RequestingPlayer) {
		return false;
	}
	if (!CanRequest()) {
		return false;
	}

	TOptional<FMissionRequest> missionRequest = requestProvider();
	if (!missionRequest) {
		return false;
	}

	ServerRequestLevel(RequestingPlayer, missionRequest.GetValue());
	return true;
}

bool UMissionSelectorComponent::TryStartMissionSelection(APlayerCharacter* RequestingPlayer, const FMissionSelection& missionSelection) {
	return TryStartMission(RequestingPlayer, [&](){
		return UMissionRequestUtil::ProducePlayerStartMissionRequest(RequestingPlayer, missionSelection);
	});
}

bool UMissionSelectorComponent::TryContinueMission(APlayerCharacter* RequestingPlayer, ELevelNames levelName) {
	return TryStartMission(RequestingPlayer, [&]() {
		return UMissionRequestUtil::InspectPlayerContinueMissionRequest(RequestingPlayer, levelName);
	});
}

void UMissionSelectorComponent::ServerRequestLevel_Implementation(APlayerCharacter* requesterPlayer, const FMissionRequest& missionRequest) {
	auto gameInstance = Cast<UDungeonsGameInstance>(GetWorld()->GetGameInstance());
	if ( !gameInstance->IsLoadingScreenActive() && !mLobbyActor->IsRequestingLevel() && (!TravelTimerHandle.IsValid() || !GetWorld()->GetTimerManager().IsTimerActive(TravelTimerHandle))) {
		auto playerId = Cast<ABasePlayerController>(GetOwner())->GetPlayerId();
		auto levelSettings = missionSelector::makeMission(GetWorld(), missionRequest.getMissionState());
		const FLevelRequest& Request = mLobbyActor->RequestLevel(playerId, requesterPlayer, levelSettings);
		if (!_ServerInternalAccept()) {
			for (auto participant : GetMissionSelectors()) {
				participant->ClientLevelRequested(Request);
			}
		}
	}
}


bool UMissionSelectorComponent::ServerRequestLevel_Validate(APlayerCharacter* requesterPlayer, const FMissionRequest& missionSelection) {
	return true;
}

void UMissionSelectorComponent::ServerAccept_Implementation() {
	_ServerInternalAccept();
}

bool UMissionSelectorComponent::ServerAccept_Validate() {
	return true;
}

void UMissionSelectorComponent::ServerReject_Implementation() {
	InternalReject();
}

bool UMissionSelectorComponent::ServerReject_Validate() {
	return true;
}

void UMissionSelectorComponent::ClientLevelRequested_Implementation(const FLevelRequest& Request) {
	auto playerId = Cast<ABasePlayerController>(GetOwner())->GetPlayerId();
	OnLevelRequested.Broadcast(Request, playerId == Request.PlayerId);
}

void UMissionSelectorComponent::ClientLevelStarting_Implementation(FLevelSettings levelSettings) {
	auto gameInstance = GetGameInstance();
	gameInstance->Configuration.PrepareTravel(levelSettings);
	OnLevelStarting.Broadcast();
	if (!GetOwner()->HasAuthority()) {
		gameInstance->BeginLoadingScreenWithTravel(levelSettings, EMapLoadType::TravelIngameClient, UDungeonsGameInstance::fDefaultFadeOutTime, UDungeonsGameInstance::fDefaultFadeInTime);
	}
}

void UMissionSelectorComponent::ClientRequestFinished_Implementation() {
	OnLevelRequestFinished.Broadcast();
}

UDungeonsGameInstance* UMissionSelectorComponent::GetGameInstance() const {
	return Cast<UDungeonsGameInstance>(GetWorld()->GetGameInstance());
}
