// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "XmppModule.h"
#include "Misc/CommandLine.h"
#include "Misc/ConfigCacheIni.h"
#include "Modules/ModuleManager.h"
#include "XmppMessages.h"
#include "XmppPubSub.h"
#include "XmppLog.h"
#include "XmppTests.h"
#include "XmppNull.h"
#if WITH_XMPP_JINGLE
#include "XmppJingle/XmppJingle.h"
#endif
#if WITH_XMPP_STROPHE
#include "XmppStrophe/XmppStrophe.h"
#include "WebSocketsModule.h"
#endif
#include "OnlineSubsystem.h"

DEFINE_LOG_CATEGORY(LogXmpp);

// FXmppModule

FXmppModule* FXmppModule::Singleton = nullptr;
bool FXmppModule::bEnabled = false;

void FXmppModule::StartupModule()
{
	UE_LOG_ONLINE(Log, TEXT("[XMPP] Creating XMPP module"));
	Singleton = new FXmppModule();

	bEnabled = true;
	GConfig->GetBool(TEXT("XMPP"), TEXT("bEnabled"), bEnabled, GEngineIni);

	if (bEnabled)
	{
#if WITH_XMPP_JINGLE
		FXmppJingle::Init();
#endif
#if WITH_XMPP_STROPHE
		FXmppStrophe::Init();
		FModuleManager::LoadModuleChecked<FWebSocketsModule>("WebSockets");
#endif
	}
}

void FXmppModule::ShutdownModule()
{
	UE_LOG_ONLINE(Log, TEXT("[XMPP] Shutting down XMPP module"));

	if (bEnabled)
	{
#if WITH_XMPP_JINGLE
		FXmppJingle::Cleanup();
#endif
#if WITH_XMPP_STROPHE
		FXmppStrophe::Cleanup();
#endif
	}
	delete Singleton;
	Singleton = nullptr;
}

FXmppModule& FXmppModule::Get()
{
	if (Singleton == nullptr)
	{
		check(false && "Failed to get XMPP singleton");
	}
	check(Singleton != nullptr);
	return *Singleton;
}

bool FXmppModule::IsAvailable()
{
	return Singleton != nullptr;
}

TSharedRef<IXmppConnection> FXmppModule::CreateConnection(const FString& UserId)
{
	TSharedPtr<IXmppConnection> Connection = GetConnection(UserId);
	if (Connection.IsValid())
	{
		return Connection.ToSharedRef();
	}
	else
	{
		bool bEnableWebsockets = false;
		GConfig->GetBool(TEXT("XMPP"), TEXT("bEnableWebsockets"), bEnableWebsockets, GEngineIni);

		bool bUseStrophe = WITH_XMPP_STROPHE && bEnableWebsockets;
		bool bUseJingle = WITH_XMPP_JINGLE && !bUseStrophe;
		if (!bUseJingle && !bUseStrophe)
		{
			// if not using websockets, use the previous default implementation (jingle if available, otherwise strophe)
#if WITH_XMPP_JINGLE
			bUseJingle = true;
#elif WITH_XMPP_STROPHE
			bUseStrophe = true;
#endif
		}

		if (bEnabled && (bUseStrophe || bUseJingle))
		{
#if WITH_XMPP_STROPHE
			if (bUseStrophe)
			{
				Connection = FXmppStrophe::CreateConnection();
			}
#endif
#if WITH_XMPP_JINGLE
			if (bUseJingle)
			{
				Connection = FXmppJingle::CreateConnection();
			}
#endif
		}
		else
		{
			Connection = FXmppNull::CreateConnection();
		}

		return ActiveConnections.Add(UserId, Connection.ToSharedRef());
	}
}

TSharedPtr<IXmppConnection> FXmppModule::GetConnection(const FString& UserId) const
{
	TSharedPtr<IXmppConnection> Result;

	const TSharedRef<IXmppConnection>* Found = ActiveConnections.Find(UserId);
	if (Found != nullptr)
	{
		Result = *Found;
	}

	return Result;
}

void FXmppModule::RemoveConnection(const FString& UserId)
{
	TSharedPtr<IXmppConnection> Existing = GetConnection(UserId);
	if (Existing.IsValid())
	{
		CleanupConnection(Existing.ToSharedRef());
		// If we found a TSharedPtr, keep on a removal list until the next tick so it doesn't get destroyed while ticking other things that depend on it
		PendingRemovals.Add(Existing);
	}

	ActiveConnections.Remove(UserId);
}

void FXmppModule::ProcessPendingRemovals()
{
	if (PendingRemovals.Num() > 0)
	{
		PendingRemovals.Empty();
	}
}

void FXmppModule::RemoveConnection(const TSharedRef<IXmppConnection>& Connection)
{
	for (auto It = ActiveConnections.CreateIterator(); It; ++It)
	{
		if (It.Value() == Connection)
		{
			CleanupConnection(Connection);
			It.RemoveCurrent();
			break;
		}
	}
}

void FXmppModule::CleanupConnection(const TSharedRef<class IXmppConnection>& Connection)
{

}

void FXmppModule::OnXmppRoomCreated(const TSharedRef<IXmppConnection>& Connection, bool bSuccess, const FXmppRoomId& RoomId, const FString& Error)
{
	Connection->MultiUserChat()->OnRoomCreated().RemoveAll(this);

	UE_LOG(LogXmpp, Log, TEXT("FXmppModule::OnXmppRoomCreated - entered - user(%s) room(%s)"), *Connection->GetUserJid().Id, *RoomId);
}

void FXmppModule::OnXmppRoomConfigured(const TSharedRef<IXmppConnection>& Connection, bool bSuccess, const FXmppRoomId& RoomId, const FString& Error)
{
	Connection->MultiUserChat()->OnRoomConfigured().RemoveAll(this);

	UE_LOG(LogXmpp, Log, TEXT("FXmppModule::OnXmppRoomConfigured - entered - user(%s) room(%s)"), *Connection->GetUserJid().Id, *RoomId);
}