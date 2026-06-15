#pragma once

#include "CoreMinimal.h"
#include "core/HttpClient.h"
#include "HttpModule.h"
#include "DungeonsHTTPConfig.h"

DECLARE_LOG_CATEGORY_EXTERN(LogDungeonsHTTPClient, Log, All);

class DUNGEONSHTTP_API DungeonsHTTPClient : public minecraft::api::HttpClient {

public:
	DungeonsHTTPClient(const std::shared_ptr<DungeonsHTTPConfig>& config);
	
	virtual ~DungeonsHTTPClient() = default;

	void request(const minecraft::api::HttpRequest &httpRequest, const function<void(minecraft::api::HttpResponse)> &response) override;
private:
	TSharedRef<IHttpRequest> AdaptRequest(const minecraft::api::HttpRequest& httpRequest) const;
	static minecraft::api::HttpResponse AdaptResponse(const FHttpResponsePtr& response);
	static minecraft::api::HttpResponse GetFailResponse(const char* exceptionMessage);

	FHttpModule* Http;
};
