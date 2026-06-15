#pragma once

#include "CoreMinimal.h"
#include "OnlineAsyncTaskManagerDungeons.h"
#include "OnlineSessionSettings.h"
#include <atomic>
#include <vector>
#include "xsapiServicesInclude.h"

class OnlineAsyncTaskDungeonsQueryLobbies : public FOnlineAsyncTaskDungeons
{
public:
	OnlineAsyncTaskDungeonsQueryLobbies(FOnlineSubsystemDungeons* InSubsystem
		, TSharedPtr<FOnlineSessionSearch> InSearchSettings
		, int32 InSearchingPlayerNum, std::vector<XblMultiplayerActivityDetails> InSessions
	);
	virtual ~OnlineAsyncTaskDungeonsQueryLobbies() {}

	FString ToString() const override
	{
		return FString::Printf(TEXT("OnlineAsyncTaskDungeonsQueryLobbies bWasSuccessful: %d"), WasSuccessful());
	}

	void Tick() override;
	void Finalize() override;
	void TriggerDelegates() override;

private:
	void QuerySessions(const std::vector<XblMultiplayerActivityDetails>& sessions);
	
	FOnlineSubsystemDungeons* Subsystem;
	TSharedPtr<class FOnlineSessionSearch> SearchSettings;
	int32 SearchingPlayerNum;
	std::vector<XblMultiplayerActivityDetails> Sessions;
	std::atomic<int> PendingCalls;
};