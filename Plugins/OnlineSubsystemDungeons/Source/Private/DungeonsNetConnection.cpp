#include "DungeonsNetConnection.h"


UDungeonsNetConnection::UDungeonsNetConnection(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
}

void UDungeonsNetConnection::InitRemoteConnection(class UNetDriver* InDriver, class FSocket* InSocket, const FURL& InURL, const class FInternetAddr& InRemoteAddr, EConnectionState InState, int32 InMaxPacket /*= 0*/, int32 InPacketOverhead /*= 0*/)
{
	Super::InitRemoteConnection(InDriver, InSocket, InURL, InRemoteAddr, InState, InMaxPacket, InPacketOverhead);
}

void UDungeonsNetConnection::InitLocalConnection(class UNetDriver* InDriver, class FSocket* InSocket, const FURL& InURL, EConnectionState InState, int32 InMaxPacket /*= 0*/, int32 InPacketOverhead /*= 0*/)
{
	Super::InitLocalConnection(InDriver, InSocket, InURL, InState, InMaxPacket, InPacketOverhead);
}

void UDungeonsNetConnection::CleanUp()
{
	Super::CleanUp();
}

