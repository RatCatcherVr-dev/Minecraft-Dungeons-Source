#include "BotAutomationModule.h"

#define LOCTEXT_NAMESPACE "BotAutomation"

/** Called right after the module DLL has been loaded and the module object has been created */
void FBotAutomationModule::StartupModule()
{
}

/** Called before the module is unloaded, right before the module object is destroyed. */
void FBotAutomationModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE

/** Register module (Module Main Class, Module Name) */
IMPLEMENT_GAME_MODULE(FBotAutomationModule, BotAutomation);
