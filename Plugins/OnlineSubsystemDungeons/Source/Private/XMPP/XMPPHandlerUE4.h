#pragma once

#include "CoreMinimal.h"

#include "XmppModule.h"
#include "XmppConnection.h"
#include "XmppMessages.h"
#include "XmppRelayTokenStrophe.h"
#include "Templates/Atomic.h"
#include <atomic>

class Peer2PeerManager;
class XMPPHandlerUE4
{

public:
	XMPPHandlerUE4(Peer2PeerManager* InPeer2PeerMgr);
	~XMPPHandlerUE4();

	void Signout(const FString& XUID);
	void Signin(const FString& XUID, const FString& XBLToken);
	bool IsSignedIn() const;

	void SendMessage(const FString& XUID, const FString& RemoteXUID, const FString& Message);
	void SetPresence(const FString& XUID);

	bool ShouldUpdateAuthToken() const;
	void UpdateAuthToken();
	bool HasRelayToken() const { return RelayTokenResponseTime != 0; }

	bool IsAllowedToSignIn() const;
	bool IsAllowedToSignOut() const;
	void ClearLoginCounters();

private:
	void OnXMPPLoginComplete(const FXmppUserJid& /*UserJid*/, bool /*bWasSuccess*/, const FString& /*Error*/);
	void OnXMPPLogoutComplete(const FXmppUserJid& /*UserJid*/, bool /*bWasSuccess*/, const FString& /*Error*/);
	void OnXMPPLoginChanged(const FXmppUserJid& /*UserJid*/, EXmppLoginStatus::Type /*LoginState*/);
	void OnXMPPMessageReceived(const TSharedRef<IXmppConnection>& /*Connection*/, const FXmppUserJid& /*FromJid*/, const TSharedRef<FXmppMessage>& /*Message*/);
	void OnRelayTokenReceived(const TSharedRef<RelayTokenInfo>& RelayToken);

	bool ParseMessageXUID(FString& BodyText, FString& XUID);

	void ClearDelegates(const FXmppUserJid& UserJid);
	void SetupDelegates(const TSharedPtr<IXmppConnection>& XMPPConnection);

	void ClearRelayTokenState();

	FDelegateHandle LoginCompleteDelegateHandle;
	FDelegateHandle LogoutCompleteDelegateHandle;
	FDelegateHandle LoginChangedCompleteDelegateHandle;
	FDelegateHandle MessageReceivedDelegate;
	FDelegateHandle RelayTokenReceivedDelegate;

	Peer2PeerManager *Peer2PeerMgr;
	FString LoggedInXUID;
	bool IsSigningInOut;

	//Write/read from gamethread & xmppthread
	std::atomic<bool> RelayTokenRequestInProgress;
	FString RelayTokenRequestId;
	FCriticalSection CriticalSection;
	std::atomic<unsigned long long> RelayTokenRequestTime;
	std::atomic<unsigned long long> RelayTokenResponseTime;

	int BackOffCounter;
	unsigned long long SignInTime;
};
