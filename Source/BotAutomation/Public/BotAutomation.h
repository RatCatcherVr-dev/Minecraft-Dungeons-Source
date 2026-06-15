#pragma once

#include "PlayerAutomator.h"
#include "GameFramework/Actor.h"
#include "game/team/TeamName.h"

enum class EPlayerOnFunctionType;

class BOTAUTOMATION_API BotAutomation
{
public:
	DECLARE_MULTICAST_DELEGATE_OneParam(FBindableStaticEvent_AutomationFinished, class APlayerAutomator*);
	DECLARE_MULTICAST_DELEGATE_OneParam(FBindableStaticEvent_AutomationPaused, class APlayerAutomator*);
	DECLARE_MULTICAST_DELEGATE_OneParam(FBindableStaticEvent_AutomationResumed, class APlayerAutomator*);
	DECLARE_MULTICAST_DELEGATE_TwoParams(FBindableStaticEvent_ActionStarted, class APlayerAutomator*, class PlayerBotActionBase* const);
	DECLARE_MULTICAST_DELEGATE_ThreeParams(FBindableStaticEvent_ActionFinished, class APlayerAutomator*, class PlayerBotActionBase* const, EPlayerBotActionResult);

	static FBindableStaticEvent_AutomationFinished AutomationFinishedEvent;
	static FBindableStaticEvent_AutomationPaused AutomationPausedEvent;
	static FBindableStaticEvent_AutomationResumed AutomationResumedEvent;
	static FBindableStaticEvent_ActionStarted ActionStartedEvent;
	static FBindableStaticEvent_ActionFinished ActionFinishedEvent;

	static void AddAutomatorToPlayer(int PlayerIndex, ULocalPlayer* Player);
	static void RemoveAutomatorFromPlayer(int PlayerIndex);
	static void ClearActions(int PlayerIndex);

	static bool AddPlayerKeyPressActionToBot(int PlayerIndex, ULocalPlayer* Player, FString Name, FKey Key, bool Viewport);
	static bool AddPlayerOnFunctionActionToBot(int PlayerIndex, ULocalPlayer* Player, FString Name, EPlayerOnFunctionType Action, int Index = 0);
	static bool AddPlayerMoveToLocationBot(int PlayerIndex, ULocalPlayer* Player, FString Name, FVector Location, bool UsePathFinding, float AcceptanceRadius);
	static bool AddPlayerMoveToActorBot(int PlayerIndex, ULocalPlayer* Player, FString Name, AActor* Target, bool UsePathFinding, float AcceptanceRadius);
	static bool AddPlayerAttackMobBot(int PlayerIndex, ULocalPlayer* Player, FString Name, ABaseCharacter* Target, bool AllowRanged);
	static bool AddPlayerAttackRadiusBot(int PlayerIndex, ULocalPlayer* Player, FString Name, FVector Location, ETeamName AttackableTeam, float AttackRadius, bool AllowRanged);

#if !UE_BUILD_SHIPPING
	static TAutoConsoleVariable<float> CVarControllerInputTimeout;
#endif

private:
	static bool AddActionToBot(int PlayerIndex, ULocalPlayer* Player, std::unique_ptr<PlayerBotActionBase> Action);

	static TMap<int, TWeakObjectPtr<class APlayerAutomator>> PlayerAutomators;
};