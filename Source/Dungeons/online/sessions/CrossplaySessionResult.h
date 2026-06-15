#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "OnlineSessionSettings.h"
#include "FindSessionsCallbackProxy.h"
#include "CrossplaySessionResult.generated.h"

UENUM(BlueprintType)
enum class ESessionOnlineService : uint8 {
	PSN,
	XBL,
	UNKNOWN
};

UENUM(BlueprintType)
enum class EPlayerPlatform : uint8 {
	PC_PLATFORM,
	XBOXONE_PLATFORM,
	SWITCH_PLATFORM,
	PS4_PLATFORM,
	UNKNOWN
};

USTRUCT(BlueprintType)
struct FSessionPlayerInfo {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString GamerTag;

	UPROPERTY(BlueprintReadOnly)
	FString PSNName;

	UPROPERTY(BlueprintReadOnly)
	EPlayerPlatform Platform;

	UPROPERTY(BlueprintReadOnly)
	ESessionOnlineService OnlineService;

	UPROPERTY(BlueprintReadOnly)
	bool ShouldShowCrossplayIcon;

	UPROPERTY(BlueprintReadOnly)
	bool ShouldShowPSNIcon;

	UPROPERTY(BlueprintReadOnly)
	bool isHost;
};

UCLASS()
class DUNGEONS_API UDungeonsSessionAttributesEntity : public UBlueprintFunctionLibrary {
	GENERATED_BODY()

	UFUNCTION(BlueprintPure, Category = "Dungeons|Online")
	static TArray<FSessionPlayerInfo> GetSessionPlayerInfo(const FBlueprintSessionResult& Result);

	UFUNCTION(BlueprintPure, Category = "Dungeons|Online")
	static ESessionOnlineService GetOnlineService(const FBlueprintSessionResult& BlueprintSession);

private:
	static FSessionPlayerInfo CreateXBLPlayer(const FString& GamerTag, const FString& Platform, const FOnlineSessionSearchResult&);
	static void SetupCommonPSNPlayerAttributes(FSessionPlayerInfo& PlayerInfo, const FOnlineSessionSearchResult&);
	static FSessionPlayerInfo CreatePSNHost(const FOnlineSessionSearchResult&);
	static FSessionPlayerInfo CreatePSNClient(const FString& PSNMember, const FOnlineSessionSearchResult&);
};
