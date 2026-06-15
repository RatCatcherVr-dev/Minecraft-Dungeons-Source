#include "DataModels.h"
#include <StringUtility.h>
#include "PlayfabServices.h"
#include <Runtime/Json/Public/Json.h>

namespace DataModel 
{

const FString HTTPS = "https://";
const FString CLIENT_ADDRESS = ".playfabapi.com/Client/";
const FString EVENT_ADDRESS = ".playfabapi.com/Event/";
const FString FILE_ADDRESS = ".playfabapi.com/File/";

static const FString CLOUD_ACCOUNT_SOURCE = "title_player_account";


BaseRequest::BaseRequest(const FString& url,
	const std::map<FString, FString>& header,
	const FString& content,
	const FString& verb) : Url(url),
					Headers(header),
					Content(content),
					Verb(verb) {
	
}

const FString& BaseRequest::GetUrl() const {
	return Url;
}

const std::map<FString, FString>& BaseRequest::GetHeaders() const {
	return Headers;
}

const FString& BaseRequest::GetContent() const {
	return Content;
}

const FString& BaseRequest::GetVerb() const {
	return Verb;
}

void BaseRequest::SetContent(TSharedRef<FJsonObject> root) {
	FString content;
	TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&content);
	FJsonSerializer::Serialize(root, Writer);
	Content = content;
}

LoginWithPSNRequest::LoginWithPSNRequest(FString titleId, const FString& authToken, const FString& issuerId)
	: BaseRequest(HTTPS + titleId + CLIENT_ADDRESS + "LoginWithPSN",
		{ { "Content-Type", "application/json" } },
		"",
		"POST") {
	TSharedPtr<FJsonObject> rootObject = MakeShareable(new FJsonObject);

	rootObject->SetStringField("AuthCode", authToken);
	rootObject->SetStringField("RedirectUri", "orbis://games");
	rootObject->SetStringField("IssuerId", issuerId);

	TSharedPtr<FJsonObject> PlayerProfileViewConstraints = MakeShareable(new FJsonObject);
	PlayerProfileViewConstraints->SetBoolField("ShowLinkedAccounts", true);

	TSharedPtr<FJsonObject> InfoRequestParameters = MakeShareable(new FJsonObject);
	InfoRequestParameters->SetBoolField("GetUserAccountInfo", true);
	InfoRequestParameters->SetBoolField("GetPlayerProfile", true);
	InfoRequestParameters->SetBoolField("GetUserReadOnlyData", true);

	rootObject->SetStringField("TitleId", titleId);
	rootObject->SetBoolField("CreateAccount", true);
	rootObject->SetObjectField("InfoRequestParameters", InfoRequestParameters);

	SetContent(rootObject.ToSharedRef());
}

LoginWithXboxRequest::LoginWithXboxRequest(FString titleId, const FString& authToken, const FString& issuerId)
	: BaseRequest(HTTPS + titleId + CLIENT_ADDRESS + "LoginWithXbox",
		{ { "Content-Type", "application/json" } },
		"",
		"POST") {
	TSharedPtr<FJsonObject> rootObject = MakeShareable(new FJsonObject);

	rootObject->SetStringField("XboxToken", authToken);

	TSharedPtr<FJsonObject> PlayerProfileViewConstraints = MakeShareable(new FJsonObject);
	PlayerProfileViewConstraints->SetBoolField("ShowLinkedAccounts", true);

	TSharedPtr<FJsonObject> InfoRequestParameters = MakeShareable(new FJsonObject);
	InfoRequestParameters->SetBoolField("GetUserAccountInfo", true);
	InfoRequestParameters->SetBoolField("GetPlayerProfile", true);
	InfoRequestParameters->SetBoolField("GetUserReadOnlyData", true);

	rootObject->SetStringField("TitleId", titleId);
	rootObject->SetBoolField("CreateAccount", true);
	rootObject->SetObjectField("InfoRequestParameters", InfoRequestParameters);

	SetContent(rootObject.ToSharedRef());
}

ExecuteCloudScriptRequest::ExecuteCloudScriptRequest(FString titleId, FString functionName, FString SessionTicket, TSharedPtr<FJsonObject>& functionParameter)
	: BaseRequest(HTTPS + titleId + CLIENT_ADDRESS + "ExecuteCloudScript",
				{ { "Content-Type", "application/json" },
				  { "X-Authentication", SessionTicket}},
				"",
				"POST") {
	TSharedPtr<FJsonObject> rootObject = MakeShareable(new FJsonObject);
	rootObject->SetStringField("FunctionName", functionName);
	rootObject->SetObjectField("FunctionParameter", functionParameter);
	rootObject->SetBoolField("GeneratePlayStreamEvent", true);

	SetContent(rootObject.ToSharedRef());
}

GetAccountInfoRequest::GetAccountInfoRequest(FString titleId, FString SessionTicket)
	: BaseRequest(HTTPS + titleId + CLIENT_ADDRESS + "GetAccountInfo",
				{ { "Content-Type", "application/json" },
				  { "X-Authentication", SessionTicket}},
				"",
				"POST") {
}

WriteTelemetryEventsRequest::WriteTelemetryEventsRequest(FString titleId, FString EntityToken, TArray<TSharedPtr<FJsonValue>> events)
	: BaseRequest(HTTPS + titleId + EVENT_ADDRESS + "WriteTelemetryEvents",
				{ { "Content-Type", "application/json" },
				  { "X-EntityToken", EntityToken}},
				"",
				"POST") {
	TSharedPtr<FJsonObject>   rootObject = MakeShared<FJsonObject>();
	rootObject->SetArrayField("Events", events);

	SetContent(rootObject.ToSharedRef());
}

UpdateUserPublisherDataRequest::UpdateUserPublisherDataRequest(FString titleId, FString SessionTicket, TSharedPtr<FJsonObject>& data)
	: BaseRequest(HTTPS + titleId + CLIENT_ADDRESS + "UpdateUserPublisherData",
				{ { "Content-Type", "application/json" },
				  { "X-Authentication", SessionTicket}},
				"",
				"POST") {
	TSharedPtr<FJsonObject>   rootObject = MakeShared<FJsonObject>();
	rootObject->SetObjectField("Data", data);

	SetContent(rootObject.ToSharedRef());
}

GetUserPublisherDataRequest::GetUserPublisherDataRequest(FString titleId, FString SessionTicket, TArray<TSharedPtr<FJsonValue>> keys)
	: BaseRequest(HTTPS + titleId + CLIENT_ADDRESS + "GetUserPublisherData",
				{ { "Content-Type", "application/json" },
				  { "X-Authentication", SessionTicket}},
				"",
				"POST") {
	TSharedPtr<FJsonObject>   rootObject = MakeShared<FJsonObject>();
	rootObject->SetArrayField("Keys", keys);

	SetContent(rootObject.ToSharedRef());
}

CloudGetFilesRequest::CloudGetFilesRequest(FString titleId, FString EntityToken, FString PlayerId, const std::vector<FString>&)
	: BaseRequest(HTTPS + titleId + FILE_ADDRESS + "GetFiles",
		{ { "Content-Type", "application/json" },
		  { "X-EntityToken", EntityToken }  },
		"",
		"POST") 
{
	TSharedPtr<FJsonObject>   rootObject = MakeShared<FJsonObject>();
	TSharedPtr<FJsonObject>   entityObject = MakeShared<FJsonObject>();
	entityObject->SetStringField("Id", PlayerId);
	entityObject->SetStringField("Type", CLOUD_ACCOUNT_SOURCE);
	rootObject->SetObjectField("Entity", entityObject);

	SetContent(rootObject.ToSharedRef());
}

CloudBaseRequest::CloudBaseRequest(FString titleId, FString EntityToken, FString PlayerId, const std::vector<FString>& FilesToDelete, FString RestOperation)
	: BaseRequest(HTTPS + titleId + FILE_ADDRESS + RestOperation,
		{ { "Content-Type", "application/json" },
		  { "X-EntityToken", EntityToken } },
		"",
		"POST")
{

	TSharedPtr<FJsonObject>   rootObject = MakeShared<FJsonObject>();
	TSharedPtr<FJsonObject>   entityObject = MakeShared<FJsonObject>();
	entityObject->SetStringField("Id", PlayerId);
	entityObject->SetStringField("Type", CLOUD_ACCOUNT_SOURCE);
	rootObject->SetObjectField("Entity", entityObject);

	TArray<TSharedPtr<FJsonValue>> FileNames;
	for (auto& FileName : FilesToDelete)
	{
		TSharedPtr<FJsonValue> FileNameValue = MakeShareable(new FJsonValueString(FileName));
		FileNames.Add(FileNameValue);
	}
	rootObject->SetArrayField("FileNames", FileNames);
	SetContent(rootObject.ToSharedRef());
}


CloudDeleteFilesRequest::CloudDeleteFilesRequest(FString titleId, FString EntityToken, FString PlayerId, const std::vector<FString>& Files) 
	: CloudBaseRequest(titleId, EntityToken,PlayerId,Files,"DeleteFiles")
{
}


CloudInitiateFileUploadsRequest::CloudInitiateFileUploadsRequest(FString titleId, FString EntityToken, FString PlayerId, const std::vector<FString>& Files)
	: CloudBaseRequest(titleId, EntityToken, PlayerId, Files, "InitiateFileUploads")
{
}


CloudFinalizeFileUploadsRequest::CloudFinalizeFileUploadsRequest(FString titleId, FString EntityToken, FString PlayerId, const std::vector<FString>& Files)
	: CloudBaseRequest(titleId, EntityToken, PlayerId, Files, "FinalizeFileUploads")
{
}

CloudAbortFileUploadsRequest::CloudAbortFileUploadsRequest(FString titleId, FString EntityToken, FString PlayerId, const std::vector<FString>& Files)
	: CloudBaseRequest(titleId, EntityToken, PlayerId, Files, "AbortFileUploads")
{
}
}

