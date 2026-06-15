#pragma once

#include "CoreMinimal.h"

#include "WebRTCCommon.h"
#include "Utils.h"
#include <mutex>
#include "Peer2PeerManager.h"
#include "SessionDescriptors.h"
#include "OnlineSubsystemDungeons.h"
#include "OnlineSessionInterface.h" //Must explicitly be included to get SUBSYSTEM_HAS_TELEMETRY_CALLBACKS

struct WebRTCConfig;
class SignalThread;

class WebRTCSessionHandler : public webrtc::PeerConnectionObserver, public rtc::MessageHandler
{
public:

	WebRTCSessionHandler(FOnlineSubsystemDungeons* InDungeonsSubsystem
		, const FString& InLocalXUID
		, const FString& InRemoteXUID
		, bool InIsOutgoing
		, Peer2PeerManager* InPeer2PeerManager
		, WebRTCConfig IceServerConfiguration
		, SignalThread *InSigThread
		, int32 InUniquePort);
	virtual ~WebRTCSessionHandler();

	/* Connection handling */
	void InitiateOutGoing();
	void InitiateIncoming(const FString& WebRTCConnectionInfo);
	void ProcessConnectResponse(const FString& ConnectResponse);
	void ProcessCandidateAdd(const FString& WebRTCCandidate);
	void UpdateDataChannelState();
	bool IsOutgoingConnection() { return IsOutgoing; }

	/* Send/Recv */
	void SendPacket(const uint8* Data, unsigned int Size);
	void OnPacketReceived(const void* Data, size_t Size);

	/* Activity/Cleanup */
	void UpdateSessionActivity() { LastSessionActivity = TimerUtils::GetCurrentTime(); }
	bool IsInactive();
	bool IsDead();

	/* Stats */
	void OnConnectionStatsComplete(const webrtc::StatsReports& Reports);

	/* Update RTCConfig */
	void UpdateRelayServerInfo(webrtc::PeerConnectionInterface::IceServers Servers);

	/* Overrides */
	virtual void OnMessage(rtc::Message* msg) override;
	virtual void OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState new_state) override;
	virtual void OnDataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel) override;
	virtual void OnRenegotiationNeeded() override;
	virtual void OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState new_state) override;
	virtual void OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState new_state) override;
	virtual void OnIceCandidate(const webrtc::IceCandidateInterface* candidate) override;

private:

	static void CreateOffer(const SessionDescriptionInfo& Info);
	static void CreateAnswer(const SessionDescriptionInfo& Info);

	void ProcessIceCandidates();
	void Cleanup();

	enum ENegotiationState
	{
		ENegotiationState_None,
		ENegotiationState_WaitingForResponse,
		//eNegotiationState_WaitingForAccept,
		ENegotiationState_ICEProcessing
	};
	ENegotiationState NegotiationState;
	unsigned long long NegotiationStateTimer;

	void EnterNewNegotiationState(ENegotiationState InNegotiationState)
	{
		NegotiationState = InNegotiationState;
		NegotiationStateTimer = TimerUtils::GetCurrentTime();
	}
	unsigned long long GetTimeInNegotiationState()
	{
		return TimerUtils::GetTimeSince(NegotiationStateTimer);
	}

	FOnlineSubsystemDungeons* DungeonsSubsystem;
	Peer2PeerManager* Peer2PeerMgr;
	/* WebRTC */
	SignalThread* SigThread;
	std::mutex RTCConfigLock;
	webrtc::PeerConnectionInterface::RTCConfiguration RTCConfig;
	rtc::scoped_refptr<webrtc::PeerConnectionInterface> PeerConnection;
	rtc::scoped_refptr<webrtc::StatsObserver> StatsObserver;
	std::atomic<unsigned long long> LastSessionActivity;
	unsigned long long LastStatsUpdate;
	bool StatsRequestInProgress;

	rtc::scoped_refptr<webrtc::DataChannelInterface> DataChannel;
	webrtc::DataChannelInterface::DataState DataChannelState;
	
	webrtc::PeerConnectionInterface::IceConnectionState CurrentIceState;

	/* Candidates */
	std::vector<std::unique_ptr<webrtc::IceCandidateInterface>> DeferredIceCandidates;
	std::mutex DefferedIceCandidatesMutex;

	bool IsOutgoing;
	FString LocalXUID;
	FString RemoteXUID;
	int32 UniquePort;

	void CheckUpdates();
};

class P2PDataChannelObserver : public webrtc::DataChannelObserver
{
public:

	P2PDataChannelObserver(WebRTCSessionHandler* InSessionHandler) 
		: SessionHandler(InSessionHandler)
	{}
	~P2PDataChannelObserver() {}

	virtual void OnStateChange() override;
	virtual void OnMessage(const webrtc::DataBuffer& buffer) override;

private:

	WebRTCSessionHandler* SessionHandler;

};

class P2PStatsObserver : public rtc::RefCountedObject<webrtc::StatsObserver>
{
public:
	P2PStatsObserver(WebRTCSessionHandler* InSessionHandler, FString InRemoteXUID)
		:  SessionHandler(InSessionHandler), RemoteXUID(InRemoteXUID)
	{}

	virtual void OnComplete(const webrtc::StatsReports& reports) override;

private:
	WebRTCSessionHandler* SessionHandler;
	FString RemoteXUID;
};
