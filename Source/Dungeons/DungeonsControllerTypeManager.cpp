#include "DungeonsControllerTypeManager.h"
#include "Engine/LocalPlayer.h"
#include "online/sessions/OnlineUtil.h"
#include "DungeonsGameInstance.h"
#include "DungeonsGameViewportClient.h"
#include "Engine/AssetManager.h"

#include "GenericApplication.h"

#if PLATFORM_SWITCH
#include "Switch/SwitchPlatformMisc.h"
#endif

UControllerTypeManager::UControllerTypeManager(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
#if PLATFORM_XBOXONE
	, InputPhysicality(EInputPhysicality::Physical)
#endif
	, PrevMouseX(0.0f)
	, PrevMouseY(0.0f)
	, FirstControllerGamepadTimer(-1.0f)
#if PLATFORM_WINDOWS
	, FirstPlayerControllerIsGamepad(false)
#else
	, FirstPlayerControllerIsGamepad(true)
#endif
{
	TickDelegate = FTickerDelegate::CreateUObject(this, &UControllerTypeManager::Tick);
	TickDelegateHandle = FTicker::GetCoreTicker().AddTicker(TickDelegate);
	FCoreDelegates::OnControllerConnectionChange.AddUObject(this, &UControllerTypeManager::OnControllerConnectionChanged);

#ifdef HAS_CONNECTED_CONTROLLER_COUNT_DELEGATE
	FCoreDelegates::OnControllerConnectedCountChanged.AddUObject(this, &UControllerTypeManager::OnControllerConnectionCountChanged);
#endif

#if PLATFORM_XBOXONE

	if (!IsTemplate())
	{
		InitializeGameStreaming();

		if (GetIsTouchInputEnabled())
		{
			ShowTouchControl(DefaultLayout);
		}
	}


#endif
}

UControllerTypeManager::~UControllerTypeManager()
{

#if PLATFORM_XBOXONE
	if (!IsTemplate())
	{
		ShutdownGameStreaming();
	}
#endif
}


bool UControllerTypeManager::Tick(float DeltaTime)
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_UControllerTypeManager_Tick);

#if PLATFORM_WINDOWS
	if (FirstPlayerController.IsValid() && FirstPlayerController->GetLocalPlayer())
	{
		float currentMouseX = 0;
		float currentMouseY = 0;
		if (FirstPlayerController->GetMousePosition(currentMouseX, currentMouseY))
		{
			float mouseDeltaX = currentMouseX - PrevMouseX;
			float mouseDeltaY = currentMouseY - PrevMouseY;
			PrevMouseX = currentMouseX;
			PrevMouseY = currentMouseY;

			if (!FMath::IsNearlyZero(mouseDeltaX))
			{
				static const FKey key("MouseX");
				RawInputKeyPressed(FirstPlayerController->GetLocalPlayer()->GetControllerId(), key);
			}
			if (!FMath::IsNearlyZero(mouseDeltaY))
			{
				static const FKey key("MouseY");
				RawInputKeyPressed(FirstPlayerController->GetLocalPlayer()->GetControllerId(), key);
			}
		}
	}

	// D11.DB - Gamepad switch timer.
	if (FirstControllerGamepadTimer > 0.0f)
	{
		FirstControllerGamepadTimer -= DeltaTime;
		if (FirstControllerGamepadTimer < 0.0f)
		{
			FirstControllerGamepadTimer = 0.0f;
		}
	}

#endif
	return true;
}

void UControllerTypeManager::SetFirstPlayerController(APlayerControllerBase *PlayerController)
{
	FirstPlayerController = PlayerController;

	if (auto LocalPlayer = FirstPlayerController->GetLocalPlayer())
	{
		if (auto DungeonsViewportClient = Cast<UDungeonsGameViewportClient>(LocalPlayer->ViewportClient))
		{
			DungeonsViewportClient->InputKeyPressedDelegate.Remove(InputKeyPressedDelegateHandle);
			InputKeyPressedDelegateHandle = DungeonsViewportClient->InputKeyPressedDelegate.AddUObject(this, &UControllerTypeManager::RawInputKeyPressed);

#ifdef HAS_RAW_INPUT_MOUSE_LISTENER
			FSlateApplication::Get().OnRawInputKeyListener().Clear();
			FSlateApplication::Get().OnRawInputKeyListener().AddUObject(DungeonsViewportClient, &UDungeonsGameViewportClient::RawInputKey);
			FSlateApplication::Get().OnRawInputMouseListener().Clear();
			FSlateApplication::Get().OnRawInputMouseListener().AddUObject(DungeonsViewportClient, &UDungeonsGameViewportClient::RawInputKey);
#elif defined(HAS_RAW_INPUT_LISTENER)
			FSlateApplication::Get().OnRawInputListener().Clear();
			FSlateApplication::Get().OnRawInputListener().AddUObject(DungeonsViewportClient, &UDungeonsGameViewportClient::RawInputKey);
#endif
		}
	}
}

int UControllerTypeManager::GetGamepadCount() const
{
#ifdef HAS_GAMEPAD_COUNT
	TSharedPtr<GenericApplication> App = FSlateApplication::Get().GetPlatformApplication();
	return App->GetConnectedGamepadCount();
#else
	return 4;
#endif
}

// D11.SSN - used to disable controller switching on cursor centering
void UControllerTypeManager::SetPrevMousePos(int mouseX, int mouseY) {
	PrevMouseX = mouseX;
	PrevMouseY = mouseY;
}

void UControllerTypeManager::PreloadControllerTypeAssets()
{
	//examples
	//PaperSprite'/Game/UI/Materials/GamePad/_xbox/xbox_buttonSheet__icon_0.xbox_buttonSheet__icon_0'
	//Texture2D'/Game/UI/Materials/GamePad/_xbox/XBOX-256xGridCoreUI.XBOX-256xGridCoreUI'

	FString BaseAssetPath = TEXT("/Game/UI/Materials/GamePad/");

#if PLATFORM_PS4
	BaseAssetPath.Append(TEXT("_ps4"));
#elif PLATFORM_SWITCH
	BaseAssetPath.Append(TEXT("_switch"));
#elif PLATFORM_XBOXONE
	BaseAssetPath.Append(TEXT("_xbox"));
#endif		

	TArray<FSoftObjectPath> Names = TArray<FSoftObjectPath>();

	{
		TArray<FAssetData> AssetDatas;

		auto ObjectLibrary = UObjectLibrary::CreateLibrary(UObject::StaticClass(), true, true);

		ObjectLibrary->LoadAssetDataFromPath(*BaseAssetPath);
		ObjectLibrary->GetAssetDataList(AssetDatas);
		for (int32 i = 0; i < AssetDatas.Num(); ++i) {
			Names.Add(AssetDatas[i].ObjectPath.ToString());
		}

		ObjectLibrary->LoadBlueprintAssetDataFromPath(*BaseAssetPath);
		ObjectLibrary->GetAssetDataList(AssetDatas);
		for (int32 i = 0; i < AssetDatas.Num(); ++i) {
			Names.Add(AssetDatas[i].ObjectPath.ToString());
		}
	}

	UAssetManager::GetStreamableManager().RequestAsyncLoad(Names, [Names]() {

		for (const auto& name : Names) {
			if (auto object = name.ResolveObject()) {
				object->AddToRoot();
			}
		}
	});

}

void UControllerTypeManager::OnControllerConnectionCountChanged(int32 ConnectedControllersCount)
{
	OnControllerConnectionCountChangedDelegate.Broadcast(ConnectedControllersCount);
}

void UControllerTypeManager::OnControllerConnectionChanged(bool Connected, FPlatformUserId LocalUserNum, int32 ControllerIndex)
{
#if PLATFORM_SWITCH
	if (!Connected) {
		//D11.PS
		//There is no controller disconnect messages
		//This function is different on switch, this is triggered after a controller sync dialog has been shown. 
		//When we broadcast that its changed its then up to the user of this function to query the control types with GetControllerType for the controller id if needs be
		//we want to now broadcast to say that the switch controller type has changed
		OnControllerTypeChanged.Broadcast();
	}
#else
	// D11.SSN
	OnControllerConnectionChangedDelegate.Broadcast(Connected, ControllerIndex);
#endif
}

void UControllerTypeManager::EnteredForeground()
{
#if PLATFORM_SWITCH
	ControllerTypeChanged();
#else
	//D11.PC This is done to fix a problem with the xbox where if we disconnect a controller and activate Connected Standby
	//and connect the controller again and enter the game, the connection delegate is never called
	for (uint8 i = 0; i < MAX_LOCAL_PLAYERS; ++i)
	{
		OnControllerConnectionChangedDelegate.Broadcast(true, i);
	}
#endif
}


void UControllerTypeManager::ControllerTypeChanged()
{
	OnControllerTypeChanged.Broadcast();
}

void UControllerTypeManager::KeyboardInputUsed()
{
	OnKeyboardInputUsed.Broadcast();
}

void UControllerTypeManager::UpdateInputPhysicality(const FKey &Key)
{
#if PLATFORM_XBOXONE
	if (GetIsStreaming() && Key.IsGamepadKey())
	{
		EInputPhysicality CurrentInputPhysicality = Key.GetPhysicality();
		if (CurrentInputPhysicality != InputPhysicality)
		{
			InputPhysicality = CurrentInputPhysicality;
			ControllerTypeChanged();
		}
	}
#endif
}

void UControllerTypeManager::RawInputKeyPressed(int ControllerId, FKey Key)
{
	if (FirstPlayerController.IsValid() && FirstPlayerController->GetLocalPlayer() && FirstPlayerController->GetLocalPlayer()->GetControllerId() == ControllerId)
	{
		const float MouseSwitchDelay = 0.5f;
		bool GamepadActive = Key.IsGamepadKey();
		bool UpdateGamepadActive = false;

		if (FirstPlayerControllerIsGamepad && !GamepadActive)
		{
			// D11.DB - We need a delay before switching to mouse in case the player has
			//			only accidentally nudged their mouse.
			if (FMath::IsNearlyEqual(FirstControllerGamepadTimer, -1.0f) && Key.IsFloatAxis())
			{
				float delay = GetIsStreaming() ? 0.0f : MouseSwitchDelay;
				FirstControllerGamepadTimer = delay;
				return;
			}
			else if (FMath::IsNearlyZero(FirstControllerGamepadTimer) || !Key.IsFloatAxis())
			{
				UpdateGamepadActive = true;
			}
		}
		else if (!FirstPlayerControllerIsGamepad && GamepadActive)
		{
			UpdateGamepadActive = true;
		}

		if (UpdateGamepadActive)
		{
			if (GamepadActive)
			{
				FirstControllerGamepadTimer = -1.0f;
			}

			if (FirstPlayerControllerIsGamepad != GamepadActive)
			{
				if (auto viewport = FirstPlayerController->GetLocalPlayer()->ViewportClient->Viewport)
				{
					if (viewport->IsForegroundWindow())
					{
						FirstPlayerControllerIsGamepad = GamepadActive;
						FirstPlayerController->OnGamepadActiveChanged(GamepadActive);
						ControllerTypeChanged();
						return;
					}
				}
			}
		}


		// D11.JPhoenix - If the input was neither a gamepad input, mouse button, or float input, then a keyboard input was used.
		if (!GetIsStreaming() && !GamepadActive)
		{
			if (!Key.IsMouseButton() && !Key.IsFloatAxis())
			{
				KeyboardInputUsed();
			}
		}
	}

	UpdateInputPhysicality(Key);
}

EDungeonsControllerType UControllerTypeManager::GetControllerType(int ControllerId)const
{
	//D11.PS
#if PLATFORM_WINDOWS
	//D11.PS TODO - look at if we can use PS4 controller and if we can and its detected then we should return it
	//Need to setup the Dual Shock Controller plug in to use PS4 controller on PC.

	//D11.KS - On PC, the Keyboard is always set to controllerID 0, if it is 0, we can assume we are using a controller.
	if (ControllerId != 0) {
		return EDungeonsControllerType::Controller_XboxOne;
	}

	const auto gamepadActive = FirstPlayerController.IsValid() ? FirstPlayerController->GetGamepadActive() : FirstPlayerControllerIsGamepad;
	EDungeonsControllerType controllerType = gamepadActive ? EDungeonsControllerType::Controller_XboxOne : EDungeonsControllerType::Controller_MouseAndKeyboard;
	return controllerType;
#elif PLATFORM_XBOXONE
	if (FirstPlayerControllerIsGamepad)
	{
		return InputPhysicality == EInputPhysicality::Physical ? EDungeonsControllerType::Controller_XboxOne : EDungeonsControllerType::Controller_Virtual;
	}
	return (GetIsStreaming() ? EDungeonsControllerType::Controller_Touch : EDungeonsControllerType::Controller_MouseAndKeyboard);
#elif PLATFORM_PS4
	return EDungeonsControllerType::Controller_PS4;
#elif PLATFORM_SWITCH
	EDungeonsControllerType controllerType = EDungeonsControllerType::Controller_SwitchHandheld;

	if (ControllerId >= 0 && ControllerId < 8)
	{
		//Get the switch controller type and map it to our controller enum
		FSwitchPlatformMisc::ENpadControllerType switchControllerType = FSwitchPlatformMisc::GetExternalControllerTypeForPlayerArray_EightElements_ForReadWrite()[ControllerId];

		switch (switchControllerType)
		{
		case FSwitchPlatformMisc::ENpadControllerType::Npad_Handheld:
			controllerType = EDungeonsControllerType::Controller_SwitchHandheld;
			break;
		case FSwitchPlatformMisc::ENpadControllerType::Npad_DualJoyCon:
			controllerType = EDungeonsControllerType::Controller_SwitchJoyconDual;
			break;
		case FSwitchPlatformMisc::ENpadControllerType::Npad_LeftJoyCon:
			controllerType = EDungeonsControllerType::Controller_SwitchJoyconLeft;
			break;
		case FSwitchPlatformMisc::ENpadControllerType::Npad_RightJoyCon:
			controllerType = EDungeonsControllerType::Controller_SwitchJoyconRight;
			break;
		case FSwitchPlatformMisc::ENpadControllerType::Npad_Pro:
			controllerType = EDungeonsControllerType::Controller_SwitchPro;
			break;
		case FSwitchPlatformMisc::ENpadControllerType::Npad_None:
			controllerType = EDungeonsControllerType::Controller_Invalid;
			break;
		default:
			break;
		};

		return controllerType;
	}

#endif

	return EDungeonsControllerType::Controller_Invalid;
}