#include "BotAutomationPCH.h"

#include "BotAutomation.h"

#include "Dungeons.h"
#include "Engine/LocalPlayer.h"
#include "game/actor/character/player/PlayerControllerBase.h"

#include "Actions/PlayerBotActionBase.h"
#include "Actions/PlayerKeyPressAction.h"
#include "Actions/PlayerOnFunctionAction.h"
#include "Actions/PlayerMoveToLocationAction.h"
#include "Actions/PlayerMoveToActorAction.h"
#include "Actions/PlayerAttackMobAction.h"
#include "Actions/PlayerAttackRadiusAction.h"

BotAutomation::FBindableStaticEvent_AutomationFinished BotAutomation::AutomationFinishedEvent;
BotAutomation::FBindableStaticEvent_AutomationPaused BotAutomation::AutomationPausedEvent;
BotAutomation::FBindableStaticEvent_AutomationResumed BotAutomation::AutomationResumedEvent;
BotAutomation::FBindableStaticEvent_ActionStarted BotAutomation::ActionStartedEvent;
BotAutomation::FBindableStaticEvent_ActionFinished BotAutomation::ActionFinishedEvent;

#if !UE_BUILD_SHIPPING

TAutoConsoleVariable<float> BotAutomation::CVarControllerInputTimeout(
	TEXT("AutomatedBotControllerInputTimeout"),
	-1.0f,
	TEXT("When enabled, the bot will pause when a controller is used and resume after use ceases for the given number of seconds."),
	ECVF_Default);

#endif

TMap<int, TWeakObjectPtr<APlayerAutomator>> BotAutomation::PlayerAutomators;

void BotAutomation::AddAutomatorToPlayer(int PlayerIndex, ULocalPlayer* Player)
{
	APlayerControllerBase* Controller = Cast<APlayerControllerBase>(Player->PlayerController);
	if (Controller && !Controller->InputAutomator.IsValid())
	{
		TWeakObjectPtr<APlayerAutomator> PlayerAutomator;
		if (PlayerAutomators.Contains(PlayerIndex)) {
			PlayerAutomator = *PlayerAutomators.Find(PlayerIndex);
		}

		if (!PlayerAutomator.IsValid())
		{
			PlayerAutomator = Player->GetWorld()->SpawnActor<APlayerAutomator>();
			PlayerAutomator->AttachController(Controller);
			PlayerAutomators.Add(PlayerIndex, PlayerAutomator);
		}

		PlayerAutomator->AttachController(Controller);
	}
}

void BotAutomation::RemoveAutomatorFromPlayer(int PlayerIndex)
{
	auto automator = PlayerAutomators.Find(PlayerIndex);
	if (automator && automator->IsValid()) {
		(*automator)->StopAutomation();
	}
}

void BotAutomation::ClearActions(int PlayerIndex)
{
	auto automator = PlayerAutomators.Find(PlayerIndex);
	if (automator && automator->IsValid()) {
		(*automator)->RemoveAction();
	}
}

bool BotAutomation::AddPlayerKeyPressActionToBot(int PlayerIndex, ULocalPlayer* Player, FString Name, FKey Key, bool Viewport)
{
	return AddActionToBot(PlayerIndex, Player, std::make_unique<PlayerKeyPressAction>(Name, Key, Viewport));
}

bool BotAutomation::AddPlayerOnFunctionActionToBot(int PlayerIndex, ULocalPlayer* Player, FString Name, EPlayerOnFunctionType Action, int Index)
{
	return AddActionToBot(PlayerIndex, Player, std::make_unique<PlayerOnFunctionAction>(Name, Action, Index));
}

bool BotAutomation::AddPlayerMoveToLocationBot(int PlayerIndex, ULocalPlayer* Player, FString Name, FVector Location, bool UsePathFinding, float AcceptanceRadius)
{
	return AddActionToBot(PlayerIndex, Player, std::make_unique<PlayerMoveToLocationAction>(Name, Location, UsePathFinding, AcceptanceRadius));
}

bool BotAutomation::AddPlayerMoveToActorBot(int PlayerIndex, ULocalPlayer* Player, FString Name, AActor* Target, bool UsePathFinding, float AcceptanceRadius)
{
	return AddActionToBot(PlayerIndex, Player, std::make_unique<PlayerMoveToActorAction>(Name, Target, UsePathFinding, AcceptanceRadius));
}

bool BotAutomation::AddPlayerAttackMobBot(int PlayerIndex, ULocalPlayer* Player, FString Name, ABaseCharacter* Target, bool AllowRanged)
{
	return AddActionToBot(PlayerIndex, Player, std::make_unique<PlayerAttackMobAction>(Name, Target, AllowRanged));
}

bool BotAutomation::AddPlayerAttackRadiusBot(int PlayerIndex, ULocalPlayer* Player, FString Name, FVector Location, ETeamName AttackableTeam, float AttackRadius, bool AllowRanged)
{
	const auto world = Player->GetWorld();

	const FVector locationAbove{ Location.X, Location.Y, Location.Z + 1000.f };
	const FVector locationUnder{ Location.X, Location.Y, Location.Z - 1000.f };

	FHitResult hitResult;
	const bool hitSuccess = world->LineTraceSingleByChannel(hitResult, locationAbove, locationUnder, static_cast<ECollisionChannel>(ECustomTraceChannels::TerrainOnly));

	if (hitSuccess) {
		Location = hitResult.Location;
	}

	return AddActionToBot(PlayerIndex, Player, std::make_unique<PlayerAttackRadiusAction>(Name, Location, AttackableTeam, AttackRadius, AllowRanged));
}

bool BotAutomation::AddActionToBot(int PlayerIndex, ULocalPlayer* Player, std::unique_ptr<PlayerBotActionBase> Action)
{
	auto playerAutomator = PlayerAutomators.Find(PlayerIndex);
	if (playerAutomator == nullptr) {
		AddAutomatorToPlayer(PlayerIndex, Player);
		playerAutomator = PlayerAutomators.Find(PlayerIndex);
	}

	if (playerAutomator)
	{
		if (!playerAutomator->IsValid()) {
			AddAutomatorToPlayer(PlayerIndex, Player);
			playerAutomator = PlayerAutomators.Find(PlayerIndex);
		}

		if (playerAutomator->IsValid())
		{
			(*playerAutomator)->RemoveAction();
			(*playerAutomator)->AddAction(std::move(Action));

			return true;
		}
	}

	return false;
}