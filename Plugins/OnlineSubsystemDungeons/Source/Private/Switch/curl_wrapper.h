#pragma once

#include <httpClient/pal.h>

#define CURL_STRICTER // required by Curl

#include <curl/curl.h>
#include <httpClient/trace.h>
#include <memory>

HC_DECLARE_TRACE_AREA(HC_CURL);

namespace Curl
{

struct MultiDeleter
{
    void operator()(CURLM* multi) noexcept;
};

struct RequestDeleter
{
    void operator()(CURL* request) noexcept;
};

using Multi = std::unique_ptr<CURLM, MultiDeleter>;
using Request = std::unique_ptr<CURL, RequestDeleter>;

HRESULT HrFromCurle(CURLcode c) noexcept;
HRESULT HrFromCurlm(CURLMcode c) noexcept;

HRESULT MethodStringToOpt(char const* method, CURLoption& opt) noexcept;

HRESULT EasyInit(Request& newReequest) noexcept;
template<class T>
HRESULT EasySetOpt(Request const& request, CURLoption option, T v) noexcept;
HRESULT EasyPerform(Request const& request) noexcept;

HRESULT MultiInit(Multi& multi) noexcept;
template<class T>
HRESULT MultiSetOpt(Multi const& multi, CURLMoption option, T v) noexcept;
HRESULT MultiAddHandle(Multi const& multi, Request const& request) noexcept;

//------------------------------------------------------------------------------
// Template implementations
//------------------------------------------------------------------------------

template<class T>
HRESULT EasySetOpt(Request const& request, CURLoption option, T v) noexcept
{
    CURLcode result = curl_easy_setopt(request.get(), option, v);
    if (result != CURLE_OK)
    {
        HC_TRACE_ERROR(HC_CURL, "curl_easy_setopt(request, %d, value) failed with %d", option, result);
    }
    return HrFromCurle(result);
}

template<class T>
HRESULT MultiSetOpt(Multi const& multi, CURLMoption option, T v) noexcept
{
    CURLMcode result = curl_multi_setopt(multi.get(), option, v);
    if (result != CURLM_OK)
    {
        HC_TRACE_ERROR(HC_CURL, "curl_multi_setopt(request, %d, value) failed with %d", option, result);
    }
    return HrFromCurlm(result);
}

}