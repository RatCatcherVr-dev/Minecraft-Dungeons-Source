#pragma once

#include "CommonTypes.h"
#include <Containers/Array.h>
#include <Containers/UnrealString.h>
#include <Runtime/Engine/Classes/Kismet/BlueprintFunctionLibrary.h>
#include "UnlockKeyUtils.generated.h"


namespace unlockkey {

bool              unlocks(const TArray<FString>& unlockKeys, const FString& expr);
TOptional<bool>   unlocksAny(const Pred<std::string>&, const std::vector<std::string>& unlockConditions); // returns nullopt if unlockConditions.empty()
Pred<std::string> createPredicate(const TArray<FString>& unlockKeys);

}

UCLASS()
class DUNGEONS_API UUnlockKeyUtils : public UBlueprintFunctionLibrary {
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure, Category = "Dungeons|UnlockKeys", meta = (WorldContext = "WorldContextObject"))
	static bool IsUnlocked(UObject* WorldContextObject, const FString& Expression);

	UFUNCTION(BlueprintCallable, Category = "Dungeons|UnlockKeys", meta = (WorldContext = "Player"))
	static bool GiveUnlockKeyToLocalPlayer(APlayerCharacter* Player, const FString& Key);

	UFUNCTION(BlueprintCallable, Category = "Dungeons|UnlockKeys", meta = (WorldContext = "WorldContextObject"))
	static void GiveUnlockKeyToAllLocalPlayers(UObject* WorldContextObject, const FString& Key);
};
