#pragma once

#include "WebRTCCommon.h"
#include "XMPP/XMPPHandlerUE4.h"
#include "WebRTCSessionHandler.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemDungeons.h"

#include <mutex>
#include "OnlineIdentityDungeons.h"
#include "IpAddressDungeonsWebRTC.h"

class WebRTCSessionHandler;
class SignalThread;

struct WebRTCConfig
{
	WebRTCConfig()
		: OnlyRelay(false)
		, LogLowLevelWebRTC(false)
	{}
	/* Settings */
	bool OnlyRelay;
	bool LogLowLevelWebRTC;

	/* Server configurations */
	webrtc::PeerConnectionInterface::IceServers Servers;
};

class WebRTCLogSink : public rtc::LogSink 
{
	virtual void OnLogMessage(const std::string& message) override
	{
		std::string LogMessage = message;
		if (LogMessage[LogMessage.size() - 1] == '\n')
		{
			LogMessage = LogMessage.substr(0, LogMessage.size() - 1);
		}

		if (LogMessage.size() > 0)
		{
			UE_LOG_ONLINE(Log, TEXT("[LowLevelWebRTC] %s"), ANSI_TO_TCHAR(LogMessage.data()));
		}
	}
};


#define DEFFERED_ICECANDIDATEPROCESSING	1

#if DEFFERED_ICECANDIDATEPROCESSING

typedef const FString& FIceCandindateParam;
#define ICECANDIDATEDEFERRED_CRITICALSECTION	std::lock_guard<std::mutex> LockGuard(mLock);

struct FIceCandidateDefferedArg
{
	FIceCandidateDefferedArg()
	{
	}
	FIceCandidateDefferedArg(FIceCandindateParam LocalXUID, FIceCandindateParam RemoteXUID, FIceCandindateParam CandidateText)
		: mLocalXUID(LocalXUID), mRemoteXUID(RemoteXUID), mCandidateText(CandidateText)
	{
	}

	FString mLocalXUID, mRemoteXUID, mCandidateText;
};
#endif

class Peer2PeerManager : public rtc::MessageHandler
{
public:

	Peer2PeerManager(FString InXUID, FOnlineSubsystemDungeons* InDungeonsSubsystem);
	~Peer2PeerManager();

	bool IsXMPPLoggedIn();

	void SetP2PConfiguration(const char* Username, const char* Password);
	void InitSignalThread();
	void InitSocketFactory();

	bool InitiateIncoming(const FString& RemoteXUID, const FString& WebRTCConnectionInfo);
	
	rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> GetPeerConnectionFactory();
	void SendXMPPMessage(const FString& XUID, const FString& RemoteXUID, const FString& Message);

	void ProcessConnectRequest(const FString& FromXUID, const FString& WebRTCConnectionInfo);
	void ProcessConnectResponse(const FString& FromXUID, const FString& WebRTCConnectionInfo);
	void ProcessCandidateAdd(const FString& FromXUID, const FString& Candidate);

	virtual void OnMessage(rtc::Message* msg) override;
	void OnDataMessageReceived(const FString& XUID, int32 Port, const char* Data, size_t Size);

	/* Send/recv */
	bool ReadPacket(uint8* Data, int32 BufferSize, int32& BytesRead, FInternetAddrDungeonsWebRTC& From);
	bool SendPacket(const uint8* Data, int32 Count, int32& BytesSent, const FString& RemoteXUID);
	bool HasPendingData(uint32& MessageSize);
	void CleanupConnections();
	void TryCleanupConnections();

	bool Tick(float DeltaTime);

	void SetIsInSession(bool IsInSession, bool IsHost);

	FString GetXUID() { return XUID; };
#if DEFFERED_ICECANDIDATEPROCESSING
	void SendXMPPMessageDeffered( FIceCandindateParam LocalXUID, FIceCandindateParam RemoteXUID, FIceCandindateParam CandidateText);
#endif

private:
	void UpdateRelayToken();
	bool ShouldSignInToXMPP() const;
	bool ShouldSignOutFromXMPP() const;

	FOnlineSubsystemDungeons* DungeonsSubsystem;

	/* WebRTC */
	rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> m_pPeerConnectionFactory;
	rtc::scoped_refptr<webrtc::DataChannelInterface> m_pUnreliableDataChannel;

	/* XMPP */
	FString XUID;
	std::unique_ptr<XMPPHandlerUE4> XMPPHandler;

	/* Port counter to separate connections */
	int32 UniquePortCounter;

	/* WebRTC Sessions */
	std::map<FString, std::unique_ptr<WebRTCSessionHandler>> WebRTCSessions;
	WebRTCConfig P2PSessionConfiguration;

	/* Packet queues */
	class PacketDataRecord
	{
	public:
		std::unique_ptr<unsigned char[]> PacketData;
		size_t Len;
		FString XUID;
		int32 UniquePort;
	};

	SignalThread* SigThread;
	std::queue<std::unique_ptr<PacketDataRecord>> PacketQueue;
	std::mutex PacketQueueMutex;
	std::mutex AlterSessionsMutex;
	std::mutex P2PServerMutex;

	/* Log handling */
	WebRTCLogSink WebRTCLowLevelLog;

	float TickCount;
	bool IsInSession;
	bool IsHost;
#if DEFFERED_ICECANDIDATEPROCESSING
	void TickDeffered();
	void LogDefferedOperationString(FIceCandindateParam Title, FIceCandindateParam Arg);
	std::vector<FIceCandidateDefferedArg> mDefferedIceCandidates;
	std::mutex mLock;
#endif
};
