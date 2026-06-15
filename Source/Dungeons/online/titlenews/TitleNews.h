#pragma once

#include "TitleNews.generated.h"

class UTexture2DDynamic;

USTRUCT(BlueprintType)
struct FTitleNews
{
	GENERATED_BODY()
	
	FString Id;

	UPROPERTY(BlueprintReadOnly)
	FText Title;

	UPROPERTY(BlueprintReadOnly)
	FText Category;	

	UPROPERTY(BlueprintReadOnly)
	FText Body;

	UPROPERTY(BlueprintReadOnly)
	UTexture2DDynamic* HeaderImage = nullptr;
	
	TOptional<FString> ImageURL;
};
