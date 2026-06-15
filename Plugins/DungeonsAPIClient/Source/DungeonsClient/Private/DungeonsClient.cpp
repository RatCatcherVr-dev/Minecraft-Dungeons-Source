#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "IDungeonsClient.h"
#include "DungeonsHTTPClient.h"
#include "IDungeonsHTTP.h"

using namespace minecraft::api;

class FDungeonsClient : public IDungeonsClient {
	void Init(const DungeonsClientInfo&, FTimerManager&) override;
	shared_ptr<MinecraftClient> CreateMinecraftClient() override;
	
	shared_ptr<HttpClient> GetHttpClient();
	
	shared_ptr<HttpClient> HttpClient = nullptr;
	shared_ptr<DungeonsHTTPConfig> HttpClientConfig = nullptr;

	TOptional<DungeonsClientInfo> ClientInfo;
	FTimerManager* TimerManager = nullptr;
};

void FDungeonsClient::Init(const DungeonsClientInfo& newClientInfo, FTimerManager& timerManager) {
	ClientInfo = newClientInfo;
	HttpClientConfig = DungeonsHTTPConfig::CreateHTTPConfig();
	TimerManager = &timerManager;
}

shared_ptr<MinecraftClient> FDungeonsClient::CreateMinecraftClient() {
	check(HttpClientConfig && "Client config not set. Make sure to call Init() before attempting to create a Minecraft client.");
	check(ClientInfo && "Client info not set. Make sure to call Init() before attempting to create a Minecraft client");
	
	const auto httpClient = GetHttpClient();
	check(httpClient && "HTTP module is unavailable.");
	
	return make_shared<MinecraftClient>(httpClient, ClientInfo->ToClientInfo(), *TimerManager, HttpClientConfig->GetMinecraftAPIConfig());
}

shared_ptr<HttpClient> FDungeonsClient::GetHttpClient() {
	if (!HttpClient && IDungeonsHTTP::IsAvailable()) {
		HttpClient = IDungeonsHTTP::Get().Client(HttpClientConfig);
	}
	return HttpClient;
}

IMPLEMENT_MODULE(FDungeonsClient, DungeonsClient)

