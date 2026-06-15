#include "MinecraftAPIAuthData.h"

MinecraftAPIAuthData::MinecraftAPIAuthData(const FString& jwtToken, const FString& userId, const FString& requestId, const TArray<FString>& productIdList)
: JwtToken(jwtToken), UserId(userId), RequestId(requestId), ProductIdList(productIdList) {
}
