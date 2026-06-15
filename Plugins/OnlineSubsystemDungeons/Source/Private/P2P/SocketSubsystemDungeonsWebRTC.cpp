#include "PCHOnlineDungeonsSubsystem.h"
#include "SocketSubsystemDungeonsWebRTC.h"

#include "SocketDungeonsWebRTC.h"
#include "OnlineSubsystemDungeonsTypes.h"
#include "OnlineSubsystem.h"
#include "OnlineIdentityDungeons.h"
#include "SocketSubsystemDungeonsWebRTC.h"
#include "IpAddressDungeonsWebRTC.h"

#include "SocketSubsystemModule.h"
#include "SharedPointer.h"
#include "OnlineSubsystemUtils.h"

FSocketSubsystemDungeonsWebRTC* FSocketSubsystemDungeonsWebRTC::SocketSubsystem = nullptr;

FName CreateDungeonsSocketWebRTCSubsystem(FOnlineIdentityDungeonsPtr identity)
{
	FSocketSubsystemDungeonsWebRTC* SocketSubsystem = FSocketSubsystemDungeonsWebRTC::Create(identity);
	FString Error;
	if (SocketSubsystem->Init(Error))
	{
		FSocketSubsystemModule& SSS = FModuleManager::LoadModuleChecked<FSocketSubsystemModule>("Sockets");
		SSS.RegisterSocketSubsystem(FName(DUNGEONSPLATFORM), SocketSubsystem, !PLATFORM_PS4);
		return FName(DUNGEONSPLATFORM);
	}
	else
	{
		FSocketSubsystemDungeonsWebRTC::Destroy();
		return NAME_None;
	}

}

FSocketSubsystemDungeonsWebRTC* FSocketSubsystemDungeonsWebRTC::Create(FOnlineIdentityDungeonsPtr identity)
{
	check(!SocketSubsystem && "We can't create two");
	if (!SocketSubsystem)
	{
		SocketSubsystem = new FSocketSubsystemDungeonsWebRTC(identity);
	}
	return SocketSubsystem;
}

void FSocketSubsystemDungeonsWebRTC::Destroy()
{
	if (SocketSubsystem)
	{
		SocketSubsystem->Shutdown();
		delete SocketSubsystem;
		SocketSubsystem = nullptr;
	}
}

bool FSocketSubsystemDungeonsWebRTC::Init(FString& Error)
{
	UE_LOG_ONLINE(Log, TEXT("[WebRTC] Loading WebRTC SocketSubsystem"));
	return true;
}

void FSocketSubsystemDungeonsWebRTC::Shutdown()
{
	
}

class FSocket* FSocketSubsystemDungeonsWebRTC::CreateSocket(const FName& SocketType, const FString& SocketDescription, ESocketProtocolFamily ProtocolType)
{
	return new FSocketDungeonsWebRTC(Identity);
}

void FSocketSubsystemDungeonsWebRTC::DestroySocket(class FSocket* Socket)
{
	delete Socket;
}

FAddressInfoResult FSocketSubsystemDungeonsWebRTC::GetAddressInfo(const TCHAR* HostName, const TCHAR* ServiceName /*= nullptr*/, EAddressInfoFlags QueryFlags /*= EAddressInfoFlags::Default*/, ESocketProtocolFamily ProtocolType /*= ESocketProtocolFamily::None*/, ESocketType SocketType /*= ESocketType::SOCKTYPE_Unknown*/)
{
	UE_LOG_ONLINE(Warning, TEXT("[WebRTC] GetAddressInfo is not supported on DungeonsWebRTC Sockets"));
	return FAddressInfoResult(HostName, ServiceName);
}

ESocketErrors FSocketSubsystemDungeonsWebRTC::GetHostByName(const ANSICHAR* HostName, FInternetAddr& OutAddr)
{
	return SE_NO_ERROR;
}

bool FSocketSubsystemDungeonsWebRTC::RequiresChatDataBeSeparate()
{
	return false;
}

bool FSocketSubsystemDungeonsWebRTC::RequiresEncryptedPackets()
{
	return false;
}

bool FSocketSubsystemDungeonsWebRTC::GetHostName(FString& HostName)
{
	return false;
}

TSharedRef<FInternetAddr> FSocketSubsystemDungeonsWebRTC::CreateInternetAddr(uint32 Address /*= 0*/, uint32 Port /*= 0*/)
{
	FInternetAddrDungeonsWebRTC* DungeonAddr = new FInternetAddrDungeonsWebRTC(Address, Port);
	return MakeShareable(DungeonAddr);
}

bool FSocketSubsystemDungeonsWebRTC::HasNetworkDevice()
{
	return true;
}

const TCHAR* FSocketSubsystemDungeonsWebRTC::GetSocketAPIName() const
{
	return TEXT("DungeonsSockets");
}

ESocketErrors FSocketSubsystemDungeonsWebRTC::GetLastErrorCode()
{
	return TranslateErrorCode(LastSocketError);
}

ESocketErrors FSocketSubsystemDungeonsWebRTC::TranslateErrorCode(int32 Code)
{
	return (ESocketErrors)LastSocketError;
}

bool FSocketSubsystemDungeonsWebRTC::GetLocalAdapterAddresses(TArray<TSharedPtr<FInternetAddr>>& OutAdresses)
{
	bool bCanBindAll;

	OutAdresses.Add(GetLocalHostAddr(*GLog, bCanBindAll));

	return true;
}

bool FSocketSubsystemDungeonsWebRTC::IsSocketWaitSupported() const
{
	return false;
}

bool FSocketSubsystemDungeonsWebRTC::Tick(float DeltaTime)
{
	return true;
}
