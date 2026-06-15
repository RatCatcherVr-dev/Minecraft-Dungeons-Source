#if !UE_BUILD_SHIPPING && (PLATFORM_PS4 || PLATFORM_SWITCH)
#include "DungeonsGameInstance.h"
#include "Containers/UnrealString.h"
#include "DungeonsLoginFlow.h"
#include "game/util/ConsoleCommandHelpers.h"
#include "game/actor/character/player/PlayerControllerBase.h"
#include "online/sessions/OnlineUtil.h"

namespace loginflowcommands 
{
	UDungeonsLoginFlow* GetDungeonsLoginFlow(UWorld* world) {
		auto* dungeonsGameInstance = Cast<UDungeonsGameInstance>(world->GetGameInstance());
		return dungeonsGameInstance->GetLoginFlow();
	}

	void SetLoginResult(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
		if (!world)
			return;
		ELoginResult loginResult = ELoginResult::Success;
		if (auto optional = ArgAsEnum<ELoginResult>(commands, 0))
			loginResult = optional.GetValue();
		else {
			out.Log(ELogVerbosity::Error, TEXT("Dungeons.LoginFlow.SetLoginResult: failed to parse the LoginResult"));
			return;
		}
		auto loginFlow = GetDungeonsLoginFlow(world);
		loginFlow->SetDebugLoginResult(loginResult);
	}

	void ClearLoginResult(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
		if (!world)
			return;
		auto loginFlow = GetDungeonsLoginFlow(world);
		loginFlow->ClearDebugLoginResult();
	}

	void SetXblActive(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
		if (!world)
			return;
		bool activate = false;
		if (auto optional = ArgAsInt(commands, 0)) {
			int argument = optional.GetValue();
			activate = argument == 1;
		}
		else {
			out.Log(ELogVerbosity::Error, TEXT("Dungeons.LoginFlow.SetXblActive: failed to parse the first argument"));
			return;
		}
		auto pc = Cast<APlayerControllerBase>(world->GetFirstPlayerController());
		online::SetXblActive(activate, pc);

	}

	FAutoConsoleCommand SetLoginResultCommand(
		TEXT("Dungeons.LoginFlow.SetLoginResult"),
		TEXT("Simulates a login result on the Title screen / Configure Online Play menu") TEXT("\nUsage: Dungeons.LoginFlow.SetLoginResult <result>"),
		FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&SetLoginResult),
		ECVF_Cheat);

	FAutoConsoleCommand ClearLoginResultCommand(
		TEXT("Dungeons.LoginFlow.ClearLoginResult"),
		TEXT("Clears the debug login result") TEXT("\nUsage: Dungeons.LoginFlow.ClearLoginResult"),
		FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&ClearLoginResult),
		ECVF_Cheat);

	FAutoConsoleCommand SetXBLActive(
		TEXT("Dungeons.LoginFlow.SetXblActive"),
		TEXT("Sets Xbl Active, use to simulate account linking during the normal login flow") TEXT("\nUsage: Dungeons.LoginFlow.SetXblActive {0, 1}"),
		FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&SetXblActive),
		ECVF_Cheat);
}
#endif