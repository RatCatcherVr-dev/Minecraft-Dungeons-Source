#include "XmppRelayTokenStrophe.h"
#include "StropheStanzaConstants.h"
#include "OnlineSubsystem.h"
#include "XmppConnection.h"
#include "XmppLog.h"

FXmppRelayTokenStrophe::FXmppRelayTokenStrophe(FXmppConnectionStrophe& InConnectionManager)
	: ConnectionManager(InConnectionManager)
{
	IncomingRelayTokenUpdates.Empty();
}

void FXmppRelayTokenStrophe::OnDisconnect()
{

}

bool FXmppRelayTokenStrophe::ReceiveStanza(const FStropheStanza& IncomingStanza)
{
	if (IncomingStanza.GetType() == "result" && IncomingStanza.GetName() == Strophe::SN_IQ)
	{
		TOptional<const FStropheStanza> RelayStanza = IncomingStanza.GetChild("genrelaytokenresponse");

		if (RelayStanza.IsSet())
		{
			RelayTokenInfo RelayInfo;
			RelayInfo.Username = RelayStanza->GetAttribute("username");
			RelayInfo.Password = RelayStanza->GetAttribute("password");
			RelayInfo.StanzaId = IncomingStanza.GetId();
			UE_LOG_ONLINE(Log, TEXT("[XMPP] Received and handled relay username/password from XMPP server"));
			return IncomingRelayTokenUpdates.Enqueue(MakeUnique<RelayTokenInfo>(MoveTemp(RelayInfo)));
		}
	}
	return false;
}

bool FXmppRelayTokenStrophe::RequestAuthToken(const FXmppUserJid& RecipientId, const FString& StanzaId)
{
	if (ConnectionManager.GetLoginStatus() != EXmppLoginStatus::LoggedIn)
	{
		return false;
	}

	if (!RecipientId.IsValid())
	{
		UE_LOG(LogXmpp, Warning, TEXT("Unable to send message. Invalid jid: %s"), *RecipientId.ToDebugString());
		return false;
	}

	FStropheStanza MessageStanza(ConnectionManager, Strophe::SN_IQ);
	{
		MessageStanza.SetTo(RecipientId.Id);
		MessageStanza.SetType("get");
		MessageStanza.SetId(StanzaId);

		const char* NS_GENRELAYTOKEN = "jabber:iq:genrelaytoken";
		const char* NS_QN_QUERY = "query";
		//Strophe::SN_QUERY;
		FStropheStanza ChildStanza(ConnectionManager, Strophe::SN_QUERY);
		ChildStanza.SetNamespace(NS_GENRELAYTOKEN);

		MessageStanza.AddChild(ChildStanza);
	}

	return ConnectionManager.SendStanza(MoveTemp(MessageStanza));
}

bool FXmppRelayTokenStrophe::Tick(float DeltaTime)
{
	while(!IncomingRelayTokenUpdates.IsEmpty())
	{
		TUniquePtr<RelayTokenInfo> RelayInfoPtr;
		
		if (IncomingRelayTokenUpdates.Dequeue(RelayInfoPtr))
		{
			check(RelayInfoPtr.IsValid());
			OnXmppRelayTokenReceivedDelegate.Broadcast(MakeShareable(RelayInfoPtr.Release()));
		}
	}
	return true;
}

