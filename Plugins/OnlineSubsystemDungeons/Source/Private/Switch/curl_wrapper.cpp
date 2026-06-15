#include "curl_wrapper.h"

#include <curl/curl.h>

namespace Curl
{

void MultiDeleter::operator()(CURLM* multi) noexcept
{
    curl_multi_cleanup(multi);
}

void RequestDeleter::operator()(CURL* request) noexcept
{
    curl_easy_cleanup(request);
}

HRESULT HrFromCurle(CURLcode c) noexcept
{
    if (c == CURLE_OK)
    {
        return S_OK;
    }
    else
    {
        return E_FAIL;
    }
}

HRESULT HrFromCurlm(CURLMcode c) noexcept
{
    if (c == CURLM_OK)
    {
        return S_OK;
    }
    else
    {
        return E_FAIL;
    }
}

HRESULT MethodStringToOpt(char const* method, CURLoption& opt) noexcept
{
    if (strcmp(method, "GET") == 0)
    {
        opt = CURLOPT_HTTPGET;
    }
    else if (strcmp(method, "POST") == 0)
    {
        opt = CURLOPT_POST;
    }
    else if (strcmp(method, "PUT") == 0)
    {
        opt = CURLOPT_UPLOAD;
    }
    else
    {
        assert(false);
        return E_INVALIDARG;
    }

    return S_OK;
}

HRESULT EasyInit(Request& newRequest) noexcept
{
    newRequest.reset(curl_easy_init());
    if (!newRequest)
    {
        HC_TRACE_ERROR(HC_CURL, "curl_easy_init failed");
        return E_FAIL;
    }
    return S_OK;
}

HRESULT EasyPerform(Request const& request) noexcept
{
    CURLcode result = curl_easy_perform(request.get());
    if (result != CURLE_OK)
    {
        HC_TRACE_ERROR(HC_CURL, "curl_easy_perform failed with %d", result);
    }
    return HrFromCurle(result);
}

HRESULT MultiInit(Multi& multi) noexcept
{
	//-- D11.AH - No need to init as UE4 does this
    /*
	CURLcode initRes = curl_global_init(CURL_GLOBAL_ALL);
    if (initRes != CURLE_OK)
    {
        HC_TRACE_ERROR(HC_CURL, "curl_global_init failed with %d", initRes);
        return HrFromCurle(initRes);
    }
	*/
    Multi m{ curl_multi_init() };
    if (!m)
    {
        HC_TRACE_ERROR(HC_CURL, "curl_multi_init failed");
        return E_FAIL;
    }

    HRESULT hr = MultiSetOpt(m, CURLMOPT_MAXCONNECTS, 4);
    if (FAILED(hr)) { return hr; }

	int32 MaxTotalConnections = 0;
	if (GConfig->GetInt(TEXT("XSAPI.Curl"), TEXT("MaxTotalConnections"), MaxTotalConnections, GEngineIni) && MaxTotalConnections > 0)
	{
		hr = MultiSetOpt(m, CURLMOPT_MAX_TOTAL_CONNECTIONS, static_cast<long>(MaxTotalConnections));
		if (FAILED(hr))
		{
			HC_TRACE_ERROR(HC_CURL, "Failed to set libcurl max total connections options (%d), error %d", MaxTotalConnections, static_cast<int32>(hr));
			return hr;
		}
	}


    multi = std::move(m);
    return S_OK;
}

HRESULT MultiAddHandle(Multi const& multi, Request const& request) noexcept
{
    CURLMcode result = curl_multi_add_handle(multi.get(), request.get());
    if (result != CURLM_OK)
    {
        HC_TRACE_ERROR(HC_CURL, "curl_multi_add_handle failed with %d", result);
    }
    return HrFromCurlm(result);
}

}
