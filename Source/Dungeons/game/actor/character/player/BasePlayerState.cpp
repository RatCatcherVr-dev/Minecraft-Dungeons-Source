#include "Dungeons.h"
#include "BasePlayerState.h"
#include "UnrealNetwork.h"
#include <OnlineSubsystem.h>
#include <OnlineSubsystemUtils.h>
#include "online/sessions/OnlineUtil.h"
#include "online/crossplay/CrossplayOSS.h"
#include "DungeonsUserManagement.h"
#include "DungeonsGameInstance.h"
#include "Engine/LocalPlayer.h"
#include "game/actor/character/player/DungeonsLocalPlayer.h"

ABasePlayerState::ABasePlayerState() {
	bReplicates = true;
	bAlwaysRelevant = true;
	bLocallyDroppingIn = false;
}
void ABasePlayerState::BeginPlay() {
	Super::BeginPlay();
	RefreshDisplayName();
	RefreshSecondaryUniqueId();
	RefreshPlayerPlatform();
}

void ABasePlayerState::SetOwner(AActor* netOwner) {
	Super::SetOwner(netOwner);
	RefreshDisplayName();
	RefreshSecondaryUniqueId();
	RefreshPlayerPlatform();
}


bool ABasePlayerState::IsDisplayNameAssigned() const {
	return mDisplayNameAssigned;
}

void ABasePlayerState::ClientInitialize(AController* C) {
	Super::ClientInitialize(C);
	RefreshDisplayName();	
	RefreshSecondaryUniqueId();
	RefreshPlayerPlatform();
}

void ABasePlayerState::RefreshDisplayName() {
	if (GetOwner()) {
		if (auto controller = Cast<AController>(GetOwner())) {
			if (controller->IsLocalController()) {

				UDungeonsUserManager* userManagement = GetWorld()->GetGameInstance<UDungeonsGameInstance>()->GetUserManager();

				ULocalPlayer* localPlayer = Cast<ULocalPlayer>(controller->GetNetOwningPlayer());

				int32 controllerID = (localPlayer ? localPlayer->GetControllerId() : userManagement->GetInitialUser());

				FString primaryName = userManagement->GetLocalUserName(controllerID);
				FString secondaryName = online::getIdentityInterface()->GetSecondaryPlayerNickname(controllerID);

				SetPlayerDisplayName(primaryName, secondaryName);
				ServerAssignDisplayName(primaryName, secondaryName);
			}
		}
	}
}

void ABasePlayerState::RefreshSecondaryUniqueId() {
	if (GetOwner()) {
		if (auto controller = Cast<AController>(GetOwner())) {
			if (controller->IsLocalController()) {
				if (online::getCrossplayOss()->IsDungeonsActive() && online::getCrossplayOss()->IsPS4Active() && UniqueId.IsValid()) {
					auto id = online::getIdentityInterface()->GetUniqueNetIdFromType(*UniqueId.GetUniqueNetId().Get(), SubsystemType::Dungeons);
					FUniqueNetIdRepl secondaryUniqueId;
					secondaryUniqueId.SetUniqueNetId(id);
					SetSecondaryUniqueId(secondaryUniqueId);
					ServerAssignSecondaryUniqueId(secondaryUniqueId);
				}
			}
		}
	}
}

void ABasePlayerState::RefreshPlayerPlatform() {
	if (GetOwner()) {
		if (auto controller = Cast<AController>(GetOwner())) {
			if (controller->IsLocalController()) {
				auto platform = UGameVersion::GetPlatformEnum();
				SetPlayerPlatform(platform);
				ServerAssignPlayerPlatform(platform);
			}
		}
	}
}


bool ABasePlayerState::ServerAssignDisplayName_Validate(const FString& primaryName, const FString& secondaryName) {
	return true;
}

void ABasePlayerState::ServerAssignDisplayName_Implementation(const FString& primaryName, const FString& secondaryName) {
	SetPlayerDisplayName(primaryName, secondaryName);
}

bool ABasePlayerState::ServerAssignPlayerPlatform_Validate(EPlatformType platform) {
	return true;
}

void ABasePlayerState::ServerAssignPlayerPlatform_Implementation(EPlatformType platform) {
	SetPlayerPlatform(platform);
}

void ABasePlayerState::SetPlayerDisplayName(const FString& primaryName, const FString& secondaryName) {
	mDisplayName = primaryName;
	mSecondaryDisplayName = secondaryName;
	mDisplayNameAssigned = true;
	OnRep_DisplayName();
}

FString ABasePlayerState::GetPlayerDisplayName() const {
	return online::shouldShowSecondaryName(mSecondaryDisplayName) ? mSecondaryDisplayName : mDisplayName;
}

FString ABasePlayerState::GetPlayerPrimaryDisplayName() const {
	return mDisplayName;
}

FString ABasePlayerState::GetPlayerSecondaryDisplayName() const {
	return mSecondaryDisplayName;
}

void ABasePlayerState::OnRep_DisplayName() {
	OnPlayerNameChanged.Broadcast(this);
}

void ABasePlayerState::OnRep_SecondaryDisplayName() {
	OnPlayerNameChanged.Broadcast(this);
}

void ABasePlayerState::OnRep_PlayerPlatform() {
	OnPlayerPlatformChanged.Broadcast();
}

void ABasePlayerState::OnRep_OwnedByHost() {
	OnPlayerIsHostChanged.Broadcast();
}

void ABasePlayerState::OnRep_SetReady() {
	OnReadyChanged.Broadcast();
}

void ABasePlayerState::SetOwnedByHost_Implementation(bool owned) {		
	OwnedByHost = owned;
	OnPlayerIsHostChanged.Broadcast();
}

bool ABasePlayerState::SetOwnedByHost_Validate(bool owned) {
	return true;
}

void ABasePlayerState::SetReady_Implementation(bool ready) {
	mReady = ready;
	OnRep_SetReady();
}

bool ABasePlayerState::SetReady_Validate(bool ready) {
	return true;
}

bool ABasePlayerState::IsReady() const {
	return mReady;
}

void ABasePlayerState::SetRespawnSeconds(float seconds) {
	RespawnSeconds = seconds;
}

float ABasePlayerState::GetRespawnSeconds() const {
	return RespawnSeconds;
}

void ABasePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABasePlayerState, mDisplayNameAssigned);
	DOREPLIFETIME(ABasePlayerState, mDisplayName);
	DOREPLIFETIME(ABasePlayerState, mSecondaryDisplayName);
	DOREPLIFETIME(ABasePlayerState, RespawnSeconds);
	DOREPLIFETIME(ABasePlayerState, OwnedByHost);
	DOREPLIFETIME(ABasePlayerState, mReady);
	DOREPLIFETIME(ABasePlayerState, PlayerNumber);
	DOREPLIFETIME(ABasePlayerState, SecondaryUniqueId);
	DOREPLIFETIME(ABasePlayerState, PlayerPlatform);
}

void ABasePlayerState::SetPlayerNumber(int number) {
	PlayerNumber = number;
	OnRep_PlayerNumber();
}

int ABasePlayerState::GetPlayerNumber() const {
	return PlayerNumber;
}

void ABasePlayerState::SetPlayerPlatform(EPlatformType platformToSet) {
	PlayerPlatform = platformToSet;
	OnRep_PlayerPlatform();
}

EPlatformType ABasePlayerState::GetPlayerPlatform() const {
	return PlayerPlatform;
}

void ABasePlayerState::OnRep_PlayerNumber() {
	OnPlayerNumberChanged.Broadcast();
}

void ABasePlayerState::CopyProperties(APlayerState* PlayerState) {
	Super::CopyProperties(PlayerState);

	if(auto basePlayerState = Cast<ABasePlayerState>(PlayerState))
	{
		basePlayerState->PlayerNumber = PlayerNumber;
		basePlayerState->PlayerPlatform = PlayerPlatform;
	}
}

void ABasePlayerState::SetSecondaryUniqueId(const FUniqueNetIdRepl& InUniqueId) {
	SecondaryUniqueId = InUniqueId;
}

void ABasePlayerState::RegisterPlayerWithSession(bool bWasFromInvite) {
	if (GetNetMode() != NM_Standalone)
	{
		if (UniqueId.IsValid()) // May not be valid if this is was created via DebugCreatePlayer
		{
			// Register the player as part of the session
			const APlayerState* PlayerState = GetDefault<APlayerState>();
			online::getSessionInterface()->RegisterPlayer(PlayerState->SessionName, *UniqueId, bWasFromInvite);
		}
	}
}

void ABasePlayerState::UnregisterPlayerWithSession() {
	if (GetNetMode() == NM_Client && UniqueId.IsValid())
	{
		const APlayerState* PlayerState = GetDefault<APlayerState>();
		if (PlayerState->SessionName != NAME_None)
		{
			online::getSessionInterface()->UnregisterPlayer(PlayerState->SessionName, *UniqueId);
		}
	}
}


bool ABasePlayerState::ServerAssignSecondaryUniqueId_Validate(const FUniqueNetIdRepl& InUniqueId) {
	return true;
}

void ABasePlayerState::ServerAssignSecondaryUniqueId_Implementation(const FUniqueNetIdRepl& InUniqueId) {
	SetSecondaryUniqueId(InUniqueId);
}
