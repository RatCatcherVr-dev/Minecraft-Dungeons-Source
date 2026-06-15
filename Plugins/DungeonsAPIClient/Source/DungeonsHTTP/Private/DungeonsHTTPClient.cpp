#include "CoreMinimal.h"
#include "DungeonsHTTPClient.h"
#include "HttpModule.h"
#include "IHttpResponse.h"
#include "ClientStringUtil.h"
#include "core/HttpClient.h"

DEFINE_LOG_CATEGORY(LogDungeonsHTTPClient)

using namespace minecraft::api;

DungeonsHTTPClient::DungeonsHTTPClient(const std::shared_ptr<DungeonsHTTPConfig>& config) {
	Http = &FHttpModule::Get();

	if (config) {
		Http->SetHttpTimeout(config->GetRequestTimeoutInSeconds());
	}
}

void DungeonsHTTPClient::request(const HttpRequest& httpRequest, const function<void(HttpResponse)>& response) {
	auto unrealHttpRequest = AdaptRequest(httpRequest);

	const auto& onRequestCompleted = [=](FHttpRequestPtr requestPtr, FHttpResponsePtr responsePtr, bool successful) {
		try {
			response(AdaptResponse(responsePtr));
		} catch (std::exception& e) {
			UE_LOG(LogDungeonsHTTPClient, Error, TEXT("HTTP request failed: %s"), *dungeonsapiclient::utils::toFString(e.what()));
			response(GetFailResponse(e.what()));
		}
	};

	unrealHttpRequest->OnProcessRequestComplete().BindLambda(onRequestCompleted);
	unrealHttpRequest->ProcessRequest();
}

TSharedRef<IHttpRequest> DungeonsHTTPClient::AdaptRequest(const HttpRequest& httpRequest) const {
	auto unrealHttpRequest = Http->CreateRequest();

	for (const auto& header : httpRequest.headers) {
		unrealHttpRequest->SetHeader(dungeonsapiclient::utils::toFString(header.key), dungeonsapiclient::utils::toFString(header.value));
	}

	unrealHttpRequest->SetVerb(dungeonsapiclient::utils::toFString(httpRequest.method));
	unrealHttpRequest->SetURL(dungeonsapiclient::utils::toFString(httpRequest.domain + httpRequest.path));

	unrealHttpRequest->SetContentAsString(dungeonsapiclient::utils::toFString(httpRequest.body));

	return unrealHttpRequest;
}

HttpResponse DungeonsHTTPClient::AdaptResponse(const FHttpResponsePtr& response) {
	if (const auto httpResponse = response.Get()) {
		return HttpResponse(httpResponse->GetResponseCode(), dungeonsapiclient::utils::toString(httpResponse->GetContentAsString()));
	}
	return GetFailResponse("Invalid response.");
}

HttpResponse DungeonsHTTPClient::GetFailResponse(const char* exceptionMessage) {
	return HttpResponse(503, exceptionMessage);
}