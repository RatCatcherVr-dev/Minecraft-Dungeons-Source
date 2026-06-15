#include "Dungeons.h"
#include "DungeonsGameInstance.h"

//D11.PS - PLATFORM_WINDOWS is defined on XB1 so need to be more specific.
#if (!PLATFORM_PS4 && !PLATFORM_SWITCH) && (PLATFORM_XBOXONE || PLATFORM_WINDOWS) 
#include "UnrealToolsFrameworkPCH.h"
#include "UnrealEngineHelperFunctions.h"
#endif

#ifdef ENABLE_GAMESTEST_RPC

#include "RpcArchive.h"
#include "RpcArchiveExtensions.h"
#include "RpcErrorCodes.h"

#include "BotAutomationPCH.h"

#ifdef ENABLE_BOTAUTOMATION

Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive CreateArchive(APlayerAutomator* Automator)
{
	Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive archive = Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive();
	FString Name = Automator->GetName();
	archive.SetNamedStringValue(TEXT("InputAutomatorId"), *Name);

	if (APlayerControllerBase* Controller = Automator->GetAttachedController()) {
		archive.SetNamedValue(TEXT("PlayerIndex"), Controller->NetPlayerIndex);
	}

	return archive;
}

HRESULT BotRaiseGameEvent(const wchar_t* EventName, const Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive& EventArgs)
{
	HRESULT Result = Microsoft::Internal::GamesTest::UnrealToolsFramework::FUTFHookManager::RaiseGameEvent(EventName, EventArgs);
	if (FAILED(Result))
	{
		UE_LOG(LogUToolsFramework, Warning, TEXT("Failed to raise %s event. Error: %X"), EventName, Result);
	}
	return Result;
}

void BotAutomationFinishedEvent(APlayerAutomator* Automator) {
	Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive archive = CreateArchive(Automator);
	BotRaiseGameEvent(TEXT("BotAutomationFinished"), archive);
}

void BotAutomationPausedEvent(APlayerAutomator* Automator) {
	Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive archive = CreateArchive(Automator);
	BotRaiseGameEvent(TEXT("BotAutomationPaused"), archive);
}

void BotAutomationResumedEvent(APlayerAutomator* Automator) {
	Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive archive = CreateArchive(Automator);
	BotRaiseGameEvent(TEXT("BotAutomationResumed"), archive);
}

void BotActionStartedEvent(APlayerAutomator* Automator, PlayerBotActionBase* const Action) {
	Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive archive = CreateArchive(Automator);
	archive.SetNamedStringValue(TEXT("ActionName"), *Action->UserFriendlyName());
	archive.SetNamedValue(TEXT("ActionResult"), (int)Action->GetStatus());

	BotRaiseGameEvent(TEXT("BotActionStarted"), archive);
}

void BotActionFinishedEvent(APlayerAutomator* Automator, PlayerBotActionBase* const Action, EPlayerBotActionResult ActionResult) {
	Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive archive = CreateArchive(Automator);
	archive.SetNamedStringValue(TEXT("ActionName"), *Action->UserFriendlyName());
	archive.SetNamedValue(TEXT("ActionResult"), (int)ActionResult);

	BotRaiseGameEvent(TEXT("BotActionFinished"), archive);
}

#endif // ENABLE_BOTAUTOMATION

void RegisterGameEvents() {
#ifdef ENABLE_BOTAUTOMATION
	BotAutomation::AutomationFinishedEvent.AddStatic(&BotAutomationFinishedEvent);
	BotAutomation::AutomationPausedEvent.AddStatic(&BotAutomationPausedEvent);
	BotAutomation::AutomationResumedEvent.AddStatic(&BotAutomationResumedEvent);

	BotAutomation::ActionStartedEvent.AddStatic(&BotActionStartedEvent);
	BotAutomation::ActionFinishedEvent.AddStatic(&BotActionFinishedEvent);
#endif // ENABLE_BOTAUTOMATION
}

#endif // ENABLE_GAMESTEST_RPC