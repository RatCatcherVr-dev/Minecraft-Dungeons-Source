#pragma once

#if (!UE_BUILD_SHIPPING) && (!PLATFORM_PS4 && !PLATFORM_SWITCH) && (PLATFORM_XBOXONE || PLATFORM_WINDOWS)
#include "BotAutomationDefines.h"
#endif

#ifdef ENABLE_BOTAUTOMATION
class APlayerAutomator;
#endif

#include "GameFramework/PlayerController.h"
#include "save/CharacterSaveData.h"
#include "game/input/KeyBinding.h" // D11.DB
#include "PlayerCharacterSaveSlot.h"
#include "DungeonsControllerTypeManager.h" // D11.DB
#include "PlayerControllerBase.generated.h"

class UGlobalStateData;
class UCharacterSaveDataComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRecentSaveDataIndexChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSaveDataChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNoSaveDataFound);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSkinEverSelectedChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnToggleHUDUI);

// D11.DB
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInputMethodChanged, bool, gamepad);

UCLASS()
class DUNGEONS_API APlayerControllerBase : public APlayerController
{
	GENERATED_BODY()
public:
	APlayerControllerBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	UFUNCTION(BlueprintPure, Category = "Dungeons")
	inline bool IsOwnedByInitialLocalPlayer();

	const TSharedPtr<const FUniqueNetId> GetUniqueNetId() const;
	FString GetRecentSaveDataKey() const;

	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void LoadSaveData();
	void SetPawn(APawn* InPawn) override;

	UFUNCTION()
	void ControllerTypeChanged();
	
	template <class T>
	T* GetFirstHandlerForInputActionOfType(const FName& inputAction) const {
		for (int stackIndex = CurrentInputStack.Num() - 1; stackIndex >= 0; stackIndex--) {
			auto tweakInputComponent = CurrentInputStack[stackIndex];
			if (tweakInputComponent.IsValid()) {
				for (int index = 0; index < tweakInputComponent->GetNumActionBindings(); index++) {
					const FInputActionBinding& binding = tweakInputComponent->GetActionBinding(index);
					if (binding.GetActionName() == inputAction) {
						if (auto outerOfAskedType = Cast<T>(tweakInputComponent->GetOuter())) {
							return outerOfAskedType;
						}
					}
				}
			}
		}
		return nullptr;
	}	

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	const FText& GetLocalPlayerDisplayText() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void SetCoopGamepadLightColourIndex(int32 index);

	// #D11.CM - Sets gamepad colour for the duration then sets it back to the local player colour.
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void SetGamepadLightColourForDuration(FColor colour, float duration);

	// D11.DB
	void BindKeys( const TArray<FKeybinding>& InKeybindList );
	void UnbindKeys( const TArray<FKeybinding>& InKeybindList );


#ifdef ENABLE_BOTAUTOMATION
	/** Method called prior to processing input */
	virtual void PreProcessInput(const float DeltaTime, const bool bGamePaused) override;
	TWeakObjectPtr<APlayerAutomator> InputAutomator;
#endif

	//////////////////////////////////////////////////////////////////////////
	// Local Data
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool HasSaveData() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void AssignSaveData(UCharacterSaveData* saveData);

	void SetRecentSaveDataFilename(int32 LocalUserNum, const FString& inFilename);
	
	UCharacterSaveData* GetSaveData() const;

	UCharacterSerializeComponent* GetCharacterSerializeComponent() const;

	void InitialiseKeybinds();

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void SaveCharacterData();

	// @Note: After we've had time to verify it's always "safe"
	//        to call this, move this to SaveCharacterData.
	void SaveCharacterData_OnlyIfValidSaveDataNum();

	//////////////////////////////////////////////////////////////////////////
	// Global Data

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool CreateNewCharacter();

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool CloneCharacter(int32 inIndex);

	bool DeepCloneCharacter(UCharacterSaveData* pToDeepClone);


	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool DeleteCharacterByIndex(int32 inIndex);

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool CanCreateNewCharacter() const;
	
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool CanDeleteCharacterByIndex(int32 inIndex) const;

	// per *xbox?* profile, currently reading from one location
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	UCharacterSaveData* GetAvailableSaveDataByIndex(int32 index) const;
	
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	APlayerCharacterSaveSlot* GetCharacterSlotByIndex(int32 index, bool forceRefreshSlot = false);

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool GetIsCharacterSlotIndexSelectable(int32 index) const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	int32 GetRecentSaveDataIndex() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	int32 GetFirstAvailableSaveIndex() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	int32 GetNumProfiles() const;

	int32 GetMaxProfiles() const;	
	
	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnSkinEverSelectedChanged OnSkinEverSelectedChanged;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnRecentSaveDataIndexChanged OnRecentSaveDataIndexChanged;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnSaveDataChanged OnSaveDataChanged;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Dungeons")
	FOnToggleHUDUI OnToggleHUDUI;

	UPROPERTY(BlueprintReadOnly, Config)
	bool EnableDebugUIControls = false;

	// #D11.CM
	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnNoSaveDataFound OnNoSaveDataFound;

	// D11.DB
	UFUNCTION(BlueprintCallable)
		bool GetGamepadActive() const {
		return bGamepadActive;
	}

	virtual void OnGamepadActiveChanged(bool GamepadActive);

	void UpdateLocalPlayerDisplayIndex();

	UFUNCTION(BlueprintPure)
	int32 GetSaveLocalUserNum() const;

	static const int32 INVALID_LOCAL_USER_NUM = -1;

	// D11.DJB
	void SetInputMode(const FInputModeDataBase& InData) override;
	void SetUserMouseLockMode(const EMouseLockMode InMode);

	// D11.DJB - Retrieves the X and Y displacement of the given analog stick. Performed using a scaled radial deadzone
	UFUNCTION(BlueprintCallable, Category = "Game|Player")
	void GetInputAnalogStickStateRadialDeadzone(EControllerAnalogStick::Type WhichStick, float& StickX, float& StickY) const;

	// D11.JPhoenix - Retrieves the X and Y displacement on the D-pad. Left -1.0 to Right 1.0. Up -1.0 to Down 1.0.
	UFUNCTION(BlueprintCallable, Category = "Game|Player")
	void GetInputDirectionalPadState(float& ValueX, float& ValueY) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	bool HasEverSelectedSkinId(const FName& skinId);

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void OnSelectedSkinId(const FName& skinId);

	UFUNCTION(BlueprintCallable)
	bool IsInitialized() const;

	void SetInitialized(bool initialized);

	UGlobalStateData* EditGlobalSaveState();
	const UGlobalStateData* ReadGlobalSaveState() const;

	bool CloneCharacter(UCharacterSaveData* characterSaveData);
protected:
	//D11.KS - Use this for any initialisation regarding the local player.
	void ReceivedPlayer() override;

	// D11.DJB
	void InitialiseMouseLockMode();
	void InitialiseControllerTypeBinding();
	void GetInputAnalogStickRawState(EControllerAnalogStick::Type WhichStick, float& StickX, float& StickY) const;
	void CalculateScaledRadialDeadzone(float& RawStickX, float& RawStickY, const float& axisDeadzone) const;

private:

	void TryLoadCharacterSaveData();

	void UpdateControllerType();
	
	
	UPROPERTY()
	UCharacterSerializeComponent* mCharacterSerializeComponent;

	EMouseLockMode mUserMouseLockMode;

	FTimerHandle mControllerFlashHandle;
	
	UPROPERTY()
	// D11.DB - Determine the currently active input device.
	bool bGamepadActive;
	EDungeonsControllerType prevControllerType;
		
	TOptional<int> mLocalPlayerDisplayIndex;
	UPROPERTY()
	FText mLocalPlayerDisplayText;

	bool bInitialized;
};

 // D11.DB - inline required by blueprint.
inline bool APlayerControllerBase::IsOwnedByInitialLocalPlayer()
{
	if (!IsLocalController())
		return false;

	if (ULocalPlayer* localPlayer = GetLocalPlayer())
	{
		if(UGameInstance* gameInstance = GetGameInstance())
		{
			return gameInstance->GetNumLocalPlayers() > 0 && gameInstance->GetLocalPlayerByIndex(0) == localPlayer;
		}
	}

	return false;
}

