#pragma once

//#include <include/cef_values.h>
//#include <include/cef_request.h>

//#include "AnalyticsDataTypes.h"
//#include <atomic>
#include <IHttpRequest.h>

#include <string>
#include <vector>

namespace analytics
{

/** State enum to keep track loader callbacks*/
struct ETelemetryState
{
	enum Type
	{
		NotSet,

		SendingTelemetry,
		Ready,
		Cancelled,
	};
};

class HTTPClientInterfaceRequestClient
{
public:
	explicit HTTPClientInterfaceRequestClient()
		: m_state(ETelemetryState::Type::NotSet)
		, m_callback(nullptr)
		
	{
	}

	void SetHeaders(const TMap<FString, FString>& headers, TSharedRef<IHttpRequest> request);

	//bool GetAuthCredentials(bool isProxy, const CefString& host, int port, const CefString& realm, const CefString& scheme, CefRefPtr<CefAuthCallback> callback) override
	//{
	//	return false;
	//}

	TSharedRef<IHttpRequest> CreateHttpRequest();

protected:
	static const TCHAR* ToString(ETelemetryState::Type state);


private:
	std::function<void(bool)> m_callback;

	//std::vector<std::hash<std::string>> m_headerKeys;
	std::vector<std::string> m_headerKeyNames;
	bool m_debug = true;

	/**
	* Callback from HTTP library when a request has completed
	* @param HttpRequest The request object
	* @param HttpResponse The response from the server
	* @param bSucceeded Whether a response was successfully received
	*/
	void OnProcessRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	bool ParseServerResponse(FHttpResponsePtr Response, bool& OutValidReport);

	void SetCurrentState(ETelemetryState::Type state);

	ETelemetryState::Type m_state;

	//IMPLEMENT_REFCOUNTING(HTTPClientInterfaceRequestClient);
};

} // namespace analytics
