#pragma once

#include "WebRTCCommon.h"


class PeerConnectionHandler : public webrtc::PeerConnectionObserver 
{	

public:
	virtual void OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState new_state) override;
	virtual void OnDataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel) override;
	virtual void OnRenegotiationNeeded() override;
	virtual void OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState new_state) override;
	virtual void OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState new_state) override;
	virtual void OnIceCandidate(const webrtc::IceCandidateInterface* candidate) override;
};