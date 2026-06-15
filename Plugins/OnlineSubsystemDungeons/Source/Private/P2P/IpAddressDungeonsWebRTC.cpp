#include "PCHOnlineDungeonsSubsystem.h"
#include "IpAddressDungeonsWebRTC.h"

#include "OnlineSubsystem.h"

#include <string>

FInternetAddrDungeonsWebRTC::FInternetAddrDungeonsWebRTC(const FInternetAddrDungeonsWebRTC& Src) 
	: DungeonsId(Src.DungeonsId), WebRTCPort(Src.WebRTCPort)
{

}

FInternetAddrDungeonsWebRTC::FInternetAddrDungeonsWebRTC(const FUniqueNetIdDungeons& InDungeonsID, int32 Port) 
	: DungeonsId(InDungeonsID.ToString()), WebRTCPort(Port)

{

}

FInternetAddrDungeonsWebRTC::FInternetAddrDungeonsWebRTC(const FString& XUID, int32 Port)
	: DungeonsId(XUID), WebRTCPort(Port)
{}

FInternetAddrDungeonsWebRTC::FInternetAddrDungeonsWebRTC(uint64 XUID, int32 Port)
	: WebRTCPort(Port)
{
	DungeonsId = FString::Printf(TEXT("%" PRIu64), XUID);
}

FInternetAddrDungeonsWebRTC::FInternetAddrDungeonsWebRTC()
	: DungeonsId(""), WebRTCPort(0)
{}

FInternetAddrDungeonsWebRTC::~FInternetAddrDungeonsWebRTC()
{

}

void FInternetAddrDungeonsWebRTC::SetIp(uint32 InAddr)
{
	UE_LOG_ONLINE(Warning, TEXT("FInternetAddrDungeonsWebRTC::SetIp is not supported."));
}

void FInternetAddrDungeonsWebRTC::SetIp(const TCHAR* InAddr, bool& bIsValid)
{
	FString InAddrStr(InAddr);
	FString XUID;
	FString Port;

	if (InAddrStr.StartsWith("WebRTC."))
	{	
		InAddrStr = InAddrStr.Mid(ARRAY_COUNT("WebRTC.") - 1);
	}

	if (InAddrStr.Split(":", &XUID, &Port, ESearchCase::CaseSensitive, ESearchDir::FromEnd))
	{
		WebRTCPort = FCString::Atoi(*Port);
	}
	else
	{
		XUID = InAddrStr;
	}

	DungeonsId = XUID;
	bIsValid = true;
}

void FInternetAddrDungeonsWebRTC::GetIp(uint32& OutAddr) const
{
	OutAddr = 0;
	UE_LOG_ONLINE(Warning, TEXT("FInternetAddrDungeonsWebRTC::GetIp is not supported and will set OutAddr to 0."));
}

void FInternetAddrDungeonsWebRTC::SetPort(int32 InPort)
{
	WebRTCPort = InPort;
}

int32 FInternetAddrDungeonsWebRTC::GetPort() const
{
	return WebRTCPort;
}

void FInternetAddrDungeonsWebRTC::SetRawIp(const TArray<uint8>& RawAddr)
{
	uint64 NewDungeonsId = 0;

	// Make a quick copy of the array
	TArray<uint8> WorkingArray = RawAddr;

	// Flip the entire array.
	// Normally we would just do a ntohll on the final result but it's not a portable function.
#if PLATFORM_LITTLE_ENDIAN
	Algo::Reverse(WorkingArray);
#endif

	for (int32 i = 0; i < WorkingArray.Num(); ++i)
	{
		NewDungeonsId |= (uint64)WorkingArray[i] << (i * 8);
	}

	DungeonsId = FString::Printf(TEXT("%" PRIu64), NewDungeonsId);
}

TArray<uint8> FInternetAddrDungeonsWebRTC::GetRawIp() const
{
	TArray<uint8> RawAddressArray;
	const uint8* DungeonsIdWalk = (uint8*)TCHAR_TO_ANSI(*DungeonsId);
	while (RawAddressArray.Num() < DungeonsId.Len())
	{
		RawAddressArray.Add(*DungeonsIdWalk);
		++DungeonsIdWalk;
	}

	// We want to make sure that these arrays are in big endian format.
#if PLATFORM_LITTLE_ENDIAN
	Algo::Reverse(RawAddressArray);
#endif

	return RawAddressArray;
}

void FInternetAddrDungeonsWebRTC::SetAnyAddress()
{}

void FInternetAddrDungeonsWebRTC::SetBroadcastAddress()
{}

void FInternetAddrDungeonsWebRTC::SetLoopbackAddress()
{}

FString FInternetAddrDungeonsWebRTC::ToString(bool bAppendPort) const
{
	if (bAppendPort)
	{
		return FString::Printf(TEXT("%s:%d"), *DungeonsId, WebRTCPort);
	}
	else
	{
		return DungeonsId;
	}
}

const FString& FInternetAddrDungeonsWebRTC::ToRefString() const
{
	return DungeonsId;
}

uint32 FInternetAddrDungeonsWebRTC::GetTypeHash() const
{
	return ::GetTypeHash(ToString(true));
}

bool FInternetAddrDungeonsWebRTC::operator==(const FInternetAddr& Other) const
{
	const FInternetAddrDungeonsWebRTC& OtherAddr = static_cast<const FInternetAddrDungeonsWebRTC&>(Other);
	return OtherAddr.DungeonsId == DungeonsId && OtherAddr.WebRTCPort == WebRTCPort;
}

bool FInternetAddrDungeonsWebRTC::IsValid() const
{
	return DungeonsId != "";
}

TSharedRef<FInternetAddr> FInternetAddrDungeonsWebRTC::Clone() const
{
	return MakeShareable(new FInternetAddrDungeonsWebRTC(DungeonsId, WebRTCPort));
}
