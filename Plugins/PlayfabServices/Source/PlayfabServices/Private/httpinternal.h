#pragma once

#include "PlayfabClient.h"
#include <list>
#include <Runtime/Json/Public/Json.h>
#include <functional>


namespace http_internal
{

	typedef std::function<void(FHttpRequestPtr, FHttpResponsePtr, bool)> httpCallback;


	bool doHttpRequest(const DataModel::BaseRequest& request, httpCallback callback);
	bool doHttpRequestBinaryUpload(FString UploadUrl, httpCallback callback, const TArray<uint8>& Contents);
	bool doHttpRequestBinaryDownload(FString DownloadUrl, httpCallback callback);
	bool checkJsonFields(const std::list<std::list<FString>>& fields, const FJsonObject& object);
	bool hasNoCloudScriptErrors(const FJsonObject& object);


	inline auto createBinaryRequest(FString UploadUrl, httpCallback callback, const char* pVerb)
	{
		auto& httpModule = FHttpModule::Get();
		auto httpRequest = httpModule.CreateRequest();
		httpRequest->SetURL(UploadUrl);
		httpRequest->SetVerb(pVerb);
		httpRequest->SetHeader("Content-Type", "application/octet-stream");
		httpRequest->OnProcessRequestComplete().BindLambda(callback);
		return httpRequest;
	}


	inline auto createResponseLambda(const std::list<std::list<FString>>& expectedJsonData, std::function<void(FJsonObject&)> successfulCallback, std::function<void(FString)> failedCallback) {
		return [successfulCallback, failedCallback, expectedJsonData](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
			if (!Response || !Response.IsValid()) {
				failedCallback("");
				return;
			}
			auto content = Response->GetContentAsString();
			UE_LOG(LogPlayfabServices, Warning, TEXT("Response from playfab:\n%s"), *content);

			if (bWasSuccessful && EHttpResponseCodes::IsOk(Response->GetResponseCode())) {
				TSharedPtr<FJsonObject> object;
				if (!content.IsEmpty()) {
					TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(content);
					bool jsonParseSuccessful = FJsonSerializer::Deserialize(reader, object);
					if (jsonParseSuccessful && object.IsValid() && checkJsonFields(expectedJsonData, *object) && hasNoCloudScriptErrors(*object)) {
						successfulCallback(*object);
						return;
					}
				}
				else {
					successfulCallback(*object);
					return;
				}
			}

			failedCallback(content);
		};
	}

	inline  auto createResponseLambda(const std::list<std::list<FString>>& expectedJsonData, std::function<void(void)> successfulCallback, std::function<void(FString)> failedCallback) {
		return [successfulCallback, failedCallback, expectedJsonData](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
			if (!Response || !Response.IsValid()) {
				failedCallback("");
				return;
			}
			auto content = Response->GetContentAsString();
			UE_LOG(LogPlayfabServices, Warning, TEXT("Response from playfab:\n%s"), *content);

			if (bWasSuccessful && EHttpResponseCodes::IsOk(Response->GetResponseCode()))
				successfulCallback();
			else
				failedCallback(content);
		};
	}


	inline auto createResponseLambda(const std::list<std::list<FString>>& expectedJsonData, std::function<void(TArray<uint8>)> successfulCallback, std::function<void(FString,EHttpResponseCodes::Type)> failedCallback) {
		return [successfulCallback, failedCallback, expectedJsonData](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
			if (!Response || !Response.IsValid()) {
				failedCallback("", EHttpResponseCodes::Unknown);
				return;
			}
			auto content = Response->GetContentAsString();
			UE_LOG(LogPlayfabServices, Warning, TEXT("Response from playfab:\n%s"), *content);

			auto Code = Response->GetResponseCode();
			if (bWasSuccessful && EHttpResponseCodes::IsOk(Response->GetResponseCode()))
			{
				TArray<uint8> r = Response->GetContent();
				successfulCallback(r);
			}
			else
				failedCallback(content, EHttpResponseCodes::Type(Code));
		};
	}


}
