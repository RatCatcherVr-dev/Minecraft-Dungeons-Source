#pragma once
#include "StropheStanza.h"
#include "Queue.h"
#include "Ticker.h"
#include "XmppConnectionStrophe.h"

struct RelayTokenInfo
{
	FString Username;
	FString Password;
	FString StanzaId;
};

class FXmppRelayTokenStrophe : public FTickerObjectBase
{
public:
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnXmppRelayTokenReceived, const TSharedRef<RelayTokenInfo>&);

	FXmppRelayTokenStrophe(FXmppConnectionStrophe& InConnectionManager);
	virtual ~FXmppRelayTokenStrophe() = default;
	void OnDisconnect();
	bool ReceiveStanza(const FStropheStanza& IncomingStanza);
	FOnXmppRelayTokenReceived& OnReceiveRelayToken() { return OnXmppRelayTokenReceivedDelegate; }

	bool RequestAuthToken(const FXmppUserJid& RecipientId, const FString& StanzaId);

	virtual bool Tick(float DeltaTime) override;

protected:

private:
	FXmppConnectionStrophe& ConnectionManager;
	TQueue<TUniquePtr<RelayTokenInfo>> IncomingRelayTokenUpdates;
	FOnXmppRelayTokenReceived OnXmppRelayTokenReceivedDelegate;
};