#include "TitleNewsImageFetcher.h"
#include "AsyncTaskDownloadImage.h"

bool UTitleImageFetcher::GetImage(FString imageURL, const FOnDownloadImageFinished::FDelegate& DownloadCallback) {
	if (Pending) return false;

	Pending = true;

	ImageURL = FName(*imageURL);
	OnDownloadFinished.Add(DownloadCallback);

	UAsyncTaskDownloadImage* mDownloadTask = UAsyncTaskDownloadImage::DownloadImage(imageURL);
	mDownloadTask->OnSuccess.AddDynamic(this, &UTitleImageFetcher::DownloadImageSuccessCallback);
	mDownloadTask->OnFail.AddDynamic(this, &UTitleImageFetcher::DownloadImageFailureCallback);

	return true;
}

void UTitleImageFetcher::DownloadImageSuccessCallback(UTexture2DDynamic* Texture) {
	Image = Texture;
	Pending = false;

	OnDownloadFinished.Broadcast(ImageURL, Texture);
}

void UTitleImageFetcher::DownloadImageFailureCallback(UTexture2DDynamic* dummy) {
	Pending = false;
	OnDownloadFinished.Broadcast(ImageURL, dummy);
}
