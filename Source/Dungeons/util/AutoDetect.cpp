#include "util/AutoDetect.h"
#include "util/AutoDetectDefs.h"

#include "Engine/Engine.h"
#include "HAL/PlatformSurvey.h"
#include "GenericPlatform/GenericPlatformSurvey.h"
#include "RHI.h"
#include "game/actor/character/player/PlayerControllerBase.h"
#include "GameSettingsFunctionLibrary.h"
#include "DungeonsGameInstance.h"

#if PLATFORM_WINDOWS
#include <powrprof.h>
#pragma comment( lib, "PowrProf.lib" )

THIRD_PARTY_INCLUDES_START
#include <subauth.h>
THIRD_PARTY_INCLUDES_END
#endif // PLATFORM_WINDOWS


DEFINE_LOG_CATEGORY_STATIC(LogAutoDetect, Log, All);

// Tweak this value to alter the minimum performance threshold before we set the graphics preset to lowest on initial boot
constexpr float LOW_PERFORMANCE_THRESHOLD = 90.f;
constexpr int32 LOW_VIDEO_MEMORY_THRESHOLD_MB = 2048;


AutoDetect::AutoDetect()
{
	mPendingHardwareSurveyResults = false;
	mPlayerControllerBase = nullptr;
}

bool AutoDetect::AutoDetectCanRun()
{
#if !WITH_EDITOR && PLATFORM_WINDOWS
	return true;
#else
	return false;
#endif
}

bool AutoDetect::IsHardwareSurveyRequired()
{
	if (mPlayerControllerBase == nullptr || IsRunningDedicatedServer() || GIsBuildMachine)
	{
		return false;
	}
	return true;
}

void AutoDetect::StartAutoDetect(APlayerControllerBase* PlayerControllerBase)
{
	// Early exit on invalid platform
	if (!AutoDetectCanRun())
	{
		UE_LOG(LogAutoDetect, Log, TEXT("Not currently supporting this platform"));
		return;
	}

	mPlayerControllerBase = PlayerControllerBase;

	UE_LOG(LogAutoDetect, Log, TEXT("Starting..."));
	if (IsHardwareSurveyRequired())
	{
		mPendingHardwareSurveyResults = true;
		TickerHandle = FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateRaw(this, &AutoDetect::TickHardwareSurvey));
	}
}

bool AutoDetect::TickHardwareSurvey(float delta)
{
	bool bContinueTick = true;
	if (mPendingHardwareSurveyResults)
	{
		UE_LOG(LogAutoDetect, Log, TEXT("Tick hardware survey | This should only be seen once during initial boot of the application"));
		FHardwareSurveyResults HardwareSurveyResults;
		if (FPlatformSurvey::GetSurveyResults(HardwareSurveyResults))
		{
			mPendingHardwareSurveyResults = false;
			bContinueTick = false;
			OnHardwareSurveyCompleted(HardwareSurveyResults);
		}
	}
	else
	{
		bContinueTick = false;
	}
	return bContinueTick;
}


void AutoDetect::OnHardwareSurveyCompleted(const FHardwareSurveyResults& SurveyResults)
{
	UE_LOG(LogAutoDetect, Log, TEXT("Hardware survey completed"));

	// 100: avg good CPU/GPU, <100:slower, >100:faster
	TArray<float> CPUPerfIndexResults, GPUPerfIndexResults;
	float CPUPerfIndex = SurveyResults.SynthBenchmark.ComputeCPUPerfIndex(&CPUPerfIndexResults);
	float GPUPerfIndex = SurveyResults.SynthBenchmark.ComputeGPUPerfIndex(&GPUPerfIndexResults);

	GConfig->SetString(TEXT("SynthBenchmark"), TEXT("CPU Perf Index"), *FString::SanitizeFloat(CPUPerfIndex), GScalabilityIni);
	GConfig->SetString(TEXT("SynthBenchmark"), TEXT("GPU Perf Index"), *FString::SanitizeFloat(GPUPerfIndex), GScalabilityIni);

	auto SetLowPerfGraphicsSettings = [&](APlayerControllerBase* PlayerControllerBase)
	{
		if (GEngine)
		{
			if (UGameUserSettings* gUserSettings = GEngine->GetGameUserSettings())
			{
				gUserSettings->SetFrameRateLimit(30.f);
				gUserSettings->SetFullscreenMode(EWindowMode::Fullscreen);
				gUserSettings->SetScreenResolution(FIntPoint(1280, 720));
				gUserSettings->ApplySettings(false);
				gUserSettings->ConfirmVideoMode();
				gUserSettings->SaveSettings();
				// This should execute after the PC settings init in GameSettingsFunctionLibrary is called due to hardware survey tick,
				// however, just to ensure the internal save data matches what we're doing save the setting here.
				USettingsBlueprintFunctionLibrary::SetSettingByName("EFPSLimit", 0, PlayerControllerBase);
			}
		}
		UGameSettingsFunctionLibrary::SetGraphicsPreset(0, PlayerControllerBase);
	};

	auto SendUserNotifyMessage = [&](const FText& MessageBody)
	{
		if (GEngine)
		{
			if (UDungeonsGameInstance* dgi = Cast<UDungeonsGameInstance>(GEngine->GameViewport->GetGameInstance()))
			{
				dgi->NotifyUserOfLowPerformanceDetected(MessageBody);
			}
		}
	};

	auto HasLowVideoMemory = [&](const FHardwareSurveyResults& SurveyResults) -> bool
	{
		if (IsRHIDeviceIntel())		// Fairly certain an Intel integrated graphics processor will be given at least 2GB of shared memory, so this could simply return false without further checks.
		{
			FTextureMemoryStats TextureMemStats;		// TotalGraphicsMemory could be a better option to check against here, would need to test using a system running on an integrated graphics processor.s
			RHIGetTextureMemoryStats(TextureMemStats);
			return ((TextureMemStats.DedicatedVideoMemory + TextureMemStats.SharedSystemMemory) / (1024 * 1024)) < LOW_VIDEO_MEMORY_THRESHOLD_MB;
		}
		else
		{
			return FCString::Atoi(SurveyResults.RHIAdapter.AdapterDedicatedMemoryMB) < LOW_VIDEO_MEMORY_THRESHOLD_MB;
		}
	};

	auto HasAcOnline = []() -> bool
	{
#if PLATFORM_WINDOWS
		SYSTEM_BATTERY_STATE BatteryState;
		NTSTATUS NTStatus = CallNtPowerInformation(SystemBatteryState, NULL, 0L, (PVOID)&BatteryState, sizeof(SYSTEM_BATTERY_STATE));
		if (NT_SUCCESS(NTStatus))
		{
			return BatteryState.AcOnLine;
		}
#endif
		UE_LOG(LogAutoDetect, Log, TEXT("Failed to retrieve battery state information, assuming we have AC power"));
		return true;
	};

	// Feedback from auto-detect to end-user
	if (SurveyResults.bIsLaptopComputer && !HasAcOnline())
	{
		UE_LOG(LogAutoDetect, Log, TEXT("Laptop detected, applying minimum settings to conserve power"));
		SetLowPerfGraphicsSettings(mPlayerControllerBase);
		SendUserNotifyMessage(util::autodetect::GetNotifyMessage(ELowSpecNotifyType::OnBatteryPower));
	}
	else if (CPUPerfIndex < LOW_PERFORMANCE_THRESHOLD || GPUPerfIndex < LOW_PERFORMANCE_THRESHOLD)
	{
		UE_LOG(LogAutoDetect, Log, TEXT("Low CPU/GPU performance detected, applying minimum settings"));
		SetLowPerfGraphicsSettings(mPlayerControllerBase);
		SendUserNotifyMessage(util::autodetect::GetNotifyMessage(ELowSpecNotifyType::LowPerformance));
	}
	else if (HasLowVideoMemory(SurveyResults))
	{
		UE_LOG(LogAutoDetect, Log, TEXT("Low video memory detected, applying minimum settings"));
		SetLowPerfGraphicsSettings(mPlayerControllerBase);
		SendUserNotifyMessage(FText::Format(util::autodetect::GetNotifyMessage(ELowSpecNotifyType::LowMemory), LOW_VIDEO_MEMORY_THRESHOLD_MB / 1024));
	}
}
