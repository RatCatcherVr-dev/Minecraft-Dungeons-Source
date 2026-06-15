#include "Dungeons.h"
#include "DungeonsUserManagement.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "game/LobbyBP.h"
#include "online/ui/OnlineTextLabels.h"
#include "ui/dialog/UserFeedbackTextLabels.h"
#include "ui/EnumDefine.h"
#include "util/EnumUtil.h"

namespace DungeonsQA {
	
	TAutoConsoleVariable<int32> CVInfiniteLife(TEXT("Dungeons.QA.InfiniteLife"), 0, TEXT("Save the cheerleader, save the world..."), ECVF_Cheat);

	TAutoConsoleVariable<int32> CVImmortal(TEXT("Dungeons.QA.Immortal"), 0, TEXT("There can be only one!"), ECVF_Cheat);

	TAutoConsoleVariable<int32> CVAllLootChests(TEXT("Dungeons.QA.AllLootChests"), 0, TEXT("Place loot chests in all loot regions"), ECVF_Cheat);

	TAutoConsoleVariable<int32> CVShowMobAudioTriggers(TEXT("Dungeons.QA.ShowMobAudioTriggers"), 0, TEXT("Show (on/off) spheres around mobs with audio triggers."), ECVF_Cheat);

	TAutoConsoleVariable<int32> CVShowVersionString(TEXT("Dungeons.QA.ShowVersionString"), 1, TEXT("Show (on/off) version string in Menu"), ECVF_Cheat);

	void TriggerAchievement(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
#if !UE_BUILD_SHIPPING
		if (commands.Num() <= 0) {
			out.Log(TEXT("ERROR: need at least one argument (the Achievement message)"));
			return;
		}

		if (auto* pc = Cast<ABasePlayerController>(UDungeonsUserManager::Instance()->GetInitialPlayerController())) {
			if (auto* character = pc->GetControlledPlayerCharacter()) {
				if (auto* trackerComp = character->GetStatTracker()) {
					// or just get the int?
					if(commands[0] == "All")
					{
						for (uint64 i = static_cast<uint64>(EAchievement::REVIVE_20); i < static_cast<uint64>(EAchievement::ENUM_END); i++) {
							trackerComp->WriteAchievements(EAchievement(i), 100.0f);
						}
					}
					else
					{
						EAchievement achievementId = EnumValueFromString(EAchievement, commands[0]).Get(EAchievement::REVIVE_20);
						trackerComp->WriteAchievements(achievementId, 100.0f);
					}
				}
			}
		}


#endif
	}
};

static TOptional<FText> mapToMessage(FString enumString) {
	if (auto loginResult = EnumValueFromString(ELoginResult, enumString))
	{
		return TOptional<FText>(UOnlineTextLabels::GetSignInError(loginResult.GetValue()));
	}
	else if (auto blockingMessage = EnumValueFromString(EBlockingMessageType, enumString))
	{
		return TOptional<FText>(UOnlineTextLabels::GetAuthenticationErrorMessage(blockingMessage.GetValue()));
	}

	// couple non-enum Id to callback, since no enums are used here at the moment:
	if (enumString == "controllerDisconnected") 
	{
		return TOptional<FText>(UUserFeedbackTextLabels::GetControllerDisconnectedMessage().Body);
	};

	return TOptional<FText>();
}

void TriggerAnnounce(const TArray<FString>& commands, UWorld* world, FOutputDevice& out)
{
#if !UE_BUILD_SHIPPING
	if (auto* lobbyBP = actorquery::getFirstActor<ALobbyBP>(world))
	{
		EEndGameContentType endGameContentType = EEndGameContentType::Invalid;
		EGameDifficulty gameDifficulty = EGameDifficulty::Invalid;

		for (FString arg : commands) {
			if (auto endgamecontent = GetEnumValueFromStringT<EEndGameContentType>(arg)) {
				endGameContentType = endgamecontent.GetValue();
			}
			if (auto gamedifficulty = GetEnumValueFromStringT<EGameDifficulty>(arg)) {
				gameDifficulty = gamedifficulty.GetValue();
			}
		}

		lobbyBP->OnTriggerAnnouncement.Broadcast(gameDifficulty, endGameContentType);
	}

#endif
}

static FAutoConsoleCommand DebugTriggerAnnounce(
	TEXT("Dungeons.QA.Announce"),
	TEXT("Trigger a popup window (via LobbyBP), {0} interprets to enum which decides the popup widget to display (add to WidgetQueue). If you specify 2 args, each will try to cast to either EEndGameContentType or EGameDifficulty") TEXT("\nUsage: Dungeons.QA.Announce [0]"),
	FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&TriggerAnnounce),
	ECVF_Cheat
);

void TriggerPopup(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
#if !UE_BUILD_SHIPPING

	EPopupDialogType popupType = EPopupDialogType::OK;
	if (auto* pc = Cast<ABasePlayerController>(UDungeonsUserManager::Instance()->GetInitialPlayerController()))
	{
		if (commands.Num() == 2) {
			// get popup 'type'
			popupType = EnumValueFromString(EPopupDialogType, commands[0]).Get(EPopupDialogType::OK);
		}

		if (commands.Num() > 0) {
			auto message = mapToMessage(commands[0]);
			if (!message.IsSet()) {
				if (auto globalMessageEnum = EnumValueFromString(EGlobalMessageTypes, commands[0]))
				{
					pc->OnTriggerPopupGlobal.Broadcast(popupType, globalMessageEnum.Get(EGlobalMessageTypes::Unset));
				}				
			}
			else 
			{
				pc->OnTriggerPopup.Broadcast(popupType, message.GetValue());
			}

		}
		

		out.Log(TEXT("call DebugPopup command with 1st param === "));
	}
#endif
}

static FAutoConsoleCommand DebugTriggerPopup(
	TEXT("Dungeons.QA.Popup"),
	TEXT("Trigger a popup window (via BP_PlayerController), {0} is an integer or string matching the set in UOnlineTextLabels::GetSignInError().") TEXT("\nUsage: Dungeons.QA.Popup [0]"),
	FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&TriggerPopup),
	ECVF_Cheat
);

void TriggerAchievementUI(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
#if !UE_BUILD_SHIPPING
	if (commands.Num() <= 0) {
		out.Log(TEXT("ERROR: need at least one argument (the Achievement message)"));
		return;
	}
	
	// get EAchievement enum
	//EAchievement achievementId = EnumValueFromString(EAchievement, commands[0]).Get(EAchievement::REVIVE_20);
	//
	//if (auto* gi = Cast<UDungeonsGameInstance>(world->GetGameInstance())) {
	//	if (auto pc = gi->GetFirstLocalPlayerController())
	//		pc->OnTriggerAchievementUI.Broadcast(FText::FromString(commands[0]));
	//}

	//out.Log(TEXT("call DebugAchivement-command with 1st param === "));
#endif
}

// add actual achievement JSON parsed data
static FAutoConsoleCommand DebugTriggerAchievementUI(
	TEXT("Dungeons.QA.AchievementPopup"),
	TEXT("Trigger achivement popup with [optional] custom string.") TEXT("\nUsage: Dungeons.QA.AchievementPopup foo"),
	FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&TriggerAchievementUI),
	ECVF_Cheat
);

// add actual achievement JSON parsed data
static FAutoConsoleCommand DebugGainAchivement(
	TEXT("Dungeons.QA.Achievement"),
	TEXT("Trigger achivement popup with [optional] custom string.") TEXT("\nUsage: Dungeons.QA.Achievement foo"),
	FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DungeonsQA::TriggerAchievement),
	ECVF_Cheat
);
