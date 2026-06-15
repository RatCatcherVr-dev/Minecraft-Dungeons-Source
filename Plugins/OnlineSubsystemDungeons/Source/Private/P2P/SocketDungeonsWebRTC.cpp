#include "PCHOnlineDungeonsSubsystem.h"
#include "SocketDungeonsWebRTC.h"
#include "P2P/Peer2PeerManager.h"

#include "IpAddressDungeonsWebRTC.h"

FSocketDungeonsWebRTC::FSocketDungeonsWebRTC(FOnlineIdentityDungeonsPtr InIdentityInterface)
	: IdentityInterface(InIdentityInterface), WebRTCPort(0)
{}

FSocketDungeonsWebRTC::~FSocketDungeonsWebRTC()
{
	Close();
}

bool FSocketDungeonsWebRTC::Shutdown(ESocketShutdownMode Mode)
{
	return false;
}

bool FSocketDungeonsWebRTC::Close()
{
	if (const auto p2p = IdentityInterface->GetPeer2PeerManager())  {
		p2p->CleanupConnections();
	}
	return true;
}

bool FSocketDungeonsWebRTC::Bind(const FInternetAddr& Addr)
{
	WebRTCPort = Addr.GetPort();
	if (WebRTCPort == 0) {
		// Fake binding to a different port
		WebRTCPort = 1;
	}
	return true;
}

bool FSocketDungeonsWebRTC::Connect(const FInternetAddr& Addr)
{
	/* Not supported - UDP only */
	return false;
}

bool FSocketDungeonsWebRTC::Listen(int32 MaxBacklog)
{
	/* Not supported - UDP only */
	return false;
}

bool FSocketDungeonsWebRTC::WaitForPendingConnection(bool& bHasPendingConnection, const FTimespan& WaitTime)
{
	/* Not supported - UDP only */
	return false;
}

bool FSocketDungeonsWebRTC::HasPendingData(uint32& PendingDataSize)
{
	if (const auto p2p = IdentityInterface->GetPeer2PeerManager()) {
		p2p->HasPendingData(PendingDataSize);
		return PendingDataSize > 0;	
	}
	
	return false;
}

class FSocket* FSocketDungeonsWebRTC::Accept(const FString& InSocketDescription)
{
	/* Not supported - UDP only */
	return nullptr;
}

class FSocket* FSocketDungeonsWebRTC::Accept(FInternetAddr& OutAddr, const FString& InSocketDescription)
{
	/* Not supported - UDP only */
	return nullptr;
}

bool FSocketDungeonsWebRTC::SendTo(const uint8* Data, int32 Count, int32& BytesSent, const FInternetAddr& Destination)
{
	if (const auto p2p = IdentityInterface->GetPeer2PeerManager()) {
		//TODO: Consider adding logic to fetch cases where WebRTC failed to send
		const auto& DungeonsDest = static_cast<const FInternetAddrDungeonsWebRTC&>(Destination);
		return p2p->SendPacket(Data, Count, BytesSent, DungeonsDest.ToRefString());
	}
	return false;
}

bool FSocketDungeonsWebRTC::Send(const uint8* Data, int32 Count, int32& BytesSent)
{
	BytesSent = 0;
	return false;
}

bool FSocketDungeonsWebRTC::RecvFrom(uint8* Data, int32 BufferSize, int32& BytesRead, FInternetAddr& Source, ESocketReceiveFlags::Type Flags /*= ESocketReceiveFlags::None*/)
{
	if (const auto p2p = IdentityInterface->GetPeer2PeerManager()) {
		FInternetAddrDungeonsWebRTC& DungeonsDest = static_cast<FInternetAddrDungeonsWebRTC&>(Source);
		return p2p->ReadPacket(Data, BufferSize, BytesRead, DungeonsDest);
	}
	return false;
}

bool FSocketDungeonsWebRTC::Recv(uint8* Data, int32 BufferSize, int32& BytesRead, ESocketReceiveFlags::Type Flags /*= ESocketReceiveFlags::None*/)
{
	BytesRead = 0;
	return false;
}

bool FSocketDungeonsWebRTC::Wait(ESocketWaitConditions::Type Condition, FTimespan WaitTime)
{
	return false;
}

ESocketConnectionState FSocketDungeonsWebRTC::GetConnectionState()
{
	/** Not supported - connectionless (UDP) only */
	return SCS_NotConnected;
}

void FSocketDungeonsWebRTC::GetAddress(FInternetAddr& OutAddr)
{
	return;
}

bool FSocketDungeonsWebRTC::GetPeerAddress(FInternetAddr& OutAddr)
{
	return false;
}

bool FSocketDungeonsWebRTC::SetNonBlocking(bool bIsNonBlocking /*= true*/)
{
	return true;
}

bool FSocketDungeonsWebRTC::SetBroadcast(bool bAllowBroadcast /*= true*/)
{
	return true;
}

bool FSocketDungeonsWebRTC::JoinMulticastGroup(const FInternetAddr& GroupAddress)
{
	return false;
}

bool FSocketDungeonsWebRTC::JoinMulticastGroup(const FInternetAddr& GroupAddress, const FInternetAddr& InterfaceAddress)
{
	return false;
}

bool FSocketDungeonsWebRTC::LeaveMulticastGroup(const FInternetAddr& GroupAddress)
{
	return false;
}

bool FSocketDungeonsWebRTC::LeaveMulticastGroup(const FInternetAddr& GroupAddress, const FInternetAddr& InterfaceAddress)
{
	return false;
}

bool FSocketDungeonsWebRTC::SetMulticastLoopback(bool bLoopback)
{
	return false;
}

bool FSocketDungeonsWebRTC::SetMulticastTtl(uint8 TimeToLive)
{
	return false;
}

bool FSocketDungeonsWebRTC::SetMulticastInterface(const FInternetAddr& InterfaceAddress)
{
	return false;
}

bool FSocketDungeonsWebRTC::SetReuseAddr(bool bAllowReuse /*= true*/)
{
	return true;
}

bool FSocketDungeonsWebRTC::SetLinger(bool bShouldLinger /*= true*/, int32 Timeout /*= 0*/)
{
	return true;
}

bool FSocketDungeonsWebRTC::SetRecvErr(bool bUseErrorQueue /*= true*/)
{
	return true;
}

bool FSocketDungeonsWebRTC::SetSendBufferSize(int32 Size, int32& NewSize)
{
	return true;
}

bool FSocketDungeonsWebRTC::SetReceiveBufferSize(int32 Size, int32& NewSize)
{
	return true;
}

int32 FSocketDungeonsWebRTC::GetPortNo()
{
	return WebRTCPort;
}
