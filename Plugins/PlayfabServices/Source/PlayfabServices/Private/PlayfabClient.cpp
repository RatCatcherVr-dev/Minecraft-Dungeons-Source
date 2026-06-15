#include "PlayfabClient.h"
#include <PlatformProcess.h>
#include <StringUtility.h>
#include "PlayfabServices.h"
#include <Runtime/Core/Public/Misc/DateTime.h>
#include <Async.h>
#include <Runtime/Json/Public/Json.h>
#include <functional>
#include "DataModels.h"
#include <list>
#include "httpinternal.h"

namespace internal
{
	
TSharedPtr<FJsonObject> MakeProperties(const PropertyList& properties) {
	TSharedPtr<FJsonObject> object = MakeShareable(new FJsonObject);
	for (auto prop : properties) {
		switch (prop.Value.getValue().getType()) {
		case PlayfabServicesWildCardValue::ValueType::Boolean:
			object->SetBoolField(prop.Key, prop.Value.getValue().asBoolean());
			break;
		case PlayfabServicesWildCardValue::ValueType::Float:
			object->SetNumberField(prop.Key, prop.Value.getValue().asFloat());
			break;
		case PlayfabServicesWildCardValue::ValueType::SignedInt32:
			object->SetNumberField(prop.Key, prop.Value.getValue().asInt32());
			break;
		case PlayfabServicesWildCardValue::ValueType::UnsignedInt32:
			object->SetNumberField(prop.Key, prop.Value.getValue().asUInt32());
			break;
		case PlayfabServicesWildCardValue::ValueType::String:
			object->SetStringField(prop.Key, prop.Value.getValue().asString());
		case PlayfabServicesWildCardValue::ValueType::NotSet:
		default:
			break;
		}
	}
	return object;
}

TSharedPtr<FJsonObject> MakeMeasurements(const MeasurementList& measurements) {
	TSharedPtr<FJsonObject> object = MakeShareable(new FJsonObject);
	for (auto measure : measurements) {
		switch (measure.Value.getValue().getType()) {
		case PlayfabServicesWildCardValue::ValueType::Boolean:
			object->SetBoolField(measure.Key, measure.Value.getValue().asBoolean());
			break;
		case PlayfabServicesWildCardValue::ValueType::Float:
			object->SetNumberField(measure.Key, measure.Value.getValue().asFloat());
			break;
		case PlayfabServicesWildCardValue::ValueType::SignedInt32:
			object->SetNumberField(measure.Key, measure.Value.getValue().asInt32());
			break;
		case PlayfabServicesWildCardValue::ValueType::UnsignedInt32:
			object->SetNumberField(measure.Key, measure.Value.getValue().asUInt32());
			break;
		case PlayfabServicesWildCardValue::ValueType::String:
			object->SetStringField(measure.Key, measure.Value.getValue().asString());
		case PlayfabServicesWildCardValue::ValueType::NotSet:
		default:
			break;
		}
	}
	return object;
}

TSharedPtr<FJsonObject> MakePayload(const AnalyticsEvent& event) {
	TSharedPtr<FJsonObject> object = MakeShareable(new FJsonObject);
	if (event.getShouldAggregate()) {
		object->SetNumberField("AggregationCompletionTime", std::chrono::system_clock::to_time_t(event.getAggregationCompleteTime()));
	}

	object->SetStringField("TimestampUTC", FDateTime::UtcNow().ToIso8601());
	object->SetObjectField("Properties", MakeProperties(event.getProperties()));
	object->SetObjectField("Measurements", MakeMeasurements(event.getMeasurements()));
	
	return object;
}
	
constexpr auto AUTHENTICATION_ATTEMPTS = 5;

bool IsUnableToAuthenticate(const PlayFabPlayer* player) {
	return (player->GetPlayerId().IsEmpty() || player->FailedAuthenticationAttempts >= AUTHENTICATION_ATTEMPTS);
}

}

SendAnalyticEventResult PlayFabClient::SendAnalyticsEvent(PlayFabPlayer* player, AnalyticsEvent& event) {
	SendAnalyticEventResult result = AuthenticateIfNeeded(player);
	if (result != SendAnalyticEventResult::Success) {
		return result;
	}

	TSharedPtr<FJsonObject> eventsItem0Object = MakeShareable(new FJsonObject);
	TSharedPtr<FJsonObject> entityObject = MakeShareable(new FJsonObject);

	entityObject->SetStringField("Id", player->GetPlayFabId());
	entityObject->SetStringField("Type", "title_player_account");
	entityObject->SetStringField("TypeString", "title_player_account");
	eventsItem0Object->SetStringField("EventNamespace", "custom");
	eventsItem0Object->SetStringField("Name", event.getName());
	eventsItem0Object->SetObjectField("Entity", entityObject);
	eventsItem0Object->SetObjectField("Payload", internal::MakePayload(event));

	{
		std::lock_guard<std::mutex> m(mEventPayloadsMutex);
		mEventPayloads.Add(MakeShared<FJsonValueObject>(eventsItem0Object));
	}
	return result;
}

void PlayFabClient::Tick() {
	if (mLastPayloadSend + std::chrono::minutes(1) < std::chrono::steady_clock::now()) {
		if (mPlayfabEntityToken.Len() > 0 && mEventPayloads.Num() > 0) {
			Async<void>(EAsyncExecution::ThreadPool, [this] { SendEventPayloads(); });
			mLastPayloadSend = std::chrono::steady_clock::now();
		}
	}
}

void PlayFabClient::OnShutdown() {
	if (mPlayfabEntityToken.Len() > 0 && mEventPayloads.Num() > 0) {
		SendEventPayloads();
	}
}

SendAnalyticEventResult PlayFabClient::GetAuthenticationStatus(PlayFabPlayer* player) {
	if (internal::IsUnableToAuthenticate(player)) {
		return SendAnalyticEventResult::UnableToAuthenticate;
	}

	if (mTokenExpiredResponse) {
		// A recent attempt to send telemetry failed due to an expired token. 
		player->ClearCredentials();
		player->AuthenticationInProgress = false;
		player->FailedAuthenticationAttempts = 0;
		mTokenExpiredResponse = false;
		return SendAnalyticEventResult::AuthenticationNeeded;
	}

	return player->HasCredentials() ? SendAnalyticEventResult::Success : SendAnalyticEventResult::AuthenticationInProcess;
}

SendAnalyticEventResult PlayFabClient::AuthenticateIfNeeded(PlayFabPlayer* player) {
	mPlayfabEntityToken = player->GetEntityToken();

	std::lock_guard<std::mutex> m(mAuthPlayerMutex);
	SendAnalyticEventResult result = GetAuthenticationStatus(player);
	switch (result) {
		case SendAnalyticEventResult::AuthenticationNeeded: {
			FString issuerId;
			FString authToken = player->GetAuthToken(issuerId);
			if (!authToken.IsEmpty()) {
				player->AuthenticationInProgress = true;
				AuthenticatePlayer(player, { authToken, issuerId });
			}
			result = SendAnalyticEventResult::AuthenticationInProcess;
		}
		case SendAnalyticEventResult::AuthenticationInProcess:
		case SendAnalyticEventResult::Success:
		default:
			break;
	}
	return result;
}

void PlayFabClient::SendEventPayloads() {
	TArray<TSharedPtr<FJsonValue>> events;
	{
		std::lock_guard<std::mutex> m(mEventPayloadsMutex);
		events.Reserve(mEventPayloads.Num());
		for (auto& jsonValue : mEventPayloads) {
			events.Add(std::move(jsonValue));
		}
		mEventPayloads.Reset();
	}

	auto failedCallback = [this](FString content) {
		if (content.Contains("EntityTokenExpired"))
			mTokenExpiredResponse = true;
	};

	auto successCallback = [](FJsonObject& object) {};

	auto response = http_internal::createResponseLambda({ {} }, successCallback, failedCallback);
	http_internal::doHttpRequest(DataModel::WriteTelemetryEventsRequest(mTitleId, mPlayfabEntityToken, events), response);
}

bool PlayFabClient::AuthenticatePlayer(PlayFabPlayer* player, AuthenticationConfig config) {
	auto failedCallback = [player](FString content) {
		player->FailedAuthenticationAttempts++;
		player->AuthenticationInProgress = false;
		player->ForAllCallbacks(false);
	};

	auto successCallback = [player](FJsonObject& object) {
		FString entityToken = object.GetObjectField("data")->GetObjectField("EntityToken")->GetStringField("EntityToken");
		FString sessionTicket = object.GetObjectField("data")->GetStringField("SessionTicket");
		FString playfabId = object.GetObjectField("data")->GetObjectField("EntityToken")->GetObjectField("Entity")->GetStringField("Id");
		FString linkedXboxAccount = "";
		if (http_internal::checkJsonFields({{"data", "InfoResultPayload", "AccountInfo", "XboxInfo", "XboxUserId"}}, object)) {
			linkedXboxAccount = object.GetObjectField("data")->GetObjectField("InfoResultPayload")->GetObjectField("AccountInfo")->GetObjectField("XboxInfo")->GetStringField("XboxUserId");
		}
		player->SetCredentials(playfabId, sessionTicket, entityToken, linkedXboxAccount);
		player->AuthenticationInProgress = false;
		player->ForAllCallbacks(true);
	};
	auto response = http_internal::createResponseLambda({{"data", "EntityToken", "EntityToken"}, 
														{"data", "SessionTicket"},
														{"data", "EntityToken", "Entity", "Id"}}, 
														successCallback, 
														failedCallback);
#if PLATFORM_PS4
	auto model = DataModel::LoginWithPSNRequest(mTitleId, config.authToken, config.issuerId);
#else
	auto model = DataModel::LoginWithXboxRequest(mTitleId, config.authToken, config.issuerId);
#endif

	if (!http_internal::doHttpRequest(model, response)) {
		player->FailedAuthenticationAttempts++;
		player->AuthenticationInProgress = false;
		UE_LOG(LogPlayfabServices, Error, TEXT("Authenticating player with id %s has failed. Unable To Process HTTP Request."), *(player->GetPlayerId()));
		return false;
	}

	return true;
}

bool PlayFabClient::LinkAccounts(PlayFabPlayer* player, AccountLinkConfig config) {
	if (!config.issuerId.IsNumeric()) {
		UE_LOG(LogPlayfabServices, Error, TEXT("UnlinkPsnAccountFromXBL the issuer id is not numeric: %s"), *config.issuerId);
		return false;
	}

	TSharedPtr<FJsonObject> functionParameter = MakeShareable(new FJsonObject);
	functionParameter->SetStringField("psnAccountToken", config.psnAuthToken);
	functionParameter->SetNumberField("issuerId", FCString::Atoi(*config.issuerId));
	functionParameter->SetStringField("xblAccountToken", config.xblAuthToken);
	functionParameter->SetStringField("expectedXuid", config.xblAccount);
	functionParameter->SetBoolField("restrictRelinking", true);

	auto failedCallback = [callback = config.callback, player] (FString content) {
		/* We can end up here because the cloud script is not returning the result code at all !
		Let's dig through the logs for errors. */
		/* this XBL account is already linked to a different PSN account - this should not happen, fix the backend cloud script */
		ensure(!content.Contains("AccountAlreadyLinked")); // error code 1011
		callback(AccountLinkStatus::Error);
	};

	auto successCallback = [player, callback = config.callback, xblAccount = config.xblAccount](FJsonObject& object) {
		AccountLinkingResult result = static_cast<AccountLinkingResult>(object.GetObjectField("data")->GetObjectField("FunctionResult")->GetIntegerField("result"));
		switch (result) {
		case AccountLinkingResult::Success:
			player->ClearCredentials();
			player->SetLinkedXblAccount(xblAccount);
			callback(AccountLinkStatus::Linked);
			break;
		case AccountLinkingResult::PsnAlreadyLinked:
			callback(AccountLinkStatus::ThisPsnAlreadyLinkedToDifferentMSA);
			break;
		case AccountLinkingResult::MsaAlreadyLinked:
			callback(AccountLinkStatus::ThisMsaAlreadyLinkedToDifferentPSN);
			break;
		default:
			callback(AccountLinkStatus::Error);
			break;
		}
	};

	auto response = http_internal::createResponseLambda({{"data", "FunctionResult", "result"}}, successCallback, failedCallback);
	
	if (!http_internal::doHttpRequest(DataModel::ExecuteCloudScriptRequest(mTitleId, 
						"mergePSNAccountIntoXBLAccount", 
						player->GetSessionTicket(), 
						functionParameter), 
						response)) {
		config.callback(AccountLinkStatus::Error);
		return false;
	}

	return true;
}

bool PlayFabClient::GetLinkStatus(PlayFabPlayer* p, TFunction<void(bool success)> callback) {

	auto failedCallback = std::bind(callback, false);

	auto successCallback = [p, callback](FJsonObject& object) {
		FString linkedXboxAccount = object.GetObjectField("data")->GetObjectField("AccountInfo")->GetObjectField("XboxInfo")->GetStringField("XboxUserId");
		p->SetCredentials(p->GetPlayFabId(), p->GetSessionTicket(), p->GetEntityToken(), linkedXboxAccount);
		callback(true);
	};

	auto responseCallback = http_internal::createResponseLambda({{"data", "AccountInfo", "XboxInfo", "XboxUserId"}}, successCallback, failedCallback);

	if (!http_internal::doHttpRequest(DataModel::GetAccountInfoRequest(
						mTitleId,
						p->GetSessionTicket()),
					responseCallback)) {
		callback(false);
		return false;
	}

	return true;
}

bool PlayFabClient::UnlinkPsnAccountFromXBL(PlayFabPlayer* playfabAccount, AccountUnlinkConfig config) {
	if (!config.issuerId.IsNumeric()) {
		UE_LOG(LogPlayfabServices, Error, TEXT("UnlinkPsnAccountFromXBL the issuer id is not numeric: %s"), *config.issuerId);
		return false;
	}

	auto failedCallback = [callback = config.callback, playfabAccount](FString content) {
		if (content.Contains("InvalidPSNAuthCode")) {
			UE_LOG(LogPlayfabServices, Error, TEXT("Failed to unlink XBL account because the PSN auth token is Invalid"));
		} else if (content.Contains("Timeout")) {
			UE_LOG(LogPlayfabServices, Error, TEXT("Failed to unlink XBL account because the cloud script timed out"));
		}

		playfabAccount->ClearCredentials();
		playfabAccount->ClearLinkedXblAccount();
		callback(false); 
	};

	auto successCallback = [this, playfabAccount, callback = config.callback](FJsonObject& object) {
		/* The way unlinking is implemented on the backend, this Playfab account stays linked with XBL, 
		and is no longer linked with PSN. We cannot use it anymore and need to request a new Playfab account for this PSN.*/
		playfabAccount->ClearCredentials();
		playfabAccount->ClearLinkedXblAccount();
		callback(true);
	};

	TSharedPtr<FJsonObject> requestBody = MakeShareable(new FJsonObject);
	requestBody->SetStringField("psnAccountToken", config.psnAuthToken);
	requestBody->SetNumberField("issuerId", FCString::Atoi(*config.issuerId));
	requestBody->SetStringField("xuid", config.xblAccount);
	requestBody->SetBoolField("restrictRelinking", true);

	auto responseCallback = http_internal::createResponseLambda({}, successCallback, failedCallback);

	if (!http_internal::doHttpRequest(DataModel::ExecuteCloudScriptRequest(mTitleId,
		"splitOffPSNAccount",
		playfabAccount->GetSessionTicket(),
		requestBody),
		responseCallback)) {
		config.callback(false);
		return false;
	}
	return true;
}


bool PlayFabClient::SetGamertagHint(PlayFabPlayer* p,
									FString xblGamerTag) {

	TSharedPtr<FJsonObject> data = MakeShareable(new FJsonObject);
	data->SetStringField("GamertagHint", xblGamerTag);

	auto failedCallback = [](FString) {};
	auto successCallback = [](FJsonObject& object) {};
	auto responseCallback = http_internal::createResponseLambda({{}}, successCallback, failedCallback);

	if (!http_internal::doHttpRequest(DataModel::UpdateUserPublisherDataRequest(
						mTitleId,
						p->GetSessionTicket(),
						data),
					responseCallback)) {
		return false;
	}

	return true;
}

bool PlayFabClient::GetGamertagHint(PlayFabPlayer* p,
									TFunction<void(FString gamerTag)> callback) {

	TArray<TSharedPtr<FJsonValue>> data;
	data.Add(MakeShareable(new FJsonValueString("GamertagHint")));

	auto failedCallback = [callback](FString) {
		callback("");
	};
	auto successCallback = [callback](FJsonObject& object) {
		FString gamerTagHint = object.GetObjectField("data")->GetObjectField("Data")->GetObjectField("GamertagHint")->GetStringField("Value");
		callback(gamerTagHint);
	};
	auto responseCallback = http_internal::createResponseLambda({{"data", "Data", "GamertagHint", "Value"}}, successCallback, failedCallback);

	if (!http_internal::doHttpRequest(DataModel::GetUserPublisherDataRequest(
						mTitleId,
						p->GetSessionTicket(),
						data),
					responseCallback)) {
		callback("");
		return false;
	}

	return true;
}

FString PlayFabClient::GetTitleId() const {
	return mTitleId;
}

void PlayFabClient::PlayfabCleanup(PlayFabPlayer* p) {
	p->ClearCredentials();
	p->AuthenticationInProgress = false;
	p->FailedAuthenticationAttempts = 0;
	mTokenExpiredResponse = false;
}

