#include "Dungeons.h"
#include "ReconnectStates.h"

int ReconnectStates::ReconnectState::count = 0;

void ReconnectStates::ReconnectState::Connect() {	
	isReconnect = true;
}

void ReconnectStates::ReconnectState::Disconnect(const TOptional<RestoreState>& restoreState) {
	RestoreData = restoreState;
	DisconnectTimeSec = FApp::GetCurrentTime();
	isReconnect = false;
}

bool ReconnectStates::ReconnectState::CanReconnect() const {
	if (DisconnectTimeSec <= 0) {
		return true;
	}
	static const double ReconnectAvailableSec = 5 * 60;
	return FApp::GetCurrentTime() - DisconnectTimeSec < ReconnectAvailableSec;
}

ReconnectStates::ReconnectState::ReconnectState(const FString& guid):
	index(count++), 
	Guid(guid) {
}

void ReconnectStates::Connect(const FString& guid) {
	const auto i = Find(guid);
	if (i == INDEX_NONE) {
		ReconnectState clientInfo(guid);
		ClientStates.Add(std::move(clientInfo));
		UE_LOG(LogMultiplayer, Log, TEXT("Reconnect: New player '%s' connected."), *guid);
	} else {
		UE_LOG(LogMultiplayer, Log, TEXT("Reconnect: Existing player '%s' reconnected."), *guid);
		ClientStates[i].Connect();
	}
}

TArray<FString> ReconnectStates::GetGuids() const {
	TArray<FString> r;
	for (auto rs : ClientStates) {
		r.Add(rs.Guid);
	}
	return r;
}

bool ReconnectStates::CanReconnect(const FString& guid) const {
	const auto index = Find(guid);
	if (index == INDEX_NONE) {
		UE_LOG(LogMultiplayer, Warning, TEXT("Reconnect: Unknown player '%s' requests to reconnect."), *guid);
		return false;
	}
	return ClientStates[index].CanReconnect();
}

bool ReconnectStates::IsReconnect(const FString& guid) const {
	const auto i = Find(guid);
	if (i == INDEX_NONE) {
		UE_LOG(LogMultiplayer, Log, TEXT("Reconnect: Checking if player controller has reconnected, guid: %s is not registered"), *guid);
		return false;
	}
	return ClientStates[i].isReconnect;
}

void ReconnectStates::ClearAll() {
	UE_LOG(LogMultiplayer, Log, TEXT("Reconnect: Clearing all client info."));
	ClientStates.Empty();
}

void ReconnectStates::ClearOnlyRestoreStates(){
	UE_LOG(LogMultiplayer, Log, TEXT("Reconnect: Clearing all client restore state data."));
	for (auto&& state : ClientStates) {
		state.RestoreData.Reset();
	}
}

void ReconnectStates::ClearRestoreState(const FString& guid) {
	const auto i = Find(guid);
	if (i == INDEX_NONE) {
		UE_LOG(LogMultiplayer, Log, TEXT("Reconnect: Clearing restore state for unknown guid: %s"), *guid);
		return;
	}
	UE_LOG(LogMultiplayer, Log, TEXT("Reconnect: Clearing restore state data for %s"), *guid);
	return ClientStates[i].RestoreData.Reset();
}

void ReconnectStates::CompleteReconnect(const FString& guid) {
	const auto i = Find(guid);
	if (i == INDEX_NONE) {
		UE_LOG(LogMultiplayer, Log, TEXT("Reconnect: Trying to complete reconnect state for unknown guid: %s"), *guid);
		return;
	}
	if (ClientStates[i].isReconnect) {
		UE_LOG(LogMultiplayer, Log, TEXT("Reconnect: Complete reconnect for %s"), *guid);
		ClientStates[i].isReconnect = false;
	}
}

void ReconnectStates::Disconnect(const FString& guid, const TOptional<RestoreState>& restoreState) {
	const auto index = Find(guid);
	if (index == INDEX_NONE) {
		UE_LOG(LogMultiplayer, Warning, TEXT("Reconnect: Disconnecting a guid that doesn't exist: %s"), *guid);
		return;
	}
	UE_LOG(LogMultiplayer, Log, TEXT("Reconnect: Disconnecting setting restore state for guid: %s"), *guid);
	ClientStates[index].Disconnect(restoreState);
}

TOptional<RestoreState> ReconnectStates::GetRestoreState(const FString& guid) const {
	const auto index = Find(guid);
	if (index == INDEX_NONE) {
		UE_LOG(LogMultiplayer, Warning, TEXT("Reconnect: Getting the restore state for a guid that doesn't exist: %s"), *guid);
		return TOptional<RestoreState>();
	}
	return ClientStates[index].RestoreData;
}

int ReconnectStates::Find(const FString& guid) const {
	return ClientStates.IndexOfByPredicate([&guid](const ReconnectState& c) { return guid == c.Guid; });
}

ReconnectStates::KickInfo::KickInfo(const FString& guid, int64 kickTime) : 
	Guid(guid), KickTime(kickTime) {
}

void ReconnectStates::KickPlayer(const FString& guid) {
	Kicked.Add(KickInfo(guid, FApp::GetCurrentTime()));
}

bool ReconnectStates::IsKicked(const FString& guid) {
	UpdateKickedPlayers();
	return Kicked.ContainsByPredicate([guid](const KickInfo& ki)->bool {return ki.Guid.Contains(guid);});
}

void ReconnectStates::RemoveKick(const FUniqueNetIdWrapper& netId) {
	UpdateKickedPlayers();
	Kicked.RemoveAll([netId](const KickInfo& ki)->bool {return !ki.Guid.Compare(netId->ToString()); });
}

void ReconnectStates::UpdateKickedPlayers() {
	int64 now = FApp::GetCurrentTime();
	Kicked.RemoveAll([now, Timeout=KickedTimeoutSeconds](const KickInfo& ki)->bool {return now - ki.KickTime >= Timeout; });
}