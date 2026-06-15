#include "PCHOnlineDungeonsSubsystem.h"
#include "WebRTCSessionHandler.h"

#include "DataChannelObserver.h"
#include "api/peer_connection_interface.h"
#include "OnlineSubsystem.h"

#include <mutex>

#include "P2P/SignalThread.h"

FString ToString(webrtc::PeerConnectionInterface::IceConnectionState state)
{
	switch (state)
	{
	case webrtc::PeerConnectionInterface::IceConnectionState::kIceConnectionNew: return "kIceConnectionNew";
	case webrtc::PeerConnectionInterface::IceConnectionState::kIceConnectionChecking: return "kIceConnectionChecking";
	case webrtc::PeerConnectionInterface::IceConnectionState::kIceConnectionConnected: return "kIceConnectionConnected";
	case webrtc::PeerConnectionInterface::IceConnectionState::kIceConnectionCompleted: return "kIceConnectionCompleted";
	case webrtc::PeerConnectionInterface::IceConnectionState::kIceConnectionFailed: return "kIceConnectionFailed";
	case webrtc::PeerConnectionInterface::IceConnectionState::kIceConnectionDisconnected: return "kIceConnectionDisconnected";
	case webrtc::PeerConnectionInterface::IceConnectionState::kIceConnectionClosed: return "kIceConnectionClosed";
	case webrtc::PeerConnectionInterface::IceConnectionState::kIceConnectionMax: return "kIceConnectionMax";
	default: return "Unknown";
	}
}

FString ToString(webrtc::PeerConnectionInterface::SignalingState state)
{
	switch (state)
	{
	case webrtc::PeerConnectionInterface::SignalingState::kStable: return "kStable";
	case webrtc::PeerConnectionInterface::SignalingState::kHaveLocalOffer: return "kHaveLocalOffer";
	case webrtc::PeerConnectionInterface::SignalingState::kHaveLocalPrAnswer: return "kHaveLocalPrAnswer";
	case webrtc::PeerConnectionInterface::SignalingState::kHaveRemoteOffer: return "kHaveRemoteOffer";
	case webrtc::PeerConnectionInterface::SignalingState::kHaveRemotePrAnswer: return "kHaveRemotePrAnswer";
	case webrtc::PeerConnectionInterface::SignalingState::kClosed: return "kClosed";
	default: return "Unknown";
	}
}

FString ToString(webrtc::PeerConnectionInterface::IceGatheringState state)
{
	switch (state)
	{
	case webrtc::PeerConnectionInterface::IceGatheringState::kIceGatheringNew: return "kIceGatheringNew";
	case webrtc::PeerConnectionInterface::IceGatheringState::kIceGatheringGathering: return "kIceGatheringGathering";
	case webrtc::PeerConnectionInterface::IceGatheringState::kIceGatheringComplete: return "kIceGatheringComplete";
	default: return "Unknown";
	}
}

WebRTCSessionHandler::WebRTCSessionHandler(FOnlineSubsystemDungeons* InDungeonsSubsystem
	, const FString& InLocalXUID
	, const FString& InRemoteXUID
	, bool InIsOutgoing
	, Peer2PeerManager* InPeer2PeerManager
	, WebRTCConfig IceServerConfiguration
	, SignalThread *InSigThread
	, int32 InUniquePort
)
	: DungeonsSubsystem(InDungeonsSubsystem)
	, Peer2PeerMgr(InPeer2PeerManager)
	, SigThread(InSigThread)
	, StatsRequestInProgress(false)
	, DataChannelState(webrtc::DataChannelInterface::DataState::kClosed)
	, CurrentIceState(webrtc::PeerConnectionInterface::kIceConnectionNew)
	, IsOutgoing(InIsOutgoing)
	, LocalXUID(InLocalXUID)
	, RemoteXUID(InRemoteXUID)
	, UniquePort(InUniquePort)
{

	UpdateSessionActivity();
	EnterNewNegotiationState(ENegotiationState_None);

	StatsObserver = new P2PStatsObserver(this, RemoteXUID);
	LastStatsUpdate = TimerUtils::GetCurrentTime();
	{
		std::lock_guard<std::mutex> ScopeLock(RTCConfigLock);

		RTCConfig.servers = IceServerConfiguration.Servers;
		if (IceServerConfiguration.OnlyRelay)
		{
			/* Do only use relay */
			RTCConfig.type = webrtc::PeerConnectionInterface::kRelay;
		}
	}
}

WebRTCSessionHandler::~WebRTCSessionHandler()
{
	UE_LOG_ONLINE(Log, TEXT("[WebRTC] Cleaning up WebRTCSession (dstr) XUID='%s'"), *RemoteXUID);
	Cleanup();
	SigThread->ClearMessageHandler(this);
}

void WebRTCSessionHandler::InitiateOutGoing()
{
	UpdateSessionActivity();

	/* Update state to waiting */
	EnterNewNegotiationState(ENegotiationState_WaitingForResponse);

	//No RTConfig lock needed here because we are never changing these values runtime
	RTCConfig.rtcp_mux_policy = webrtc::PeerConnectionInterface::kRtcpMuxPolicyRequire;
	RTCConfig.tcp_candidate_policy = webrtc::PeerConnectionInterface::kTcpCandidatePolicyDisabled; //Do not gather TCP candidates

#if PLATFORM_XBOXONE
	RTCConfig.preferred_candidate_udp_min_port = 10000;
	RTCConfig.preferred_candidate_udp_max_port = 10000 + 20;
#endif //PLATFORM_XBOXONE

#if PLATFORM_SWITCH || PLATFORM_PS4
	RTCConfig.disable_ipv6 = true;
#endif

	/* Setup Channel */
	if (!Peer2PeerMgr->GetPeerConnectionFactory())
	{
		UE_LOG_ONLINE(Warning, TEXT("[WebRTC] Error, PeerConnectionFactory is null"));
		EnterNewNegotiationState(ENegotiationState_None);
		return;
	}

	{
		std::lock_guard<std::mutex> LockGuard(RTCConfigLock);
		PeerConnection = Peer2PeerMgr->GetPeerConnectionFactory()->CreatePeerConnection(RTCConfig, nullptr, nullptr, this);
	}

	if (!PeerConnection)
	{
		UE_LOG_ONLINE(Warning, TEXT("[WebRTC] Error, PeerConnection is null"));
		EnterNewNegotiationState(ENegotiationState_None);
		return;
	}

	webrtc::DataChannelInit unreliableDataChannelConfig;
	unreliableDataChannelConfig.maxRetransmits = 0;
	unreliableDataChannelConfig.ordered = false;

	DataChannel = PeerConnection->CreateDataChannel("UnreliableDataChannel", &unreliableDataChannelConfig);
	DataChannel->RegisterObserver(new P2PDataChannelObserver(this));

	UpdateDataChannelState();

	//Setup Session description info that we will pass into the async/lambdas
	SessionDescriptionInfo Info;
	Info.Peer2PeerMgr = Peer2PeerMgr;
	Info.PeerConnection = PeerConnection;
	Info.LocalXUID = LocalXUID;
	Info.RemoteXUID = RemoteXUID;
	WebRTCSessionHandler::CreateOffer(Info);
}

void WebRTCSessionHandler::InitiateIncoming(const FString& WebRTCConnectionInfo)
{
	UpdateSessionActivity();

	//No RTConfig lock needed here because we are never changing these values runtime
	RTCConfig.rtcp_mux_policy = webrtc::PeerConnectionInterface::kRtcpMuxPolicyRequire;
	RTCConfig.tcp_candidate_policy = webrtc::PeerConnectionInterface::kTcpCandidatePolicyDisabled; //Do not gather TCP candidates

#if PLATFORM_XBOXONE
	RTCConfig.preferred_candidate_udp_min_port = 10000;
	RTCConfig.preferred_candidate_udp_max_port = 10000 + 20;
#endif //PLATFORM_XBOXONE

#if  PLATFORM_SWITCH || PLATFORM_PS4
	RTCConfig.disable_ipv6 = true;
#endif //  PLATFORM_SWITCH || PLATFORM_PS4

	/* Setup Channel */
	if (!Peer2PeerMgr->GetPeerConnectionFactory())
	{
		UE_LOG_ONLINE(Warning, TEXT("[WebRTC] Error, PeerConnectionFactory is null"));
		EnterNewNegotiationState(ENegotiationState_None);
		return;
	}

	{
		std::lock_guard<std::mutex> LockGuard(RTCConfigLock);
		PeerConnection = Peer2PeerMgr->GetPeerConnectionFactory()->CreatePeerConnection(RTCConfig, nullptr, nullptr, this);
	}

	if (!PeerConnection)
	{
		UE_LOG_ONLINE(Warning, TEXT("[WebRTC] Error, PeerConnection is null"));
		EnterNewNegotiationState(ENegotiationState_None);
		return;
	}

	EnterNewNegotiationState(ENegotiationState_ICEProcessing);

	UE_LOG_ONLINE(Log, TEXT("[WebRTC] Received incoming connection request. RemoteXUID='%s', Message='%s'"), *RemoteXUID, *WebRTCConnectionInfo);

	//Create a session based on the incoming connection request
	std::unique_ptr<webrtc::SessionDescriptionInterface> pSessionDescription = webrtc::CreateSessionDescription(webrtc::SdpType::kOffer, TCHAR_TO_ANSI(*WebRTCConnectionInfo), nullptr);
	
	//Set up struct to pass through all the async/lambdas
	SessionDescriptionInfo Info;
	Info.PeerConnection = PeerConnection;
	Info.Peer2PeerMgr = Peer2PeerMgr;
	Info.LocalXUID = LocalXUID;
	Info.RemoteXUID = RemoteXUID;

	//PeerConnection (WebRTC) takes ownership of SessionDescription
	PeerConnection->SetRemoteDescription(std::move(pSessionDescription), SetRemoteDescriptionObserver::Create([Info](bool success, const char* message)
	{
		if (!success)
		{
			UE_LOG_ONLINE(Log, TEXT("[WebRTC] Failed to create RemoteDescription (type='incoming', RemoteXUID=%s, error='%s')"), *Info.RemoteXUID, ANSI_TO_TCHAR(message));
			return;
		}

		WebRTCSessionHandler::CreateAnswer(Info);
	}));
}

void WebRTCSessionHandler::ProcessConnectResponse(const FString& ConnectResponse)
{
	UE_LOG_ONLINE(Log, TEXT("[WebRTC] Received ConnectResponse. RemoteXUID='%s', Message='%s')"), *RemoteXUID, *ConnectResponse);

	UpdateSessionActivity();

	EnterNewNegotiationState(ENegotiationState_ICEProcessing);

	SigThread->InvokeMessageHandlerOnSignalThread(RTC_FROM_HERE, this, [PeerConnection = PeerConnection, ConnectResponse]() {
		auto SessionDescription = webrtc::CreateSessionDescription(webrtc::SessionDescriptionInterface::kAnswer, TCHAR_TO_ANSI(*ConnectResponse), nullptr);

		PeerConnection->SetRemoteDescription(SetSessionDescriptionObserver::Create([](bool success){}), SessionDescription);
	});
	ProcessIceCandidates();
}

void WebRTCSessionHandler::ProcessCandidateAdd(const FString& WebRTCCandidate)
{
	UpdateSessionActivity();

	webrtc::IceCandidateInterface* IceCandidate = webrtc::CreateIceCandidate("data", 0, TCHAR_TO_ANSI(*WebRTCCandidate), nullptr);
	   
	if (!IceCandidate)
	{
		UE_LOG_ONLINE(Warning, TEXT("[WebRTC] Failed to parse ice candidate (RemoteId='%s')"), *RemoteXUID);
		return;
	}

	std::string Candidate;
	IceCandidate->ToString(&Candidate);
	UE_LOG_ONLINE(Log, TEXT("[WebRTC] Candidate (XUID='%s') : %s"), *RemoteXUID, ANSI_TO_TCHAR(Candidate.data()));

	{
		std::lock_guard<std::mutex> guard(DefferedIceCandidatesMutex);
		DeferredIceCandidates.emplace_back(IceCandidate);
	}
	
	ProcessIceCandidates();
}

void WebRTCSessionHandler::UpdateDataChannelState()
{
	if (DataChannel)
	{
		UE_LOG_ONLINE(Log, TEXT("[WebRTC] DataChannel::OnStateChange (%s -> %s)")
			, ANSI_TO_TCHAR(webrtc::DataChannelInterface::DataStateString(DataChannelState))
			, ANSI_TO_TCHAR(webrtc::DataChannelInterface::DataStateString(DataChannel->state())));
		DataChannelState = DataChannel->state();
	}
	else
	{
		UE_LOG_ONLINE(Log, TEXT("[WebRTC] DataChannel::OnStateChange (%s -> %s)")
			, ANSI_TO_TCHAR(webrtc::DataChannelInterface::DataStateString(DataChannelState))
			, ANSI_TO_TCHAR(webrtc::DataChannelInterface::DataStateString(webrtc::DataChannelInterface::kClosed)));
		DataChannelState = webrtc::DataChannelInterface::kClosed;
	}
}

void WebRTCSessionHandler::SendPacket(const uint8* Data, unsigned int Size)
{
	UpdateSessionActivity();

	auto bufferWithChannel = rtc::CopyOnWriteBuffer(Data, Size);
	webrtc::DataBuffer DataBuffer(bufferWithChannel, true);

	SigThread->InvokeMessageHandlerOnSignalThread(RTC_FROM_HERE, this, 
		[PeerConnection = PeerConnection, DataChannel = DataChannel, DataChannelState = DataChannelState, DataBuffer]()
	{
		if (!PeerConnection)
		{
			return;
		}

		if (DataChannel != nullptr &&
			DataChannelState == webrtc::DataChannelInterface::DataState::kOpen)
		{
			DataChannel->Send(DataBuffer);
		}
	});
}

void WebRTCSessionHandler::OnPacketReceived(const void* Data, size_t Size)
{
	UpdateSessionActivity();
	Peer2PeerMgr->OnDataMessageReceived(RemoteXUID, UniquePort, static_cast<const char*>(Data), Size);
}

bool WebRTCSessionHandler::IsInactive()
{
	return TimerUtils::GetTimeSince(LastSessionActivity) > OnlineSubsystemConstants::WEBRTCISINACTIVE;
}

bool WebRTCSessionHandler::IsDead()
{
	if (CurrentIceState == webrtc::PeerConnectionInterface::kIceConnectionFailed ||
		CurrentIceState == webrtc::PeerConnectionInterface::kIceConnectionDisconnected ||
		CurrentIceState == webrtc::PeerConnectionInterface::kIceConnectionClosed)
	{
		return true;
	}

	return NegotiationState == ENegotiationState_WaitingForResponse && GetTimeInNegotiationState() > OnlineSubsystemConstants::WEBRTCISDEAD;
}

bool GetValue(const webrtc::StatsReport* Report, webrtc::StatsReport::StatsValueName Name, std::string* Value) {
	const webrtc::StatsReport::Value* V = Report->FindValue(Name);
	if (!V)
		return false;
	*Value = V->ToString();
	return true;
}

void WebRTCSessionHandler::OnConnectionStatsComplete(const webrtc::StatsReports& reports)
{
	rtc::SocketAddress LocalSocketAddress;
	rtc::SocketAddress RemoteSocketAddress;
	bool IsUsingRelays = false;

	for (int i = 0; i < reports.size(); ++i)
	{
		const auto& report = reports[i];

		std::string IsActiveConnection;
		if (!GetValue(reports[i], webrtc::StatsReport::StatsValueName::kStatsValueNameActiveConnection, &IsActiveConnection) || IsActiveConnection != "true")
		{
			//Skip inactive connections
			continue;
		}

		std::string LocalCandidateType;
		if (GetValue(reports[i], webrtc::StatsReport::StatsValueName::kStatsValueNameLocalCandidateType, &LocalCandidateType) && LocalCandidateType == "relay")
		{
			IsUsingRelays = true;
		}

		std::string RemoteCandidateType;
		if (GetValue(reports[i], webrtc::StatsReport::StatsValueName::kStatsValueNameRemoteCandidateType, &RemoteCandidateType) && RemoteCandidateType == "relay")
		{
			IsUsingRelays = true;
		}

		std::string LocalAddress;
		if (GetValue(reports[i], webrtc::StatsReport::StatsValueName::kStatsValueNameLocalAddress, &LocalAddress))
		{
			LocalSocketAddress.FromString(LocalAddress);
		}

		std::string RemoteAddress;
		if (GetValue(reports[i], webrtc::StatsReport::StatsValueName::kStatsValueNameRemoteAddress, &RemoteAddress))
		{
			RemoteSocketAddress.FromString(RemoteAddress);
		}
	}

	UE_LOG_ONLINE(Log, TEXT("[WebRTC] Connection Stats gathered (XUID='%s', IsRelay='%s', LocalAddress='%s', RemoteAddress='%s')")
		, *RemoteXUID
		, IsUsingRelays ? *FString("true") : *FString("false")
		, ANSI_TO_TCHAR(LocalSocketAddress.ToString().data())
		, ANSI_TO_TCHAR(RemoteSocketAddress.ToString().data())
	);

#if defined(SUBSYSTEM_HAS_TELEMETRY_CALLBACKS)
	if (!IsOutgoingConnection()) //Only send telemetry as the host
	{
		DungeonsSubsystem->QueueAsyncTask([subsystem = DungeonsSubsystem, IsUsingRelays]() 
			{
				subsystem->GetSessionInterface()->TriggerOnWriteConnectionTypeTelemetryDelegates(IsUsingRelays);
			});
	}
#endif

	StatsRequestInProgress = false;
}

void WebRTCSessionHandler::UpdateRelayServerInfo(webrtc::PeerConnectionInterface::IceServers Servers)
{
	std::lock_guard<std::mutex> ConfigLock(RTCConfigLock);

	/* Update STUN/TURN servers */
	RTCConfig.servers = Servers;
	if (PeerConnection)
	{
		PeerConnection->SetConfiguration(RTCConfig);
	}
}

void WebRTCSessionHandler::OnMessage(rtc::Message* msg)
{
	((ExecutableMessageData*)msg->pdata)->Execute();
	delete msg->pdata;
	msg->pdata = nullptr;
}

void WebRTCSessionHandler::OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState new_state)
{
	UpdateSessionActivity();
	UE_LOG_ONLINE(Log, TEXT("[WebRTC] OnSignalingChange. State='%s'"), *ToString(new_state));
}

void WebRTCSessionHandler::OnDataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel)
{
	UpdateSessionActivity();
	UE_LOG_ONLINE(Log, TEXT("[WebRTC] OnDataChannel"));

	DataChannel = data_channel;
	DataChannel->RegisterObserver(new P2PDataChannelObserver(this));

	UpdateDataChannelState();
}

void WebRTCSessionHandler::OnRenegotiationNeeded()
{
	UE_LOG_ONLINE(Log, TEXT("[WebRTC] OnRenegotiationNeeded"));
}

void WebRTCSessionHandler::OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState new_state)
{
	UE_LOG_ONLINE(Log, TEXT("[WebRTC] OnIceConnectionChange. '%s'->'%s', RemoteId='%s'"), *ToString(CurrentIceState), *ToString(new_state), *RemoteXUID);
	CurrentIceState = new_state;

	if (CurrentIceState == webrtc::PeerConnectionInterface::kIceConnectionConnected)
	{
		CheckUpdates();
	}
}

void WebRTCSessionHandler::OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState new_state)
{
	UE_LOG_ONLINE(Log, TEXT("[WebRTC] OnIceGatheringChange (State='%s')"), *ToString(new_state));
	if (new_state == webrtc::PeerConnectionInterface::kIceGatheringGathering || new_state == webrtc::PeerConnectionInterface::kIceGatheringComplete)
	{
		ProcessIceCandidates();
	}
}


void WebRTCSessionHandler::OnIceCandidate(const webrtc::IceCandidateInterface* candidate)
{
	UE_LOG_ONLINE(Log, TEXT("[WebRTC] OnIceCandidate"));

	auto& CandidateInfo = candidate->candidate();
	std::string CandidateString;
	candidate->ToString(&CandidateString);

	std::string CandidateText = std::string("CANDIDATEADD ") + std::string(TCHAR_TO_ANSI(*LocalXUID)) + std::string(" ") + CandidateString;

#if DEFFERED_ICECANDIDATEPROCESSING
	Peer2PeerMgr->SendXMPPMessageDeffered(LocalXUID, RemoteXUID, ANSI_TO_TCHAR(CandidateText.data()));
#else
	Peer2PeerMgr->SendXMPPMessage(LocalXUID, RemoteXUID, ANSI_TO_TCHAR(CandidateText.data()));
#endif
}

void WebRTCSessionHandler::CreateOffer(const SessionDescriptionInfo& Info)
{
	webrtc::PeerConnectionInterface::RTCOfferAnswerOptions Constraints;

	Info.PeerConnection->CreateOffer(SessionDescriptionObserver::Create([Info](webrtc::SessionDescriptionInterface* desc, const std::string& error)
	{
		if (!desc)
		{
			UE_LOG_ONLINE(Warning, TEXT("[WebRTC] Failed to create offer (type='outgoing', RemoteXUID=%s, error=%s)"), *Info.RemoteXUID, ANSI_TO_TCHAR(error.data()));
			return;
		}

		Info.PeerConnection->SetLocalDescription(SetSessionDescriptionObserver::Create([Info, desc](bool success)
		{
			if (!success)
			{
				UE_LOG_ONLINE(Log, TEXT("[WebRTC] Failed to set local description (type='outgoing', RemoteXUID=%s)"), *Info.RemoteXUID);
				return;
			}

			std::string LocalDescription;
			desc->ToString(&LocalDescription);
			std::string RequestText = std::string("CONNECTREQUEST ") + std::string(TCHAR_TO_ANSI(*Info.LocalXUID)) + std::string(" ") + LocalDescription;

			Info.Peer2PeerMgr->SendXMPPMessage(Info.LocalXUID, Info.RemoteXUID, ANSI_TO_TCHAR(RequestText.data()));

			UE_LOG_ONLINE(Log, TEXT("[WebRTC] Sending connecting request. LocalXUID='%s', RemoteXUID='%s', Message='%s'")
				, *Info.LocalXUID
				, *Info.RemoteXUID
				, ANSI_TO_TCHAR(RequestText.data()));
		}), desc);
	}), Constraints);
}

void WebRTCSessionHandler::CreateAnswer(const SessionDescriptionInfo& Info)
{
	webrtc::PeerConnectionInterface::RTCOfferAnswerOptions answerOptions;
	Info.PeerConnection->CreateAnswer(SessionDescriptionObserver::Create([Info](webrtc::SessionDescriptionInterface* pSession, const std::string& error)
	{
		if (!pSession)
		{
			UE_LOG_ONLINE(Log, TEXT("[WebRTC] Failed to set local description (type='incoming', RemoteXUID=%s, error=%s)"), *Info.RemoteXUID, ANSI_TO_TCHAR(error.data()));
			return;
		}

		Info.PeerConnection->SetLocalDescription(SetSessionDescriptionObserver::Create([Info, pSession](bool success)
		{
			if (!success)
			{
				UE_LOG_ONLINE(Log, TEXT("[WebRTC] Failed to set local description (type='incoming', RemoteXUID=%s)"), *Info.RemoteXUID);
				return;
			}
			std::string localDescription;
			pSession->ToString(&localDescription);

			std::string responseText = "CONNECTRESPONSE " + std::string(TCHAR_TO_ANSI(*Info.LocalXUID)) + std::string(" ") + localDescription;

			UE_LOG_ONLINE(Log, TEXT("[WebRTC] Sending connect response. XUID='%s', RemoteXUID='%s', Message='%s'")
				, *Info.LocalXUID
				, *Info.RemoteXUID
				, ANSI_TO_TCHAR(responseText.data()));

			Info.Peer2PeerMgr->SendXMPPMessage(Info.LocalXUID, Info.RemoteXUID, ANSI_TO_TCHAR(responseText.data()));
		}), pSession);
	}), answerOptions);
}

void WebRTCSessionHandler::ProcessIceCandidates()
{
	SigThread->InvokeMessageHandlerOnSignalThread(RTC_FROM_HERE, this, [this]()
	{
		std::lock_guard<std::mutex> guard(DefferedIceCandidatesMutex);
		auto it = DeferredIceCandidates.begin();
		while (it != DeferredIceCandidates.end())
		{
			if (PeerConnection && PeerConnection->AddIceCandidate(it->get()))
			{
				it = DeferredIceCandidates.erase(it);
			}
			else
			{
				it++;
			}
		}
	});
}

void WebRTCSessionHandler::Cleanup()
{
	if (PeerConnection)
	{
		PeerConnection->Close();
	}
}

void WebRTCSessionHandler::CheckUpdates()
{
	if (TimerUtils::GetTimeSince(LastStatsUpdate) > 0/*2000*/ && !StatsRequestInProgress)
	{
		SigThread->InvokeMessageHandlerOnSignalThread(RTC_FROM_HERE, this, [this]()
		{
			PeerConnection->GetStats(StatsObserver, nullptr, webrtc::PeerConnectionInterface::kStatsOutputLevelStandard);
		});
		LastStatsUpdate = TimerUtils::GetCurrentTime();
		StatsRequestInProgress = true;
	}
}

void P2PDataChannelObserver::OnStateChange()
{
	UE_LOG_ONLINE(Log, TEXT("[WebRTC] DataChannel::OnStateChange"));
	SessionHandler->UpdateDataChannelState();
}

void P2PDataChannelObserver::OnMessage(const webrtc::DataBuffer& buffer)
{
	SessionHandler->OnPacketReceived(buffer.data.data(), buffer.data.size());
}

void P2PStatsObserver::OnComplete(const webrtc::StatsReports& reports)
{
	SessionHandler->OnConnectionStatsComplete(reports);
}
