#include "PCHOnlineDungeonsSubsystem.h"
#include "DataChannelObserver.h"

#include "OnlineSubsystem.h"

DataChannelObserver::DataChannelObserver()
{

}

DataChannelObserver::~DataChannelObserver()
{

}

void DataChannelObserver::OnStateChange()
{
	UE_LOG_ONLINE(Log, TEXT("[WebRTC] DataChannelObserver::OnStateChange"));
}

void DataChannelObserver::OnMessage(const webrtc::DataBuffer& buffer)
{
	UE_LOG_ONLINE(Log, TEXT("[WebRTC] DataChannelObserver::OnMessage"));
}
