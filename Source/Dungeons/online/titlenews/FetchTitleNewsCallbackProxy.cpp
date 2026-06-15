#include "Dungeons.h"
#include "FetchTitleNewsCallbackProxy.h"
#include "util/EnumUtil.h"
#include "HttpModule.h"
#include "util/StringUtil.h"
#include "IHttpResponse.h"
#include "AsyncTaskDownloadImage.h"
#include "GenericPlatformHttp.h"
#include "locale/LocalizationUtils.h"
#if PLATFORM_SWITCH
#include "Switch/SwitchPlatformMisc.h"
#endif
#include "util/Algo.h"
#include "DungeonsGameState.h"
#include "util/DateTimeUtil.h"

std::string GetPlatformString() {
#if PLATFORM_WINDOWS
#if defined(GDK_API_ENABLED) && GDK_API_ENABLED == 1
	return "Windows_Win10";
#else
	return "Windows_Launcher";
#endif
#elif PLATFORM_XBOXONE
	return "XboxOne";
#elif PLATFORM_SWITCH
	return "Switch";
#elif PLATFORM_PS4
	return "PS4";
#endif
	// default news is through launcher
	return "Windows_Launcher";
}

std::string GetLocale() {
	DungeonsLocale locale = ULocalizationUtils::CurrentLocale();
	FString localeStr = ULocalizationUtils::GetLocaleString(locale);

	if (localeStr != FString("en")) {
		return stringutil::toStdString(localeStr);
	}
	return "en-US";
}

enum class ENewsFeed : uint8 {
	Menu,
	Map
};


std::string MakeURL(ENewsFeed feed) {
	switch (feed)
	{
	case ENewsFeed::Menu:
#if USE_TITLENEWS_TESTING_ENVIRONMENT
		return std::string("https://launchercontent.mojang.com/testing/dungeonsNewsInGame/") + GetPlatformString() + std::string("/") + GetLocale() + std::string(".json");
#else
		return std::string("https://launchercontent.mojang.com/dungeonsNewsInGame/") + GetPlatformString() + std::string("/") + GetLocale() + std::string(".json");
#endif
	case ENewsFeed::Map:
#if USE_TITLENEWS_TESTING_ENVIRONMENT
		return std::string("https://launchercontent.mojang.com/testing/dungeonsMapPopupInGame/") + GetPlatformString() + std::string("/") + GetLocale() + std::string(".json");
#else
		return std::string("https://launchercontent.mojang.com/dungeonsMapPopupInGame/") + GetPlatformString() + std::string("/") + GetLocale() + std::string(".json");
#endif
	}

	return "";
}


void UFetchTileNewsCallProxyBase::Activate() {
	AddToRoot();
	UE_LOG(LogTemp, Log, TEXT("UFetchTileNewsCallProxyBase::Activate(): fetching %s"), *URL);
	if (auto* object = mWeakWorldContextObject.Get()) {
		TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
		HttpRequest->SetVerb("GET");
		HttpRequest->SetURL(URL);
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UFetchTileNewsCallProxyBase::InternalOnHttpRequestCompleted);
		HttpRequest->ProcessRequest();
	}
	else {
		InternalFail(EFetchTitleNewsFailReason::MissingWorld);
	}
}


bool UFetchTileNewsCallProxyBase::IsDownloadingImages() const
{
	return !(ImageFetchers.Num() == 0 || algo::all_of(ImageFetchers, [](const UTitleImageFetcher* fetcher) {return fetcher->IsFinished(); }));
}

void UFetchTileNewsCallProxyBase::DownloadImageForURL(const FString& imageURL) {
	FName nameURL(*imageURL);
	if (auto fetcher = ImageFetchers.FindByPredicate([&](const UTitleImageFetcher* f) { return f->GetURL() == nameURL; })) {
		UTitleImageFetcher* fetcherActual = *fetcher;
		if (fetcherActual->IsFinished()) {
			InternalOnImageFetched(nameURL, fetcherActual->GetTexture());
		}
	}
	else {
		ImageFetchers.Add(NewObject<UTitleImageFetcher>());
		ImageFetchers.Last()->GetImage(imageURL, FOnDownloadImageFinished::FDelegate::CreateUObject(this, &UFetchTileNewsCallProxyBase::InternalOnImageFetched));
	}
}

void UFetchTileNewsCallProxyBase::InternalOnImageFetched(FName url, UTexture2DDynamic * texutre) {
	OnImageFetched(url, texutre);
	CheckIsDone();
}

void UFetchTileNewsCallProxyBase::InternalOnHttpRequestCompleted(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!bWasSuccessful) {
		InternalFail(EFetchTitleNewsFailReason::CouldntReachNews);
	}
	else {
		OnHttpRequestSucceeded(Request, Response);
		CheckIsDone();
	}
}

void UFetchTileNewsCallProxyBase::CheckIsDone() {
	if (!Pending && !IsDownloadingImages()) {
		InternalOnAllRequestsCompleted();
	}
}

void UFetchTileNewsCallProxyBase::InternalFail(EFetchTitleNewsFailReason reason) {
	FetchTitleNewsFailed.Broadcast(reason);
	if (IsRooted()) RemoveFromRoot();
}

void UFetchTileNewsCallProxyBase::InternalOnAllRequestsCompleted() {
	OnAllRequestsCompleted();
	if (IsRooted()) RemoveFromRoot();
}

TOptional<FString> UFetchTileNewsCallProxyBase::MakeImageUrl(const Json::Value & imageNode)
{
	if (imageNode.isMember("url") && imageNode.isMember("url")) {
		FString imageUrl = stringutil::toFString(imageNode["url"].asString(""));

		// Strip first encoded slash
		if (imageUrl.Len() > 1) {
			if (imageUrl[0] == '/') {
				imageUrl = imageUrl.Mid(1, imageUrl.Len());
			}
		}

		return FString("https://launchercontent.mojang.com/") + imageUrl;
	}

	return {};
}


UFetchTitleNewsCallbackProxy::UFetchTitleNewsCallbackProxy(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
}

UFetchTitleNewsCallbackProxy* UFetchTitleNewsCallbackProxy::FetchTitleNews(UObject* WorldContextObject) {
	UFetchTitleNewsCallbackProxy* node = NewObject<UFetchTitleNewsCallbackProxy>();	
	node->mWeakWorldContextObject = WorldContextObject;
	node->URL = stringutil::toFString(MakeURL(ENewsFeed::Menu));
	return node;
}

void UFetchTitleNewsCallbackProxy::OnHttpRequestSucceeded(FHttpRequestPtr Request, FHttpResponsePtr Response) {
	Json::Value node;
	Json::Reader reader;
		
	std::string responseStr = stringutil::toStdString(Response->GetContentAsString());
	if (reader.parse(responseStr, node, false)) {

		if (!node.isMember("version")) {
			InternalFail(EFetchTitleNewsFailReason::MalformedJson);
			return;
		}

		// news version 1:
		if (node["version"].asInt(0) == 1) {
			
			if (!node.isMember("entries") || !node["entries"].isArray()) {
				// if this happens, the json we fetched has not yet been updated to have the correct types.
				InternalFail(EFetchTitleNewsFailReason::MalformedJson);
				return;
			}

			// entries array:
			for (const auto& item : node["entries"]) {
				FTitleNews news;
				// ID
				news.Id = stringutil::toFString(item["id"].asString(""));

				// title					
				news.Title = FText::FromString(stringutil::utf8ToFString(item["title"].asString("")));

				// Body	
				news.Body = FText::FromString(stringutil::utf8ToFString(item["body"].asString("")));

				//Category
				news.Category = FText::FromString(stringutil::utf8ToFString(item["category"].asString("")));
					
				// Image (Load last, as it calls success/fail once its loaded
				if (item.isMember("image")) {
					news.ImageURL = MakeImageUrl(item["image"]);
				}

				mFetchedNews.Add(news);
			}
		}

		//Download any images requested
		for (const auto& news : mFetchedNews) {
			if (news.ImageURL.IsSet()) {
				DownloadImageForURL(news.ImageURL.GetValue());
			}
		}
	}
	else {
		InternalFail(EFetchTitleNewsFailReason::MalformedJson);
	}
}

void UFetchTitleNewsCallbackProxy::OnAllRequestsCompleted() {
	FetchTitleNewsSucceeded.Broadcast(mFetchedNews);
}

void UFetchTitleNewsCallbackProxy::OnImageFetched(FName url, UTexture2DDynamic* image) {
	if (image) {
		for (auto& newsItem : mFetchedNews) {
			if (url == FName(*newsItem.ImageURL.Get(""))) {
				newsItem.HeaderImage = image;
			}
		}
	}
}


UFetchTitleNewsMapPopupCallbackProxy::UFetchTitleNewsMapPopupCallbackProxy(const FObjectInitializer& o) : Super(o) {}

UFetchTitleNewsMapPopupCallbackProxy * UFetchTitleNewsMapPopupCallbackProxy::FetchTitleNewsPopups(UObject * WorldContextObject)
{
	UFetchTitleNewsMapPopupCallbackProxy* node = NewObject<UFetchTitleNewsMapPopupCallbackProxy>();
	node->mWeakWorldContextObject = WorldContextObject;
	node->URL = stringutil::toFString(MakeURL(ENewsFeed::Map));
	return node;
}

void UFetchTitleNewsMapPopupCallbackProxy::OnHttpRequestSucceeded(FHttpRequestPtr Request, FHttpResponsePtr Response) {
	Json::Value node;
	Json::Reader reader;

	std::string responseStr = stringutil::toStdString(Response->GetContentAsString());
	if (reader.parse(responseStr, node, false)) {

		if (!node.isMember("version")) {
			InternalFail(EFetchTitleNewsFailReason::MalformedJson);
			return;
		}

		// news version 1:
		if (node["version"].asInt(0) == 1) {

			if (!node.isMember("entries") || !node["entries"].isArray()) {
				// if this happens, the json we fetched has not yet been updated to have the correct types.
				InternalFail(EFetchTitleNewsFailReason::MalformedJson);
				return;
			}

			int64 utcOffset = (FDateTime::Now() - FDateTime::UtcNow()).GetTicks();

			// entries array:
			for (const auto& item : node["entries"]) {
				FTitleNewsMapPopup popup;
				// ID
				popup.Id = stringutil::toFString(item["id"].asString(""));

				// title
				popup.Title = FText::FromString(stringutil::utf8ToFString(item["title"].asString("")));

				// Body
				popup.Body = FText::FromString(stringutil::utf8ToFString(item["body"].asString("")));

				// Theme
				if (item.isMember("theme")) {
					popup.Theme = EnumValueFromString(EMissionTheme, stringutil::toFString(item["theme"].asString())).Get(EMissionTheme::Invalid);
				}

				// Image (Load last, as it calls success/fail once its loaded
				if (item.isMember("image")) {
					popup.ImageURL = MakeImageUrl(item["image"]);
				}


				// Image (Load last, as it calls success/fail once its loaded
				if (item.isMember("thumbnail")) {
					popup.ThumbnailURL = MakeImageUrl(item["thumbnail"]);
				}

				// This end time is - despite the field name - in UTC. "localEndTime" will be deprecated and
				// replaced with "endTime" in the future.
				if (item.isMember("localEndTime")) {
					FString endTimeUTCString = stringutil::toFString(item["localEndTime"].asString());
					endTimeUTCString.Append(":00");
					FDateTime endTimeUTC;

					if (FDateTime::ParseIso8601(*endTimeUTCString, endTimeUTC) && !DateTimeUtil::HasPassed(endTimeUTC)) {
						if (item.isMember("showCountDown") && item["showCountDown"].asBool()) {
							popup.EndTime = DateTimeUtil::UTCToLocal(endTimeUTC);
							popup.HasCountdown = true;
						}
						mFetchedNewsPopups.Add(popup);
					}
				}
				else {
					mFetchedNewsPopups.Add(popup);
				}
			}
		}

		//Download any images requested
		for (const auto& news : mFetchedNewsPopups) {
			if (news.ImageURL.IsSet()) {
				DownloadImageForURL(news.ImageURL.GetValue());
			}

			if (news.ThumbnailURL.IsSet()) {
				DownloadImageForURL(news.ThumbnailURL.GetValue());
			}
		}
	}
	else {
		InternalFail(EFetchTitleNewsFailReason::MalformedJson);
	}
}

void UFetchTitleNewsMapPopupCallbackProxy::OnAllRequestsCompleted() {
	FetchTitleNewsMapPopupSucceeded.Broadcast(mFetchedNewsPopups);
}

void UFetchTitleNewsMapPopupCallbackProxy::OnImageFetched(FName url, UTexture2DDynamic* image) {
	if (image) {
		for (auto& newsItem : mFetchedNewsPopups) {
			if (url == FName(*newsItem.ImageURL.Get(""))) {
				newsItem.HeaderImage = image;
			}
			else if (url == FName(*newsItem.ThumbnailURL.Get(""))) {
				newsItem.ThumbnailImage = image;
			}
		}
	}
}