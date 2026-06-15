#include "Dungeons.h"
#include "HTTPClientInterface.h"
#include <HttpModule.h>
#include "AnalyticsDataTypes.h"
#include "IHttpResponse.h"
#include "util/StringUtil.h"

namespace analytics
{

// using Unreal HTTP interface for telemetry uploads, see e.g. CrashUpload.h
///////////////////// // // //  HTTPClientInterfaceRequestClient // // // /////////////////////////////

const TCHAR* HTTPClientInterfaceRequestClient::ToString(ETelemetryState::Type state)
{
	switch (state)
	{
	case ETelemetryState::Type::NotSet:
		return TEXT("NotSet");
	case ETelemetryState::Type::SendingTelemetry:
		return TEXT("Sending Telemetry");
	case ETelemetryState::Type::Ready:
		return TEXT("Ready");
	case ETelemetryState::Type::Cancelled:
		return TEXT("Cancelled");

	default:
		break;
	}


	return TEXT("Unknown UploadState value");
}

void HTTPClientInterfaceRequestClient::OnProcessRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	UE_LOG(LogTelemetry, Log, TEXT("OnProcessRequestComplete(), State=%s bSucceeded=%i"), ToString(m_state), (int32)bSucceeded);
	switch (m_state)
	{
	default:
		// May get here if response is received after time-out has passed
		break;

	case ETelemetryState::Type::SendingTelemetry:
	{
		bool bCheckedOkay = false;

		if (!bSucceeded || !ParseServerResponse(HttpResponse, bCheckedOkay))
		{
			if (!bSucceeded)
			{
				UE_LOG(LogTelemetry, Warning, TEXT("Request to server failed"));
			}
			else
			{
				UE_LOG(LogTelemetry, Warning, TEXT("Did not get a valid server response."));
			}

			// Failed to check with the server - skip this report for now
			/*AddReportToFailedList();
			CheckPendingReportsForFilesToUpload();*/
		}
		else if (!bCheckedOkay)
		{
			// Server rejected the report
			UE_LOG(LogTelemetry, Warning, TEXT("Did not get a valid server response."));
			//CheckPendingReportsForFilesToUpload();
		}
		else
		{
			SetCurrentState(ETelemetryState::Type::Ready);
		}
	}
	break;

	}

	// handle callback signature
	//if (m_callback)
	//{
	//	m_callback(HttpRequest->GetStatus() == EHttpRequestStatus::Type::Succeeded && HttpRequest->GetResponse()->GetResponseCode() / 100 == 2);
	//}
}

bool HTTPClientInterfaceRequestClient::ParseServerResponse(FHttpResponsePtr Response, bool& OutValidReport)
{
	UE_LOG(LogTelemetry, Log, TEXT("(%d) Response->GetContentAsString(): '%s'"), Response->GetResponseCode(), *Response->GetContentAsString());
	OutValidReport = Response->GetResponseCode() < 300; // do better checks?
	return Response->GetResponseCode() < 300; 
}

void HTTPClientInterfaceRequestClient::SetCurrentState(ETelemetryState::Type state)
{
	m_state = state;
}

void HTTPClientInterfaceRequestClient::SetHeaders(const TMap<FString, FString>& headers, TSharedRef<IHttpRequest> request)
{
	for (auto& header : headers)
	{
		if (m_debug)
			UE_LOG(LogTelemetry, Log, TEXT("Adding header %s : %s"), *header.Key, *header.Value)

		std::string key = stringutil::toStdString(header.Key);
		if (std::find(m_headerKeyNames.begin(), m_headerKeyNames.end(), key) != m_headerKeyNames.end())
			request->AppendToHeader(header.Key, header.Value);
		else
		{
			request->SetHeader(header.Key, header.Value);
			m_headerKeyNames.push_back(key);
		}
	}
}

TSharedRef<IHttpRequest> HTTPClientInterfaceRequestClient::CreateHttpRequest()
{
	auto Request = FHttpModule::Get().CreateRequest();
	
	Request->OnProcessRequestComplete().BindRaw(this, &HTTPClientInterfaceRequestClient::OnProcessRequestComplete);
	SetCurrentState(ETelemetryState::SendingTelemetry);

	return Request;
}

}
