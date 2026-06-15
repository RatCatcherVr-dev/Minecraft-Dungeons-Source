#pragma once

#include "CoreMinimal.h"

struct DUNGEONSAUTH_API MinecraftAPIAuthData {

	MinecraftAPIAuthData(const FString& jwtToken, const FString& userId, const FString& requestId, const TArray<FString>& productIdList);
	
	TOptional<FString> JwtToken;
	TOptional<FString> UserId;
    TOptional<FString> RequestId;
    TOptional<TArray<FString>> ProductIdList;
};