#pragma once

#include "CoreMinimal.h"

#include "SocketSubsystem.h"
#include "Ticker.h"
#include "OnlineSubsystemDungeons.h"

FName CreateDungeonsSocketWebRTCSubsystem(FOnlineIdentityDungeonsPtr);

class FSocketSubsystemDungeonsWebRTC : public ISocketSubsystem, public FTickerObjectBase 
{

public:
	FSocketSubsystemDungeonsWebRTC(FOnlineIdentityDungeonsPtr identity) : LastSocketError(0), Identity(identity)
	{}
	~FSocketSubsystemDungeonsWebRTC() {}

	static FSocketSubsystemDungeonsWebRTC* Create(FOnlineIdentityDungeonsPtr);
	static void Destroy();

	virtual bool Init(FString& Error) override;
	virtual void Shutdown() override;
   	virtual class FSocket* CreateSocket(const FName& SocketType, const FString& SocketDescription, ESocketProtocolFamily ProtocolType) override;
	virtual void DestroySocket(class FSocket* Socket) override;
	virtual FAddressInfoResult GetAddressInfo(const TCHAR* HostName, const TCHAR* ServiceName = nullptr, EAddressInfoFlags QueryFlags = EAddressInfoFlags::Default, ESocketProtocolFamily ProtocolType = ESocketProtocolFamily::None, ESocketType SocketType = ESocketType::SOCKTYPE_Unknown) override;
	virtual ESocketErrors GetHostByName(const ANSICHAR* HostName, FInternetAddr& OutAddr) override;
	virtual bool RequiresChatDataBeSeparate() override;
	virtual bool RequiresEncryptedPackets() override;
	virtual bool GetHostName(FString& HostName) override;
	virtual TSharedRef<FInternetAddr> CreateInternetAddr(uint32 Address = 0, uint32 Port = 0) override;
	virtual bool HasNetworkDevice() override;
	virtual const TCHAR* GetSocketAPIName() const override;
	virtual ESocketErrors GetLastErrorCode() override;
	virtual ESocketErrors TranslateErrorCode(int32 Code) override;
	virtual bool GetLocalAdapterAddresses(TArray<TSharedPtr<FInternetAddr>>& OutAdresses) override;
	virtual bool IsSocketWaitSupported() const override;

	virtual bool Tick(float DeltaTime) override;

private:
	static FSocketSubsystemDungeonsWebRTC* SocketSubsystem;
	int32 LastSocketError;
	FOnlineIdentityDungeonsPtr Identity;
};