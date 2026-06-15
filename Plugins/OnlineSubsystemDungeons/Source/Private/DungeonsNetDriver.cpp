#include "PCHOnlineDungeonsSubsystem.h"
#include "DungeonsNetDriver.h"
#include "Engine.h"
#include "OnlineSubsystem.h"
#include "xsapiServicesInclude.h"
#include "SocketSubsystem.h"
#include "OnlineSubsystemDungeonsTypes.h"
#include "utils.h"
#include "P2P/IpAddressDungeonsWebRTC.h"

UDungeonsNetDriver::UDungeonsNetDriver(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
}

void UDungeonsNetDriver::PostInitProperties()
{
	Super::PostInitProperties();
}

bool UDungeonsNetDriver::DoInitBase(bool bInitAsClient, FNetworkNotify* InNotify, const FURL& URL, bool bReuseAddressAndPort, FString& Error)
{
	if (!UNetDriver::InitBase(bInitAsClient, InNotify, URL, bReuseAddressAndPort, Error))
	{
		return false;
	}

	if (!Socket)
	{
		Error = "[DungeonsNetDriver] Socket failed";
		return false;
	}
	//Host is not using LocalAddr but we need a pointer with dummy data to prevent the engine from crashing.
	LocalAddr = MakeShareable(new FInternetAddrDungeonsWebRTC());
	LocalAddr->SetPort(URL.Port);

	return true;
}

bool UDungeonsNetDriver::InitBase(bool bInitAsClient, FNetworkNotify* InNotify, const FURL& URL, bool bReuseAddressAndPort, FString& Error)
{
	return DoInitBase(bInitAsClient, InNotify, URL, bReuseAddressAndPort, Error);
}

void UDungeonsNetDriver::DoInitConnect(FNetworkNotify* InNotify, const FURL& ConnectURL, FString& Error)
{
	if (ISocketSubsystem* DungeonsSockets = ISocketSubsystem::Get(DUNGEONSPLATFORM))
	{
		Socket = DungeonsSockets->CreateSocket(FName(TEXT("DungeonsClientSocket")), TEXT("Unreal client (Dungeons)"));
	}
}

bool UDungeonsNetDriver::InitConnect(FNetworkNotify* InNotify, const FURL& ConnectURL, FString& Error)
{
	DoInitConnect(InNotify, ConnectURL, Error);
	return Super::InitConnect(InNotify, ConnectURL, Error);
}

bool UDungeonsNetDriver::DoInitListen(FNetworkNotify* InNotify, FURL& LocalURL, bool bReuseAddressAndPort, FString& Error)
{

	if (ISocketSubsystem* DungeonsSocketSubsystem = ISocketSubsystem::Get(DUNGEONSPLATFORM))
	{
		Socket = DungeonsSocketSubsystem->CreateSocket(FName("DungeonsServerSocket"), TEXT("Unreal server (Dungeons)"));
	}
	else
	{
		UE_LOG_ONLINE(Warning, TEXT("[DungeonsNetDriver] SocketSubsystem is null"));
		return false;
	}
	return true;
}

bool UDungeonsNetDriver::InitListen(FNetworkNotify* InNotify, FURL& LocalURL, bool bReuseAddressAndPort, FString& Error)
{
	if (!DoInitListen(InNotify, LocalURL, bReuseAddressAndPort, Error))
	{
		return false;
	}
	return Super::InitListen(InNotify, LocalURL, bReuseAddressAndPort, Error);
}

bool UDungeonsNetDriver::IsAvailable() const
{
	ISocketSubsystem* DungeonsSockets = ISocketSubsystem::Get(DUNGEONSPLATFORM);
	return DungeonsSockets != nullptr;
}