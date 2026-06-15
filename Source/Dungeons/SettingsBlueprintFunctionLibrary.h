#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "online/titlenews/TitleNews.h"
#include "online/titlenews/TitleNewsMapPopup.h"
#include "SettingsBlueprintFunctionLibrary.generated.h"

class APlayerController;
class UGlobalStateData;

UCLASS(BlueprintType)
class USettingsBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Settings|Generic")
	static bool GetSettingFromSave(FString varName, const APlayerControllerBase* PlayerController, int& outInt);

	UFUNCTION(BlueprintCallable, Category = "Settings|Generic")
	static bool SetSettingByName(FString varName, int newValue, APlayerControllerBase* PlayerControllerBase);

	UFUNCTION(BlueprintCallable, Category = "Settings|Generic")
	static bool GetSettingFromSaveInt64(FString varName, const APlayerControllerBase* PlayerController, int64& outInt);

	UFUNCTION(BlueprintCallable, Category = "Settings|Generic")
	static bool SetSettingByNameInt64(FString varName, int64 newValue, APlayerControllerBase* PlayerControllerBase);

	UFUNCTION(BlueprintCallable, Category = "Settings|Generic")
	static bool SetXblActive(bool newValue, APlayerControllerBase * PlayerControllerBase);

	UFUNCTION(BlueprintCallable, Category = "Settings|Generic")
	static bool GetIntByName(UObject* target, FName varName, int& outInt);

	UFUNCTION(BlueprintCallable, Category = "Settings|Generic")
	static bool SetIntByName(UObject* target, FName varName, int32 newValue, int32& outInt);

	UFUNCTION(BlueprintCallable, Category = "Settings|Generic")
	static void SaveGame(UObject* target, APlayerControllerBase* PlayerController);

	UFUNCTION(BlueprintCallable, Category = "Settings|Generic")
	static void LoadGame(UObject* target, APlayerControllerBase* PlayerController);

	UFUNCTION(BlueprintCallable, Category = "Settings|Generic")
	static bool GetTrackedStatByName(FString varName, const APlayerControllerBase* PlayerController);

	UFUNCTION(BlueprintCallable, Category = "Settings|Generic")
	static bool GetIsAccountLinked(const APlayerControllerBase * PlayerController);

	UFUNCTION(BlueprintCallable, Category = "Settings|Generic")
	static bool GetCrossplay(const APlayerControllerBase * PlayerController);

	UFUNCTION(BlueprintCallable, Category = "Settings|Generic")
	static bool GetXblActive(const APlayerControllerBase * PlayerController);

	UFUNCTION(BlueprintCallable, Category = "Settings|Generic")
	static bool IsSignedInToXBL();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static bool HasSeenNews(const FTitleNews& news, const APlayerControllerBase* PlayerController);

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	static void SetSeenNews(const FTitleNews& news, APlayerControllerBase* PlayerController);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static bool HasSeenMapNews(const FTitleNewsMapPopup& news, const APlayerControllerBase* PlayerController);

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	static void SetSeenMapNews(const FTitleNewsMapPopup& news, APlayerControllerBase* PlayerController);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static bool HasSeenDLC6Cinematic(const APlayerControllerBase* PlayerController);

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	static void SetSeenDLC6Cinematic(ABasePlayerController* PlayerController);

	UFUNCTION(BlueprintPure, Category = "Dungeons")
	static bool IsDLCOwned(const UObject* WorldContextObject, EDLCName dlc);

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	static void UnlockAllDLC();

private:
	static bool bUnlockAllDLC;

};
