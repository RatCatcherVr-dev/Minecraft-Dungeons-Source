#pragma once

#include "GameFramework/PlayerInput.h"
#include "DungeonsControllerTypeManager.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "KeyBinding.generated.h"

class APlayerController;

// D11.DB - Enumeration of supported input types.
UENUM(BlueprintType)
enum class EKeybindPlatform : uint8
{
	KeyboardMouse,
	Gamepad,
	JoyconL,
	JoyconR,
};

// D11.DB - Preconfigured default control schemes.
UENUM(BlueprintType)
enum class EKeybindDefaultConfig : uint8
{
	Keyboard1,
	Gamepad1,
	JoyconL1,
	JoyconR1,
};

// D11.DB - Container for a single keybind, either an axis or action
//			We only expose Name and Key to blueprints to try to simplify
//			usage.
USTRUCT( Blueprintable )
struct FKeybinding
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	FName Name;

	UPROPERTY(BlueprintReadWrite)
	FKey Key;
	
	EKeybindPlatform Type = EKeybindPlatform::Gamepad;

	FKeybinding() {}

	FKeybinding(const FName& InActionName, const FKey& InKey, const EKeybindPlatform& InType)
		: Name(InActionName)
		, Key(InKey)
		, Type(InType)
	{
	}
};

FName ConvertBindingName(const FKeybinding& InKeybinding);

// D11.DB - Function library for assiting with the creation and manipulation of keybinds.
UCLASS(BlueprintType)
class UKeybindHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// Get all of the bindings for a particular control config.
	UFUNCTION(BlueprintPure)
	static const TArray<FKeybinding>& GetDefaultConfig(EKeybindDefaultConfig Config);
	
	static void GetDefaultConfig(EKeybindDefaultConfig Config, TArray<FKeybinding>& OutKeybinds);
	static void GetDefaultConfig(EKeybindPlatform Type, TArray<FKeybinding>& OutKeybinds); // D11.SSN

	// Get left-handed default bindings
	UFUNCTION(BlueprintPure)
	static const TArray<FKeybinding>& GetLeftConfig();

	// Gets the keybindings that will end up in the player's savegame.
	UFUNCTION(BlueprintCallable)
	static void GetSavedKeybinds(APlayerControllerBase* PlayerController, TArray<FKeybinding>& KeybindList, EKeybindPlatform Type = EKeybindPlatform::Gamepad);

	// Sets the keybindings that will end up in the player's savegame.
	UFUNCTION(BlueprintCallable)
	static void SetSavedKeybinds(APlayerControllerBase* PlayerController, UPARAM(ref) TArray<FKeybinding>& KeybindList, EKeybindPlatform Type = EKeybindPlatform::Gamepad);

	// D11.DB - Converts a key to a non-localised string.
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FString KeyToString(const FKey& key); 

	// D11.DB - Helper functions that determine which EKeybindPlatform or EKeybindDefaultConfig to use internally.
	static void GetDefaultConfig(EDungeonsControllerType Config, TArray<FKeybinding>& OutKeybinds);
	static void GetSavedKeybinds(APlayerControllerBase* PlayerController, TArray<FKeybinding>& KeybindList, EDungeonsControllerType Type);
	static void SetSavedKeybinds(APlayerControllerBase* PlayerController, UPARAM(ref) TArray<FKeybinding>& KeybindList, EDungeonsControllerType Type);

	static void InitializeKeybinds(APlayerControllerBase* PlayerController);

	static FString KeybindPlatformToString(EKeybindPlatform Platform);
	static EKeybindPlatform StringToKeybindPlatform(const FString& Platform);

	static bool TryFixDeprecatedBind(FKeybinding& keybinding);

	// D11.DJB - Debugger to display active keybindings for the current controller type.
	static void DebugActiveKeybindings(APlayerControllerBase* PlayerController, EDungeonsControllerType Type);
};
