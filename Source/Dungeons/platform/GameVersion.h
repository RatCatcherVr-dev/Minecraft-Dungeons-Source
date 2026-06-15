/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameVersion.generated.h"


UENUM(BlueprintType, meta = (Bitflags))
enum class EPlatformType : uint8
{
	//None = 0,  // Clear flag. Required by ENUM_CLASS_FLAGS macro

	E_PLATFORM_XBONE = 1		UMETA(DisplayName = "XBOne"),
	E_PLATFORM_PC				UMETA(DisplayName = "PC"),
	E_PLATFORM_PS4				UMETA(DisplayName = "PS4"),
	E_PLATFORM_SWITCH			UMETA(DisplayName = "Switch"),
	E_PLATFORM_MAC				UMETA(DisplayName = "Mac"),
	E_PLATFORM_IOS				UMETA(DisplayName = "IOs"),
	E_PLATFORM_ANDROID			UMETA(DisplayName = "Android"),
	E_PLATFORM_LINUX			UMETA(DisplayName = "Linux")

};
ENUM_CLASS_FLAGS(EPlatformType)
ENUM_NAME(EPlatformType)

UENUM(BlueprintType, meta = (Bitflags))
enum class ESubPlatformType : uint8
{
	//None = 0,  // Clear flag. Required by ENUM_CLASS_FLAGS macro

	E_SUBPLATFORM_BASE= 1				UMETA(DisplayName = "Base Unit"),
	
	//Xbox One Subplatforms
	E_SUBPLATFORM_XBONE_S = 2			UMETA(DisplayName = "XBOne S"),
	E_SUBPLATFORM_XBONE_X = 3			UMETA(DisplayName = "XBOne X"),
	E_SUBPLATFORM_XBONE_LOCKHART = 4	UMETA(DisplayName = "XBOne Lockhart"),
	E_SUBPLATFORM_XBONE_SERIES_X = 5	UMETA(DisplayName = "XBOne SeriesX"),

	//Playstation Subplatforms
	E_SUBPLATFORM_PS4_PRO = 2			UMETA(DisplayName = "PS4 Pro"),
	E_SUBPLATFORM_PS5 = 3				UMETA(DisplayName = "PS5")
};
ENUM_CLASS_FLAGS(ESubPlatformType)


UENUM(BlueprintType, meta = (Bitflags))
enum class EConfigurationType : uint8
{
	//None = 0,  // Clear flag. Required by ENUM_CLASS_FLAGS macro

	E_CONFIG_DEBUG							UMETA(DisplayName = "Debug"),
	E_CONFIG_DEVELOPMENT			UMETA(DisplayName = "Development"),
	E_CONFIG_TEST								UMETA(DisplayName = "Test"),
	E_CONFIG_SHIPPING						UMETA(DisplayName = "Shipping")

};
ENUM_CLASS_FLAGS(EConfigurationType)


UCLASS()
class DUNGEONS_API UGameVersion : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/* Get git HEAD for display in the game */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Build Version String"), Category="Versioning")
	static FString BuildVersion();

	/* Get only build number */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Build Number String"), Category = "Versioning")
	static FString GetBuildNumber();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Version Number String"), Category = "Versioning")
	static FString GetVersionNumber();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Platform")
	static EPlatformType GetPlatformEnum();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Platform")
	static ESubPlatformType GetSubPlatformEnum();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Platform")
	static bool GetPlatformMatch(int32 PlatformFlags);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Platform")
	static bool GetSubPlatformMatch(int32 PlatformFlags);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Platform")
	static EConfigurationType GetBuildConfiguration();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Platform")
	static bool GetConfigurationMatch(int32 ConfigFlags);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Platform")
	static bool PlatformCheckMatch(const EPlatformType PlatformFlags) { return GetPlatformMatch(1 << (int32)PlatformFlags); }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Platform")
	static bool SubPlatformCheckMatch(const ESubPlatformType PlatformFlags) { return GetSubPlatformMatch(1 << (int32)PlatformFlags); }

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", CallableWithoutWorldContext), Category = "Utilities|Text")
	static void DungeonsPrintString(UObject* WorldContextObject, const FString& InString = FString(TEXT("Hello")), bool bPrintToScreen = true, FLinearColor TextColor = FLinearColor(0.0, 0.66, 1.0), float Duration = 2.f);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", CallableWithoutWorldContext), Category = "Utilities|Text")
	static void DungeonsPrintText(UObject* WorldContextObject, const FText InText, bool bPrintToScreen = true, FLinearColor TextColor = FLinearColor(0.0, 0.66, 1.0), float Duration = 2.f)
	{
		DungeonsPrintString(WorldContextObject, InText.ToString(), bPrintToScreen,TextColor, Duration);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Dungeons")
	static FText GetMenuVersionString();

};
