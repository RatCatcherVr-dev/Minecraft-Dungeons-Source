#pragma once

#include <map>
#include <vector>
#include <SharedPointer.h>
#include <JsonValue.h>

namespace DataModel
{
	class BaseRequest {
	public:
		BaseRequest(const FString& url, const std::map<FString, FString>& header, const FString& content, const FString& verb);
		const FString& GetUrl() const;
		const std::map<FString, FString>& GetHeaders() const;
		const FString& GetContent() const;
		const FString& GetVerb() const;
	protected:
		void SetContent(TSharedRef<FJsonObject> root);
	private:
		FString Url;
		std::map<FString, FString> Headers;
		FString Content;
		FString Verb;
	};

	class LoginWithPSNRequest : public BaseRequest {
	public:
		LoginWithPSNRequest(FString titleId, const FString& authToken, const FString& issuerId);
	};

	class LoginWithXboxRequest : public BaseRequest {
	public:
		LoginWithXboxRequest(FString titleId, const FString& authToken, const FString& issuerId);
	};

	class ExecuteCloudScriptRequest : public BaseRequest {
	public:
		ExecuteCloudScriptRequest(FString titleId, FString functionName, FString SessionTicket, TSharedPtr<FJsonObject>& functionParameter);
	};

	class GetAccountInfoRequest : public BaseRequest {
	public:
		GetAccountInfoRequest(FString titleId, FString SessionTicket);
	};

	class WriteTelemetryEventsRequest : public BaseRequest {
	public:
		WriteTelemetryEventsRequest(FString titleId, FString EntityToken, TArray<TSharedPtr<FJsonValue>> events);
	};

	class UpdateUserPublisherDataRequest : public BaseRequest {
	public:
		UpdateUserPublisherDataRequest(FString titleId, FString SessionTicket, TSharedPtr<FJsonObject>& data);
	};

	class GetUserPublisherDataRequest : public BaseRequest {
	public:
		GetUserPublisherDataRequest(FString titleId, FString SessionTicket, TArray<TSharedPtr<FJsonValue>> keys);
	};

	class CloudGetFilesRequest : public BaseRequest {
	public:
		CloudGetFilesRequest(FString titleId, FString EntityToken , FString PlayerId, const std::vector<FString>&);
	};


	class CloudBaseRequest : public BaseRequest {
	public:
		CloudBaseRequest(FString titleId, FString EntityToken, FString PlayerId, const std::vector<FString>& Files , FString RestOperation );
	};


	class CloudDeleteFilesRequest : public CloudBaseRequest {
	public:
		CloudDeleteFilesRequest(FString titleId, FString EntityToken, FString PlayerId , const std::vector<FString>& FilesToDelete );
	};

	class CloudInitiateFileUploadsRequest : public CloudBaseRequest {
	public:
		CloudInitiateFileUploadsRequest(FString titleId, FString EntityToken, FString PlayerId, const std::vector<FString>& FilesToUpload);
	};

	class CloudFinalizeFileUploadsRequest : public CloudBaseRequest {
	public:
		CloudFinalizeFileUploadsRequest(FString titleId, FString EntityToken, FString PlayerId, const std::vector<FString>& FilesToUpload);
	};

	class CloudAbortFileUploadsRequest : public CloudBaseRequest {
	public:
		CloudAbortFileUploadsRequest(FString titleId, FString EntityToken, FString PlayerId, const std::vector<FString>& FilesToAbort);
	};

	
}
