#include "GameSettingsFunctionLibrary.h"

#include "DungeonsGameInstance.h"
#include "Engine/DataTable.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "game/actor/character/player/DungeonsLocalPlayer.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/actor/character/player/PlayerControllerBase.h"
#include "game/avatar/AvatarInfo.h"
#include "game/skins/SkinsUtil.h"
#include "GameFramework/GameUserSettings.h"
#include "GameFramework/PlayerInput.h"
#include "locale/LocalizationUtils.h"
#include "SettingsBlueprintFunctionLibrary.h"
#include "ui/hints/HintManager.h"
#include "util/FloatRange.h"

#include "util/AutoDetect.h"

#if PLATFORM_SWITCH
#include "Switch/SwitchPlatformMisc.h"
#endif
#include "game/actor/DungeonsPlayerCameraManager.h"
#include "online/sessions/OnlineUtil.h"
#include "online/crossplay/CrossplayOSS.h"

#ifdef EPIC_STORE_BUILD
#include "EpicServices.h"
#include "Misc/DateTime.h"
#endif

static constexpr float MINIMUM_GAMEPAD_DEADZONE = 0.1f;
static constexpr int32 TRUE = 1;
static constexpr int32 FALSE = 0;

FOnGraphicsQualityChanged UGameSettingsFunctionLibrary::OnLightFunctionQualitySet;

// Game Settings
const TArray<FColor> UGameSettingsFunctionLibrary::GetPlayerColours()
{
	return PlayerIdentityColors;
}

const bool UGameSettingsFunctionLibrary::GetChatEnabled(const APlayerControllerBase* PlayerController)
{
	int saveDataValue;
	if (USettingsBlueprintFunctionLibrary::GetSettingFromSave("ETextChat", PlayerController, saveDataValue))
	{
		return saveDataValue != FALSE;
	}
	return true; // default to enabled = true
}

const InputController::EOutlineState UGameSettingsFunctionLibrary::GetEnemyHighlightColour(const APlayerControllerBase* PlayerController)
{
	int saveDataValue;
	if (USettingsBlueprintFunctionLibrary::GetSettingFromSave("EEnemyOutlineColour", PlayerController, saveDataValue))
	{
		switch (static_cast<EEnemyOutlineColours>(saveDataValue))
		{
		case EEnemyOutlineColours::Red:
			return InputController::EOutlineState::Target;
		case EEnemyOutlineColours::Orange:
			return InputController::EOutlineState::RangedTarget;
		case EEnemyOutlineColours::Yellow:
			return InputController::EOutlineState::LocalP1Outline;
		case EEnemyOutlineColours::Gold:
			return InputController::EOutlineState::LocalP1TargetOutline;
		case EEnemyOutlineColours::Green:
			return InputController::EOutlineState::LocalP2TargetOutline;
		case EEnemyOutlineColours::MintGreen:
			return InputController::EOutlineState::LocalP2Outline;
		case EEnemyOutlineColours::Pink:
			return InputController::EOutlineState::LocalP3Outline;
		case EEnemyOutlineColours::Magenta:
			return InputController::EOutlineState::LocalP3TargetOutline;
		case EEnemyOutlineColours::Purple:
			return InputController::EOutlineState::LocalP4TargetOutline;
		case EEnemyOutlineColours::Lavender:
			return InputController::EOutlineState::LocalP4Outline;
		case EEnemyOutlineColours::White:
			return InputController::EOutlineState::Interactable;
		default:
			break;
		}
	}
	return InputController::EOutlineState::Target;
}

// D11.DJB Pathfinder functionality not yet implemented.
const bool UGameSettingsFunctionLibrary::GetPathfinderEnabled(const APlayerControllerBase* PlayerController)
{
	int saveDataValue;
	if (USettingsBlueprintFunctionLibrary::GetSettingFromSave("EPathfinder", PlayerController, saveDataValue))
	{
		return saveDataValue != FALSE;
	}
	return true; // default to enabled = true
}

void UGameSettingsFunctionLibrary::SetTutorialHintsEnabled(bool Enabled, APlayerControllerBase* PlayerControllerBase)
{
	if (UDungeonsGameInstance* gi = Cast<UDungeonsGameInstance>(PlayerControllerBase->GetWorld()->GetGameInstance()))
	{
		// If we're in game this cast will succeed and refresh the tutorial state
		if (ABasePlayerController* BasePlayerController = Cast<ABasePlayerController>(PlayerControllerBase))
		{
			gi->GetHintManager()->SetHintsUserEnabled(Enabled);
		}
		// Else if we're in the menu, we just initialise the tutorial toggle state
		else 
		{
			gi->GetHintManager()->InitHintsUserEnabled(Enabled);
		}
	}
}

void UGameSettingsFunctionLibrary::SetTutorialHintsEnabled(APlayerControllerBase* PlayerControllerBase)
{
	int saveDataValue;
	if (USettingsBlueprintFunctionLibrary::GetSettingFromSave("ETutorialHints", PlayerControllerBase, saveDataValue))
	{
		SetTutorialHintsEnabled(saveDataValue != FALSE, PlayerControllerBase);
	}
}

void UGameSettingsFunctionLibrary::SetDefaultCrossplayState(APlayerControllerBase* PlayerControllerBase)
{
	online::getCrossplayOss()->SetCrossplaySetting(USettingsBlueprintFunctionLibrary::GetCrossplay(PlayerControllerBase));
}

void UGameSettingsFunctionLibrary::SetDefaultXblActiveState(APlayerControllerBase* PlayerControllerBase)
{
	if (online::getCrossplayOss()->IsPS4Active()) {
		if (USettingsBlueprintFunctionLibrary::GetXblActive(PlayerControllerBase)) {
			if (!online::getCrossplayOss()->IsDungeonsActive()) {
				online::getCrossplayOss()->ActivateDungeonsOSS();
			}
		} else {
			if (online::getCrossplayOss()->IsDungeonsActive()) {
				online::getCrossplayOss()->DeactivateDungeonsOSS();
			}
		}
	}
}

const EMouseLockMode UGameSettingsFunctionLibrary::GetUserMouseLockMode(const APlayerControllerBase* PlayerController)
{
	int saveDataValue;
	if (USettingsBlueprintFunctionLibrary::GetSettingFromSave("ESmartCursorBinding", PlayerController, saveDataValue))
	{
		switch (saveDataValue)
		{
		case 0:
			return EMouseLockMode::LockOnCapture;
		case 1:
			return EMouseLockMode::LockAlways;
		case 2:
			return EMouseLockMode::DoNotLock;
		case 3:
			return EMouseLockMode::LockInFullscreen;
		default:
			break;
		}
	}
	return EMouseLockMode::DoNotLock;
}

void UGameSettingsFunctionLibrary::SetMouseLockMode(EMouseLockMode LockMode, APlayerControllerBase* PlayerControllerBase)
{
	PlayerControllerBase->SetUserMouseLockMode(LockMode);
}

// Controller Settings
void UGameSettingsFunctionLibrary::SetLeftDeadzone(float newDeadzone, ULocalPlayer* LocalPlayer)
{
	if (UDungeonsLocalPlayer* dLocalPlayer = Cast<UDungeonsLocalPlayer>(LocalPlayer)) 
	{
		if (newDeadzone < MINIMUM_GAMEPAD_DEADZONE)
		{
			dLocalPlayer->SetDeadzoneLStick(MINIMUM_GAMEPAD_DEADZONE);
		}
		else
		{
			dLocalPlayer->SetDeadzoneLStick(newDeadzone);
		}
		
	}
}

void UGameSettingsFunctionLibrary::SetRightDeadzone(float newDeadzone, ULocalPlayer* LocalPlayer)
{
	if (UDungeonsLocalPlayer* dLocalPlayer = Cast<UDungeonsLocalPlayer>(LocalPlayer)) 
	{
		if (newDeadzone < MINIMUM_GAMEPAD_DEADZONE)
		{
			dLocalPlayer->SetDeadzoneRStick(MINIMUM_GAMEPAD_DEADZONE);
		}
		else
		{
			dLocalPlayer->SetDeadzoneRStick(newDeadzone);
		}
	}
}

// D11.DJB - Light bar functionality not yet implemented for DS4
const bool UGameSettingsFunctionLibrary::GetPS4LightbarEffectsEnabled(const APlayerControllerBase* PlayerControllerBase)
{
	int saveDataValue;
	if (USettingsBlueprintFunctionLibrary::GetSettingFromSave("ELightBarEffects", PlayerControllerBase, saveDataValue))
	{
		return saveDataValue != FALSE;
	}
	return true; // default to enabled = true
}

// Display Settings
void UGameSettingsFunctionLibrary::SetGraphicsPreset(int PresetValue, APlayerControllerBase* PlayerControllerBase)
{
	// Define preset
	struct GraphicalPreset 
	{
		GraphicalPreset(EPresetValue _ao, EPresetValue _aa, EPresetValue _bloom, EPresetValue _particles, EPresetValue _shadows) :
			AmbientOcclusion{ static_cast<int>(_ao) },
			AntiAlias{ static_cast<int>(_aa) },
			Bloom{ static_cast<int>(_bloom) },
			Particles{ static_cast<int>(_particles) -1 },
			Shadows{ static_cast<int>(_shadows) }
		{}

		int AmbientOcclusion;
		int AntiAlias;
		int Bloom;
		int Particles;
		int Shadows;
	}; GraphicalPreset GraphicsPreset{ EPresetValue::Off, EPresetValue::Off, EPresetValue::Off, EPresetValue::Low, EPresetValue::Off };


	// Assign preset based on value
	/*	Available Options
	 AO -| Off	|		|		|		| On
	 AA -| Off	| Low	| Med	| High	|
	 Bl -| Off	|		|		|		| On
	 Pa -|		| Low	| Med	| High	|
	 Sh -| Off	| Low	| Med	| High	|
	 */

	switch (static_cast<EGraphicalPresetLevel>(PresetValue))
	{
	case EGraphicalPresetLevel::Fanciest:
		GraphicsPreset = GraphicalPreset(EPresetValue::On, EPresetValue::High, EPresetValue::On, EPresetValue::High, EPresetValue::High);
		break;
	case EGraphicalPresetLevel::Fancy:
		GraphicsPreset = GraphicalPreset(EPresetValue::On, EPresetValue::Medium, EPresetValue::On, EPresetValue::Medium, EPresetValue::Medium);
		break;
	case EGraphicalPresetLevel::Balanced:
		GraphicsPreset = GraphicalPreset(EPresetValue::Off, EPresetValue::Medium, EPresetValue::On, EPresetValue::Medium, EPresetValue::Medium);
		break;
	case EGraphicalPresetLevel::Fast:
		GraphicsPreset = GraphicalPreset(EPresetValue::Off, EPresetValue::Low, EPresetValue::Off, EPresetValue::Low, EPresetValue::Low);
		break;
	case EGraphicalPresetLevel::Fastest:
		// Leave as default (all 0)
		break;
	default:
		checkNoEntry();
	}

	// Execute
	ExecuteAmbientOcclusionQualityCommand(GraphicsPreset.AmbientOcclusion != FALSE, PlayerControllerBase);
	ExecuteAntiAliasQualityCommand(GraphicsPreset.AntiAlias, PlayerControllerBase);
	ExecuteBloomQualityCommand(GraphicsPreset.Bloom != FALSE, PlayerControllerBase);
	ExecuteParticleQualityCommand(GraphicsPreset.Particles, PlayerControllerBase);
	ExecuteShadowQualityCommand(GraphicsPreset.Shadows, PlayerControllerBase);

	// Update save data
	USettingsBlueprintFunctionLibrary::SetSettingByName("EAmbientOcclusion", GraphicsPreset.AmbientOcclusion, PlayerControllerBase);
	USettingsBlueprintFunctionLibrary::SetSettingByName("EAntiAliasingQuality", GraphicsPreset.AntiAlias, PlayerControllerBase);
	USettingsBlueprintFunctionLibrary::SetSettingByName("EBloom", GraphicsPreset.Bloom, PlayerControllerBase);
	USettingsBlueprintFunctionLibrary::SetSettingByName("EParticles", GraphicsPreset.Particles, PlayerControllerBase);
	USettingsBlueprintFunctionLibrary::SetSettingByName("EShadowQuality", GraphicsPreset.Shadows, PlayerControllerBase);
	USettingsBlueprintFunctionLibrary::SetSettingByName("EGraphics", PresetValue, PlayerControllerBase);
}

void UGameSettingsFunctionLibrary::ExecuteBrightnessCommand(float value, APlayerController* PlayerController)
{
    check(PlayerController);
	if (!PlayerController)
		return;
	// D11.DB - Changed this to expect a normalized value. Converted to gamma here using the DungeonsGameInstance
	//			GammaCurve
	auto gameInstance = Cast<UDungeonsGameInstance>(PlayerController->GetGameInstance());
	if( gameInstance != nullptr && gameInstance->GammaCurve != nullptr )
	{
        float gamma = gameInstance->GammaCurve->GetFloatValue( value );
#if PLATFORM_SWITCH
		
		SwitchOperationMode operationMode = gameInstance->GetSwitchOperationMode();

		if (operationMode == SwitchOperationMode::Handheld)
		{
			PlayerController->ConsoleCommand(FString("GAMMA " + FString::SanitizeFloat(gamma * 1.15 )));
		}

#endif
        PlayerController->ConsoleCommand(FString("GAMMA " + FString::SanitizeFloat( gamma )));
        return;
	}
}

void UGameSettingsFunctionLibrary::ExecuteBrightnessCommand(APlayerControllerBase* PlayerControllerBase)
{
    check(PlayerControllerBase);

    int saveDataValue = 50;
    USettingsBlueprintFunctionLibrary::GetSettingFromSave("EBrightness", PlayerControllerBase, saveDataValue);
    float normalizedValue = static_cast<float>(saveDataValue) / 100.0f;
    ExecuteBrightnessCommand( normalizedValue, PlayerControllerBase );
}

void UGameSettingsFunctionLibrary::ExecuteResolutionCommand(FIntPoint Resolution, EWindowMode::Type WindowMode, APlayerController* PlayerController)
{
	switch (WindowMode)
	{
	case EWindowMode::Fullscreen:
		PlayerController->ConsoleCommand(FString("r.SetRes " + FString::FromInt(Resolution.X) + "x" + FString::FromInt(Resolution.Y) + "f"));
		break;
	case EWindowMode::Windowed:
		PlayerController->ConsoleCommand(FString("r.SetRes " + FString::FromInt(Resolution.X) + "x" + FString::FromInt(Resolution.Y) + "w"));
		break;
	case EWindowMode::WindowedFullscreen:
		PlayerController->ConsoleCommand(FString("r.SetRes " + FString::FromInt(Resolution.X) + "x" + FString::FromInt(Resolution.Y) + "wf"));
		break;
	default:
		checkNoEntry();
	}
}

bool UGameSettingsFunctionLibrary::IsResolutionValid(const FIntPoint& resolution)
{
	return resolution != FIntPoint::NoneValue && resolution != FIntPoint::ZeroValue;
}


// Graphics Settings
void UGameSettingsFunctionLibrary::ExecuteVSyncCommand(APlayerControllerBase* PlayerControllerBase)
{
	UGameUserSettings* gUserSettings = GEngine->GetGameUserSettings();

	int saveDataValue;
	if (USettingsBlueprintFunctionLibrary::GetSettingFromSave("EVSync", PlayerControllerBase, saveDataValue))
	{
		gUserSettings->SetVSyncEnabled(saveDataValue != FALSE);
		return;
	}
	gUserSettings->SetVSyncEnabled(false); // default to off
	USettingsBlueprintFunctionLibrary::SetSettingByName("EVSync", FALSE, PlayerControllerBase); // ensure front-end matches
}

void UGameSettingsFunctionLibrary::ExecuteFPSLimitCommand(APlayerControllerBase* PlayerControllerBase)
{
	UGameUserSettings* gUserSettings = GEngine->GetGameUserSettings();

	int saveDataValue;
	if (USettingsBlueprintFunctionLibrary::GetSettingFromSave("EFPSLimit", PlayerControllerBase, saveDataValue))
	{
		switch (saveDataValue)
		{
		case 0:
			gUserSettings->SetFrameRateLimit(30.f);
			return;
		case 1:
			gUserSettings->SetFrameRateLimit(60.f);
			return;
		case 2:
			gUserSettings->SetFrameRateLimit(120.f);
			return;
		case 3:
			gUserSettings->SetFrameRateLimit(144.f);
			return;
		case 4:
			gUserSettings->SetFrameRateLimit(240.f);
			return;
		case 5:
			gUserSettings->SetFrameRateLimit(0.f);
			return;
		default:
			break;
		}
	}
	gUserSettings->SetFrameRateLimit(0.f); // default to unlimited
	USettingsBlueprintFunctionLibrary::SetSettingByName("EFPSLimit", 5, PlayerControllerBase); // ensure front-end matches
}


// Audio Settings
void UGameSettingsFunctionLibrary::SetBackgroundAutoMute(bool enable, APlayerControllerBase* PlayerControllerBase)
{
	FApp::SetUnfocusedVolumeMultiplier(enable ? 0.f : 1.f);
}

void UGameSettingsFunctionLibrary::EnableTextToSpeech(APlayerControllerBase* PlayerControllerBase)
{
	int saveDataValue;
	if (USettingsBlueprintFunctionLibrary::GetSettingFromSave("ETextToSpeech", PlayerControllerBase, saveDataValue))
	{
		PlayerControllerBase->ConsoleCommand(FString(saveDataValue != FALSE ? "EnableTextToSpeech" : "DisableTextToSpeech"));
	}
}

void UGameSettingsFunctionLibrary::EnableSubtitles(bool enable, APlayerController* PlayerController)
{
	if (GEngine)
		GEngine->bSubtitlesForcedOff = !enable;
}

void UGameSettingsFunctionLibrary::EnableSubtitles(APlayerControllerBase* PlayerControllerBase)
{
	int saveDataValue;
	if (USettingsBlueprintFunctionLibrary::GetSettingFromSave("ESubtitles", PlayerControllerBase, saveDataValue))
	{
		EnableSubtitles(saveDataValue != FALSE, PlayerControllerBase);
	}
}

bool UGameSettingsFunctionLibrary::AreSubtitlesForceDisabled()
{
	if (GEngine)
	{
		return GEngine->bSubtitlesForcedOff;
	}
	return 0;
}

void UGameSettingsFunctionLibrary::EnableScreenShake(bool enable, APlayerController* PlayerController)
{
	// Cast here to avoid casting in Blueprints
	const auto PlayerControllerBase = Cast<APlayerControllerBase>(PlayerController);

	USettingsBlueprintFunctionLibrary::SetSettingByName("EScreenShake", enable ? TRUE : FALSE, PlayerControllerBase);
}

bool UGameSettingsFunctionLibrary::IsScreenShakeEnabled(const APlayerControllerBase* PlayerControllerBase)
{
	int saveDataValue;
	if(USettingsBlueprintFunctionLibrary::GetSettingFromSave("EScreenShake", PlayerControllerBase, saveDataValue))
	{
		return saveDataValue != FALSE;
	}

	return true;
}

void UGameSettingsFunctionLibrary::EnableScreenShake(APlayerControllerBase* PlayerControllerBase)
{
	EnableScreenShake(IsScreenShakeEnabled(PlayerControllerBase), PlayerControllerBase);
}

// Initialisation
void UGameSettingsFunctionLibrary::InitialiseLocalizationFromSaveData(APlayerControllerBase* PlayerControllerBase)
{
// do not load saved locale in editor, since it interferes with the Unreal Editor UI
#if !UE_EDITOR
	int saveDataValue;
	if (USettingsBlueprintFunctionLibrary::GetSettingFromSave("ELanguageSelect", PlayerControllerBase, saveDataValue))
	{
		ULocalizationUtils::ChangeLocalization(static_cast<DungeonsLocale>(saveDataValue));
	}
#endif
}

void UGameSettingsFunctionLibrary::InitialiseControllerSettings(APlayerControllerBase* PlayerControllerBase)
{
	int saveDataValue;
	if (USettingsBlueprintFunctionLibrary::GetSettingFromSave("EDeadZoneLeftStick", PlayerControllerBase, saveDataValue))
	{
		SetLeftDeadzone(static_cast<float>(saveDataValue) / 100.f, PlayerControllerBase->GetLocalPlayer());
	}

	if (USettingsBlueprintFunctionLibrary::GetSettingFromSave("EDeadZoneRightStick", PlayerControllerBase, saveDataValue))
	{
		SetRightDeadzone(static_cast<float>(saveDataValue) / 100.f, PlayerControllerBase->GetLocalPlayer());
	}

}

void UGameSettingsFunctionLibrary::InitialiseMouseSettings(APlayerControllerBase* PlayerControllerBase)
{
	SetMouseLockMode(GetUserMouseLockMode(PlayerControllerBase), PlayerControllerBase);
}

void UGameSettingsFunctionLibrary::ExecuteAmbientOcclusionQualityCommand(bool enabled, APlayerController* PlayerController)
{
	if (enabled)
	{
		PlayerController->ConsoleCommand(FString("r.AmbientOcclusionLevels 1"));
		PlayerController->ConsoleCommand(FString("r.AmbientOcclusionMipLevelFactor 1"));
		PlayerController->ConsoleCommand(FString("r.AmbientOcclusionMaxQuality 10"));
		PlayerController->ConsoleCommand(FString("r.AmbientOcclusionRadiusScale 1.2"));
	}
	else
	{
		PlayerController->ConsoleCommand(FString("r.AmbientOcclusionLevels 0"));
	}
}

void UGameSettingsFunctionLibrary::ExecuteAmbientOcclusionQualityCommand(APlayerControllerBase* PlayerControllerBase)
{
	int saveDataValue;
	if (USettingsBlueprintFunctionLibrary::GetSettingFromSave("EAmbientOcclusion", PlayerControllerBase, saveDataValue))
	{
		ExecuteAmbientOcclusionQualityCommand(saveDataValue != FALSE, PlayerControllerBase);
		return;
	}
	ExecuteAmbientOcclusionQualityCommand(true, PlayerControllerBase); // default to true
	USettingsBlueprintFunctionLibrary::SetSettingByName("EAmbientOcclusion", TRUE, PlayerControllerBase); // ensure front-end matches
}

void UGameSettingsFunctionLibrary::ExecuteAntiAliasQualityCommand(int value, APlayerController* PlayerController)
{
	switch (value)
	{
	case 3:
		PlayerController->ConsoleCommand(FString("r.PostProcessAAQuality 5"));
		return;
	case 2:
		PlayerController->ConsoleCommand(FString("r.PostProcessAAQuality 4"));
		return;
	case 1:
		PlayerController->ConsoleCommand(FString("r.PostProcessAAQuality 3"));
		return;
	case 0:
		PlayerController->ConsoleCommand(FString("r.PostProcessAAQuality 0"));
		return;
	default:
		checkNoEntry();
	}
}

void UGameSettingsFunctionLibrary::ExecuteAntiAliasQualityCommand(APlayerControllerBase* PlayerControllerBase)
{
	int saveDataValue;
	if (USettingsBlueprintFunctionLibrary::GetSettingFromSave("EAntiAliasingQuality", PlayerControllerBase, saveDataValue))
	{
		ExecuteAntiAliasQualityCommand(saveDataValue, PlayerControllerBase);
		return;
	}
	ExecuteAntiAliasQualityCommand(3, PlayerControllerBase); // default to high
	USettingsBlueprintFunctionLibrary::SetSettingByName("EAntiAliasingQuality", 3, PlayerControllerBase); // ensure front-end matches
}

void UGameSettingsFunctionLibrary::ExecuteShadowQualityCommand(int value, APlayerController* PlayerController)
{	
	struct ShadowQualityValues
	{
		ShadowQualityValues(int _lfq, int _sq, int _cs, int _scmc, float _scts) :
			LightFunctionQuality{_lfq}, 
			ShadowQuality{_sq}, 
			CapsuleShadows{_cs}, 
			ShadowCSMMaxCascades{_scmc}, 
			ShadowCSMTransitionScale{_scts} 
		{}

		int LightFunctionQuality;
		int ShadowQuality;
		int CapsuleShadows;
		int ShadowCSMMaxCascades;
		float ShadowCSMTransitionScale;

	}; ShadowQualityValues shadowQualityValues{ 1, 3, 1, 1, 0.25f }; // default values

	switch (value)
	{
	case 3:	// High
		shadowQualityValues = ShadowQualityValues(3, 3, 1, 3, 0.1f);
		break;
	case 2:	// Medium
		shadowQualityValues = ShadowQualityValues(2, 2, 1, 2, 1.f);
		break;
	case 1:	// Low
		shadowQualityValues = ShadowQualityValues(1, 1, 1, 2, 3.f);
		break;
	case 0:	// Off
		shadowQualityValues = ShadowQualityValues(0, 1, 0, 0, 5.f);
		break;
	default:
		break;
	}

	PlayerController->ConsoleCommand(FString("r.LightFunctionQuality " + FString::FromInt(shadowQualityValues.LightFunctionQuality)));
	PlayerController->ConsoleCommand(FString("r.ShadowQuality " + FString::FromInt(shadowQualityValues.ShadowQuality)));
	PlayerController->ConsoleCommand(FString("r.CapsuleShadows " + FString::FromInt(shadowQualityValues.CapsuleShadows)));
	PlayerController->ConsoleCommand(FString("r.Shadow.CSM.MaxCascades " + FString::FromInt(shadowQualityValues.ShadowCSMMaxCascades)));
	PlayerController->ConsoleCommand(FString("r.Shadow.CSM.TransitionScale " + FString::SanitizeFloat(shadowQualityValues.ShadowCSMTransitionScale)));

	OnLightFunctionQualitySet.Broadcast(shadowQualityValues.LightFunctionQuality, PlayerController);
}										  
										  
void UGameSettingsFunctionLibrary::ExecuteShadowQualityCommand(APlayerControllerBase* PlayerControllerBase)
{
	int saveDataValue;
	if (USettingsBlueprintFunctionLibrary::GetSettingFromSave("EShadowQuality", PlayerControllerBase, saveDataValue))
	{
		ExecuteShadowQualityCommand(saveDataValue, PlayerControllerBase);
		return;
	}
	ExecuteShadowQualityCommand(3, PlayerControllerBase); // default to high
	USettingsBlueprintFunctionLibrary::SetSettingByName("EShadowQuality", 3, PlayerControllerBase); // ensure front-end matches
}

void UGameSettingsFunctionLibrary::ExecuteParticleQualityCommand(int value, APlayerController* PlayerController)
{
	switch (value)
	{
	case 2:	// High
		PlayerController->ConsoleCommand(FString("r.ParticleLightQuality 2"));
		PlayerController->ConsoleCommand(FString("r.EmitterSpawnRateScale 2"));
		PlayerController->ConsoleCommand(FString("FX.MaxCPUParticlesPerEmitter 1000")); 
		return;
	case 1:	// Medium
		PlayerController->ConsoleCommand(FString("r.ParticleLightQuality 1"));
		PlayerController->ConsoleCommand(FString("r.EmitterSpawnRateScale 1"));
		PlayerController->ConsoleCommand(FString("FX.MaxCPUParticlesPerEmitter 500")); 
		return;
	case 0:	// Low
		PlayerController->ConsoleCommand(FString("r.ParticleLightQuality 0"));
		PlayerController->ConsoleCommand(FString("r.EmitterSpawnRateScale 0.5"));
		PlayerController->ConsoleCommand(FString("FX.MaxCPUParticlesPerEmitter 250"));
		return;
	default:
		break;
	}
}

void UGameSettingsFunctionLibrary::ExecuteParticleQualityCommand(APlayerControllerBase* PlayerControllerBase)
{
	int saveDataValue;
	if (USettingsBlueprintFunctionLibrary::GetSettingFromSave("EParticles", PlayerControllerBase, saveDataValue))
	{
		ExecuteParticleQualityCommand(saveDataValue, PlayerControllerBase);
		return;
	}
	ExecuteParticleQualityCommand(2, PlayerControllerBase); // default to high
	USettingsBlueprintFunctionLibrary::SetSettingByName("EParticles", 2, PlayerControllerBase); // ensure front-end matches
}

void UGameSettingsFunctionLibrary::ExecuteBloomQualityCommand(bool enabled, APlayerController* PlayerController)
{
	if (enabled)
	{
		PlayerController->ConsoleCommand(FString("r.BloomQuality 5"));	
	}
	else
	{
		PlayerController->ConsoleCommand(FString("r.BloomQuality 0"));
	}
}

void UGameSettingsFunctionLibrary::ExecuteBloomQualityCommand(APlayerControllerBase* PlayerControllerBase)
{
	int saveDataValue;
	if (USettingsBlueprintFunctionLibrary::GetSettingFromSave("EBloom", PlayerControllerBase, saveDataValue))
	{
		ExecuteBloomQualityCommand(saveDataValue != FALSE, PlayerControllerBase);
		return;
	}
	ExecuteBloomQualityCommand(true, PlayerControllerBase); // default to true
	USettingsBlueprintFunctionLibrary::SetSettingByName("EBloom", TRUE, PlayerControllerBase); // ensure front-end matches
}

#ifdef EPIC_STORE_BUILD
void static InitialiseEpicStoreSettings(APlayerControllerBase *PlayerControllerBase)
{
	const char *setting = "ELastBootTimestampEpoch";
	int64 lastBootTimestamp = 0;
	USettingsBlueprintFunctionLibrary::GetSettingFromSaveInt64(setting, PlayerControllerBase, lastBootTimestamp);		
	USettingsBlueprintFunctionLibrary::SetSettingByNameInt64(setting, FDateTime::UtcNow().ToUnixTimestamp(), PlayerControllerBase);

	using namespace minecraft::epicstore;
	/* Load the Module, Initialize the Epic Online Services SDK and initiate the login sequence */
	FEpicServicesModule &EosModule = FEpicServicesModule::Get();
	EosModule.SetLastBootTimestamp(lastBootTimestamp);
}
#endif

void UGameSettingsFunctionLibrary::InitialiseUserSettings(APlayerControllerBase* PlayerControllerBase)
{
 	// Init settings that apply to PC & console
 	InitialiseControllerSettings(PlayerControllerBase);
 	ExecuteBrightnessCommand(PlayerControllerBase);
	EnableTextToSpeech(PlayerControllerBase);
	EnableSubtitles(PlayerControllerBase);
	EnableScreenShake(PlayerControllerBase);
	SetTutorialHintsEnabled(PlayerControllerBase);
	SetDefaultCrossplayState(PlayerControllerBase);
	SetDefaultXblActiveState(PlayerControllerBase);

 	// Init PC specific settings
#if PLATFORM_WINDOWS || WITH_EDITOR

	if (Cast<UDungeonsGameInstance>(PlayerControllerBase->GetGameInstance())->IsInitialBoot())
	{
		RunAutoDetect(PlayerControllerBase);
	}

	// D11.DJB - Moved loc load to PC only until we revisit hot-reloading loc on consoles.
	InitialiseLocalizationFromSaveData(PlayerControllerBase);
	InitialiseMouseSettings(PlayerControllerBase);
	ExecuteVSyncCommand(PlayerControllerBase);
	ExecuteFPSLimitCommand(PlayerControllerBase);
	ExecuteAmbientOcclusionQualityCommand(PlayerControllerBase);
	ExecuteAntiAliasQualityCommand(PlayerControllerBase);
	ExecuteBloomQualityCommand(PlayerControllerBase);
	ExecuteParticleQualityCommand(PlayerControllerBase);
	ExecuteShadowQualityCommand(PlayerControllerBase);
#ifdef EPIC_STORE_BUILD
	InitialiseEpicStoreSettings(PlayerControllerBase);
#endif //EPIC_STORE_BUILD
	if (UGameUserSettings* gUserSettings = GEngine->GetGameUserSettings())
	{
		gUserSettings->ApplySettings(false);	
	}

#endif

	PlayerControllerBase->SetInitialized(true);
}

void UGameSettingsFunctionLibrary::RunAutoDetect(APlayerControllerBase* PlayerControllerBase)
{
	if (UDungeonsGameInstance* DGI = Cast<UDungeonsGameInstance>(PlayerControllerBase->GetGameInstance()))
	{
		DGI->GetAutoDetect()->StartAutoDetect(PlayerControllerBase);
		return;
	}
}
