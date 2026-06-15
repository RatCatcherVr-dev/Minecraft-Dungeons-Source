#include "Dungeons.h"
#include "DungeonsLocalPlayer.h"
#include "game/component/ReconnectComponent.h"
#include "DungeonsGameInstance.h"
#include "save/GlobalSaveData.h"
#include "game/component/CharacterSerializeComponent.h"
#include "online/reconnect/ReconnectUtil.h"

UDungeonsLocalPlayer::UDungeonsLocalPlayer(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	
}

FString UDungeonsLocalPlayer::GetGameLoginOptions() const {
	UE_LOG(LogTemp, Log, TEXT("GetGameLoginOptions"));
	if (UDungeonsGameInstance* gameInstance = Cast<UDungeonsGameInstance>(GetWorld()->GetGameInstance())) {
		if (!gameInstance->IsDedicatedServerInstance()) {
			if (const auto* playerController = GetWorld()->GetFirstPlayerController<APlayerController>()) {
				const FString guid = reconnect::getGuid(playerController);
				const FString invitedSession = gameInstance->GetAcceptedInvite();

				UE_LOG(LogTemp, Log, TEXT("GetGameLoginOptions appending guid %s to connection string."), *guid);
				ELevelNames levelName = gameInstance->Configuration.GetLevelName();
				return FString("guid=" + guid + "?inviteId=" + invitedSession + "?level=" + GetEnumValueToStringStripped(levelName));
			}
		}
	}
	return "";
}

void UDungeonsLocalPlayer::RefreshDeadzones(ABasePlayerController* BasePlayerController)
{
	if (UPlayerInput* PlayerInput = BasePlayerController->PlayerInput)
	{
		static const TArray<FKey> gamepadTriggerAxes = {
			EKeys::Gamepad_LeftTriggerAxis,
			EKeys::Gamepad_RightTriggerAxis,
		};

		static const TArray<FKey> gamepadLStickAxes = {
			EKeys::Gamepad_LeftX,
			EKeys::Gamepad_LeftY,
		};

		static const TArray<FKey> gamepadRStickAxes = {
			EKeys::Gamepad_RightX,
			EKeys::Gamepad_RightY,
		};

		auto SetDeadzone = [&](const FKey& axis, const float deadzone) {
			FInputAxisProperties properties;
			if (PlayerInput->GetAxisProperties(axis, properties))
			{
				properties.DeadZone = deadzone;
				PlayerInput->SetAxisProperties(axis, properties);
			}
		};

		for (auto& triggerAxis : gamepadTriggerAxes)
		{
			SetDeadzone(triggerAxis, BasePlayerController->IsInputCapturedByUI() ? DeadzoneUI : DeadzoneTriggers);
		}

		for (auto& lStickAxis : gamepadLStickAxes)
		{
			SetDeadzone(lStickAxis, BasePlayerController->IsInputCapturedByUI() ? DeadzoneUI : DeadzoneIngameLStick);
		}

		for (auto& rStickAxis : gamepadRStickAxes)
		{
			SetDeadzone(rStickAxis, BasePlayerController->IsInputCapturedByUI() ? DeadzoneUI : DeadzoneIngameRStick);
		}
	}
}

int32 UDungeonsLocalPlayer::GetSystemUserId()
{
	return UserSystemId;
}

void UDungeonsLocalPlayer::SetUserSystemId(int32 SystemId)
{
	UserSystemId = SystemId;
}
