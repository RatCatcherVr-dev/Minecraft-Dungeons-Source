#pragma once

#include "IPAddress.h"
#include "OnlineSubsystemDungeonsTypes.h"

class FInternetAddrDungeonsWebRTC : public FInternetAddr
{
public:

	FInternetAddrDungeonsWebRTC();
	FInternetAddrDungeonsWebRTC(uint64 XUID, int32 Port);
	FInternetAddrDungeonsWebRTC(const FString& XUID, int32 Port);
	FInternetAddrDungeonsWebRTC(const FInternetAddrDungeonsWebRTC& Src);
	FInternetAddrDungeonsWebRTC(const FUniqueNetIdDungeons& InDungeonsID, int32 Port);
	~FInternetAddrDungeonsWebRTC();

	virtual void SetIp(uint32 InAddr) override;
	virtual void SetIp(const TCHAR* InAddr, bool& bIsValid) override;
	virtual void GetIp(uint32& OutAddr) const override;
	virtual void SetPort(int32 InPort) override;
	virtual int32 GetPort() const override;
	virtual void SetRawIp(const TArray<uint8>& RawAddr) override;
	virtual TArray<uint8> GetRawIp() const override;
	virtual void SetAnyAddress() override;
	virtual void SetBroadcastAddress() override;
	virtual void SetLoopbackAddress() override;
	virtual FString ToString(bool bAppendPort) const override;
	const FString& ToRefString() const;
	virtual uint32 GetTypeHash() const override;
	virtual bool operator==(const FInternetAddr& Other) const override;
	virtual bool IsValid() const override;
	virtual TSharedRef<FInternetAddr> Clone() const override;

	friend class FSocketDungeonsWebRTC;

private:
	FString DungeonsId;
	int32 WebRTCPort; //Currently not used - only present for Get/SetPort
};