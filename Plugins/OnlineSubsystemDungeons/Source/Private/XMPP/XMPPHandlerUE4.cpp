#include "PCHOnlineDungeonsSubsystem.h"
#include "XMPPHandlerUE4.h"

#include "OnlineSubsystem.h"
#include "P2P/Peer2PeerManager.h"
#include "utils.h"
#include "XmppConnectionStrophe.h"
#include "StropheStanza.h"
#include "StropheStanzaConstants.h"
#include "XmppRelayTokenStrophe.h"
#include "Misc/ScopeLock.h"
#include "Online.h"

XMPPHandlerUE4::XMPPHandlerUE4(Peer2PeerManager* InPeer2PeerMgr)
	: 	Peer2PeerMgr(InPeer2PeerMgr)
	, IsSigningInOut(false)
	, RelayTokenRequestInProgress(false)
	, RelayTokenRequestId("")
	, RelayTokenRequestTime(0ULL)
	, RelayTokenResponseTime(0ULL)
{
	ClearLoginCounters();
}

XMPPHandlerUE4::~XMPPHandlerUE4()
{
	TSharedPtr<IXmppConnection> XMPPConnection = FXmppModule::Get().GetConnection(LoggedInXUID);

	if (XMPPConnection && XMPPConnection->GetLoginStatus() != EXmppLoginStatus::LoggedOut)
	{
		Signout(LoggedInXUID);
	}

	if (XMPPConnection) {
		FXmppUserJid Jid;
		Jid.Id = LoggedInXUID;
		ClearDelegates(Jid);
	}
}

//Signot all XMPPs
void XMPPHandlerUE4::Signout(const FString& XUID) 
{
	TSharedPtr<IXmppConnection> XMPPConnection = FXmppModule::Get().GetConnection(XUID);

	if (!XMPPConnection)
	{
		return;
	}

	XMPPConnection->Logout();
	IsSigningInOut = true;
	ClearLoginCounters();
}

void XMPPHandlerUE4::Signin(const FString& XUID, const FString& XMPPToken)
{
	TSharedPtr<IXmppConnection> XMPPConnection = FXmppModule::Get().GetConnection(XUID);

	if (XMPPToken.IsEmpty())
	{
		UE_LOG_ONLINE(Warning, TEXT("[XMPP] XblToken is empty, will not try to signin"));
		return;
	}

	SignInTime = TimerUtils::GetCurrentTime();

	if (XMPPConnection)
	{
		XMPPConnection->Login(XUID, XMPPToken);
		LoggedInXUID = XUID;
		SetupDelegates(XMPPConnection);

		IsSigningInOut = true;
		return;
	}

	XMPPConnection = FXmppModule::Get().CreateConnection(XUID);
	SetupDelegates(XMPPConnection);

	FXmppServer XMPPServerConf;
	XMPPServerConf.bUseSSL = true;
	XMPPServerConf.ClientResource = "none";
	XMPPServerConf.AppId = FString(ANSI_TO_TCHAR(OnlineSubsystemConstants::XMPPAPPID));
	XMPPServerConf.ServerAddr = FString(ANSI_TO_TCHAR(OnlineSubsystemConstants::XMPPADDRESS));
	XMPPServerConf.Domain = FString(ANSI_TO_TCHAR(OnlineSubsystemConstants::XMPPDOMAIN));

	XMPPServerConf.ServerPort = OnlineSubsystemConstants::XMPPPORT;
	XMPPServerConf.bUsePlainTextAuth = true;
	XMPPConnection->SetServer(XMPPServerConf);
	   
	LoggedInXUID = XUID;
	XMPPConnection->Login(XUID, XMPPToken);
	IsSigningInOut = true;
}

bool XMPPHandlerUE4::IsSignedIn() const
{
	TSharedPtr<IXmppConnection> XMPPConnection = FXmppModule::Get().GetConnection(LoggedInXUID);

	if(!XMPPConnection)
	{
		return false;
	}

	if (XMPPConnection->GetLoginStatus() != EXmppLoginStatus::LoggedIn)
	{
		return false;
	}
	return true;
}

void XMPPHandlerUE4::SendMessage(const FString& XUID, const FString& RemoteXUID, const FString& Message)
{
	TSharedPtr<IXmppConnection> XMPPConnection = FXmppModule::Get().GetConnection(XUID);

	if (!XMPPConnection)
	{
		return;
	}

	if (XMPPConnection->GetLoginStatus() != EXmppLoginStatus::LoggedIn)
	{
		UE_LOG_ONLINE(Log, TEXT("[XMPP] User not logged in"));
	}

	FXmppUserJid UserJid;
	UserJid.Id = RemoteXUID;
	UserJid.Domain = FString(ANSI_TO_TCHAR(OnlineSubsystemConstants::XMPPDOMAIN));

	bool result = XMPPConnection->Messages()->SendMessage(UserJid, "", Message, false);

	if (!result)
	{
		UE_LOG_ONLINE(Log, TEXT("[XMPP] Failed to send message to '%s'"), *UserJid.Id);
	}
}

void XMPPHandlerUE4::SetPresence(const FString& XUID)
{
	TSharedPtr<IXmppConnection> XMPPConnection = FXmppModule::Get().GetConnection(XUID);
	FXmppUserPresence XmppPresence = XMPPConnection->Presence()->GetPresence();
	XmppPresence.Status = EXmppPresenceStatus::Online;
	XMPPConnection->Presence()->UpdatePresence(XmppPresence);
}

bool XMPPHandlerUE4::ShouldUpdateAuthToken() const
{
	if (!IsSignedIn())
	{
		return false;
	}

	if (RelayTokenResponseTime != 0
		&& RelayTokenRequestInProgress
		&& RelayTokenRequestTime != 0
		&& TimerUtils::GetCurrentTime() - RelayTokenRequestTime <= OnlineSubsystemConstants::TOKENREQUESTTIMEOUT)
	{
		return false;
	}

	if (RelayTokenResponseTime != 0 && TimerUtils::GetTimeSince(RelayTokenResponseTime) < OnlineSubsystemConstants::RELAYTOKENVALIDITYTIME && RelayTokenResponseTime != 0)
	{
		return false;
	}

	return true; 
}

void XMPPHandlerUE4::UpdateAuthToken()
{
	UE_LOG_ONLINE(Log, TEXT("[XMPP] Requesting relay token"));

	TSharedPtr<IXmppConnection> XMPPConnection = FXmppModule::Get().GetConnection(LoggedInXUID);

	/* Request genrelay token */
	FXmppUserJid RelaytokenServer;
	RelaytokenServer.Id = FString(ANSI_TO_TCHAR(OnlineSubsystemConstants::XMPPMSGADDRESS));
	RelaytokenServer.Domain = FString(ANSI_TO_TCHAR(OnlineSubsystemConstants::XMPPDOMAIN));
	FString TempRelayTokenId;
	{
		FScopeLock IdLock(&CriticalSection);
		TempRelayTokenId = RelayTokenRequestId = FGuid::NewGuid().ToString();
	}
	XMPPConnection->RelayToken()->RequestAuthToken(RelaytokenServer, TempRelayTokenId);

	RelayTokenRequestTime = TimerUtils::GetCurrentTime();
	RelayTokenRequestInProgress = true;
}

bool XMPPHandlerUE4::IsAllowedToSignIn() const
{
	//Calculate backoff time
	unsigned long long RetryTime = BackOffCounter * OnlineSubsystemConstants::XMPPRETRYTIME;

	if (SignInTime != 0 && TimerUtils::GetTimeSince(SignInTime) < RetryTime)
	{
		return false;
	}

	return !IsSignedIn() && !IsSigningInOut;
}

bool XMPPHandlerUE4::IsAllowedToSignOut() const
{
	return IsSignedIn() && !IsSigningInOut;
}

void XMPPHandlerUE4::ClearLoginCounters()
{
	BackOffCounter = 1;
	SignInTime = 0;
}

void XMPPHandlerUE4::OnXMPPLoginComplete(const FXmppUserJid& UserJid, bool bWasSuccess, const FString& Error)
{
	UE_LOG_ONLINE(Log, TEXT("[XMPP] Login to XMPP server %s"), bWasSuccess ? *FString("was successful") : *FString("failed"), *Error);
	IsSigningInOut = false;

	if (!bWasSuccess)
	{
		ClearDelegates(UserJid);
		if (BackOffCounter < 5)
		{
			BackOffCounter++;
		}
		return;
	}

	ClearLoginCounters();
	TSharedPtr<IXmppConnection> XMPPConnection = FXmppModule::Get().GetConnection(UserJid.Id);
	SetPresence(UserJid.Id);
}

void XMPPHandlerUE4::OnXMPPLogoutComplete(const FXmppUserJid& UserJid, bool bWasSuccess, const FString& Error)
{
	UE_LOG_ONLINE(Log, TEXT("[XMPP] Logout %s - %s"), bWasSuccess ? *FString("successfully") : *FString("failed"), *Error);
	ClearDelegates(UserJid);
	IsSigningInOut = false;
	ClearRelayTokenState();
	ClearLoginCounters();
}

void XMPPHandlerUE4::OnXMPPLoginChanged(const FXmppUserJid& UserJid, EXmppLoginStatus::Type LoginState)
{
	UE_LOG_ONLINE(Log, TEXT("[XMPP] LoginState changed: '%s'"), ToString(LoginState));
	if (LoginState == EXmppLoginStatus::LoggedOut)
	{
		ClearDelegates(UserJid);
		IsSigningInOut = false;
		ClearRelayTokenState();
		ClearLoginCounters();
	}
}

void XMPPHandlerUE4::OnXMPPMessageReceived(const TSharedRef<IXmppConnection>& Connection, const FXmppUserJid& FromJid, const TSharedRef<FXmppMessage>& Message)
{
	static const char* szConnectRequestCommand = "CONNECTREQUEST ";
	static const char* szConnectResponseCommand = "CONNECTRESPONSE ";
	static const char* szCandidateAddCommand = "CANDIDATEADD ";

	/* Parse incoming XML document */
	FString Payload = Message->Payload;
	
	if (Payload.IsEmpty())
	{
		return;
	}

	if (Payload.Find(szConnectRequestCommand) == 0)
	{
		FString RequestBody = Payload.RightChop(strlen(szConnectRequestCommand));

		/* Process XUID */
		FString XUID;
		if (ParseMessageXUID(RequestBody, XUID))
		{
			if (XUID == "0")
			{
				UE_LOG_ONLINE(Log, TEXT("[WebRTC] Received connection request from XUID='0', returning.."));
				return;
			}
			Peer2PeerMgr->ProcessConnectRequest(XUID, RequestBody);
		}
	}
	else if (Payload.Find(szConnectResponseCommand) == 0)
	{
		FString RequestBody = Payload.RightChop(strlen(szConnectResponseCommand));

		/* Process XUID */
		FString XUID;
		if (ParseMessageXUID(RequestBody, XUID))
		{
			if (XUID == "0")
			{
				UE_LOG_ONLINE(Log, TEXT("[WebRTC] Received connect response from XUID='0', returning.."));
				return;
			}
			Peer2PeerMgr->ProcessConnectResponse(XUID, RequestBody);
		}
	}
	else if (Payload.Find(szCandidateAddCommand) == 0)
	{
		FString RequestBody = Payload.RightChop(strlen(szCandidateAddCommand));

		/* Process XUID */
		FString XUID;
		if (ParseMessageXUID(RequestBody, XUID))
		{
			if (XUID == "0")
			{
				UE_LOG_ONLINE(Log, TEXT("[WebRTC] Received candidate add from XUID='0', returning.."));
				return;
			}
			Peer2PeerMgr->ProcessCandidateAdd(XUID, RequestBody);
		}
	}
	else
	{
		//Error log?
	}
}

void XMPPHandlerUE4::OnRelayTokenReceived(const TSharedRef<RelayTokenInfo>& RelayToken)
{
	if (RelayToken->Username.IsEmpty() || RelayToken->Password.IsEmpty())
	{
		UE_LOG_ONLINE(Warning, TEXT("[XMPP] Received empty username or password from XMPP server (username='%s', password='%s')")
			, *(RelayToken->Username)
			, *(RelayToken->Password)
		);

		RelayTokenResponseTime = 0;
	}
	else
	{
		{
			FScopeLock IdLock(&CriticalSection);
			if (RelayToken->StanzaId != RelayTokenRequestId)
			{
				//If the received relay token was not the most recent -> discard
				return;
			}
		}
		const FString Username = *RelayToken->Username;
		const FString Password = *RelayToken->Password;
		Peer2PeerMgr->SetP2PConfiguration(TCHAR_TO_ANSI(*Username), TCHAR_TO_ANSI(*Password));
		RelayTokenResponseTime = TimerUtils::GetCurrentTime();
	}
	RelayTokenRequestInProgress = false;
}

bool XMPPHandlerUE4::ParseMessageXUID(FString& BodyText, FString& XUID)
{
	int32 pos = BodyText.Find(" ");

	if (pos == -1)
	{
		return false;
	}

	XUID = BodyText.Left(pos);
	BodyText = BodyText.RightChop(pos + 1);
	return true;
}

void XMPPHandlerUE4::ClearDelegates(const FXmppUserJid& UserJid)
{
	TSharedPtr<IXmppConnection> XMPPConnection = FXmppModule::Get().GetConnection(UserJid.Id);

	if (!XMPPConnection.IsValid()) //Temporary fix until we can track down the root cause of why XMPPConnection is Invalid.
	{
		checkNoEntry();
		return;
	}

	if (LoginCompleteDelegateHandle.IsValid())
	{
		XMPPConnection->OnLoginComplete().Remove(LoginCompleteDelegateHandle);
		LoginCompleteDelegateHandle.Reset();
	}
	if (LogoutCompleteDelegateHandle.IsValid())
	{
		XMPPConnection->OnLogoutComplete().Remove(LogoutCompleteDelegateHandle);
		LogoutCompleteDelegateHandle.Reset();
	}
	if (LoginChangedCompleteDelegateHandle.IsValid())
	{
		XMPPConnection->OnLoginChanged().Remove(LoginChangedCompleteDelegateHandle);
		LoginChangedCompleteDelegateHandle.Reset();
	}
	if (MessageReceivedDelegate.IsValid())
	{
		XMPPConnection->Messages()->OnReceiveMessage().Remove(MessageReceivedDelegate);
		MessageReceivedDelegate.Reset();
	}
	if (RelayTokenReceivedDelegate.IsValid())
	{
		XMPPConnection->RelayToken()->OnReceiveRelayToken().Remove(RelayTokenReceivedDelegate);
		RelayTokenReceivedDelegate.Reset();
	}
	
	FXmppModule::Get().RemoveConnection(UserJid.Id);
}

void XMPPHandlerUE4::SetupDelegates(const TSharedPtr<IXmppConnection>& XMPPConnection)
{

	assert(!LoginCompleteDelegateHandle.IsValid());
	assert(!LogoutCompleteDelegateHandle.IsValid());
	assert(!LoginChangedCompleteDelegateHandle.IsValid());
	assert(!MessageReceivedDelegate.IsValid());
	assert(!RelayTokenReceivedDelegate.IsValid());

	LoginCompleteDelegateHandle = XMPPConnection->OnLoginComplete().AddRaw(this, &XMPPHandlerUE4::OnXMPPLoginComplete);
	LogoutCompleteDelegateHandle = XMPPConnection->OnLogoutComplete().AddRaw(this, &XMPPHandlerUE4::OnXMPPLogoutComplete);
	LoginChangedCompleteDelegateHandle = XMPPConnection->OnLoginChanged().AddRaw(this, &XMPPHandlerUE4::OnXMPPLoginChanged);
	MessageReceivedDelegate = XMPPConnection->Messages()->OnReceiveMessage().AddRaw(this, &XMPPHandlerUE4::OnXMPPMessageReceived);
	RelayTokenReceivedDelegate = XMPPConnection->RelayToken()->OnReceiveRelayToken().AddRaw(this, &XMPPHandlerUE4::OnRelayTokenReceived);
}

void XMPPHandlerUE4::ClearRelayTokenState()
{
	RelayTokenRequestInProgress = false;
	RelayTokenRequestTime = 0ULL,
	RelayTokenResponseTime = 0ULL;
	{
		FScopeLock IdLock(&CriticalSection);
		RelayTokenRequestId = "";
	}
}