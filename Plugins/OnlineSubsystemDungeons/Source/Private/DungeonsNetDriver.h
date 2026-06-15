#pragma once

#include "CoreMinimal.h"
#include "IpNetDriver.h"
#include "DungeonsNetDriver.generated.h"

UCLASS(transient, config = Engine)
class UDungeonsNetDriver : public UIpNetDriver
{
	GENERATED_UCLASS_BODY()

	//~ Begin UObject Interface
	virtual void PostInitProperties() override;
	//~ End UObject Interface

	bool DoInitBase(bool bInitAsClient, FNetworkNotify* InNotify, const FURL& URL, bool bReuseAddressAndPort, FString& Error);
	virtual bool InitBase(bool bInitAsClient, FNetworkNotify* InNotify, const FURL& URL, bool bReuseAddressAndPort, FString& Error) override;

	void DoInitConnect(FNetworkNotify* InNotify, const FURL& ConnectURL, FString& Error);
	virtual bool InitConnect(FNetworkNotify* InNotify, const FURL& ConnectURL, FString& Error) override;

	bool DoInitListen(FNetworkNotify* InNotify, FURL& LocalURL, bool bReuseAddressAndPort, FString& Error);
	virtual bool InitListen(FNetworkNotify* InNotify, FURL& LocalURL, bool bReuseAddressAndPort, FString& Error) override;

	virtual bool IsAvailable() const override;
};