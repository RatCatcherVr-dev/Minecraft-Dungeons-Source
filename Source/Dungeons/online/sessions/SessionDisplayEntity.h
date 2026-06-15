#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "online/friends/DungeonsFriendsCommon.h"
#include <FindSessionsCallbackProxy.h>
#include "game/difficulty/Difficulty.h"
#include "game/difficulty/ThreatLevel.h"
#include "CrossplaySessionResult.h"
#include "SessionDisplayEntity.generated.h"



UCLASS()
class DUNGEONS_API UDungeonsSessionDisplayEntity : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintPure, Category = "Dungeons|Online|Session")
	static int32 GetPingInMs(const FBlueprintSessionResult& Result);

	UFUNCTION(BlueprintPure, Category = "Dungeons|Online|Session")
	static int32 GetCurrentPlayers(const FBlueprintSessionResult& Result);

	UFUNCTION(BlueprintPure, Category = "Dungeons|Online|Session")
	static int32 GetMaxPlayers(const FBlueprintSessionResult& Result);

	UFUNCTION(BlueprintPure, Category = "Dungeons|Online|Session")
	static FString GetServerName(const FBlueprintSessionResult& Result);

	UFUNCTION(BlueprintPure, Category = "Dungeons|Online|Session")
	static FString GetPlayerName(const FBlueprintSessionResult& Result);

	UFUNCTION(BlueprintPure, Category = "Dungeons|Online|Session")
	static FText GetMapName(const FBlueprintSessionResult& Result);

	UFUNCTION(BlueprintPure, Category = "Dungeons|Online|Session")
	static EGameDifficulty GetDifficulty(const FBlueprintSessionResult& Result);

	UFUNCTION(BlueprintPure, Category = "Dungeons|Online|Session")
	static EThreatLevel GetThreatLevel(const FBlueprintSessionResult& Result);

};