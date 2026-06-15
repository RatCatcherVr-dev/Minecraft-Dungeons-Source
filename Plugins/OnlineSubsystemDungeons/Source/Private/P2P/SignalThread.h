#pragma once

#include "WebRTCCommon.h"

class ExecutableMessageData : public rtc::MessageData
{
public:
	virtual void Execute() = 0;
};

template <class FunctorT>
class FunctorMessageData : public ExecutableMessageData
{
public:
	explicit FunctorMessageData(const FunctorT& InFunctor)
		: Functor(InFunctor) {}

	virtual ~FunctorMessageData()
	{

	}

	virtual void Execute()
	{
		Functor();
	}

private:
	FunctorT Functor;
};

class SignalThread
{
public:

	SignalThread() {
		mpSignalThread = nullptr;
	}
	virtual ~SignalThread() {}

	/*Inteface functions */
	virtual void Initialize() = 0;
	virtual void Teardown() = 0;

	// InvokeOnSignalThread should NOT capture 'this' - as 'this' may be delayed before functor executes. Use InvokeMessageHandlerOnSignalThread
	template <class FunctorT>
	void InvokeOnSignalThread(const rtc::Location& PostedFrom, const FunctorT& Functor)
	{
		mpSignalThread->Post(PostedFrom, new rtc::FunctorMessageHandler<void, FunctorT>(Functor));
	}

	// InvokeOnSignalThread should NOT capture 'this' - as 'this' may be delayed before functor executes. Use InvokeMessageHandlerOnSignalThread
	template <class FunctorT>
	void InvokeDelayedOnSignalThread(const rtc::Location& PostedFrom, int DelayTimeMS, const FunctorT& Functor)
	{
		mpSignalThread->PostDelayed(PostedFrom, DelayTimeMS, new rtc::FunctorMessageHandler<void, FunctorT>(Functor));
	}

	// Invokes and clears a functor on the signal thread, using lambda syntax for simplicity
	template <class FunctorT>
	void InvokeMessageHandlerOnSignalThread(const rtc::Location& PostedFrom, rtc::MessageHandler* MsgHandler, const FunctorT& Functor)
	{
		mpSignalThread->Post(PostedFrom, MsgHandler, 0, new FunctorMessageData<FunctorT>(Functor));
	}

	void ClearMessageHandler(rtc::MessageHandler* MsgHandler)
	{
		mpSignalThread->Clear(MsgHandler);
	}
protected:
	std::unique_ptr<rtc::Thread> mpSignalThread;
};