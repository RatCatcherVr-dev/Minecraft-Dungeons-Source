#pragma once
#include "TitleNews.h"
#include "TitleNewsImageFetcher.generated.h"

class UTexture2DDynamic;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnDownloadImageFinished, FName, UTexture2DDynamic*);

UCLASS()
class DUNGEONS_API UTitleImageFetcher : public UObject {
public:
	GENERATED_BODY()

	bool GetImage(FString ImageURL, const FOnDownloadImageFinished::FDelegate& DownloadCallback);

	FOnDownloadImageFinished OnDownloadFinished;

	FORCEINLINE const FName& GetURL() const { return ImageURL; }
	FORCEINLINE bool IsFinished() const { return !Pending; }
	FORCEINLINE UTexture2DDynamic* GetTexture() const { return Image; }

private:
	UFUNCTION()
	void DownloadImageFailureCallback(UTexture2DDynamic* Texture);

	UFUNCTION()
	void DownloadImageSuccessCallback(UTexture2DDynamic* Texture);

	UPROPERTY()
	UTexture2DDynamic* Image = nullptr;

	bool Pending = false;
	FName ImageURL;
};