#include "PCHOnlineDungeonsSubsystem.h"
#include "OnlineAsyncTaskDungeonsTriggers.h"

#include "OnlineSubsystemDungeons.h"
#include "OnlineSessionInterfaceDungeons.h"
#include "OnlineIdentityDungeons.h"

void OnlineAsyncTaskDungeonsTriggerOnSessionInviteReceived::TriggerDelegates()
{
	if (!SessionPtr.IsValid())
	{
		UE_LOG_ONLINE(Warning, TEXT("[AsyncInviteReceived] SessionPtr is null"));
		return;
	}

	SessionPtr->TriggerOnSessionInviteReceivedDelegates(FUniqueNetIdDungeons(), FUniqueNetIdString(SenderName), "", SearchResult);
}

void OnlineAsyncTaskDungeonsTriggerOnJoinSessionComplete::Tick()
{
	if (IdentityPtr->GetPeer2PeerManager() && IdentityPtr->GetPeer2PeerManager()->IsXMPPLoggedIn())
	{
		bIsComplete = true;
		bWasSuccessful = true;
		return;
	}

	if (TimerUtils::GetCurrentTime() - TaskStart > OnlineSubsystemConstants::XMPPLOGINTIMEOUT)
	{
		/* timeout - treat it as we failed to sign in to xmpp */
		bIsComplete = true;
		bWasSuccessful = false;
		UE_LOG_ONLINE(Log, TEXT("[AsyncInviteReceived] Can't join online session as we failed to login to XMPP"));
		return;
	}
}

void OnlineAsyncTaskDungeonsTriggerOnJoinSessionComplete::TriggerDelegates()
{

	OnCompleteCallback(WasSuccessful(), SessionName);
}