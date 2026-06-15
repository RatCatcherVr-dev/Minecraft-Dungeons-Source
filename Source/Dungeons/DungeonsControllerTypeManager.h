#pragma once
#include "GameFramework/Info.h"
#include "Engine/EngineTypes.h"
#include "common_header.h"

#include "XCloudAPi.h"

#include "DungeonsControllerTypeManager.generated.h"

class APlayerControllerBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FControllerTypeChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FKeyboardInputUsed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FControllerConnectionChanged, bool, Connected, int, LocalPlayerIndex); // D11.SSN
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnControllerConnectedCountChanged, int32, ConnectedControllersCount);

//typedef FControllerRemoved::FDelegate FControllerRemovedDelegate;

UENUM(BlueprintType)
enum class EDungeonsControllerType : uint8 {
	Controller_MouseAndKeyboard,
	Controller_XboxOne,
	Controller_Touch,
	Controller_Virtual,
	Controller_PS4,
	Controller_SwitchHandheld,
	Controller_SwitchPro,
	Controller_SwitchJoyconDual,
	Controller_SwitchJoyconLeft,
	Controller_SwitchJoyconRight,
	Controller_Invalid,
};
ENUM_NAME(EDungeonsControllerType);

UCLASS()
class DUNGEONS_API UControllerTypeManager : public UObject
{
	GENERATED_BODY()
public:
	UControllerTypeManager(const FObjectInitializer& ObjectInitializer);
	~UControllerTypeManager();

	bool Tick(float DeltaTime);

	UPROPERTY(BlueprintAssignable, Category = "Dungeons|ControllerTypeManager")
	FControllerTypeChanged OnControllerTypeChanged;

	// D11.JPhoenix - Used to trigger events that rely on input changing specifically to keyboard.
	UPROPERTY(BlueprintAssignable, Category = "Dungeons|ControllerTypeManager")
	FKeyboardInputUsed OnKeyboardInputUsed;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons|ControllerTypeManager")
	FControllerConnectionChanged OnControllerConnectionChangedDelegate; // D11.SSN

	UPROPERTY(BlueprintAssignable, Category = "Dungeons|ControllerTypeManager")
	FOnControllerConnectedCountChanged OnControllerConnectionCountChangedDelegate;

	UFUNCTION(BlueprintCallable, Category = "Dungeons|ControllerTypeManager")
	EDungeonsControllerType GetControllerType(int ControllerId) const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons|ControllerTypeManager")
	int GetGamepadCount() const;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "XCloud")
	ETakLayout DefaultLayout;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "XCloud")
	FORCEINLINE bool GetIsStreaming() const
	{
#if PLATFORM_XBOXONE
		return XCloud::Get()->IsStreaming();
#endif
		return false;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "XCloud")
	FORCEINLINE bool GetIsTouchInputEnabled()
	{
#if PLATFORM_XBOXONE
		return XCloud::Get()->IsTouchInputEnabled();
#endif
		return false;
	}

	UFUNCTION(BlueprintCallable, Category = "XCloud")
	void InitializeGameStreaming() noexcept
	{
#if PLATFORM_XBOXONE
		XCloud::Create();
#endif
	}

	UFUNCTION(BlueprintCallable, Category = "XCloud")
	void ShutdownGameStreaming() noexcept
	{
#if PLATFORM_XBOXONE
		XCloud::Shutdown();
#endif
	}

	UFUNCTION(BlueprintCallable, Category = "XCloud")
	void ShowTouchControl(ETakLayout Layout) noexcept
	{
#if PLATFORM_XBOXONE

		XCloud::Get()->ShowTouchControl(Layout);

#endif
	}


	UFUNCTION(BlueprintCallable, Category = "XCloud")
	void HideTouchControl() noexcept	{
#if PLATFORM_XBOXONE
		XCloud::Get()->HideTouchControl();
#endif
	}

	UFUNCTION()
	void RawInputKeyPressed(int ControllerId, FKey Key);

	void SetFirstPlayerController(APlayerControllerBase *PlayerController);

	// D11.SSN - used to disable controller switching on cursor centering
	void SetPrevMousePos(int mouseX, int mouseY);

	static void PreloadControllerTypeAssets();

	void EnteredForeground();

private:
	void ControllerTypeChanged();
	void KeyboardInputUsed();
	void OnControllerConnectionChanged(bool Connected, FPlatformUserId LocalUserNum, int32 ConnectedControllersCount);
	void OnControllerConnectionCountChanged(int32 ControllerIndex);
	void UpdateInputPhysicality(const FKey &Key);

	TWeakObjectPtr<APlayerControllerBase>	FirstPlayerController;
	FDelegateHandle							InputKeyPressedDelegateHandle;
	FTickerDelegate							TickDelegate;
	FDelegateHandle							TickDelegateHandle;
#if PLATFORM_XBOXONE
	EInputPhysicality						InputPhysicality;
#endif
	float									PrevMouseX;
	float									PrevMouseY;
	float									FirstControllerGamepadTimer;
	bool									FirstPlayerControllerIsGamepad;
};