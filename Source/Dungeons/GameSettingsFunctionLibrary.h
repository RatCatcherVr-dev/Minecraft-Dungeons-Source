#pragma once

#include "CoreMinimal.h"

#include "game/actor/character/player/PlayerControllerBase.h"
#include "game/input/InputController.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameSettingsFunctionLibrary.generated.h"


/**
 * 
 */
class APlayerController;
class ABasePlayerController;
struct FHardwareSurveyResults;

UENUM()
enum class EEnemyOutlineColours : uint8
{
	Red, Orange, Yellow, Gold, Green, MintGreen, Pink, Magenta, Purple, Lavender, White
};

enum class EGraphicalPresetLevel : uint8
{
	Fastest, Fast, Balanced, Fancy, Fanciest
};

UENUM(BlueprintType)
enum class EPresetValue : uint8
{
	Off, 
	Low, 
	Medium, 
	High, 
	On
};

USTRUCT(BlueprintType)
struct DUNGEONS_API FSettingValueState
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(BlueprintReadWrite)
	int LastConfirmedSettingValue{ -1 };

	UPROPERTY(BlueprintReadWrite)
	int DesiredSettingValue{ -1 };

	UPROPERTY(BlueprintReadWrite)
	bool SettingChanged{ false };
};

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnGraphicsQualityChanged, int, APlayerController*);

UCLASS()
class DUNGEONS_API UGameSettingsFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	// Game Settings
	UFUNCTION(BlueprintCallable, Category = "Dungeons|Game Settings|Game Settings")
	static const bool GetChatEnabled(const APlayerControllerBase* PlayerControllerBase);

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Game Settings|Game Settings")
	static const TArray<FColor> GetPlayerColours();

	static const InputController::EOutlineState GetEnemyHighlightColour(const APlayerControllerBase* PlayerControllerBase);

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Game Settings|Game Settings")
	static const bool GetPathfinderEnabled(const APlayerControllerBase* PlayerControllerBase);

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Game Settings|Game Settings")
	static void SetTutorialHintsEnabled(bool Enabled, APlayerControllerBase* PlayerControllerBase);

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Game Settings|Game Settings")
	static void SetDefaultCrossplayState(APlayerControllerBase* PlayerControllerBase);

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Game Settings|Game Settings")
	static void SetDefaultXblActiveState(APlayerControllerBase* PlayerControllerBase);

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Game Settings|Mouse Settings")
	static void SetMouseLockMode(EMouseLockMode LockMode, APlayerControllerBase* PlayerControllerBase);

	// Controller Settings
	UFUNCTION(BlueprintCallable, Category = "Dungeons|Game Settings|Controller Settings")
	static void SetLeftDeadzone(float value, ULocalPlayer* LocalPlayer);

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Game Settings|Controller Settings")
	static void SetRightDeadzone(float value, ULocalPlayer* LocalPlayer);

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Game Settings|Controller Settings")
	static const bool GetPS4LightbarEffectsEnabled(const APlayerControllerBase* PlayerControllerBase);

	// Display Settings
	UFUNCTION(BlueprintCallable, Category = "Dungeons|Game Settings|Display Settings")
	static void SetGraphicsPreset(int PresetValue, APlayerControllerBase* PlayerControllerBase);

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Game Settings|Display Settings")
	static void ExecuteBrightnessCommand(float value, APlayerController* PlayerController);

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Game Settings|Display Settings")
	static void ExecuteResolutionCommand(FIntPoint Resolution, EWindowMode::Type WindowMode, APlayerController* PlayerController);

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Game Settings|Display Settings|Utilities")
	static bool IsResolutionValid(const FIntPoint& resolution);


	// Graphics Settings
	UFUNCTION(BlueprintCallable, Category = "Dungeons|Game Settings|Graphics Settings")
	static void ExecuteAmbientOcclusionQualityCommand(bool enabled, APlayerController* PlayerController);

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Game Settings|Graphics Settings")
	static void ExecuteAntiAliasQualityCommand(int value, APlayerController* PlayerController);

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Game Settings|Graphics Settings")
	static void ExecuteShadowQualityCommand(int value, APlayerController* PlayerController);

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Game Settings|Graphics Settings")
	static void ExecuteParticleQualityCommand(int value, APlayerController* PlayerController);

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Game Settings|Graphics Settings")
	static void ExecuteBloomQualityCommand(bool enabled, APlayerController* PlayerController);

	// Audio Settings
	UFUNCTION(BlueprintCallable, Category = "Dungeons|Game Settings|Audio Settings")
	static void SetBackgroundAutoMute(bool enable, APlayerControllerBase* PlayerControllerBase);

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Game Settings|Accessibility Settings")
	static void EnableSubtitles(bool enable, APlayerController* PlayerController);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons|Game Settings|Accessibility Settings")
	static bool AreSubtitlesForceDisabled();

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Game Settings|Accessibility Settings")
	static void EnableScreenShake(bool enable, APlayerController* PlayerController);

	static bool IsScreenShakeEnabled(const APlayerControllerBase*);
	
	// Initialisation
	static void InitialiseUserSettings(APlayerControllerBase* PlayerControllerBase);

	static const EMouseLockMode GetUserMouseLockMode(const APlayerControllerBase* PlayerControllerBase);

	//D11.KS
	static void RunAutoDetect(APlayerControllerBase* PlayerControllerBase);

	static FOnGraphicsQualityChanged &GetOnLightFunctionQualitySet() { return OnLightFunctionQualitySet; }
private:
	static void InitialiseLocalizationFromSaveData(APlayerControllerBase* PlayerControllerBase);
	static void InitialiseControllerSettings(APlayerControllerBase* PlayerControllerBase);
	static void InitialiseMouseSettings(APlayerControllerBase* PlayerControllerBase);
	static void EnableTextToSpeech(APlayerControllerBase* PlayerControllerBase);
	static void EnableSubtitles(APlayerControllerBase* PlayerControllerBase);
	static void EnableScreenShake(APlayerControllerBase* PlayerControllerBase);
	static void SetTutorialHintsEnabled(APlayerControllerBase* PlayerControllerBase);

	static void ExecuteBrightnessCommand(APlayerControllerBase* PlayerController);
	static void ExecuteVSyncCommand(APlayerControllerBase* PlayerControllerBase);
	static void ExecuteFPSLimitCommand(APlayerControllerBase* PlayerControllerBase);

	static void ExecuteAmbientOcclusionQualityCommand(APlayerControllerBase* PlayerController);
	static void ExecuteAntiAliasQualityCommand(APlayerControllerBase* PlayerController);
	static void ExecuteShadowQualityCommand(APlayerControllerBase* PlayerController);
	static void ExecuteParticleQualityCommand(APlayerControllerBase* PlayerController);
	static void ExecuteBloomQualityCommand(APlayerControllerBase* PlayerController);

	static FOnGraphicsQualityChanged OnLightFunctionQualitySet;
};
