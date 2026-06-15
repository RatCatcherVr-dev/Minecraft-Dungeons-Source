#pragma once

#include "util/Timer.h"

class UWorld;
class AMobCharacter;


/**
* Helper class thread for processing behaviors
*/
class FBehaviorSystemRunnable : public FRunnable
{
public:

	enum
	{
		EBehaviorSystemThreadPoolMax = 4
	};

	/** Default constructor. */
	FBehaviorSystemRunnable()
		:
		mTargetMode(false)
	{
		mRunning = true;
		mEvent = FPlatformProcess::GetSynchEventFromPool();
		mThread = FRunnableThread::Create(this, TEXT("BehaviorSystemThread"), 8 * 1024, TPri_Normal);
	}

	~FBehaviorSystemRunnable()
	{
		mRunning = false;
		FPlatformMisc::MemoryBarrier();

		if(mThread)
		{
			mThread->Kill(true);
			delete mThread;
		}

		FPlatformProcess::ReturnSynchEventToPool(mEvent);
		mEvent = nullptr;
	}

	void SetTargetMode(bool bVal)
	{
		mTargetMode = bVal;
		FPlatformMisc::MemoryBarrier();
	}

	void AddProcessMob(AMobCharacter* pMob);
	bool CompletedProcessingMobs()				{ return mTargetMobQueue.IsEmpty(); }

protected:

	bool Init() override
	{
		return true;
	}

	void Stop() override
	{
		FPlatformMisc::MemoryBarrier();
	}

	uint32 Run() override;

	/** Holds the thread we are running on. */
	FRunnableThread*			mThread;

	bool						mRunning;
	bool						mTargetMode;
	TQueue< AMobCharacter* >	mTargetMobQueue;

	FEvent*						mEvent;

};



class UBehaviorSystem {
public:
	UBehaviorSystem(UWorld&);
	~UBehaviorSystem();

	void Update();
	void Update_ASync();
	void Update_Sync();

	
	void SetPaused(bool);
private:
	
	bool ShouldTickMob(AMobCharacter&);

	UWorld& mWorld;
	int mTickId;
	Timer mTimer;
	bool mPaused = false;
	
	TArray< FBehaviorSystemRunnable* >								mBehaviourSystemThreads;
};
