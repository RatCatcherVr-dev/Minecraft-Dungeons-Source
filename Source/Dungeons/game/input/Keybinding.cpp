#include "KeyBinding.h"

#include "DungeonsGameInstance.h"
#include "../actor/character/player/DungeonsLocalPlayer.h"
#include "DungeonsUserManagement.h"

const TArray<FKeybinding>& GamepadConfig()
{
#if PLATFORM_SWITCH 

	// D11.SSN - swap attack/item inputs if necessary
	static const bool swap = UInputSettings::GetInputSettings()->GetPlatformUsesRightForAccept();
	if (swap) 
	{
		static const TArray<FKeybinding> swapped_list = {
		 FKeybinding("Use Item Slot 1", EKeys::Gamepad_FaceButton_Left, EKeybindPlatform::Gamepad)
		,FKeybinding("Use Item Slot 2", EKeys::Gamepad_FaceButton_Top, EKeybindPlatform::Gamepad)
		,FKeybinding("Use Potion Slot", EKeys::Gamepad_LeftShoulder, EKeybindPlatform::Gamepad)
		,FKeybinding("DodgeForward", EKeys::Gamepad_RightShoulder, EKeybindPlatform::Gamepad)
		,FKeybinding("Ender Pearl", EKeys::Gamepad_RightThumbstick, EKeybindPlatform::Gamepad)
		,FKeybinding("Pause", EKeys::Gamepad_Special_Right, EKeybindPlatform::Gamepad)
		,FKeybinding("Friends", EKeys::Gamepad_Special_Left, EKeybindPlatform::Gamepad)
		,FKeybinding("ToggleMap", EKeys::Gamepad_DPad_Down, EKeybindPlatform::Gamepad)
		,FKeybinding("Inventory", EKeys::Gamepad_DPad_Up, EKeybindPlatform::Gamepad)
		,FKeybinding("Chat", EKeys::Gamepad_LeftTrigger, EKeybindPlatform::Gamepad)
		,FKeybinding("MainAttackGamepad", EKeys::Gamepad_FaceButton_Right, EKeybindPlatform::Gamepad)
		,FKeybinding("Use Item Slot 3", EKeys::Gamepad_FaceButton_Bottom, EKeybindPlatform::Gamepad)			
		,FKeybinding("Shoot", EKeys::Gamepad_RightTrigger, EKeybindPlatform::Gamepad)	// D11.DB - Force digital on switch.
		};

		return swapped_list;
	}
	else
	{
		static const TArray<FKeybinding> normal_list = {
		 FKeybinding("Use Item Slot 1", EKeys::Gamepad_FaceButton_Left, EKeybindPlatform::Gamepad)
		,FKeybinding("Use Item Slot 2", EKeys::Gamepad_FaceButton_Top, EKeybindPlatform::Gamepad)
		,FKeybinding("Use Potion Slot", EKeys::Gamepad_LeftShoulder, EKeybindPlatform::Gamepad)
		,FKeybinding("DodgeForward", EKeys::Gamepad_RightShoulder, EKeybindPlatform::Gamepad)
		,FKeybinding("Ender Pearl", EKeys::Gamepad_RightThumbstick, EKeybindPlatform::Gamepad)
		,FKeybinding("Pause", EKeys::Gamepad_Special_Right, EKeybindPlatform::Gamepad)
		,FKeybinding("Friends", EKeys::Gamepad_Special_Left, EKeybindPlatform::Gamepad)
		,FKeybinding("ToggleMap", EKeys::Gamepad_DPad_Down, EKeybindPlatform::Gamepad)
		,FKeybinding("Inventory", EKeys::Gamepad_DPad_Up, EKeybindPlatform::Gamepad)
		,FKeybinding("Chat", EKeys::Gamepad_LeftTrigger, EKeybindPlatform::Gamepad)
		,FKeybinding("MainAttackGamepad", EKeys::Gamepad_FaceButton_Bottom, EKeybindPlatform::Gamepad)
		,FKeybinding("Use Item Slot 3", EKeys::Gamepad_FaceButton_Right, EKeybindPlatform::Gamepad)
		,FKeybinding("Shoot", EKeys::Gamepad_RightTrigger, EKeybindPlatform::Gamepad)	// D11.DB - Force digital on switch.
		};

		return normal_list;
	}

	
#elif PLATFORM_PS4

	// D11.SSN - swap attack/item inputs if necessary
	static const bool swap = UInputSettings::GetInputSettings()->GetPlatformUsesRightForAccept();
	if (swap)
	{
		static const TArray<FKeybinding> swapped_list = {
		 FKeybinding("Use Item Slot 1", EKeys::Gamepad_FaceButton_Left, EKeybindPlatform::Gamepad)
		,FKeybinding("Use Item Slot 2", EKeys::Gamepad_FaceButton_Top, EKeybindPlatform::Gamepad)
		,FKeybinding("Use Potion Slot", EKeys::Gamepad_LeftShoulder, EKeybindPlatform::Gamepad)
		,FKeybinding("DodgeForward", EKeys::Gamepad_RightShoulder, EKeybindPlatform::Gamepad)
		,FKeybinding("Ender Pearl", EKeys::Gamepad_RightThumbstick, EKeybindPlatform::Gamepad)
		,FKeybinding("Pause", EKeys::Gamepad_Special_Right, EKeybindPlatform::Gamepad)
		,FKeybinding("Friends", EKeys::Gamepad_Special_Left, EKeybindPlatform::Gamepad)
		,FKeybinding("ToggleMap", EKeys::Gamepad_DPad_Down, EKeybindPlatform::Gamepad)
		,FKeybinding("Inventory", EKeys::Gamepad_DPad_Up, EKeybindPlatform::Gamepad)
		,FKeybinding("Chat", EKeys::Gamepad_LeftTrigger, EKeybindPlatform::Gamepad)
		,FKeybinding("MainAttackGamepad", EKeys::Gamepad_FaceButton_Right, EKeybindPlatform::Gamepad)
		,FKeybinding("Use Item Slot 3", EKeys::Gamepad_FaceButton_Bottom, EKeybindPlatform::Gamepad)
		,FKeybinding("Shoot", EKeys::Gamepad_RightTrigger, EKeybindPlatform::Gamepad)
		};

		return swapped_list;
	}
	else
	{
		static const TArray<FKeybinding> normal_list = {
		 FKeybinding("Use Item Slot 1", EKeys::Gamepad_FaceButton_Left, EKeybindPlatform::Gamepad)
		,FKeybinding("Use Item Slot 2", EKeys::Gamepad_FaceButton_Top, EKeybindPlatform::Gamepad)
		,FKeybinding("Use Potion Slot", EKeys::Gamepad_LeftShoulder, EKeybindPlatform::Gamepad)
		,FKeybinding("DodgeForward", EKeys::Gamepad_RightShoulder, EKeybindPlatform::Gamepad)
		,FKeybinding("Ender Pearl", EKeys::Gamepad_RightThumbstick, EKeybindPlatform::Gamepad)
		,FKeybinding("Pause", EKeys::Gamepad_Special_Right, EKeybindPlatform::Gamepad)
		,FKeybinding("Friends", EKeys::Gamepad_Special_Left, EKeybindPlatform::Gamepad)
		,FKeybinding("ToggleMap", EKeys::Gamepad_DPad_Down, EKeybindPlatform::Gamepad)
		,FKeybinding("Inventory", EKeys::Gamepad_DPad_Up, EKeybindPlatform::Gamepad)
		,FKeybinding("Chat", EKeys::Gamepad_LeftTrigger, EKeybindPlatform::Gamepad)
		,FKeybinding("MainAttackGamepad", EKeys::Gamepad_FaceButton_Bottom, EKeybindPlatform::Gamepad)
		,FKeybinding("Use Item Slot 3", EKeys::Gamepad_FaceButton_Right, EKeybindPlatform::Gamepad)
		,FKeybinding("Shoot", EKeys::Gamepad_RightTrigger, EKeybindPlatform::Gamepad)
		};

		return normal_list;
	}


#else

	static const TArray<FKeybinding> normal_list = {
		 FKeybinding("Use Item Slot 1", EKeys::Gamepad_FaceButton_Left, EKeybindPlatform::Gamepad)
		,FKeybinding("Use Item Slot 2", EKeys::Gamepad_FaceButton_Top, EKeybindPlatform::Gamepad)
		,FKeybinding("Use Potion Slot", EKeys::Gamepad_LeftShoulder, EKeybindPlatform::Gamepad)
		,FKeybinding("DodgeForward", EKeys::Gamepad_RightShoulder, EKeybindPlatform::Gamepad)
		,FKeybinding("Ender Pearl", EKeys::Gamepad_RightThumbstick, EKeybindPlatform::Gamepad)
		,FKeybinding("Pause", EKeys::Gamepad_Special_Right, EKeybindPlatform::Gamepad)
		,FKeybinding("Friends", EKeys::Gamepad_Special_Left, EKeybindPlatform::Gamepad)
		,FKeybinding("ToggleMap", EKeys::Gamepad_DPad_Down, EKeybindPlatform::Gamepad)
		,FKeybinding("Inventory", EKeys::Gamepad_DPad_Up, EKeybindPlatform::Gamepad)
		,FKeybinding("Chat", EKeys::Gamepad_LeftTrigger, EKeybindPlatform::Gamepad)
		,FKeybinding("MainAttackGamepad", EKeys::Gamepad_FaceButton_Bottom, EKeybindPlatform::Gamepad)
		,FKeybinding("Use Item Slot 3", EKeys::Gamepad_FaceButton_Right, EKeybindPlatform::Gamepad)
		,FKeybinding("Shoot", EKeys::Gamepad_RightTrigger, EKeybindPlatform::Gamepad)
	};
 
	return normal_list;

#endif
}

const TArray<FKeybinding>& JoyconLConfig()
{
	static const TArray<FKeybinding> sKeyBind_list = {
	 FKeybinding("MainAttackGamepad", EKeys::Gamepad_FaceButton_Right, EKeybindPlatform::JoyconL)
	,FKeybinding("Use Item Slot 1", EKeys::Gamepad_FaceButton_Left, EKeybindPlatform::JoyconL)
	,FKeybinding("Use Item Slot 2", EKeys::Gamepad_FaceButton_Top, EKeybindPlatform::JoyconL)
	,FKeybinding("Use Item Slot 3", EKeys::Gamepad_FaceButton_Bottom, EKeybindPlatform::JoyconL)
	,FKeybinding("Use Potion Slot", EKeys::Gamepad_LeftTrigger, EKeybindPlatform::JoyconL)
	,FKeybinding("Shoot", EKeys::Gamepad_RightTrigger, EKeybindPlatform::JoyconL)
	,FKeybinding("DodgeForward", EKeys::Gamepad_LeftThumbstick, EKeybindPlatform::JoyconL)
	,FKeybinding("QuickActions", EKeys::Gamepad_Special_Right, EKeybindPlatform::JoyconL)
	};
	return sKeyBind_list;
}

const TArray<FKeybinding>& JoyconRConfig()
{
	static const TArray<FKeybinding> sKeyBind_list = { 
	 FKeybinding("MainAttackGamepad", EKeys::Gamepad_FaceButton_Right, EKeybindPlatform::JoyconR)
	,FKeybinding("Use Item Slot 1", EKeys::Gamepad_FaceButton_Left, EKeybindPlatform::JoyconR)
	,FKeybinding("Use Item Slot 2", EKeys::Gamepad_FaceButton_Top, EKeybindPlatform::JoyconR)
	,FKeybinding("Use Item Slot 3", EKeys::Gamepad_FaceButton_Bottom, EKeybindPlatform::JoyconR)
	,FKeybinding("Use Potion Slot", EKeys::Gamepad_LeftTrigger, EKeybindPlatform::JoyconR)
	,FKeybinding("Shoot", EKeys::Gamepad_RightTrigger, EKeybindPlatform::JoyconR)
	,FKeybinding("DodgeForward", EKeys::Gamepad_LeftThumbstick, EKeybindPlatform::JoyconR)
	,FKeybinding("QuickActions", EKeys::Gamepad_Special_Right, EKeybindPlatform::JoyconR)
	};
	return sKeyBind_list;
}

const TArray<FKeybinding>& KeyboardConfig()
{
	static const TArray<FKeybinding> sKeyBind_list = {
	 FKeybinding("MainAttack", EKeys::LeftMouseButton, EKeybindPlatform::KeyboardMouse)
	,FKeybinding("SetDestination", EKeys::LeftMouseButton, EKeybindPlatform::KeyboardMouse)
	,FKeybinding("AlternativeAttack", EKeys::RightMouseButton, EKeybindPlatform::KeyboardMouse)
	,FKeybinding("Use Item Slot 1", EKeys::One, EKeybindPlatform::KeyboardMouse)
	,FKeybinding("Use Item Slot 2", EKeys::Two, EKeybindPlatform::KeyboardMouse)
	,FKeybinding("Use Item Slot 3", EKeys::Three, EKeybindPlatform::KeyboardMouse)
	,FKeybinding("Use Potion Slot", EKeys::E, EKeybindPlatform::KeyboardMouse)
	,FKeybinding("RootPlayer", EKeys::LeftShift, EKeybindPlatform::KeyboardMouse)
	,FKeybinding("DodgeForward", EKeys::SpaceBar, EKeybindPlatform::KeyboardMouse)
	,FKeybinding("Dodge", EKeys::ThumbMouseButton, EKeybindPlatform::KeyboardMouse)

	,FKeybinding("Ender Pearl", EKeys::F1, EKeybindPlatform::KeyboardMouse)
	,FKeybinding("Use Ender Pearl 1", EKeys::F2, EKeybindPlatform::KeyboardMouse)
	,FKeybinding("Use Ender Pearl 2", EKeys::F3, EKeybindPlatform::KeyboardMouse)
	,FKeybinding("Use Ender Pearl 3", EKeys::F4, EKeybindPlatform::KeyboardMouse)
	,FKeybinding("Chat", EKeys::T, EKeybindPlatform::KeyboardMouse)

	,FKeybinding("Pause", EKeys::Escape, EKeybindPlatform::KeyboardMouse)
	,FKeybinding("Friends", EKeys::F, EKeybindPlatform::KeyboardMouse)
	,FKeybinding("ToggleMap", EKeys::M, EKeybindPlatform::KeyboardMouse)
	,FKeybinding("ToggleMapOverlay", EKeys::Tab, EKeybindPlatform::KeyboardMouse)
	,FKeybinding("Inventory", EKeys::I, EKeybindPlatform::KeyboardMouse)

	,FKeybinding("ChatCome", EKeys::R, EKeybindPlatform::KeyboardMouse)
	,FKeybinding("ChatThanks", EKeys::K, EKeybindPlatform::KeyboardMouse)
	,FKeybinding("ChatOK", EKeys::N, EKeybindPlatform::KeyboardMouse)
	,FKeybinding("ChatNeedArrows", EKeys::Y, EKeybindPlatform::KeyboardMouse)
	,FKeybinding("ChatWait", EKeys::U, EKeybindPlatform::KeyboardMouse)
	,FKeybinding("ChatNeedHealth", EKeys::Z, EKeybindPlatform::KeyboardMouse)
	,FKeybinding("ChatNo", EKeys::Q, EKeybindPlatform::KeyboardMouse)
	,FKeybinding("ChatSupplies", EKeys::W, EKeybindPlatform::KeyboardMouse)
	};
	return sKeyBind_list;
}

// D11.SSN
const TArray<FKeybinding>& LeftKeyboardConfig() {
	static const TArray<FKeybinding> sKeyBind_list = {
	 FKeybinding("MainAttack", EKeys::LeftMouseButton, EKeybindPlatform::KeyboardMouse)
	,FKeybinding("SetDestination", EKeys::LeftMouseButton, EKeybindPlatform::KeyboardMouse)
	,FKeybinding("AlternativeAttack", EKeys::RightMouseButton, EKeybindPlatform::KeyboardMouse)
	,FKeybinding("Use Item Slot 1", EKeys::Eight, EKeybindPlatform::KeyboardMouse)
	,FKeybinding("Use Item Slot 2", EKeys::Nine, EKeybindPlatform::KeyboardMouse)
	,FKeybinding("Use Item Slot 3", EKeys::Zero, EKeybindPlatform::KeyboardMouse)
	,FKeybinding("Use Potion Slot", EKeys::H, EKeybindPlatform::KeyboardMouse)
	,FKeybinding("RootPlayer", EKeys::RightShift, EKeybindPlatform::KeyboardMouse)
	,FKeybinding("DodgeForward", EKeys::SpaceBar, EKeybindPlatform::KeyboardMouse)
	,FKeybinding("Dodge", EKeys::ThumbMouseButton, EKeybindPlatform::KeyboardMouse)

	,FKeybinding("Ender Pearl", EKeys::F9, EKeybindPlatform::KeyboardMouse)
	,FKeybinding("Use Ender Pearl 1", EKeys::F6, EKeybindPlatform::KeyboardMouse)
	,FKeybinding("Use Ender Pearl 2", EKeys::F7, EKeybindPlatform::KeyboardMouse)
	,FKeybinding("Use Ender Pearl 3", EKeys::F8, EKeybindPlatform::KeyboardMouse)
	,FKeybinding("Chat", EKeys::T, EKeybindPlatform::KeyboardMouse)

	,FKeybinding("Pause", EKeys::Escape, EKeybindPlatform::KeyboardMouse)
	,FKeybinding("Friends", EKeys::F, EKeybindPlatform::KeyboardMouse)
	,FKeybinding("ToggleMap", EKeys::M, EKeybindPlatform::KeyboardMouse)
	,FKeybinding("ToggleMapOverlay", EKeys::Tilde, EKeybindPlatform::KeyboardMouse)
	,FKeybinding("Inventory", EKeys::I, EKeybindPlatform::KeyboardMouse)

	,FKeybinding("ChatCome", EKeys::R, EKeybindPlatform::KeyboardMouse)
	,FKeybinding("ChatThanks", EKeys::K, EKeybindPlatform::KeyboardMouse)
	,FKeybinding("ChatOK", EKeys::N, EKeybindPlatform::KeyboardMouse)
	,FKeybinding("ChatNeedArrows", EKeys::Y, EKeybindPlatform::KeyboardMouse)
	,FKeybinding("ChatWait", EKeys::U, EKeybindPlatform::KeyboardMouse)
	,FKeybinding("ChatNeedHealth", EKeys::Z, EKeybindPlatform::KeyboardMouse)
	,FKeybinding("ChatNo", EKeys::Q, EKeybindPlatform::KeyboardMouse)
	,FKeybinding("ChatSupplies", EKeys::W, EKeybindPlatform::KeyboardMouse)
	};
	return sKeyBind_list;
}

// D11.DB - Handy shortcut (reduces code duplication).
UGlobalStateData* GetSave(APlayerControllerBase* PlayerController)
{
	if (PlayerController) {
		auto gameInstance = PlayerController->GetWorld()->GetGameInstance<UDungeonsGameInstance>();
		if (auto player = Cast<UDungeonsLocalPlayer>(PlayerController->GetLocalPlayer())) {
			return gameInstance->EditGlobalSaveState(player->GetSystemUserId());
		}
	}
	return nullptr;
}

bool ControllerTypeCheck(EDungeonsControllerType ControllerType, EKeybindPlatform KeybindPlatform)
{
	static const TMap<EDungeonsControllerType, EKeybindPlatform> map = {
		{ EDungeonsControllerType::Controller_MouseAndKeyboard, EKeybindPlatform::KeyboardMouse },
		{ EDungeonsControllerType::Controller_XboxOne, EKeybindPlatform::Gamepad },
		{ EDungeonsControllerType::Controller_Touch,  EKeybindPlatform::Gamepad },
		{ EDungeonsControllerType::Controller_Virtual, EKeybindPlatform::Gamepad },
		{ EDungeonsControllerType::Controller_PS4, EKeybindPlatform::Gamepad },
		{ EDungeonsControllerType::Controller_SwitchHandheld, EKeybindPlatform::Gamepad },
		{ EDungeonsControllerType::Controller_SwitchPro, EKeybindPlatform::Gamepad },
		{ EDungeonsControllerType::Controller_SwitchJoyconDual, EKeybindPlatform::Gamepad },
		{ EDungeonsControllerType::Controller_SwitchJoyconLeft, EKeybindPlatform::JoyconL },
		{ EDungeonsControllerType::Controller_SwitchJoyconRight, EKeybindPlatform::JoyconR },
		{ EDungeonsControllerType::Controller_Invalid, EKeybindPlatform::Gamepad },
	};

	return map[ControllerType] == KeybindPlatform;
}

// D11.DB - This allows us to remap some bindings. This is useful for some specific cases.
//			For example the ranged attack input has two separate implementations for 
//			digital triggers and analogue triggers (Nintendo switch only has digital triggers).
//			This remapping function allows both of these cases to be automatically remapped to the
//			correct actions/axes without the user of this interface needing to know about the 
//			different implementations.
FName ConvertBindingName(const FKeybinding& InKeybinding)
{

	static const TMap<FName, FName> bindingAxisMap = {{FName("Shoot"), FName("AlternativeAttackGamepad")},{FName("Root"), FName("RangeAttackAxisSecondary")}};
	static const TMap<FName, FName> bindingActionMap = {{FName("Shoot"), FName("AlternativeAttackGamepad")},{FName("Root"), FName("RootPlayerGamepad")}};
	

	// D11.DB - Map names to actions.
	if (InKeybinding.Key.IsFloatAxis() && bindingAxisMap.Find(InKeybinding.Name))
	{
		return bindingAxisMap[InKeybinding.Name];
	}
	else if (!InKeybinding.Key.IsFloatAxis() && bindingActionMap.Find(InKeybinding.Name))
	{
		return bindingActionMap[InKeybinding.Name];
	}
	else
	{
		return InKeybinding.Name;
	}
}

const TArray<FKeybinding>& UKeybindHelper::GetDefaultConfig(EKeybindDefaultConfig Config)
{
	switch( Config )
	{
		case EKeybindDefaultConfig::Keyboard1:
			return KeyboardConfig();
			break;
		case EKeybindDefaultConfig::JoyconL1:
			return JoyconLConfig();
			break;
		case EKeybindDefaultConfig::JoyconR1:
			return JoyconRConfig();
			break;
		default:
			break;
	}
	return GamepadConfig();
}

void UKeybindHelper::GetDefaultConfig(EKeybindDefaultConfig Config, TArray<FKeybinding>& OutKeybinds)
{
	OutKeybinds.Append(UKeybindHelper::GetDefaultConfig(Config));
}


void UKeybindHelper::GetDefaultConfig(EDungeonsControllerType Type, TArray<FKeybinding>& OutKeybinds)
{
	switch (Type)
	{
	case EDungeonsControllerType::Controller_MouseAndKeyboard:
		return GetDefaultConfig(EKeybindDefaultConfig::Keyboard1, OutKeybinds);
	case EDungeonsControllerType::Controller_SwitchJoyconLeft:
		return GetDefaultConfig(EKeybindDefaultConfig::JoyconL1, OutKeybinds);
	case EDungeonsControllerType::Controller_SwitchJoyconRight:
		return GetDefaultConfig(EKeybindDefaultConfig::JoyconR1, OutKeybinds);
	case EDungeonsControllerType::Controller_XboxOne:
	case EDungeonsControllerType::Controller_PS4:
	case EDungeonsControllerType::Controller_SwitchHandheld:
	case EDungeonsControllerType::Controller_SwitchPro:
	case EDungeonsControllerType::Controller_SwitchJoyconDual:
	case EDungeonsControllerType::Controller_Invalid:
	default:
		return GetDefaultConfig(EKeybindDefaultConfig::Gamepad1, OutKeybinds);
	}
}

// D11.SSN
void UKeybindHelper::GetDefaultConfig(EKeybindPlatform Type, TArray<FKeybinding>& OutKeybinds)
{
	switch (Type) {
	case EKeybindPlatform::KeyboardMouse:
		return GetDefaultConfig(EKeybindDefaultConfig::Keyboard1, OutKeybinds);
	case EKeybindPlatform::JoyconL:
		return GetDefaultConfig(EKeybindDefaultConfig::JoyconL1, OutKeybinds);
	case EKeybindPlatform::JoyconR:
		return GetDefaultConfig(EKeybindDefaultConfig::JoyconR1, OutKeybinds);
	case EKeybindPlatform::Gamepad:
		return GetDefaultConfig(EKeybindDefaultConfig::Gamepad1, OutKeybinds);
	}
}

// D11.SSN
const TArray<FKeybinding>& UKeybindHelper::GetLeftConfig() {
	return LeftKeyboardConfig();
}

void UKeybindHelper::GetSavedKeybinds(APlayerControllerBase* PlayerController, TArray<FKeybinding>& KeybindList, EKeybindPlatform Type)
{
	if (auto save = GetSave(PlayerController))
	{
		if (PlayerController && PlayerController->GetLocalPlayer()) {
			const int playerID = PlayerController->GetGameInstance<UDungeonsGameInstance>()->GetUserManager()->GetLocalPlayerIndex(PlayerController->GetLocalPlayer());
			//D11.PS This playerId can be -1 now so ensure its valid first
			if (playerID >= 0)
			{
				KeybindList = save->GetKeybinds().FilterByPredicate([Type](const FKeybinding& keybinding) { return keybinding.Type == Type; });

				for(auto& bind : KeybindList)
				{
					TryFixDeprecatedBind(bind);
				}
			}
		}
	}
}

bool UKeybindHelper::TryFixDeprecatedBind(FKeybinding& keybinding)
{
	if(keybinding.Name.IsEqual(TEXT("Shoot")) && keybinding.Key == EKeys::Gamepad_RightTriggerAxis) {
		keybinding.Key = EKeys::Gamepad_RightTrigger;
		return true;
	}

	return false;
}

EKeybindPlatform ToEKeybindPlatform(EDungeonsControllerType Type)
{
	switch (Type)
	{
		case EDungeonsControllerType::Controller_MouseAndKeyboard:
			return EKeybindPlatform::KeyboardMouse;
		case EDungeonsControllerType::Controller_SwitchJoyconLeft:
			return EKeybindPlatform::JoyconL;
		case EDungeonsControllerType::Controller_SwitchJoyconRight:
			return EKeybindPlatform::JoyconR;
		case EDungeonsControllerType::Controller_XboxOne:
		case EDungeonsControllerType::Controller_PS4:
		case EDungeonsControllerType::Controller_SwitchHandheld:
		case EDungeonsControllerType::Controller_SwitchPro:
		case EDungeonsControllerType::Controller_SwitchJoyconDual:
		case EDungeonsControllerType::Controller_Invalid:
		default:
			return EKeybindPlatform::Gamepad;
	}
}

void UKeybindHelper::GetSavedKeybinds(APlayerControllerBase* PlayerController, TArray<FKeybinding>& KeybindList, EDungeonsControllerType Type)
{
	GetSavedKeybinds(PlayerController, KeybindList, ToEKeybindPlatform(Type));
}

void UKeybindHelper::SetSavedKeybinds(APlayerControllerBase* PlayerController, TArray<FKeybinding>& KeybindList, EKeybindPlatform Type)
{
	if (auto save = GetSave(PlayerController))
	{
		if (PlayerController && PlayerController->GetLocalPlayer())
		{
			const int playerID = PlayerController->GetGameInstance<UDungeonsGameInstance>()->GetUserManager()->GetLocalPlayerIndex(PlayerController->GetLocalPlayer());
			//D11.PS This playerId can be -1 now so ensure its valid first - This needs reworking now we have multiple profiles loading
			if (playerID >= 0)
			{
				auto& savedKeybinds = save->GetKeybinds();

				for (auto& bind : KeybindList)
				{
					bind.Type = Type;
				}

				auto pred = [Type](const FKeybinding& keybinding) { return keybinding.Type == Type; };
				auto previousBinds = savedKeybinds.FilterByPredicate(pred);
				savedKeybinds.RemoveAllSwap(pred);
				savedKeybinds.Append(KeybindList);

				if (auto gameInstance = Cast<UDungeonsGameInstance>(PlayerController->GetGameInstance()))
				{
					if (auto controllerTypeManager = gameInstance->GetControllerTypeManager())
					{
						// D11.DB - If Type is the current controllerType then we have to rebind everything now.
						//          Otherwise this is only done when the active input type changes.
						auto controllerType = gameInstance->GetControllerTypeManager()->GetControllerType(PlayerController->GetLocalPlayer()->GetControllerId());
						if (ControllerTypeCheck(controllerType, Type))
						{
							// D11.SSN - make sure to unbind any leftovers from the default bindings.
							GetDefaultConfig(Type, previousBinds);
							PlayerController->UnbindKeys(previousBinds);
							PlayerController->BindKeys(KeybindList);
						}
					}
				}
			}            
		}
	}
}

void UKeybindHelper::SetSavedKeybinds(APlayerControllerBase* PlayerController, UPARAM(ref) TArray<FKeybinding>& KeybindList, EDungeonsControllerType Type)
{
	SetSavedKeybinds(PlayerController, KeybindList, ToEKeybindPlatform(Type));
}

FString UKeybindHelper::KeyToString(const FKey& key)
{
	return key.ToString();
}

void UKeybindHelper::InitializeKeybinds(APlayerControllerBase* PlayerController)
{
	TArray<FKeybinding> list;

	GetDefaultConfig(EKeybindDefaultConfig::Gamepad1, list);
	SetSavedKeybinds(PlayerController, list, EKeybindPlatform::Gamepad);
	list.SetNum(0);
	GetDefaultConfig(EKeybindDefaultConfig::Keyboard1, list);
	SetSavedKeybinds(PlayerController, list, EKeybindPlatform::KeyboardMouse);
	list.SetNum(0);
	GetDefaultConfig(EKeybindDefaultConfig::JoyconL1, list);
	SetSavedKeybinds(PlayerController, list, EKeybindPlatform::JoyconL);
	list.SetNum(0);
	GetDefaultConfig(EKeybindDefaultConfig::JoyconR1, list);
	SetSavedKeybinds(PlayerController, list, EKeybindPlatform::JoyconR);
}

FString UKeybindHelper::KeybindPlatformToString(EKeybindPlatform Platform)
{
	static TMap<EKeybindPlatform, FString> map = {
		{ EKeybindPlatform::Gamepad, FString("Gamepad") },
		{ EKeybindPlatform::JoyconL, FString("JoyconL") },
		{ EKeybindPlatform::JoyconR, FString("JoyconR") },
		{ EKeybindPlatform::KeyboardMouse, FString("KeyboardMouse") },
	};

	return map[Platform];
}

EKeybindPlatform UKeybindHelper::StringToKeybindPlatform(const FString& Platform)
{
	static TMap<FString, EKeybindPlatform> map = {
		{ FString("Gamepad"), EKeybindPlatform::Gamepad },
		{ FString("JoyconL"), EKeybindPlatform::JoyconL },
		{ FString("JoyconR"), EKeybindPlatform::JoyconR },
		{ FString("KeyboardMouse"), EKeybindPlatform::KeyboardMouse },
	};

	return map[Platform];
}

void UKeybindHelper::DebugActiveKeybindings(APlayerControllerBase* PlayerController, EDungeonsControllerType Type)
{
	int32 PlayerId = -1;
	if (ULocalPlayer* localPlayer = PlayerController->GetLocalPlayer()) {
		PlayerId = PlayerController->GetGameInstance()->GetLocalPlayers().Find(localPlayer);
	}

	auto keyPredicate = [&Type](const FKey& key) -> bool {
		return ToEKeybindPlatform(Type) == EKeybindPlatform::KeyboardMouse ? !key.IsGamepadKey() : key.IsGamepadKey();
	};

	auto colourByLocalId = [&PlayerId]() -> FColor {
		switch (PlayerId) {
		case 0:
			return FColor::Green;
		case 1:
			return FColor::Cyan;
		case 2:
			return FColor::Magenta;
		case 3:
			return FColor::Orange;
		default:
			return FColor::White;
		}
	};

	FString outLog;
	for (auto& action : PlayerController->PlayerInput->ActionMappings) {
		if (keyPredicate(action.Key)) {
			outLog += FString::Printf(TEXT("{ [PlayerID:%i] [%s] [%s] }\n"), PlayerId, *action.ActionName.ToString(), *action.Key.GetDisplayName(true).ToString());
		}
	}
	GEngine->AddOnScreenDebugMessage(int32(PlayerController->GetClass()->GetUniqueID()) + PlayerId, PlayerController->GetActorTickInterval(), colourByLocalId(), outLog, true, FVector2D(0.9f, 0.9f));
}

