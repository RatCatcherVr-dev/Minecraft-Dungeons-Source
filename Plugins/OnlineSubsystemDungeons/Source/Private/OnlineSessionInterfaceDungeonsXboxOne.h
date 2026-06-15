#pragma once

#include "CoreMinimal.h"
#include "OnlineSessionInterface.h"
#include "xsapiServicesInclude.h"

#include "utils.h"
#include "OnlineSessionInterfaceDungeons.h"

class FOnlineSessionDungeons;



class FOnlineSessionDungeonsXboxOne : public FOnlineSessionDungeons
{

public:
	/* Methods */
	FOnlineSessionDungeonsXboxOne(FOnlineSubsystemDungeons* InSubsystem);

	void CheckInviteFromOutsideGame();

	virtual ~FOnlineSessionDungeonsXboxOne();

	void PlatformTick(float DeltaTime) override;

	/** Responds to protocol activations to determine if an invite was accepted, and joins the session if so. */
	void OnActivated(Windows::ApplicationModel::Activation::IActivatedEventArgs^ EventArgs);

	/**
	 * Tick invites captured from launch URI
	 * Waits for a delegate to be listening before triggering
	 */
	void TickPendingSession(float DeltaTime);

	bool SaveInviteFromActivation(Windows::Foundation::Uri^ ActivationUri);
	bool SaveJoinFromActivation(Windows::Foundation::Uri^ ActivationUri);
	void SaveActivationSession(Windows::Xbox::System::User^ AcceptingUser, Platform::String^ SessionHandle);

	/** Any join/invite from a protocol activation */
	struct FPendingSessionData
	{
		/** Whether there is a pending invite or not */
		bool bHaveSessionInfo;

		/** Bool to track if a user privilege is in progress */
		bool bPrivilegeCheckInProgress;

		/** Cached pointer to the user who accepted the invite */
		Windows::Xbox::System::User^ AcceptingUser;

		/** Cached handle to the session to join */
		Platform::String^ SessionHandle;

		FPendingSessionData()
			: bHaveSessionInfo(false)
			, bPrivilegeCheckInProgress(false)
			, AcceptingUser(nullptr)
			, SessionHandle(nullptr)
		{
		}
	};

	/** Contains information about a join/invite parsed from the protocol activation */
	FPendingSessionData PendingSession;

	/** Token to track activation events, for invites that occur after launch */
	Windows::Foundation::EventRegistrationToken ActivatedToken;
};

typedef TSharedPtr<FOnlineSessionDungeonsXboxOne, ESPMode::ThreadSafe> FOnlineSessionDungeonsXboxOnePtr;