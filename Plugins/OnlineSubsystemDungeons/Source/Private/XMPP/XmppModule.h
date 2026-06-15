// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Misc/CoreMisc.h"
#include "XmppConnection.h"
#include "Modules/ModuleInterface.h"
#include "XmppMultiUserChat.h"

class Error;

/**
 * Module for Xmpp connections
 * Use CreateConnection to create a new Xmpp connection
 */
class FXmppModule
{

public:

	/**
	 * Called when Xmpp module is loaded
	 * Initialize platform specific parts of Xmpp handling
	 */
	static void StartupModule();

	/**
	 * Called when Xmpp module is unloaded
	 * Shutdown platform specific parts of Xmpp handling
	 */
	static void ShutdownModule();

	/**
	 * Singleton-like access to this module's interface.  This is just for convenience!
	 * Beware of calling this during the shutdown phase, though.  Your module might have been unloaded already.
	 *
	 * @return Returns singleton instance, loading the module on demand if needed
	 */
	static FXmppModule& Get();

	/**
	 * Checks to see if this module is loaded and ready.
	 *
	 * @return True if the module is loaded and ready to use
	 */
	static bool IsAvailable();

	/**
	 * Creates a new Xmpp connection for the current platform and associated it with the user
	 *
	 * @return new Xmpp connection instance
	 */
	TSharedRef<class IXmppConnection> CreateConnection(const FString& UserId);

	/**
	 * Return an existing Xmpp connection associated with a user
	 *
	 * @return new Xmpp connection instance
	 */
	TSharedPtr<class IXmppConnection> GetConnection(const FString& UserId) const;

	/**
	 * Remove an existing Xmpp connection associated with a user
	 *
	 * @param UserId user to find connection for
	 */
	void RemoveConnection(const FString& UserId);

	/**
	 * Clean up any pending connection removals
	 */
	void ProcessPendingRemovals();

	/**
	 * Remove an existing Xmpp connection
	 *
	 * @param Connection reference to find/remove
	 */
	void RemoveConnection(const TSharedRef<class IXmppConnection>& Connection);

	/**
	 * @return true if Xmpp requests are globally enabled
	 */
	inline bool IsXmppEnabled() const
	{
		return bEnabled;
	}

private:

	// IModuleInterface

	void OnXmppRoomCreated(const TSharedRef<IXmppConnection>& Connection, bool bSuccess, const FXmppRoomId& RoomId, const FString& Error);
	void OnXmppRoomConfigured(const TSharedRef<IXmppConnection>& Connection, bool bSuccess, const FXmppRoomId& RoomId, const FString& Error);

	/**
	 * Connection cleanup before removal
	 */
	void CleanupConnection(const TSharedRef<class IXmppConnection>& Connection);

	/** toggles Xmpp requests */
	static bool bEnabled;
	/** singleton for the module while loaded and available */
	static FXmppModule* Singleton;

	/** Active Xmpp server connections mapped by user id */
	TMap<FString, TSharedRef<class IXmppConnection>> ActiveConnections;
	/** Xmpp connections pending removal on next tick */
	TSet<TSharedPtr<IXmppConnection>> PendingRemovals;
	/** Keep track of removed connections pending cleanup */
	TArray<TSharedRef<class IXmppConnection>> PendingDeleteConnections;
};
