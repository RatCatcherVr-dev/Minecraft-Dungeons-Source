#pragma once
#include "RestoreState.h"


class ReconnectStates {
public:
	void Connect(const FString& guid);
	void Disconnect(const FString& guid, const TOptional<RestoreState>&);	
	
	bool CanReconnect(const FString& guid) const;
	bool IsReconnect(const FString& guid) const;
	
	void ClearAll();
	void ClearOnlyRestoreStates();
	void ClearRestoreState(const FString& guid);
	void CompleteReconnect(const FString& guid);

	TOptional<RestoreState> GetRestoreState(const FString& guid) const;

	TArray<FString> GetGuids() const;

	void KickPlayer(const FString& guid);
	bool IsKicked(const FString& guid);
	void RemoveKick(const FUniqueNetIdWrapper&);
	
private:
	struct ReconnectState {
		static int count;

		int index;
		FString Guid;		
		TOptional<RestoreState> RestoreData;
		double DisconnectTimeSec = 0;
		bool isReconnect = false;

		void Connect();
		void Disconnect(const TOptional<RestoreState>&);
		bool CanReconnect() const;

		ReconnectState(const FString& guid);
	};
	struct KickInfo {
		FString Guid;
		int64 KickTime = 0;
		KickInfo(const FString& guid, int64 kickTime);
	};
	void UpdateKickedPlayers();
	int Find(const FString& guid) const;
	TArray<ReconnectState> ClientStates;
	TArray<KickInfo> Kicked;
	const int KickedTimeoutSeconds = 120;
};
