#pragma once

#include <thread>

#include "WebRTCCommon.h"
#include "SignalThread.h"

class GenericSignalThread : public SignalThread
{
public:
	GenericSignalThread() {}

	virtual ~GenericSignalThread() {}

	virtual void Initialize() override;
	virtual void Teardown() override;
};