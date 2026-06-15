#include "PCHOnlineDungeonsSubsystem.h"
#include "Peer2PeerManager.h"

#include "DataChannelObserver.h"
#include "OnlineSubsystem.h"
#include "SessionDescriptors.h"

#include "P2P/SignalThread.h"
#include "P2P/GenericSignalThread.h"

Peer2PeerManager::Peer2PeerManager(FString InXUID, FOnlineSubsystemDungeons* InDungeonsSubsystem)
	: DungeonsSubsystem(InDungeonsSubsystem), XUID(InXUID), UniquePortCounter(0), TickCount(0.0f), IsInSession(false), IsHost(false)
{

	/*
	--- Switch dummy create socket ---
	This is because of a bug in libstrophe. When creating the very first socket on switch OS, socket(..) returns a fd=0.
	The logic in sock.c/conn.c (libstrophe) early outs on fd=0. By creating a dummy socket here the libstrophe
	will get a fd > 0 and thus function correctly
	*/
#if PLATFORM_SWITCH
	int StupidSocket = nn::socket::Socket(nn::socket::Family::Af_Inet, nn::socket::Type::Sock_Stream, nn::socket::Protocol::IpProto_Ip);
	int EvenMoreStupidSocket = nn::socket::Socket(nn::socket::Family::Af_Inet, nn::socket::Type::Sock_Stream, nn::socket::Protocol::IpProto_Ip);
#endif

	///* Setup XMPP service */
	XMPPHandler = std::make_unique<XMPPHandlerUE4>(this);

	InitSignalThread();
	InitSocketFactory();

	/* Read configurations */
	P2PSessionConfiguration.OnlyRelay = environmentConfig::GetConfigBool("WebRTC", "TURNOnly", false);

	if (!environmentConfig::IsShippingMode() && environmentConfig::GetConfigBool("WebRTC", "LogWebRTC", false))
	{
		rtc::LogMessage::AddLogToStream(&WebRTCLowLevelLog, rtc::LS_WARNING);
		P2PSessionConfiguration.LogLowLevelWebRTC = true;
	}

#if PLATFORM_PS4
	orbisInitNetworkingLibraries();
#endif

	SetP2PConfiguration("", "");
	UE_LOG_ONLINE(Log, TEXT("[WebRTC] Peer2Peer manager initialized"));
}

Peer2PeerManager::~Peer2PeerManager()
{
	CleanupConnections();

	SigThread->InvokeMessageHandlerOnSignalThread(RTC_FROM_HERE, this, [this]() {
		m_pPeerConnectionFactory = {};
	});

	SigThread->Teardown();
	delete SigThread;
	SigThread = nullptr;

	if (P2PSessionConfiguration.LogLowLevelWebRTC)
	{
		rtc::LogMessage::RemoveLogToStream(&WebRTCLowLevelLog);
	}
#if PLATFORM_PS4
	orbisFinalizeNetworkingLibraries();
#endif

}

bool Peer2PeerManager::IsXMPPLoggedIn()
{
	if (!XMPPHandler)
	{
		return false;
	}

	return XMPPHandler->IsSignedIn();
}

/* Updates Coturn auth username/password - this is created and received by the XMPP server */
void Peer2PeerManager::SetP2PConfiguration(const char* Username, const char* Password)
{
	webrtc::PeerConnectionInterface::IceServers CoturnServers;
	{
		//We only need to lock when modifying coturn servers
		std::lock_guard<std::mutex> P2PLock(P2PServerMutex);
		P2PSessionConfiguration.Servers.clear();

		webrtc::PeerConnectionInterface::IceServer SServer;
		SServer.uri = OnlineSubsystemConstants::TURNADDRESS;
		SServer.username = Username;
		SServer.password = Password;
		P2PSessionConfiguration.Servers.push_back(std::move(SServer));

		webrtc::PeerConnectionInterface::IceServer TServer;
		TServer.uri = OnlineSubsystemConstants::STUNADDRESS;
		TServer.username = Username;
		TServer.password = Password;
		P2PSessionConfiguration.Servers.push_back(std::move(TServer));

		//Copy CoturnServer configuration before releasing lock
		CoturnServers = P2PSessionConfiguration.Servers;
	}

	//Make sure we update active WebRTCSessions with the new config
	{
		std::lock_guard<std::mutex> guard(AlterSessionsMutex);
		for (const auto& kv : WebRTCSessions)
		{
			kv.second->UpdateRelayServerInfo(CoturnServers);
		}
	}
}

void Peer2PeerManager::InitSignalThread()
{
	rtc::InitializeSSL();

	SigThread = new GenericSignalThread();
	SigThread->Initialize();
}

void Peer2PeerManager::InitSocketFactory()
{
	/* Connection Options */
	SigThread->InvokeMessageHandlerOnSignalThread(RTC_FROM_HERE, this, [this]() {
		webrtc::PeerConnectionFactoryInterface::Options peerConnectionOptions;

		m_pPeerConnectionFactory = webrtc::CreateModularPeerConnectionFactory(nullptr //network thread
			, nullptr //worker thread
			, nullptr //signal thread
			, nullptr
			, nullptr
			, nullptr
		);

		m_pPeerConnectionFactory->SetOptions(peerConnectionOptions);
	});
}

bool Peer2PeerManager::InitiateIncoming(const FString& RemoteXUID, const FString& WebRTCConnectionInfo)
{
	if (!XMPPHandler->IsSignedIn())
	{
		UE_LOG_ONLINE(Log, TEXT("[WebRTC] Failed to initiate incoming connection '%s', no xmpp-connection"), *RemoteXUID);
		return false;
	}

	std::lock_guard<std::mutex> guard(AlterSessionsMutex);
	auto WebRTCSession = WebRTCSessions.find(RemoteXUID);
	if (WebRTCSession != WebRTCSessions.end())
	{
		/* Session already exists - the client might have lost the connection and tries to reconnect */
		/* - TOOD: Is this the right way to handle it? */

		/* Remove old connection */
		WebRTCSessions.erase(WebRTCSession);
	}

	/* Initiate new connection */
	auto pWebRTCSession = WebRTCSessions.emplace(RemoteXUID, std::make_unique<WebRTCSessionHandler>(DungeonsSubsystem, XUID, RemoteXUID, false, this, P2PSessionConfiguration, SigThread, UniquePortCounter++));
	pWebRTCSession.first->second->InitiateIncoming(WebRTCConnectionInfo);

	return true;
}

rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> Peer2PeerManager::GetPeerConnectionFactory()
{
	return m_pPeerConnectionFactory;
}

void Peer2PeerManager::SendXMPPMessage(const FString& LocalXUID, const FString& RemoteXUID, const FString& Message)
{
	XMPPHandler->SendMessage(LocalXUID, RemoteXUID, Message);
}

void Peer2PeerManager::ProcessConnectRequest(const FString& FromXUID, const FString& WebRTCConnectionInfo)
{
	InitiateIncoming(FromXUID, WebRTCConnectionInfo);
}

void Peer2PeerManager::ProcessConnectResponse(const FString& FromXUID, const FString& ConnectResponse)
{
	if (!XMPPHandler->IsSignedIn())
	{
		UE_LOG_ONLINE(Log, TEXT("[WebRTC] Failed to initiate incoming connection '%s', no xmpp-connection"), *FromXUID);
		return;
	}

	std::lock_guard<std::mutex> guard(AlterSessionsMutex);

	auto WebSession = WebRTCSessions.find(TCHAR_TO_ANSI(*FromXUID));
	if (WebSession == WebRTCSessions.end())
	{
		return; //Session does not exist
	}

	WebSession->second->ProcessConnectResponse(ConnectResponse);
}

void Peer2PeerManager::ProcessCandidateAdd(const FString& RemoteXUID, const FString& Candidate)
{
	if (!XMPPHandler->IsSignedIn())
	{
		UE_LOG_ONLINE(Log, TEXT("[WebRTC] Failed to initiate incoming connection '%s', no xmpp-connection"), *RemoteXUID);
		return;
	}

	std::lock_guard<std::mutex> guard(AlterSessionsMutex);

	auto WebRTCSession = WebRTCSessions.find(TCHAR_TO_ANSI(*RemoteXUID));

	if (WebRTCSession == WebRTCSessions.end())
	{
		return; //Session do not exist
	}

	WebRTCSession->second->ProcessCandidateAdd(Candidate);
}

void Peer2PeerManager::OnMessage(rtc::Message* msg)
{
	((ExecutableMessageData*)msg->pdata)->Execute();
	delete msg->pdata;
	msg->pdata = nullptr;
}

void Peer2PeerManager::OnDataMessageReceived(const FString& FromXUID, int32 Port, const char* Data, size_t Size)
{
	auto Packet = std::unique_ptr<PacketDataRecord>(new PacketDataRecord());
	Packet->PacketData = std::unique_ptr<unsigned char[]>(new unsigned char[Size]);
	memcpy(Packet->PacketData.get(), Data, Size);

	Packet->Len = Size;
	Packet->XUID = FromXUID;
	Packet->UniquePort = Port;
	{
		std::lock_guard<std::mutex> guard(PacketQueueMutex);
		PacketQueue.emplace(std::move(Packet));
	}
}

bool Peer2PeerManager::ReadPacket(uint8* Data, int32 BufferSize, int32& BytesRead, FInternetAddrDungeonsWebRTC& From)
{
	std::lock_guard<std::mutex> guard(PacketQueueMutex);

	if (PacketQueue.empty())
	{
		return false;
	}

	PacketDataRecord* Packet = PacketQueue.front().get();

	bool IsValid;
	From.SetIp(*Packet->XUID, IsValid);
	From.SetPort(Packet->UniquePort);
	BytesRead = Packet->Len;

	size_t Write = std::min((size_t)BufferSize, Packet->Len);
	memcpy(Data, Packet->PacketData.get(), Write);

	PacketQueue.pop();
	return true;
}

bool Peer2PeerManager::SendPacket(const uint8* Data, int32 Count, int32& BytesSent, const FString& RemoteXUID)
{
	std::map<FString, std::unique_ptr<WebRTCSessionHandler>>::iterator WebRTCSession;
	std::lock_guard<std::mutex> guard(AlterSessionsMutex);
	WebRTCSession = WebRTCSessions.find(RemoteXUID);

	if (WebRTCSession == WebRTCSessions.end())
	{
		if (IsHost || !XMPPHandler->HasRelayToken())
		{
			//Do not InitiateOutGoing if we are host - let the client initiate - or if we don't have a local relay token
			BytesSent = 0;
			return false;
		}

		//Session do not exist - create it
		WebRTCSession = WebRTCSessions.emplace(RemoteXUID, std::make_unique<WebRTCSessionHandler>(DungeonsSubsystem, XUID, RemoteXUID, true, this, P2PSessionConfiguration, SigThread, 0)).first;
		WebRTCSession->second->InitiateOutGoing();
	}

	WebRTCSession->second->SendPacket(Data, Count);
	/* Assume we managed to send - TODO: Research on this - how does this affect the UDP layer in unreal? */
	BytesSent = Count;
	return true;
}

bool Peer2PeerManager::HasPendingData(uint32& MessageSize)
{
	std::lock_guard<std::mutex> guard(PacketQueueMutex);
	if (PacketQueue.empty())
	{
		return false;
	}
	MessageSize = PacketQueue.front()->Len;
	return true;
}

void Peer2PeerManager::CleanupConnections()
{
	std::lock_guard<std::mutex> guard(AlterSessionsMutex);

	WebRTCSessions.clear();
}

void Peer2PeerManager::TryCleanupConnections()
{
	std::lock_guard<std::mutex> guard(AlterSessionsMutex);
	if (WebRTCSessions.size() > 0)
	{
		auto Iter = WebRTCSessions.begin();
		while (Iter != WebRTCSessions.end())
		{
			if (Iter->second && Iter->second->IsInactive())
			{
				UE_LOG_ONLINE(Log, TEXT("[WebRTC] Cleaning up inactive connection (RemoteXUID='%s')"), *Iter->first);
				Iter = WebRTCSessions.erase(Iter);
			}
			else if (Iter->second && Iter->second->IsDead())
			{
				UE_LOG_ONLINE(Log, TEXT("[WebRTC] Cleaning up dead connection (RemoteXUID='%s')"), *Iter->first);
				Iter = WebRTCSessions.erase(Iter);
			}
			else
			{
				Iter++;
			}
		}
	}
}

bool Peer2PeerManager::Tick(float DeltaTime)
{

#if DEFFERED_ICECANDIDATEPROCESSING
	TickDeffered();
#endif
	/* Check every second second */
	if (TickCount < 2.0f)
	{
		TickCount += DeltaTime;
		return true;
	}
	TickCount = 0.0f;

	if (ShouldSignInToXMPP())
	{
		UE_LOG_ONLINE(Log, TEXT("[XMPP] Signing in to XMPP"));
		XMPPHandler->Signin(XUID, DungeonsSubsystem->GetIdentityInterface()->GetAuthToken(OnlineSubsystemConstants::AuthtokenLocaluserNum));
	}

	if (ShouldSignOutFromXMPP())
	{
		UE_LOG_ONLINE(Log, TEXT("[XMPP] Signing out from XMPP"));
		XMPPHandler->Signout(XUID);
	}

	/* Remove timed out sessions */
	TryCleanupConnections();
	UpdateRelayToken();

	return true;
}

void Peer2PeerManager::SetIsInSession(bool InSession, bool Host)
{
	this->IsInSession = InSession;
	this->IsHost = Host;
	//Force reset internal counters - if we fail to signin and then leave the session, the internal timers won't reset.
	XMPPHandler->ClearLoginCounters();
	UE_LOG_ONLINE(Log, TEXT("[WebRTC] Setting IsInSession=%s, IsHost=%s")
		, InSession ? *FString("true") : *FString("false")
		, Host ? *FString("true") : *FString("false")
	);
}

void Peer2PeerManager::UpdateRelayToken()
{
	if (XMPPHandler->ShouldUpdateAuthToken())
	{
		XMPPHandler->UpdateAuthToken();
	}
}

bool Peer2PeerManager::ShouldSignInToXMPP() const
{
	return IsInSession && XMPPHandler->IsAllowedToSignIn();
}

bool Peer2PeerManager::ShouldSignOutFromXMPP() const
{
	return !IsInSession && XMPPHandler->IsAllowedToSignOut();
}

#if DEFFERED_ICECANDIDATEPROCESSING

void Peer2PeerManager::SendXMPPMessageDeffered( FIceCandindateParam LocalXUID, FIceCandindateParam RemoteXUID, FIceCandindateParam CandidateText)
{
	LogDefferedOperationString("Writing", CandidateText);
	ICECANDIDATEDEFERRED_CRITICALSECTION;
	mDefferedIceCandidates.push_back(FIceCandidateDefferedArg( LocalXUID, RemoteXUID, CandidateText));
}

void Peer2PeerManager::TickDeffered()
{
	std::vector<FIceCandidateDefferedArg> DefferedIceCandidates;
	{
		ICECANDIDATEDEFERRED_CRITICALSECTION;
		DefferedIceCandidates.swap(mDefferedIceCandidates);
	}
	for (auto& Deffered : DefferedIceCandidates)
	{
		const auto& LocalXUID = Deffered.mLocalXUID;
		const auto& RemoteXUID = Deffered.mRemoteXUID;
		const auto& CandidateText = Deffered.mCandidateText;
		LogDefferedOperationString("Reading", CandidateText);
		SendXMPPMessage(LocalXUID, RemoteXUID, CandidateText);
	}
}

void Peer2PeerManager::LogDefferedOperationString(FIceCandindateParam Title, FIceCandindateParam Arg)
{
	UE_LOG_ONLINE(Log, TEXT("[FIceCandidateDefferedManager] %s = %s "), *Title, *Arg);
}

#endif