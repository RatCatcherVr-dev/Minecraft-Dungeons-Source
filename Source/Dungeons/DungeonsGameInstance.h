#pragma once

#include "Engine/GameInstance.h"
#include "CharacterLevel.h"
#include "save/GlobalSaveData.h"
#include "game/LevelSettings.h"
#include "game/component/AwardsGeneratorComponent.h"
#include "game/component/ItemStashComponent.h"
#include "ui/EnumDefine.h"
#include "online/OnlineCommon.h"
#include "online/friends/DungeonsFriendsCommon.h"
#include "online/friends/DungeonsFriendsInterface.h"
#include "online/reconnect/ReconnectStates.h"
#include "test/LogBasedIssueDetector.h"
#include "util/telemetry/Analytics.h"
#include "util/telemetry/AnalyticsReflection.h"
#include "game/level/sound/SoundMixManager.h"
#include "game/level/sound/AudioMusicManager.h"
#include "game/level/sound/AudioSFXManager.h"
#include "game/mission/trial/TrialTypes.h"
#include <SharedPointer.h>
#include "LoadingScreen/LoadingScreenBuilder.h"
#include "DungeonsGameViewportClient.h"
#include "UI/input/VirtualKeyboardManager.h"
#include "Assets/DungeonsInventoryAssetManager.h"
#include "Assets/DungeonsAssetManager.h"
#include "game/cosmetics/CosmeticsLibrary.h"
#include "game/mission/theme/data/MissionThemeLibrary.h"
#include "game/mission/info/data/MobIconLibrary.h"
#include "game/skins/SkinsLibrary.h"
#include "DungeonsDefsMinimal.h"
#include "DungeonsControllerTypeManager.h"
#include <OnlineSessionInterface.h>
#include <OnlineIdentityInterface.h>
#include "Net/OnlineEngineInterface.h"
#include "online/sessions/JoinDungeonsSessionCallbackProxy.h"
#include <GameplayEffect.h>
#include "OnlineSubsystemTypes.h"
#include "online/MinecraftAPI.h"
#include "online/entitlements/EntitlementsRepository.h"
#include "ui/dialog/BlockingMessageBoard.h"
#include "network/ConnectionChecker.h"
#include "util/AutoDetect.h"
#include <vector>
#include "game/inventory/InventoryItemDataHolder.h"
#include "DungeonsGameInstance.generated.h"

class APlayerController;

//D11.PC
class FDungeonsPendingInvite
{
public:
	FDungeonsPendingInvite() : ControllerId(-1), UserId(nullptr), bPrivilegesCheckedAndAllowed(false), bSuccessfullyAccepted(false) {}

	int32							 ControllerId;
	TSharedPtr< const FUniqueNetId > UserId;
	FOnlineSessionSearchResult 		 InviteResult;
	bool							 bPrivilegesCheckedAndAllowed;
	bool							 bSuccessfullyAccepted;
};

UENUM(BlueprintType)
enum class EBootMode : uint8
{
	Unset,
	InitialBoot,
	SwitchUser,
	UserSignedOut,
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSaveStateLoaded);
//D11.PS
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAnyKeyPressedControllerIdDelegate, int, ControllerId, FKey, Key, bool, IsRepeat);

//D11.SSN
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAnyKeyReleasedControllerIdDelegate, int, ControllerId, FKey, Key, bool, IsRepeat);

// #D11.CM
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSwitchOperationModeChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCheckpointRespawnCooldown);

// D11.DJB
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnViewportResizedDelegate, const FIntPoint&, resizedResolution);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNotifyUserLowPerformanceDetected, const FText&, NotifyMessageBody);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnSwitchedUserDelegate, int, ControllerIndex, const FString&, PreviousUser, const FString&, NewUser);

//D11.KS
DECLARE_MULTICAST_DELEGATE_OneParam(FOnLocalPlayerLeave, ULocalPlayer*);

//Moj.DB
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLocalPlayerAddedToViewport);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLocalPlayerRemovedFromViewport);

// D11.PC
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPrivilegeTaskStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPrivilegeTaskFinished);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPrivilegeTaskSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPrivilegeTaskFailed, int32, PrivilegResult);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLocalPlayerChangedUser, int, LocalPlayerControllerId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnJoinSessionFailed, ESessionJoinFailureReason, result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnJoinSessionCompleted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCreateSessionCompleted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCreateSessionFailed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGlobalSaveDataLoadFinished, int32, userIndex);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEntitlementsReceived);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUnlinkTaskFinished);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLevelStart);


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAchievementUnlockedWithImageURL, FString, achievementTitle, FString, achievementImageURL);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUserNotLoggedIn);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGraphicsSettingChanged, int, newLevel, APlayerController*, playerController);

//Intended to be used ONLY when starting a level - not as an accurate representation of the level state.
struct FDungeonsConfiguration {
	void Reset();
	void PrepareTravel(const FLevelSettings&);

	const TOptional<FLevelSettings>& GetLevelSettings() const;
	FString GetLevelDisplayName() const;
	ELevelNames GetLevelName() const;	
	
private:
	TOptional<FLevelSettings> LevelSettings;	
};

struct FRecentlLeftControllerData {
	FRecentlLeftControllerData(int32 ControllerID = -1, float GracePeriod = 2.0f) :mControllerID(ControllerID), mGracePeriod(GracePeriod){}
	int32 mControllerID;
	float mGracePeriod;
};


UCLASS()
class DUNGEONS_API UPoppingEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UPoppingEffect();
};

class UDungeonsUserManager;
class UDungeonsLoginFlow;
class UHintManager;
class ULoadingScreenInitializer;
class UCreateDungeonsSessionCallbackProxy;
class UFindSessionHandler;

UCLASS(minimalapi)
class UDungeonsGameInstance : public UGameInstance {
	
	GENERATED_UCLASS_BODY()
public:

	FDungeonsConfiguration Configuration;

#if WITH_EDITOR
	FGameInstancePIEResult InitializeForPlayInEditor(int32 PIEInstanceIndex, const FGameInstancePIEParameters& Params) override;

	void LoadPlayerControllerGlobalSaveDataInEditor();
#endif

	void Init() override;

	void SetInitConnectionType();

	void OnStart() override;

	void Shutdown() override;

	void FailedIntegrityCheck();

	UGlobalStateData* EditGlobalSaveState(int32 LocalUserNum) const;
	const UGlobalStateData* ReadGlobalSaveState(int32 LocalUserNum) const;
	bool IsSaveStateValid(int32 LocalUserNum) const;

	UFUNCTION(BlueprintCallable)
	bool IsInitialBoot() const;

	UFUNCTION(BlueprintCallable)
	void SetInitialBootFlowComplete();
	bool IsXblActiveInSaveFile(int playerIx) const;

	bool IsUserAlreadyInGame(TSharedPtr<const FUniqueNetId> UniqueId);

	UFUNCTION(BlueprintCallable)
	static FCharacterLevel createCharacterLevel(int32 xp);
	static FCharacterLevel createCharacterLevelForLevel(int level);
	
	void SetMissionFinishedSummary(const FMissionFinishedSummary& summary);
	const TOptional<FMissionFinishedSummary>& GetMissionFinishedSummary() const;
	void ClearMissionFinishedSummary();
	
	void SetLevelSettingsLastStarted(const FLevelSettings&);
	const FLevelSettings& GetLevelSettingsLastStarted() const;

	bool QueryUnlockKeys(const FString& expression) const;

	void SetCurrentTrials(const TArray<FTrialDef>&);
	TOptional<FString> GetTrialType(const FString&);
	TOptional<FString> GetCurrentTrialId(ELevelNames) const;
	TOptional<FString> GetTrialIdLastStarted() const;

	ReconnectStates& GetReconnectStates();
	const ReconnectStates& GetReconnectStates() const;

	UFUNCTION(BlueprintPure)
	bool IsHost();

	void OnJoinSessionCompletePS4(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	UFUNCTION(BlueprintCallable)
	void OnInGameJoin();
	void OnInGameJoinPrivilegesFinished(const FUniqueNetId& UserId, EUserPrivileges::Type Privilege, uint32 PrivilegeResults);
	void OnInGameJoinHavePrivileges();

	//D11.PC
	/** Start task to get user privileges. */
	void StartOnlinePrivilegeTask(const IOnlineIdentity::FOnGetUserPrivilegeCompleteDelegate& Delegate, EUserPrivileges::Type Privilege, TSharedPtr< const FUniqueNetId > UserId);
	/** Delegate function executed after checking privileges for accepting an invite */
	void OnUserCanPlayInvite(const FUniqueNetId& UserId, EUserPrivileges::Type Privilege, uint32 PrivilegeResults);
	/** Delegate function executed after checking privileges for Play Together on PS4 */
	void OnUserCanPlayTogether(const FUniqueNetId& UserId, EUserPrivileges::Type Privilege, uint32 PrivilegeResults);
	void OnPrivilegeError(const FUniqueNetId& UserId, uint32 PrivilegeError);

	UFUNCTION(BlueprintCallable, Category = "Online Privileges")
	void StartOnlinePrivilegeCheck();
	void OnOnlinePrivilegesCheckTaskFinished(const FUniqueNetId& UserId, EUserPrivileges::Type Privilege, uint32 PrivilegeResults);
	void OnHasOnlinePrivileges();

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnPrivilegeTaskStarted OnPrivilegeTaskStarted;
	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnPrivilegeTaskFinished OnPrivilegeTaskFinished;
	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnPrivilegeTaskFailed OnPrivilegeTaskFailed;
	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnPrivilegeTaskSuccessful OnPrivilegeTaskSuccessful;

	void OnFakeSessionInviteReceived(FBlueprintGameSession& GameSession);
	void AcceptInvite(const FOnlineSessionSearchResult& sessionResult);

	UFUNCTION(BlueprintCallable)
	void AcceptInvite(const FBlueprintGameSession& gameSession);
	UFUNCTION(BlueprintCallable)
	void DeclineInvite();

	void CatchNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString);
	void HandleNetworkConnectionStatusChanged(const FString& ServiceName, EOnlineServerConnectionStatus::Type LastConnectionStatus, EOnlineServerConnectionStatus::Type ConnectionStatus);
	EOnlineServerConnectionStatus::Type CurrentConnectionStatus;
	bool bIgnoreNetworkStatusChanged;

	UFUNCTION(BlueprintCallable)
	bool CheckConnectionStatus();

	UFUNCTION(BlueprintCallable)
	bool IsConnected();

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Online|Sessions")
	bool CanGoOnline();

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Online|Sessions")
	void GoOnline();

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnJoinSessionFailed OnJoinSessionFailed;
	UFUNCTION()
	void JoinSessionFailed(ESessionFailureReason Reason, ESessionJoinFailureReason ExtendedReason);

	UPROPERTY(BlueprintAssignable, Category = "Dungeons|Online|Sessions")
	FOnCreateSessionCompleted OnCreateSessionCompleted;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons|Online|Sessions")
	FOnCreateSessionFailed OnCreateSessionFailed;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons|Settings")
	FOnGraphicsSettingChanged OnLightFunctionQualitySet;

	UFUNCTION(BlueprintCallable)
	UDungeonsFriendsInterface* GetFriendsInterface();

	UFindSessionHandler* GetFindSessionHandler() const;

	UFUNCTION(BlueprintCallable)
	int32 GetLocalPlayerUserCount() const;

	UFUNCTION(BlueprintCallable)
	bool IsLocalCoop() const;

	UFUNCTION()
	void OnPreLoadMap(const FString& mapName);

	UFUNCTION()
	void OnPostLoadMap(UWorld* world);

	UFUNCTION()
	void BeginLoadingScreen(const FString& mapName, bool isSeamlessTravel);

#if PLATFORM_WINDOWS
	static constexpr float fDefaultFadeOutTime = 0.5f;
	static constexpr float fDefaultFadeInTime = 1.0f;
#else
	static constexpr float fDefaultFadeOutTime = 0.25f;
	static constexpr float fDefaultFadeInTime = 0.5f;
#endif

	
	UFUNCTION(BlueprintCallable)
	void BeginLoadingScreenWithTravel(const FLevelSettings& levelSettings, EMapLoadType mapLoadType, float fadeOutTime = -1.0f, float fadeinTime = -1.0f, APlayerController* playerController = nullptr, const FString& connectionString = "");
	void BeginLoadingScreenWithTravel(const struct MapLoadData&, float fadeOutTime, float fadeinTime);

	UFUNCTION()
	void EndLoadingScreen(UWorld* world, FName worldName, float fadeOutTime, float fadeInTime);

	UFUNCTION(BlueprintCallable)
	void SetupInputKeyPressedDelegate();

	UFUNCTION(BlueprintCallable)
	void SetupInputKeyReleasedDelegate();

	UFUNCTION(BlueprintCallable)
	bool IsAnyPlayerAliveAndInWorld();

	UFUNCTION(BlueprintCallable)
	bool IsAnyPlayerInWorld();

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnAnyKeyPressedControllerIdDelegate OnAnyKeyPressedControllerIdDelegate;

	// D11.SSN
	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnAnyKeyReleasedControllerIdDelegate OnAnyKeyReleasedControllerIdDelegate;

	// D11.DJB
	UPROPERTY(BlueprintAssignable, Category = "Dungeons|Game Settings|Utilities")
	FOnViewportResizedDelegate OnViewportResizedDynamicDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnLocalPlayerAddedToViewport OnLocalPlayerAddedToViewport;
	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnLocalPlayerRemovedFromViewport OnLocalPlayerRemovedFromViewport;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnNotifyUserLowPerformanceDetected OnNotifyLowPerformanceDetected;

	//D11.PS
	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnSwitchedUserDelegate OnSwitchedUserDelegate;

	// #D11.CM - Popping
	UFUNCTION(BlueprintCallable)
	void HandlePlayerPopping(float deltaTime);

	UFUNCTION(BlueprintCallable)
	void ResetPoppingData();

	UFUNCTION(BlueprintCallable)
	void PopLocalPlayersToCheckpoint();

	UFUNCTION(BlueprintCallable)
	int GetTimeUntilTeleportCooldown();

	UFUNCTION(BlueprintCallable)
	bool CanTeleportToCheckpoint();

	UPROPERTY(BlueprintAssignable)
	FOnCheckpointRespawnCooldown OnCheckpointRespawnCooldown;

	// #D11.CM - Popping
	// It is possible that there could be no viable pop target
	UFUNCTION(BlueprintCallable)
	APlayerCharacter* GetNextObjectiveBasedLeader(FVector target, APlayerCharacter* excludeCharacter = nullptr);

	APlayerCharacter* GetPoppingLeader();

	APlayerCharacter* GetLastLeader();

	// #D11.CM - Popping
	UFUNCTION(BlueprintCallable)
	bool GetNextProgressTarget(FVector& outPosition) const;

	UFUNCTION(BlueprintCallable)
 	void MessageBoxButtonPressed(EMessageType messagetype, int button) const;

	void SetDebugPromptMessage(const FText& DebugMessage);

	UFUNCTION(BlueprintCallable)
	void SetSelectedGameMode(uint8 NewGameMode) { SelectedGameMode = NewGameMode; }

	// #D11.CM
	UFUNCTION(BlueprintCallable)
	APlayerCharacter* GetAlivePlayerCharacter(ULocalPlayer* local);

	//D11.KS
	UFUNCTION(BlueprintCallable, Category = "LocalCoop")
	void UpdateLocalCoopCamera(const float startZoomRadius, const float endZoomRadius, const float zoomMaxmimum, float& outZoomAmount, FVector& outPosition);
	UFUNCTION(BlueprintCallable, Category = "LocalCoop")
	void UpdateLocalCoopScreenStates();
	const float GetLocalCoopHardBoundaryWidthPercentage() const { return 0.075f; };
	const float GetLocalCoopDangerZoneWidthPercentage() const { return GetLocalCoopHardBoundaryWidthPercentage() * 2; };

	UFUNCTION(BlueprintPure)
	inline uint8 GetSelectedGameMode() const { return SelectedGameMode; }

	UFUNCTION(BlueprintCallable)
	inline UControllerTypeManager *GetControllerTypeManager() const { return ControllerTypeManager; };

	UFUNCTION(BlueprintCallable)
	inline UDungeonsUserManager *GetUserManager() const { return UserManager; };

	UFUNCTION(BlueprintCallable)
	inline UDungeonsLoginFlow *GetLoginFlow() const { return LoginFlow; };

	UFUNCTION(BlueprintCallable)
	inline UVirtualKeyboardManager* GetKeyboardManager() const { return KeyboardManager; };

	UFUNCTION(BlueprintPure)
	inline UDungeonsAssetManager* GetDungeonsAssetManager() const { return AssetManager; };

	UFUNCTION(BlueprintPure)
	inline UDungeonsInventoryAssetManager* GetDungeonsInventoryAssetManager() const { return InventoryAssetManager; };

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	AAudioMusicManager* GetAudioMusicManager();

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	AAudioSFXManager* GetAudioSFXManager();

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	USoundMixManager* GetSoundMixManager();

	UFUNCTION(BlueprintPure, Category = "Dungeons")
	ULoadingScreenInitializer* GetLoadingScreenInitializer();

	UFUNCTION(BlueprintPure, Category = "Dungeons")
	UHintManager* GetHintManager();

	UMinecraftAPI* GetMinecraftAPI() const;

	UCosmeticsLibrary* GetCosmeticsLibrary() const;

	UMissionThemeLibrary* GetMissionThemeLibrary() const;

	UMobIconLibrary* GetMobIconLibrary() const;

	USkinsLibrary* GetSkinsLibrary() const;

	UEndlessStruggleLibrary* GetEndlessStruggleLibrary() const;

	UEntitlementsRepository* GetEntitlementsRepository() const;

	// D11.PS
	// D11.DB - Made this public (see the baseplayercontroller).
	// D11.SSN - added onKeyUp delegate
	FOnInputKeyPressedDelegate OnInputKeyPressedDelgate;
	FOnInputKeyReleasedDelegate OnInputKeyReleasedDelgate;

	int GetPIEClientIndex() const;

	UPROPERTY(BlueprintAssignable)
	FOnSaveStateLoaded OnSaveStateLoaded;

	UPROPERTY(BlueprintAssignable)
	FOnUserNotLoggedIn OnUserNotLoggedIn;

	//D11.KS - Used for deciding if the local player should do post login.
	TArray<ULocalPlayer*> localPlayersThatCanLogin;

	UFUNCTION(BlueprintImplementableEvent, Category = "Dungeons")
	void PushGlobalErrorMessage(const EGlobalMessageTypes& GlobalMessageType);

	UFUNCTION(BlueprintImplementableEvent, Category = "Dungeons")
	void CreateMessage(const FString& Text, EMessageType messagetype);

	// #D11.CM
	UFUNCTION(BlueprintCallable)
	SwitchOperationMode GetSwitchOperationMode();

	// #D11.CM
	void SwitchOperationModeChanged();

	// #D11.CM
	UPROPERTY(BlueprintAssignable)
	FOnSwitchOperationModeChanged OnSwitchOperationModeChanged;

	// D11.DB
	UPROPERTY()
	FVector CoopCameraLookAtCache;

	//D11.KS - For local coop we should only need to calculate if the mobs are valid and are on screen once instead of each player doing it.
	TArray<TWeakObjectPtr<AMobCharacter>> OnScreenMobs;
	TArray<TWeakObjectPtr<AMobCharacter>> MobsInOuterScreenPadding;

	// D11.DB
	UPROPERTY()
	UCurveFloat* GammaCurve;

	//D11.KS - Moved from Gamemode as we need persistance across lobby and in-game, this gets cleared in the Menu Gamemode.
	UPROPERTY(BlueprintReadOnly)
	TArray<int> FreeSlots;
	int PlayerNumberCounter = 0;

	int GetMaxLocalPlayers();

	APlayerCharacter* GetFirstPlayerCharacter();
	
	void OnControllerPairingChangedCommand(int32 LocalUserNum);
	
	void KickBackPlayerToMenu(EBootMode Reason);
	//D11.KS
	FOnLocalPlayerLeave OnLocalPlayerLeave;

	UFUNCTION(BlueprintCallable)
	void SaveGlobalState(int32 LocalUserNum);

	UFUNCTION(BlueprintCallable)
	void LookForInvites();

	bool IsLoadingScreenActive() const;

	UFUNCTION(BlueprintCallable)
	bool IsLoadingScreenShown() const;

	UFUNCTION(BlueprintCallable)
	bool HasBaseGameEntitlement() const;

	UFUNCTION(BlueprintCallable)
	void UpdateUsingMultiplayerFeatures(bool bIsUsingMultiplayerFeatures);

	UFUNCTION(BlueprintCallable)
	UBlockingMessageBoard* GetBlockingMessages() const;
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool ShouldDisableOnlineGame();

	UFUNCTION(BlueprintCallable)
	void ForceLeaveRecentlyLeftControllers();

	void UpdateRecentlyLeftControllers(float fTimeDelta);

	UFUNCTION(BlueprintImplementableEvent, Category = "Dungeons")
	void OnClientKicked();

	UFUNCTION(BlueprintCallable)
	bool IsInMenu() const;

	UFUNCTION(BlueprintCallable)
	bool IsControllerIdInGracePeriod(int32 controllerID) const;

	UFUNCTION(BlueprintCallable)
	void LoadSaveData(int LocalPlayerIndex, APlayerController *PlayerController);

	void OnAchievementUnlocked(FString achievementName, FString achievementURL, FString achievementTitle, FString achievementDesc);

	int GetSessionNumPlayers();

	void CheckKickToTitleScreenReasons();

	const FString& GetAcceptedInvite() const;

	void LoadGlobalSaveData(int32 localUserNum = 0);

	UFUNCTION(BlueprintImplementableEvent, Category = "Dungeons")
	void OnNetworkError(ENetworkFailure::Type ErrorType, bool bIsServer);

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Online|Sessions")
	void CheckMultiplayerFeatures();

	//D11.KS
	UFUNCTION(BlueprintCallable)
	void TitleScreenKickback(EBootMode Mode);

	UFUNCTION(BlueprintCallable)
	void CleanUpGameRenderTargets();
	
	static void SetFastLoadEnabled(bool bEnabled);

	// D11.DJB - Auto-detect
	AutoDetect* GetAutoDetect() { return mAutoDetect.get(); }

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnLevelStart OnLevelStart;

	void LevelStarted();

	void NotifyUserOfLowPerformanceDetected(const FText& NotifyMessageBody);

	UInventoryItemDataHolder* GetInventoryItemDataHolder() const { return InventoryDataholder; }

public:
	TArray<APlayerCharacter*> GetAllPlayerCharacters();


	UFUNCTION(BlueprintCallable)
	ETitle GetPlayerTitle(int32 LocalUserNum);

	UFUNCTION(BlueprintCallable)
	void SetPlayerTitle(int32 LocalUserNum, ETitle title);

protected:

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsConsoleCurrentlySaving() const;

	UGlobalStateData* mSaveState = nullptr;

	UPROPERTY()
	TMap<int32, UGlobalStateData*> mSaveStates;

	//D11.SC
	TArray< FRecentlLeftControllerData > mRecentlyLeftControllers;

	TOptional<FMissionFinishedSummary> mMissionFinishedSummary;

	FLevelSettings LevelSettingsLastStarted;	

	void OnSessionFailure(const FUniqueNetId&, ESessionFailure::Type);

	FOnSessionFailureDelegate OnSessionFailureDelegate;
	FDelegateHandle OnSessionFailureDelegateHandle;

	void OnSessionInviteReceived(const FUniqueNetId& UserId, const FUniqueNetId& FromId, const FString& AppId, const FOnlineSessionSearchResult& InviteResult);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "SessionInviteReceived"), Category = "Dungeons|Online|Sessions")
	void SessionInviteReceived(const FBlueprintGameSession& Session);

	FOnSessionInviteReceivedDelegate OnSessionInviteReceivedDelegate;
	FDelegateHandle OnSessionInviteReceivedDelegateHandle;

	void HandleInviteAcceptedError();

	void OnSessionInviteAccepted(const bool bSuccess, const int32 ControllerId, TSharedPtr<const FUniqueNetId> PlayerUniqueNetId, const FOnlineSessionSearchResult& sessionResult);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "SessionInviteAccepted"), Category = "Dungeons|Online|Sessions")
	void OnSessionInviteAcceptedBP(const bool bSuccess, const int32 remoteControllerId);

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Online|Sessions")
	void ProcessAcceptedInvite();

	FOnSessionUserInviteAcceptedDelegate OnSessionInviteAcceptedDelegate;
	FDelegateHandle OnSessionInviteAcceptedDelegateHandle;

	void Logout(int32 LocalUserNum);

	void OnLoginStatusChanged(int32 LocalUserNum, ELoginStatus::Type OldStatus, ELoginStatus::Type NewStatus, const FUniqueNetId& NewId);
	void OnWriteConnectiontypeTelemetry(bool IsUsingRelay);
	void OnControllerPairingChanged(int32 LocalUserNum, const FUniqueNetId& PreviousUser, const FUniqueNetId& NewUser);

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnLocalPlayerChangedUser OnLocalPlayerChangedUser;

	void ClearSaveStates();

public:
	//D11.RR
	void OnLoginLocalPlayerComplete(int localUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);
	FOnLoginCompleteDelegate OnLoginCompleteDelegate;
	FDelegateHandle OnLoginCompleteDelegateHandle;
	//D11.RR
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool HasDLCEntitlements(EDLCName dlcName) const;
protected:

	FOnControllerPairingChangedDelegate OnControllerPairingChangedDelegate;
	FDelegateHandle OnControllerPairingChangedDelegateHandle;

	FOnLoginStatusChangedDelegate OnLoginStatusChangedDelegate;
#if defined(SUBSYSTEM_HAS_TELEMETRY_CALLBACKS)
	FOnWriteConnectionTypeTelemetryDelegate OnWriteConnectionTypeTelemetryDelegate;
#endif
	FDelegateHandle OnLoginStatusChangedDelegateHandle[MAX_LOCAL_PLAYERS];

	FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnGlobalSaveDataLoadFinished OnGlobalSaveDataLoadFinished;
	UFUNCTION()
	void OnGlobalSaveDataLoaded(UGlobalStateData* saveData, int32 systemId);

	void UnlockCachedAchievements(UGlobalStateData* saveData, int32 systemId);

	void WriteAchievements(TSharedPtr<const FUniqueNetId> netId, std::bitset<128> &achievementList);

	void LoadAllLocalControllerSaveData() const;

	UFUNCTION()
	void OnUserAdded(int LocalPlayerIndex, APlayerController* PlayerController);

	UFUNCTION()
	void UpdateControllerLocalPlayerDisplayIndex();

	//D11.KS
	UFUNCTION(BlueprintCallable, Meta = (ExpandEnumAsExecs = "Branches"))
	APlayerController* HandleLocalPlayerRequest(int32 incomingID, FKey incomingKey, StatusEnum& Branches);

	APlayerController* GetPlayerControllerFromIndex(int32 incomingID);

	UFUNCTION(BlueprintCallable)
	APlayerController* GetPlayerControllerFromControllerID(int32 ControllerID);

	//D11.KS
	UFUNCTION(BlueprintCallable)
	bool HandleLocalPlayerLeave(int32 incomingID);

	void AddRecentlyLeftController(int32 incomingID, const float fGracePeriod);
	void RemoveRecentlyLeftController(int32 incomingID);

	bool LocalPlayerLeave(int32 incomingID);

	//D11.PC
	UFUNCTION()
	void HandleControllerConnectionChange(bool bIsConnection, int32 GameUserIndex);
	
	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnEntitlementsReceived OnEntitlementsReceived;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnUnlinkTaskFinished OnUnlinkTaskFinished;

	UPROPERTY(BlueprintAssignable, Category = "Achievement")
	FOnAchievementUnlockedWithImageURL OnAchievementUnlockedWithImageURL;

	UFUNCTION(BlueprintImplementableEvent, Category = "Dungeons")
	void OnDebugPromptMessage(const FText& DebugMessage);

	//D11.PS
	FDelegateHandle OnInputKeyPressedDelgateHandle;
	//D11.SSN
	FDelegateHandle OnInputKeyReleasedDelgateHandle;

	void OnInputKeyPressed(int ControllerId, FKey Key);
	void OnInputKeyPressed(int ControllerId, FKey Key, bool isRepeat = false);
	void OnInputKeyReleased(int ControllerId, FKey Key);
	FReply OnUnhandledKeyDown(const FKeyEvent& InKeyEvent);

	UPROPERTY()
	AAudioMusicManager* AudioMusicManager;

	UPROPERTY()
	AAudioSFXManager* AudioSFXManager;
	
	UPROPERTY()
	USoundMixManager* SoundMixManager;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	TSubclassOf<AAudioMusicManager> AudioMusicManagerBP;
	
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	TSubclassOf<AAudioSFXManager> AudioSFXManagerBP;

	// D11.SSN - D11.PS - Changed this to an enum
	UPROPERTY(BlueprintReadWrite, Category = "Dungeons")
	EBootMode BootMode = EBootMode::InitialBoot;

	UFUNCTION(BlueprintImplementableEvent, Category = "Dungeons")
	void OnNonInitialUserAcceptedInvite();

	UFUNCTION(BlueprintImplementableEvent, Category = "Dungeons")
	void OnInviteAcceptedOnLocalPlay();

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Online|Sessions")
	void SetPlayerWasKicked(bool bPlayerKicked);
	UFUNCTION(BlueprintCallable, Category = "Dungeons|Online|Sessions")
	bool GetPlayerWasKicked();

	UFUNCTION(BlueprintCallable, Category = "Dungeons|UI")
	UUserWidget*	GetCachedUIWidget(TSubclassOf< UUserWidget > WidgetClass);
	
	UFUNCTION(BlueprintCallable, Category = "Dungeons|UI")
	void			SetCachedUIWidget(UUserWidget*  pWidget );

	UFUNCTION(BlueprintCallable, Category = "Dungeons|UI")
	void			ClearCachedUIWidgets();


private:

	UPROPERTY()
	UInventoryItemDataHolder* InventoryDataholder = nullptr;

	static FCriticalSection authLock;

	// D11.DJB
	Unique<AutoDetect> mAutoDetect;

	void WindowDidResize(FViewport* viewport, uint32 value);

	void InitialiseUserSettingsFromSaveData(APlayerControllerBase* PlayerControllerBase);

	// #D11.CM - Popping
	bool PopPlayers();

	// #D11.CM - Popping
	TArray<TWeakObjectPtr<APlayerCharacter>> mWaitingToPop;

	TWeakObjectPtr<APlayerCharacter> lastLeader;

	// #D11.CM - Popping
	float mPopTimer = 0;

	const int mPlayerCountPopThreshold = 2;
	const float mPopTimerThreshold = 6;

	ReconnectStates ReconnectStates;
	int mPIEClientIndex = 0;

	static const uint32 DEFAULT_MAX_FPS;
	
	static const float REFRESH_ENTITLEMENTS_DELAY_SECONDS;

	EBootMode KickBackToTitleScreenReason = EBootMode::Unset;

	UPROPERTY()
	class UJoinDungeonsSessionCallbackProxy* JoinSessionProxy;

	UPROPERTY()
	UCreateDungeonsSessionCallbackProxy* CreateSessionProxy;
	
	UPROPERTY()
	UDungeonsFriendsInterface* FriendsInterface;

	UPROPERTY()
	UFindSessionHandler* FindSessionHandler;

	UPROPERTY()
	UControllerTypeManager* ControllerTypeManager;

	UPROPERTY()
	UDungeonsUserManager * UserManager;

	UPROPERTY()
	UDungeonsLoginFlow * LoginFlow;

	UPROPERTY()
	UVirtualKeyboardManager* KeyboardManager;

	UPROPERTY()
	UDungeonsAssetManager* AssetManager;

	UPROPERTY()
	UDungeonsInventoryAssetManager* InventoryAssetManager;

	UPROPERTY()
	ULoadingScreenInitializer* LSInitializer;

	UPROPERTY()
	UHintManager* HintManager;

	UPROPERTY()
	UConnectionChecker* ConnectionChecker;
		
	TSharedPtr<game::loadingscreen::LoadingScreenBuilder> LSBuilder;

	bool bFirstLoad = true;
	Unique<logissues::Detectors> LogDetectors; // We can do with a single one for now

	UPROPERTY()
	UMinecraftAPI* MinecraftAPI;

	UPROPERTY()
	UCosmeticsLibrary* CosmeticsLibrary;

	UPROPERTY()
	UMissionThemeLibrary* MissionThemeLibrary;

	UPROPERTY()
	UMobIconLibrary* MobIconLibrary;
	
	UPROPERTY()
	USkinsLibrary* SkinsLibrary;

	UPROPERTY()
	UEndlessStruggleLibrary* EndlessStruggleLibrary;


	UPROPERTY()
	UEntitlementsRepository* EntitlementsRepository;

	void LoadCachedEntitlements(UGlobalStateData*) const;
	void SaveCachedEntitlements(const TArray<FEntitlement>&) const;

#if PLATFORM_SWITCH
	SwitchOperationMode mCurrentOperationMode;

	void OnChangeHandheldScreenPercentage(IConsoleVariable* Var);

	void OnChangeConsoleScreenPercentage(IConsoleVariable* Var);
	
	static void LogSessionInterval(FString s);
	float		PollSessionTimerCurrentTime = 0.0f;
#endif

#ifdef EPIC_STORE_BUILD
	void InitEpicSDK();
	void OnEpicGameServicesSignInFailed();
	FTimerHandle EpicSignInRetryTimer{};
#endif
	UFUNCTION()
	void OnReadyToSignIntoEGS(int32 playerIndex);

	void LocalPlayerAddedToViewport(int playerIndex);
	void LocalPlayerRemovedFromViewport(int playerIndex);

	void OnApplicationEnterBackground();
	void OnApplicationEnterForeground();

	void InitTelemetry(int LocalPlayerIndex, APlayerController *PlayerController);

	UFUNCTION(BlueprintCallable)
	void AssignOnlineEvents();
	UFUNCTION(BlueprintCallable)
	void ClearOnlineEvents();
	
	UFUNCTION()
	void OnFireHeartBeatAnalytics();
	FTimerHandle HeartBeatTimerHandle;

	void OnEntitlementsProvidedByRepository(const TArray<FEntitlement>&);
	void RefreshEntitlements() const;

	//D11.PC
	FDungeonsPendingInvite PendingInvite;
	FString AcceptedInvite;

	void KickAllLocalPlayers();

	float mSwitchConsoleScreenPercentage = 83.33;
	float mSwitchHandheldScreenPercentage = 100.0f;

	void SetScreenPercentage(float percentage);

	void OnLightFunctionQualityLevelSet(int qualityLevel, APlayerController* playerController);

	UPROPERTY()
	UBlockingMessageBoard* BlockingMessages;

	FTimerHandle RefreshEntitlementsTimerHandle;
	FTimerHandle PollSessionTimerHandle;
	FTimerHandle PollSessionFinishHandle;


	int ProtectionCodeIntegrityCheck;
	int EnforcedCodeIntegrityCheck;
	int UserCodeIntegrityCheck;
	int DebuggerIntegrityCheck;

	bool bPlayerWasKicked = false;

	bool CheckpointRestartCooldown = false;
	FTimerHandle CheckpointRestartCooldownTimer;
	int CheckpointRestartCooldownTime = 10;

	uint8 SelectedGameMode = 1u;

	//used to cache interface widgets, so we can re-use them instead of creating them for every level
	UPROPERTY()
	TMap< TSubclassOf< UUserWidget >, UUserWidget* > mUIInterfaceWidgetCache;

	TArray<FTrialDef> TrialDefs;
	TOptional<FString> TrialIdLastStarted;
};
