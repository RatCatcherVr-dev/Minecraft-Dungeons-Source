#pragma once

#include "PlayfabServicesSettings.generated.h"

UCLASS(config = Game)
class PLAYFABSERVICES_API UPlayfabServicesSettings : public UObject
{
    GENERATED_BODY()

public:
    UPlayfabServicesSettings(const FObjectInitializer& initializer);

    UPROPERTY(Config)
    FString TitleId;

	UPROPERTY(Config)
	FString DevelopmentTitleId;

    UPROPERTY(Config)
    FString PropertyCacheFile;

};
