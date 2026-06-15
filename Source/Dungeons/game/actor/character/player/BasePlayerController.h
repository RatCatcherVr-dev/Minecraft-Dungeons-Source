#pragma once

#include "game/actor/character/player/PlayerControllerBase.h"
#include "game/component/PlayerPathFollowingComponent.h"
#include "game/component/MeleeAttackComponent.h"
#include "game/component/HealthComponent.h"
#include "game/component/InteractableComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/input/AutoTarget.h"
#include "game/input/TargetController.h"
#include "game/input/MouseCursorStates.h"
#include "game/component/AwardsGeneratorComponent.h"
#include "game/component/MusicOverrideComponent.h"
#include "util/FrameTimeTracker.h"
#include "util/telemetry/PerformanceTelemetry.h"
#include "online/OnlineFunctionTypes.h"

#include "game/input/MouseInputStateMachine.h"
#include <GenericPlatform/ICursor.h>
#include "game/actor/TickInterleaving.h"
#include "ui/EnumDefine.h"
#include "game/component/drop/EyeOfEnderDefines.h"
#include "BasePlayerController.generated.h"

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPawnWasPossessed, APawn*, pawn);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDestructibleComponentDestroyed, UPrimitiveComponent*, component, FVector, location);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPostSeamlessTravelMessage, const char*);
DECLARE_MULTICAST_DELEGATE(FOnPreSeamlessTravelMessage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAnyPlayerActionFixed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnClientWasKicked, const FText&, reason);
DECLARE_DELEGATE_OneParam(FOnRangeAttackPrimary, bool);
// D11.DB
DECLARE_DELEGATE(FOnAlternativeRangeAttack);
//DECLARE_MULTICAST_DELEGATE(FOnAlternativeRangeAttack);

//Internal
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerMovement, const ABasePlayerController*);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnAnyPlayerAction, const ABasePlayerController*);

//These are broadcast in gamemode hooks, which means they will only be triggered on the server
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FServerOnLogout);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerTeleportButton);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTriggerPopup, EPopupDialogType, dialogType, FText, message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTriggerPopupGlobal, EPopupDialogType, dialogType, EGlobalMessageTypes, messageType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTriggerAchievement, FText, message);

class UPlayerPathFollowingComponent;

UENUM(BlueprintType)
enum class EPlayerStatusType : uint8 {
	UNSET,
	Joined,
	Left,
};
ENUM_NAME(EPlayerStatusType);

//D11.KS
UENUM(BlueprintType)
enum class ELocalPlayerCameraStatus : uint8
{
	InCinematic,
	InSafeZone,
	InDangerZone,
	InHardBoundary,
	OffScreen
};

UENUM(BlueprintType)
enum class EPlayerInputType : uint8 {
	Movement,
	Attack,
	Hotbar,
	Map,
	Inventory,
	Teleport,
	Social,
	TextChat,
	UI,
	Debug
};

UENUM(BlueprintType)
enum class EClickTargetType : uint8 {
	Target,
	Clicky
};

enum EDashState {
	Executing,
	NotExecuting
};

enum EControllerMovementCheckState {
	Move_Normal,					//Controller checks normal
	Move_Zero_Required,				//controller analogue requires release to zero
	Move_Deadzone_Check_Required,	//Controller requires analogue to breach deadzone amount
};

// #D11.CM
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerIdleChanged, const ABasePlayerController*, player, EPlayerIdleState, idle);

enum class ECustomMovementType : uint8;

UCLASS()
class DUNGEONS_API ABasePlayerController : public APlayerControllerBase
{
	friend class UWalkPickupComponent;
	GENERATED_BODY()
public:
	ABasePlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void OnActorChannelOpen(class FInBunch& InBunch, class UNetConnection* Connection) override;

	// D11.DB
	void SetupInputComponent() override;

	void OnGamepadActiveChanged(bool GamepadActive) override;

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Player")
		UPathFollowingComponent* GetPathFollowingComponent() const;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Post Seamless Travel"), Category = "Dungeons|Online")
		void OnPostSeamlessTravel();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Pre Seamless Travel"), Category = "Dungeons|Online")
		void OnPreSeamlessTravel();

	void SpawnPlayerCameraManager() override;

	void PostSeamlessTravel() override;

	void GetSeamlessTravelActorList(bool bToEntry, TArray<AActor*>& ActorList) override;

	void PreClientTravel(const FString & PendingURL, ETravelType TravelType, bool bIsSeamlessTravel) override;

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Player", Meta = (AdvancedDisplay = "bStopOnOverlap,bCanStrafe,bAllowPartialPath"))
		EPathFollowingRequestResult::Type MoveToLocation(const FVector& Dest, float AcceptanceRadius = -1, bool bStopOnOverlap = true,
			bool bUsePathfinding = true, bool bProjectDestinationToNavigation = false, bool bCanStrafe = true,
			TSubclassOf<UNavigationQueryFilter> FilterClass = nullptr, bool bAllowPartialPath = true);

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Player", Meta = (AdvancedDisplay = "bStopOnOverlap,bCanStrafe,bAllowPartialPath"))
		EPathFollowingRequestResult::Type MoveToActor(AActor* Goal, float AcceptanceRadius = -1, bool bStopOnOverlap = true,
			bool bUsePathfinding = true, bool bCanStrafe = true,
			TSubclassOf<UNavigationQueryFilter> FilterClass = nullptr, bool bAllowPartialPath = true);

	void AbortPathFollowing();

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Player")
		void DodgeInDirection(FVector direction);

	void Tick(float DeltaSecs) override;
	void TickWantsToDodge(float DeltaSeconds);

	void ClearDodge();

	void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
		bool IsInputCapturedByUI() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
		bool IsTeleportListOpen() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
		void SetInputCapturedByUI(bool Captured, bool CaptureLocalPlayers = true);

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
		void SetTeleportListOpen(bool ListOpen);

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
		bool IsInputCapturedByTeleport() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
		bool IsTeleporting() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
		TArray<APlayerCharacter*> GetTeleportCandidates() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
		TArray<APlayerCharacter*> GetTeleportDependents() const;

	UFUNCTION()
		void OnPlayerDeath();

	UFUNCTION()
		void OnPlayerRespawned();

	void OnPawnTeleported();

	void Reset() override;

	void OnPossess(APawn* InPawn) override;

	void OnUnPossess() override;

	void SetPawn(APawn* InPawn) override;

	void SetPlayer(UPlayer* InPlayer) override;

	void OnRep_Pawn() override;

	void ClientWasKicked_Implementation(const FText& KickReason) override;

	void OnPawnPossessedOnce(APawn* InPawn);
	void OnControllerReceivedPlayerOnce(APlayerController* InController);

	class AGameBP* GetCachedGameBP();

	void PostInitializeComponents() override;

	bool IsCustomMovementTypeActive(ECustomMovementType movementType);

	template<ESlotType type, int index, bool activate>
	void OnWantsToActivateSlot(FKey key)
	{
		if (IsInputTypeAllowed(EPlayerInputType::Hotbar, key.IsGamepadKey())) {
			SetWantsToActivateSlot(type, index, activate);
		}
	}

	UItemSlot* GetItemSlotOfType(ESlotType type, int index) const;

	UFUNCTION(BlueprintImplementableEvent)
		bool IsChatWheelVisible();

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Player")
		void SetWantsToActivateSlot(ESlotType type, int index, bool wantsToActivate);

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Player")
		void OnDodgeButton(bool pressed);

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Player")
		void OnDodgeForwardButton(bool pressed);


	UFUNCTION(BlueprintCallable, Category = "Dungeons|Player")
		void OnMoveButton(bool pressed);

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Player")
		void OnRangedAttackButton(bool pressed);

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Player")
		void OnRangedAttackButtonGamepadPressed();

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Player")
		void OnRangedAttackButtonGamepadReleased();

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Player")
		void OnMeleeAttackButton(bool pressed, bool gamepad = false);

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Player")
		void OnMeleeAttackButtonPressed();

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Player")
		void OnMeleeAttackButtonReleased();

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Player")
		void OnRangedTargetingModeToggle(bool pressed, bool gamepad = false);

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Player")
		void OnRangedTargetingModePressed();

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Player")
		void OnRangedTargetingModeReleased();

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Player")
		void OnRootPlayerGamepadPressed();

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Player")
		void OnRootPlayerGamepadReleased();

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Player")
		void OnDebugPointer(bool pressed);

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Player")
		void OnRootPlayer(bool pressed);

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Player")
		void OnHotbarItemUsed();

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Player")
		void OnDebugState();

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Player")
		bool HasValidLocalPlayer();

	//UFUNCTION(Server, Reliable, WithValidation)
	//void ServerDestroyDestroyableComponent(UPrimitiveComponent* component);

	//@todo: Multicasts don't make sense for playercontrollers, as a representation of *this* player controller only exists on the server and the owning client
	//UFUNCTION(NetMulticast, Reliable)
	//void MulticastDestroyedDestroyableComponent(UPrimitiveComponent* component, FVector location);

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
		int32 GetPlayerId() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
		int32 GetLocalPlayerIndex() const;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
		FServerOnLogout OnServerLogout;

	FOnPlayerMovement OnPlayerMovement;
	FOnAnyPlayerAction OnAnyPlayerAction;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
		FOnAnyPlayerActionFixed ReceiveOnAnyPlayerAction; //for interfaces, doesnt care about death

		// #D11.CM
	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
		FOnPlayerIdleChanged OnPlayerIdleChanged;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
		FOnTriggerPopup OnTriggerPopup;
	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
		FOnTriggerPopupGlobal OnTriggerPopupGlobal;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
		FOnTriggerAchievement OnTriggerAchivement;

	FOnPreSeamlessTravelMessage OnPreSeamlessTravelMessage;
	FOnPostSeamlessTravelMessage OnPostSeamlessTravelMessage;

	UPROPERTY(BlueprintCallable)
		FOnPlayerTeleportButton OnPlayerTeleportButton;

	AAutoTarget* GetAutoTargetSystem() { return mAutoTarget; }

	// D11.DB - This is for projecting the gamepad joystick direction from the player relative to the camera.
	FVector ProjectInputAxesFromPlayer(const FVector& Axes, float Distance = 100.0f, bool ProjectFromPlayerPos = true);

	FVector ProjectCursorDirectionFromPlayer();


	UPROPERTY()
		float mLastMouseX;
	UPROPERTY()
		float mLastMouseY;

	UPROPERTY()
		float mLastMoveX;
	UPROPERTY()
		float mLastMoveY;

	UPROPERTY()
		float mLastRollX;
	UPROPERTY()
		float mLastRollY;

	UPROPERTY()
		float mTargetCameraFOV = -1.0f;

	UPROPERTY()
		float mCurrentCameraFOV;

	// D11.DB - Required by some in-game menus to override right stick functionality.
	UPROPERTY(BlueprintReadWrite)
		bool RightStickDodgingEnabled = true;

	// D11.SSN - used to stop immediate rolling when coming out of some menus.
	UPROPERTY(BlueprintReadWrite)
		bool RightStickHasReset = true;

	UPROPERTY(BlueprintReadWrite)
		bool bIsUIInputAllowed = true;

	UFUNCTION()
		bool GetIsMoving();

	// #D11.CM - How many inactive seconds until the player is classed as "Idling".
	UPROPERTY(BlueprintReadWrite, Category = "Dungeons|Idling")
		float mPlayerIdleTime = 8.0f;

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Player")
		void SetPlayerIsImmovable(bool immovable);

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Player")
		void CenterMouseCursor();

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Player")
		void ResetMouseCursorToLastPosition();

	bool GetDebugState();

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Player|Debug")
		void ShowDebugInfo(bool showDebug);

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Player|Camera")
		void AdjustCameraFovBasedOnCharacterSpeed();

	UFUNCTION(BlueprintImplementableEvent, Category = "Dungeons")
		void ReceiveOnPawnPossessed(APawn* possessedPawn);

	UFUNCTION(BlueprintImplementableEvent, Category = "Dungeons")
		void ReceiveOnEnchantmentTriggered(EEnchantmentTypeID typeID, bool canCombo = true) const;

	UFUNCTION(BlueprintImplementableEvent, Category = "Dungeons")
		void ReceiveOnArmorPropertyTriggered(EArmorPropertyID typeID);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Dungeons")
		void ReceiveOnObjectiveSettingsChange(bool toogleNarrator);

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
		UItemStashComponent* GetItemStashComponent() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
		class UWalletComponent* GetWalletComponent() const;

	bool IsTargetHighlightingAllowed() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
		void CancelCurrentInputActions();

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
		bool IsInputTypeAllowed(EPlayerInputType InputType, bool gamepad = false) const;

	void ToggleTargetingState(bool enter);
	int32 GetEmeraldsCount() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
		FString GetPlayerDisplayName() const;
	FString GetPlayerPrimaryDisplayName() const;
	FString GetPlayerSecondaryDisplayName() const;

	bool IsDisplayNameAssigned() const;

	class UAutoAimRangedAttackComponent* GetRangedAttackComponent();

	UCharacterSerializeComponent* GetCharacterSerializeComponent() const;

	UMeleeAttackComponent* GetMeleeAttackComponent() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
		APlayerCharacter* GetControlledPlayerCharacter() const;

	void RotatePlayerTowardsCursor();

	FORCEINLINE TargetController& GetTargetController() { return mTargetController; }
	FORCEINLINE const TargetController& GetTargetController() const { return mTargetController; }
	class UPickupStorableComponent* GetPickupStorableComponent() const;

	UFUNCTION(BlueprintImplementableEvent)
		void OnLocationClicked(FVector location);

	UFUNCTION(BlueprintImplementableEvent)
		void OnActorClicked(AActor* actor, EClickTargetType type);

	UFUNCTION(BlueprintImplementableEvent)
		void OnHighlightTargetChanged(AActor* newTarget, AActor* oldTarget);

	void OnRangeAttackPrimary(bool isPressed);

	// D11.DB
	UFUNCTION(BlueprintCallable)
		void OnRangeAttackAxisSecondary(float value);

	// #D11.CM
	UFUNCTION(BlueprintCallable)
		void OnCancelTeleport(bool forceCancel);

	// D11.DB
	void RefreshDeadzones();

	UFUNCTION(BlueprintImplementableEvent)
		void OnPlayForceFeedbackEvent(FName eventName, bool looping = false);

	UFUNCTION(BlueprintImplementableEvent)
		void OnStopForceFeedbackEvent(FName eventName);

	UFUNCTION(BlueprintImplementableEvent)
		void OnSecondaryHighlightTargetChanged(const TArray<AMobCharacter*>& targets, AMobCharacter* closest/*AActor* newTarget, AActor* oldTarget*/);

	UFUNCTION(BlueprintImplementableEvent)
		void UpdateInitialBlueprintSetup();

	UFUNCTION(BlueprintNativeEvent)
		bool InitialBlueprintSetupComplete();

	UFUNCTION(BlueprintPure, Category = "Dungeons|Audio")
		FVector GetAudioListenerLocation();

	//D11.KS
	ELocalPlayerCameraStatus CurrentCharacterScreenStatus = ELocalPlayerCameraStatus::InSafeZone;

	// D11.DB
	FOnAlternativeRangeAttack OnAlternativeRangeAttack;

	void MoveTowards(const FVector& point);

	bool GetAnyActionThisHeartbeatAndReset();

	void TriggerControllerMovementCheck() { mCurrentControllerMovementCheck = Move_Zero_Required; };

	AActor* GetCurrentTarget() const;

	void OnHealthFractionChanged(float newFraction, float oldFraction, const UHealthComponent* component);

	// D11.SSN
	UFUNCTION(BlueprintImplementableEvent, Category = "Dungeons")
		void OnAnyPlayerDamaged(float damage);

	void ResetMouseStates();

	UFUNCTION(Client, Reliable, WithValidation)
		void ClientDropItems(const FItemDropSource& dropSource, const FNetworkedItemDropData& dropData, AActor* source, bool replicateItems, bool elongatedDrops, float dropDelay, float dropDelayCountSpeedUpFactor, EItemRarityChanceCategory rarityChanceCategory);

protected:
	// --- WASD MOVEMENT CONTROLS ---
	void MoveForward(float Value);
	void MoveRight(float Value);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|Player")
		TSubclassOf<UNavigationQueryFilter> DefaultNavigationFilterClass;

	UPROPERTY()
		AActor* SecondaryHighlightTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|Player")
		float AttackTargetRetargetingWindowSeconds = 0.5f;

	UPROPERTY(VisibleDefaultsOnly, Category = "Dungeons|Player")
		class UPlayerPathFollowingComponent* PathFollowingComponent;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
		FOnDestructibleComponentDestroyed OnDestructibleComponentDestroyed;


	/** Radius limit for when clickies get a target reticule. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dungeons")
		float ClickyCollisionRadiusTargetLimit = 125.f;

	//D11.KS/DJB
	/* Returns whether or not the local player is within the hard boundary of the screen and zero's the associated vector component to prevent leaving the screen space */
	bool IsLocalCoopMovingIntoHardBoundary(FVector2D& movementValue);
	/* Returns whether or not the coop player is within the danger zone of the screen */
	bool CanLocalCoopRoll(const FVector& rollVector);

public:


	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
		float TargetReacquisitionTimeoutSeconds = 0.35f;

	class UObjectDistanceNotifier* InitializeAndGetDistanceNotifier();

	FrameTimeTracker& GetFrameTimeTracker();
protected:
	UPROPERTY(BlueprintReadWrite, Category = "Dungeons|Destructibles")
		bool DestructibleDebugVisuals = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
		bool EnableDebug = false;

public:
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerInteract(class UReplicatedInteractableComponent* interactable, AActor* interactor);

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerSpendEye(class AStrongholdFrame* strongholdFrame, const TArray<EEyeOfEnderType>& eyesSpent, AActor* spender);

	UFUNCTION(Client, Reliable, WithValidation)
		void ClientFailSpendEye(class AStrongholdFrame* strongholdFrame);

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerSetNetConnectionSpeed(int32 NewNetSpeed);

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
		void ServerRequestMoveEveryoneToLobby();

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerNotifyIdleChange(EPlayerIdleState idleState);

	// server RPC to propagate client -> server console commands
	// caller for client console commands to run on the server (WARNING, only enable for dev. builds)
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_DeferredConsoleCommand(const FString& command);

	bool ShouldTickHighlight() const;

	void PlayerLoadedInLevel();

	UFUNCTION(BlueprintPure, Category = "Dungeons")
		bool IsInitialSetupDone();

	void ToggleStateChangeAllowed(bool allow);

protected:
	UFUNCTION(Server, BlueprintCallable, Reliable, WithValidation)
		void ServerRequestAwards();

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
		bool ClickyClicked(UInteractableComponent* clicky);

	void NotifyLoadedWorld(FName WorldPackageName, bool bFinalDest) override;

	void UpdateMouseCursorState();
	EMouseCursor::Type GetMouseCursor() const override;


private:
	//D11.KS
	bool UpdateInitialSetup();
	int mInitialSetupStage = 0;
	int32 desiredNetSpeed;

	// D11.DJB
	bool mLocalPlayerInHardBoundary = false;

	void UpdateSecondaryTargets();

	bool AreMobArraysTheSame(TArray<AMobCharacter *>& array1, TArray<AMobCharacter *>& array2);

	void OnAnyPlayerActionPerformed();

	void OnPlayerMoveFinished(FAIRequestID RequestID, const FPathFollowingResult& Result);

	void RefreshPlayerIdle(bool forceRefresh = false);

	UFUNCTION()
		void OnPlayerMovementChanged(bool isMoving);

	UFUNCTION()
		void OnPlayerIdleTimer();

	bool IsRevivePrioritised() const; // D11.DJB

	EMouseCursorStates MouseCursorState;

	input::MouseInputStateMachine mMouseInputStateMachine;

	void UpdateCursorTargetSelection(void);
	void UpdateGamepadTargetSelection(float DeltaTime); // D11.DB

	enum class ERangeState { Idle, Root, Attack };
	ERangeState PrevRangeState;
	void OnRangeStateChange(ERangeState prev, ERangeState next);
	float RangeStateRoot;

	FTimerHandle mDelayedLaunchTimerHandle;

	bool playerIsImmovable;

	FPathFollowingRequestResult MoveTo(const FAIMoveRequest& MoveRequest, FNavPathSharedPtr* OutPath = nullptr);

	virtual FAIRequestID RequestMove(const FAIMoveRequest& MoveRequest, FNavPathSharedPtr Path);

	virtual void FindPathForMoveRequest(const FAIMoveRequest& MoveRequest, FPathFindingQuery& Query, FNavPathSharedPtr& OutPath) const;

	bool BuildPathfindingQuery(const FAIMoveRequest& MoveRequest, FPathFindingQuery& Query) const;

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable)
		void ResetAllStates();

	void UpdateVelocityAccumulator(float deltaTime);

	// #D11.CM
	void UICaptureLocalPlayers(bool Captured);

	UPROPERTY(replicated)
		bool IsHostingPlayer;

	TargetController mTargetController;

	UPROPERTY()
		AAutoTarget* mAutoTarget;

	UPROPERTY()
		class UObjectDistanceNotifier* PlayerDistanceNotifier;

	TWeakObjectPtr<AGameBP> CachedGameBP;

	float mStopAttackTargetRetargetingTimestamp;

	bool bInputCapturedByUI = false;
	bool bTeleportUIListOpen = false;

	bool bDebugState = false;

	bool showDebugInfo = false;

	bool mDestructibleLock = false;

	float mSavedMouseX = 0;
	float mSavedMouseY = 0;

	APawn* LastPawn = nullptr;
	class UPickupStorableComponent* PickupStorableComponent;
	class UReconnectComponent* ReconnectComponent;

	int lowVelocityCounter = 0;
	uint32 mTickStage = 0;

	UPROPERTY()
		TArray<AMobCharacter*> LastSecondaryTargets;
	UPROPERTY()
		AMobCharacter* CurrentSecondaryTarget;

	/*UPROPERTY()
	AMobCharacter* mEngagedMusicOverrideMob;
	*/
	bool bPlayerActionThisHeartBeat = false;

	EControllerMovementCheckState mCurrentControllerMovementCheck = Move_Normal;

	FrameTimeTracker mFrameTimeTracker;
	PerformanceTelemetry mPerfTelemetry;

	UTargetingTickStageComponent* TargetTicker = nullptr;

	FTimerHandle PlayerIdleTimerHandle;
	EPlayerIdleState CurrentIdleState = EPlayerIdleState::Active;

	UFUNCTION()
		void OnFirePerformanceAnalytics();

	FTimerHandle PerformanceTimerHandle;
	float FollowTheLeaderCountDown = 0.0f;

	float mTryToDodgeForSeconds = -1.0;
	FVector mDoDodgeDirection;

	class URangedAttackComponent* RangedAttackComponent = nullptr;

	static const float TRY_TO_DODGE_SECONDS;
};