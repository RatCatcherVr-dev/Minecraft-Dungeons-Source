#pragma once

#include "CoreMinimal.h"
#include "OnlineAsyncTaskManager.h"


class FOnlineAsyncTaskDungeons : public FOnlineAsyncTaskBasic<class FOnlineSubsystemDungeons>
{


public:

	FOnlineAsyncTaskDungeons() 
	{}

	virtual ~FOnlineAsyncTaskDungeons() {}

	/**
	 *	Get a human readable description of task
	*/
	virtual FString ToString() const override
	{
		return FString::Printf(TEXT("FOnlineAsyncTaskDungeons bWasSuccessful: %d"), WasSuccessful());
	}

	virtual void Tick() override
	{

	}
	
	virtual void Finalize() override
	{
		
	}

	virtual void TriggerDelegates() override 
	{

	}
};

/**
 * Dungeons version of the async task manager to register the various live callbacks with the engine
 */
class FOnlineAsyncTaskManagerDungeons : public FOnlineAsyncTaskManager
{
protected:

	/** Cached reference to the main online subsystem */
	class FOnlineSubsystemDungeons* DungeonsSubsystem;

public:

	FOnlineAsyncTaskManagerDungeons(class FOnlineSubsystemDungeons* InOnlineSubsystem)
		: DungeonsSubsystem(InOnlineSubsystem)
	{}

	~FOnlineAsyncTaskManagerDungeons() {}

	// FOnlineAsyncTaskmanager
	virtual void OnlineTick() override;
};