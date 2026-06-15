#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TitleNews.h"
#include "TitleNewsUtil.generated.h"

UCLASS()
class DUNGEONS_API UTitleNewsUtil : public UBlueprintFunctionLibrary {
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static float getTitleNewsItemAutoNextDelay(const FTitleNews& newsItem);
};


