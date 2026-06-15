#pragma once

#include "CoreMinimal.h"
#include "OnlineAsyncTaskManagerDungeons.h"
#include "xsapiServicesInclude.h"

class FOnlineSessionSearch;
class FSocketSubsystemDungeons;

class OnlineAsyncTaskDungeonsFindLobbies : public FOnlineAsyncTaskDungeons
{
	FOnlineSubsystemDungeons* Subsystem;
	TSharedPtr<class FOnlineSessionSearch> SearchSettings;
	int32 SearchingPlayerNum;

public:
	OnlineAsyncTaskDungeonsFindLobbies(FOnlineSubsystemDungeons* InSubsystem
		, TSharedPtr<FOnlineSessionSearch> InSearchSettings
		, int32 InSearchingPlayerNum
	);

	virtual ~OnlineAsyncTaskDungeonsFindLobbies () {}

	FString ToString() const override
	{
		return FString::Printf(TEXT("OnlineAsyncTaskDungeonsFindLobbies bWasSuccessful: %d"), WasSuccessful());
	}

	void Tick() override;

	void Finalize() override;

	void TriggerDelegates() override;

private:
	void Fail();
};