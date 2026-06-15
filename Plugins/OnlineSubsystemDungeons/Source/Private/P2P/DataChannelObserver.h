#pragma once

#include "WebRTCCommon.h"

class DataChannelObserver : public webrtc::DataChannelObserver
{
public:

	DataChannelObserver();
	~DataChannelObserver();


	virtual void OnStateChange() override;


	virtual void OnMessage(const webrtc::DataBuffer& buffer) override;

private:

};