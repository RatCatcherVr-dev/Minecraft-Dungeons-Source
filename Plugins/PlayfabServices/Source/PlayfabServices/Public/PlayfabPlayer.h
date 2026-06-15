#pragma once

#include <string>
#include <UnrealString.h>

class PlayFabPlayer {
public:
	PlayFabPlayer(FString playerId) : mPlayerId{ playerId } {}
	void SetCredentials(FString playfabId, FString sessionTicket, FString entityToken, FString linkedXblAccount) {
		mPlayfabId = playfabId;
		mSessionTicket = sessionTicket;
		mEntityToken = entityToken;
		mLinkedXblAccount = linkedXblAccount;
	}
	bool HasCredentials() const { return !mSessionTicket.IsEmpty() && !mEntityToken.IsEmpty(); }
	void ClearCredentials() { mEntityToken.Empty(); mSessionTicket.Empty(); mAuthCallbacks.Empty(); }

	FString GetAuthToken(FString& AuthToken) { return mAuthTokenFunc ? mAuthTokenFunc(AuthToken) : FString(); }

	using AuthTokenFunction = TFunction<FString(FString &)>;
	void SetAuthenticationMethod(AuthTokenFunction InMethod) { mAuthTokenFunc = InMethod; }
	using AuthCallback = TFunction<void (bool)>;
	void AddAuthenticateCallback(AuthCallback callback) { mAuthCallbacks.Add(callback); }
	void ForAllCallbacks(bool success) {
		for (const auto& callback : mAuthCallbacks) {
			callback(success);
		}
		mAuthCallbacks.Empty();
	}

	void ClearLinkedXblAccount() { mLinkedXblAccount.Empty(); }
	void SetLinkedXblAccount(FString linkedXblAccount) { mLinkedXblAccount = linkedXblAccount; }
	const FString& GetLinkedXblAccount() const { return mLinkedXblAccount; }

	const FString& GetSessionTicket() const { return mSessionTicket; }
	const FString& GetEntityToken() const { return mEntityToken; }
	const FString& GetPlayerId() const { return mPlayerId; }
	const FString& GetPlayFabId() const { return mPlayfabId; }

	bool AuthenticationInProgress{ false };
	int FailedAuthenticationAttempts{ 0 };
private:
	FString mPlayerId;
	FString mPlayfabId;
	FString mSessionTicket;
	FString mEntityToken;
	FString mLinkedXblAccount;

	AuthTokenFunction mAuthTokenFunc;
	TArray<AuthCallback> mAuthCallbacks;
};
