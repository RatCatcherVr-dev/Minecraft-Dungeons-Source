#pragma once

#include "CoreMinimal.h"
#include "OnlineSessionInterface.h"
#include "xsapiServicesInclude.h"

#include "utils.h"
#include "OnlineSessionInterfaceDungeons.h"
#include "GDKDungeons.h"

class FOnlineSessionDungeons;

class FOnlineSessionDungeonsGDK : public FOnlineSessionDungeons
{

public:
	/* Methods */
	FOnlineSessionDungeonsGDK(FOnlineSubsystemDungeons* InSubsystem);

	virtual ~FOnlineSessionDungeonsGDK();

	/** Struct for stashing session data received from GDK*/
	struct FPendingSessionData
	{
		bool bHaveSessionInfo;
		uint64_t joiningUserXuid;
		TOptional<FString> SessionHandle;

		FPendingSessionData()
			: bHaveSessionInfo(false)
			, joiningUserXuid(0)
			, SessionHandle()
		{
		}

		void Clear()
		{
			bHaveSessionInfo = false;
			joiningUserXuid = 0;
			SessionHandle.Reset();
		}
	};

	void PlatformTick(float DeltaTime) override;

private:
	void OnInviteReceived(const char* inviteUri);
	void TickPendingSession(float DeltaTime);

	GDKTaskQueueRegistrationToken registrationToken;
	FPendingSessionData PendingSession;
};

typedef TSharedPtr<FOnlineSessionDungeonsGDK, ESPMode::ThreadSafe> FOnlineSessionDungeonsGDKPtr;