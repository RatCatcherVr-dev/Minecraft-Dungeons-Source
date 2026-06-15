
#include "httpinternal.h"

namespace http_internal
{
	bool doHttpRequest(const DataModel::BaseRequest& request, httpCallback callback) {
		auto& httpModule = FHttpModule::Get();
		auto httpRequest = httpModule.CreateRequest();
		httpRequest->SetURL(request.GetUrl());
		httpRequest->SetVerb(request.GetVerb());
		for (const auto& pair : request.GetHeaders()) {
			httpRequest->SetHeader(pair.first, pair.second);
		}
		httpRequest->SetContentAsString(request.GetContent());
		httpRequest->OnProcessRequestComplete().BindLambda(callback);
		return httpRequest->ProcessRequest();
	}


	bool doHttpRequestBinaryUpload(FString UploadUrl, httpCallback callback, const TArray<uint8>& Contents)
	{
		auto httpRequest = createBinaryRequest(UploadUrl, callback, "PUT");
		httpRequest->SetContent(Contents);
		return httpRequest->ProcessRequest();
	}



	bool doHttpRequestBinaryDownload(FString DownloadUrl, httpCallback callback)
	{
		return createBinaryRequest(DownloadUrl, callback, "GET")->ProcessRequest();
	}

	bool checkJsonFields(const std::list<std::list<FString>>& fields, const FJsonObject& object) {
		for (auto chain : fields) {
			FJsonObject tmpObj = object;
			for (auto field : chain) {
				if (tmpObj.HasField(field)) {
					tmpObj = *tmpObj.GetObjectField(field);
				}
				else {
					return false;
				}
			}
		}
		return true;
	}

	bool hasNoCloudScriptErrors(const FJsonObject& object) {
		return !object.HasField("data") || !object.GetObjectField("data")->HasField("Error");
	}

}