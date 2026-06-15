#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "TitleNews.h"
#include <OnlineBlueprintCallProxyBase.h>
#include "HttpModule.h"
#include "TitleNewsImageFetcher.h"
#include "TitleNewsMapPopup.h"
#include "FetchTitleNewsCallbackProxy.generated.h"

#define USE_TITLENEWS_TESTING_ENVIRONMENT !UE_BUILD_SHIPPING

UENUM(BlueprintType)
enum class EFetchTitleNewsFailReason : uint8 {		
	RandomThingWentWrong,
	CouldntReachNews,
	MalformedJson,
	MissingWorld,
	Unknown,
};
ENUM_NAME(EFetchTitleNewsFailReason)

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGetTitleNewsFailed, EFetchTitleNewsFailReason, Reason);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGetTitleNewsSucceeded, const TArray<FTitleNews>&, FetchedNews);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGetTitleNewsMapPopupSucceeded, const TArray<FTitleNewsMapPopup>&, FetchedNews);

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnImageDownloaded ,UTexture2DDynamic*, FString)

UCLASS(Abstract)
class DUNGEONS_API UFetchTileNewsCallProxyBase : public UOnlineBlueprintCallProxyBase {
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintAssignable)
	FGetTitleNewsFailed FetchTitleNewsFailed;

	void Activate() override;
protected:
	void DownloadImageForURL(const FString& URL);
	void InternalFail(EFetchTitleNewsFailReason reason);

	FString URL;
	TWeakObjectPtr<UObject> mWeakWorldContextObject;

	static TOptional<FString> MakeImageUrl(const Json::Value&);
private:
	UPROPERTY()
	TArray<UTitleImageFetcher*> ImageFetchers;

	bool Pending = false;

	void CheckIsDone();
	bool IsDownloadingImages() const;

	//Hooks for child classes to implement
	virtual void OnImageFetched(FName, UTexture2DDynamic*) {};
	virtual void OnHttpRequestSucceeded(FHttpRequestPtr Request, FHttpResponsePtr Response) {};
	virtual void OnAllRequestsCompleted() {};

	void InternalOnImageFetched(FName, UTexture2DDynamic*);
	void InternalOnHttpRequestCompleted(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void InternalOnAllRequestsCompleted();
};

UCLASS()
class DUNGEONS_API UFetchTitleNewsCallbackProxy : public UFetchTileNewsCallProxyBase {
	GENERATED_UCLASS_BODY()
public:

	UPROPERTY(BlueprintAssignable)
	FGetTitleNewsSucceeded FetchTitleNewsSucceeded;

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Dungeons|Online|TitleNews")
	static UFetchTitleNewsCallbackProxy* FetchTitleNews(UObject* WorldContextObject);
private:	
	UPROPERTY()
	TArray<FTitleNews> mFetchedNews;

	void OnHttpRequestSucceeded(FHttpRequestPtr Request, FHttpResponsePtr Response) override;
	void OnAllRequestsCompleted() override;
	void OnImageFetched(FName, UTexture2DDynamic*) override;
};

UCLASS()
class DUNGEONS_API UFetchTitleNewsMapPopupCallbackProxy : public UFetchTileNewsCallProxyBase {
	GENERATED_UCLASS_BODY()
public:

	UPROPERTY(BlueprintAssignable)
	FGetTitleNewsMapPopupSucceeded FetchTitleNewsMapPopupSucceeded;

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Dungeons|Online|TitleNews")
	static UFetchTitleNewsMapPopupCallbackProxy* FetchTitleNewsPopups(UObject* WorldContextObject);
private:
	UPROPERTY()
	TArray<FTitleNewsMapPopup> mFetchedNewsPopups;

	void OnHttpRequestSucceeded(FHttpRequestPtr Request, FHttpResponsePtr Response) override;
	void OnAllRequestsCompleted() override;
	void OnImageFetched(FName, UTexture2DDynamic*) override;
};

