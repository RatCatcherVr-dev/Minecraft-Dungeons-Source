// © 2020 Mojang Synergies AB. TM Microsoft Corporation.
#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UnrealString.h"

#include "LoginFlowBpFunctionsLib.generated.h"

class APlayerControllerBase;

UCLASS()
class DUNGEONS_API ULoginFlowBpFunctionsLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Dungeons|Online|Crossplay")
	static FText GetLinkErrorGamertagHint();

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Online|Crossplay")
	static void ActivateDungeonsOSS(APlayerControllerBase* playerController);

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Online|Crossplay")
	static void SignOutXBL(APlayerControllerBase* playerController);

	// D11.GM - This Function will set and save the state of crossplay
	UFUNCTION(BlueprintCallable, Category = "Dungeons|Online|Crossplay")
	static void SetCrossplayState(APlayerControllerBase* playerController, bool EnableCrossplay);

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Online|Crossplay")
	static void UnlinkXBLAccount(APlayerControllerBase* playerController);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons|Online|Crossplay")
	static bool IsCrossplayEnabled();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons|Online|Crossplay")
	static bool AreCloudServicesEnabled();


	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons|Online|Crossplay")
	static bool ShouldDisplayCloudServicesFeature();

	

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons|Online|Crossplay")
	static FString GetXBLNameTag(int localPlayerNum);

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Online|Crossplay")
	static FString GetXBLGamerscore(int localPlayerNum);

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Online|Crossplay")
	static FString GetXBLPictureUri(int localPlayerNum);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons|Online")
	static bool IsSignedInToPSN();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons|Online|Debug")
	static bool IsDebugConsoleOpen();

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Online|Debug")
	static void SetDebugConsoleOpen(bool value);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons|Online")
	static bool IsProfileTextToSpeechEnabled(int localPlayerNum);


private:
	static bool IsInitialUserAccountLinked();
};
