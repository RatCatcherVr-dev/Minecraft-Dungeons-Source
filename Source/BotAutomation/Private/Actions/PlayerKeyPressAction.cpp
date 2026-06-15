#include "PlayerKeyPressAction.h"

#include "DungeonsGameInstance.h"

#include "game/actor/character/player/PlayerCharacter.h"
#include "game/actor/character/player/PlayerControllerBase.h"

bool PlayerKeyPressAction::ExecuteAction(float DeltaSeconds)
{
	if (auto Controller = GetPlayerControllerBase())
	{
		if (const auto LocalPlayer = Controller->GetLocalPlayer()) {
			if (const auto GameInstance = Controller->GetGameInstance<UDungeonsGameInstance>()) {
				if (!Pressed) {
					if (auto ControllerTypeManager = GameInstance->GetControllerTypeManager()) {
						ControllerTypeManager->RawInputKeyPressed(LocalPlayer->GetControllerId(), CurrentKey);
					}
				}

				if (Viewport) {
					if (auto ViewportClinet = Cast<UDungeonsGameViewportClient>(GameInstance->GetGameViewportClient())) {
						ViewportClinet->RawInputKey(Pressed, LocalPlayer->GetControllerId(), CurrentKey);
						Pressed = !Pressed;
						if (!Pressed) {
							EndAction(EPlayerBotActionResult::Success);
						}
						return Pressed;
					}
				}
			}
		}

		if (!Viewport) {
			const auto KeyAmount = Controller->GetInputAnalogKeyState(CurrentKey);

			if (KeyAmount == 0.0 || !Pressed)
			{
				Controller->InputKey(CurrentKey, EInputEvent::IE_Pressed, 1, CurrentKey.IsGamepadKey());
				Pressed = true;
			}
			else if (KeyAmount > 0.0 || Pressed)
			{
				Controller->InputKey(CurrentKey, EInputEvent::IE_Released, 0, CurrentKey.IsGamepadKey());
				Pressed = false;

				EndAction(EPlayerBotActionResult::Success);
			}
			return Pressed;
		}
	}

	EndAction(EPlayerBotActionResult::Error);
	return false;
}
