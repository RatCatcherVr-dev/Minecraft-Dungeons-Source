#include "Dungeons.h"
#include "trials/AnonymousTrialsProvider.h"
#include "online/sessions/OnlineUtil.h"
#include "MinecraftAPI.h"
#include "IDungeonsAPIClient.h"
#include "IDungeonsAuth.h"
#include "Online/sessions/OnlineUtil.h"
#include "Online/crossplay/Identity.h"

void UMinecraftAPI::Init() {
	if (IDungeonsAPIClient::IsAvailable()) {
		auto auth = IDungeonsAPIClient::Get().Auth();	
		
		if (!auth->OnClientSuccessfullyAuthenticated.IsBoundToObject(this)) {
			OnSuccessfulAuthenticationHandle = auth->OnClientSuccessfullyAuthenticated.AddUObject(this, &UMinecraftAPI::OnSuccessfulAuthentication);	
		}

		if (!auth->OnClientAuthenticationFailed.IsBoundToObject(this)) {
			OnFailedAuthenticationHandle = auth->OnClientAuthenticationFailed.AddUObject(this, &UMinecraftAPI::OnFailedAuthentication);	
		}
	} else {
		UE_LOG(LogDungeons, Warning, TEXT("Unable to get client for authenticating towards Minecraft services API. DungeonsAPIClient module is unavailable."));
	}
}

UWorld* UMinecraftAPI::GetWorld() const
{
	UGameViewportClient* GameVieport = GEngine->GameViewport;
	if (GameVieport) 
	{
		UGameInstance* GameInstance = GameVieport->GetGameInstance();
		if (GameInstance)
			return GameInstance->GetWorld();
	}
	return nullptr;
}

void UMinecraftAPI::Authenticate(const XAuthData& xauthData) {
	auto auth = IDungeonsAPIClient::Get().Auth();	
	auth->Authenticate(xauthData);
}

void UMinecraftAPI::RetryAuthenticate() {
	auto auth = IDungeonsAPIClient::Get().Auth();	
	auth->Retry();
}

void UMinecraftAPI::Logout() {
	auto auth = IDungeonsAPIClient::Get().Auth();	
	auth->Logout();
}


UTrialsProvider* UMinecraftAPI::GetTrialsProvider() {
	if (!TrialsProvider) {
#if PLATFORM_PS4 || defined(EPIC_STORE_BUILD)
		TrialsProvider = NewObject<UAnonymousTrialsProvider>();
#elif PLATFORM_SWITCH
		auto identity = online::getIdentityInterface();
		// online game, player signed in with a Microsoft account
		if (identity.IsValid() && identity->GetLoginStatus(0) == ELoginStatus::LoggedIn)
			TrialsProvider = NewObject<UTrialsProvider>();
		else // local player on Nintendo Switch
			TrialsProvider = NewObject<UAnonymousTrialsProvider>();
#else // Xbox || PC
		TrialsProvider = NewObject<UTrialsProvider>();
#endif
	}
	return TrialsProvider;
}

void UMinecraftAPI::OnSuccessfulAuthentication(const MinecraftAPIAuthData& apiAuthData, const XAuthData& xauthData) {
	GetTrialsProvider()->LoadTrials();
	UE_LOG(LogDungeons, Display, TEXT("Authenticated against Minecraft API as user %s."), *apiAuthData.UserId.GetValue());
}

void UMinecraftAPI::OnFailedAuthentication(const XAuthData& xauthData) {
	UE_LOG(LogDungeons, Warning, TEXT("Failed to authenticate towards Minecraft services API."));
	OnAuthenticationFailed.Broadcast();
}
