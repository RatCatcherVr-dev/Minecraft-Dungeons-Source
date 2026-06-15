#include "Dungeons.h"
#include "BasePlayerController.h"

#include "AbilitySystemComponent.h"
#include "BasePlayerState.h"
#include "DungeonsGameMode.h"
#include "DungeonsGameState.h"
#include "Engine/LocalPlayer.h"
#include "Kismet/KismetMathLibrary.h"
#include "UnrealNetwork.h"
#include "game/GameBP.h"
#include "game/abilities/attributes/MovementAttributeSet.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/actor/character/player/DungeonsLocalPlayer.h"
#include "game/actor/StrongholdFrame.h"
#include "game/component/AutoAimRangedAttackComponent.h"
#include "game/component/InteractableComponent.h"
#include "game/component/EquipmentComponent.h"
#include "game/component/ReplicatedInteractableComponent.h"
#include "game/component/PickupStorableComponent.h"
#include "game/component/ReconnectComponent.h"
#include "game/component/ChatComponent.h"
#include "game/component/WalletComponent.h"
#include "game/component/PlayerCharacterMovementComponent.h"
#include "game/component/movement/MovementFlyingCommon.h"
#include "game/util/ActorQuery.h"
#include "game/util/ComponentUtils.h"
#include "game/util/Log.h"
#include "game/level/sound/AudioMusicManager.h"
#include "online/OnlineConsoleCommands.h"
#include <GameFramework/HUD.h>
#include "online/reconnect/ReconnectUtil.h"
#include "GameFramework/PlayerInput.h"
#include "GameSettingsFunctionLibrary.h"
#include "lovika/LovikaLevelActor.h"
#include "LoadingScreen/LoadingScreenInitializer.h"

#if !UE_BUILD_SHIPPING
#include "DrawDebugHelpers.h"
#endif
#include <NavigationSystem.h>
#include <SlateApplication.h>
#include "game/ObjectDistanceNotifier.h"
#include "online/sessions/OnlineUtil.h"
#include "ui/MissionProgressHandler.h"
#include "game/Enchantments/DeathBarter.h"
#include "DungeonsUserManagement.h"
#include "world/entity/MobTags.h"
#include "game/component/drop/ItemDropComponent.h"
#include "game/util/DungeonsGearUtilLibrary.h"

DECLARE_CYCLE_STAT(TEXT("STAT_ABasePlayerController_Tick"), STAT_ABasePlayerController_Tick, STATGROUP_PlayerController);
DECLARE_CYCLE_STAT(TEXT("STAT_ABasePlayerController_Tick_GetInputMouseDelta"), STAT_ABasePlayerController_Tick_GetInputMouseDelta, STATGROUP_PlayerController);
DECLARE_CYCLE_STAT(TEXT("STAT_ABasePlayerController_Tick_GetControlledPlayerCharacter"), STAT_ABasePlayerController_Tick_GetControlledPlayerCharacter, STATGROUP_PlayerController);
DECLARE_CYCLE_STAT(TEXT("STAT_ABasePlayerController_Tick_SetFOV"), STAT_ABasePlayerController_Tick_SetFOV, STATGROUP_PlayerController);
DECLARE_CYCLE_STAT(TEXT("STAT_ABasePlayerController_Tick_RefreshDeadzones"), STAT_ABasePlayerController_Tick_RefreshDeadzones, STATGROUP_PlayerController);
DECLARE_CYCLE_STAT(TEXT("STAT_ABasePlayerController_Tick_mTargetControllerUpdate"), STAT_ABasePlayerController_Tick_mTargetControllerUpdate, STATGROUP_PlayerController);
DECLARE_CYCLE_STAT(TEXT("STAT_ABasePlayerController_Tick_UpdateTargetSelection"), STAT_ABasePlayerController_Tick_UpdateTargetSelection, STATGROUP_PlayerController);
DECLARE_CYCLE_STAT(TEXT("STAT_ABasePlayerController_Tick_mMouseInputStateMachine"), STAT_ABasePlayerController_Tick_mMouseInputStateMachine, STATGROUP_PlayerController);
DECLARE_CYCLE_STAT(TEXT("STAT_ABasePlayerController_Tick_UpdateHighlight"), STAT_ABasePlayerController_Tick_UpdateHighlight, STATGROUP_PlayerController);
DECLARE_CYCLE_STAT(TEXT("STAT_ABasePlayerController_Tick_UpdateMouseCursorState"), STAT_ABasePlayerController_Tick_UpdateMouseCursorState, STATGROUP_PlayerController);
DECLARE_CYCLE_STAT(TEXT("STAT_ABasePlayerController_Tick_UpdateVelocityAccumulator"), STAT_ABasePlayerController_Tick_UpdateVelocityAccumulator, STATGROUP_PlayerController);
DECLARE_CYCLE_STAT(TEXT("STAT_ABasePlayerController_Tick_UpdateSecondaryTargets"), STAT_ABasePlayerController_Tick_UpdateSecondaryTargets, STATGROUP_PlayerController);
DECLARE_CYCLE_STAT(TEXT("STAT_ABasePlayerController_Tick_IsAllowedToPerformAction"), STAT_ABasePlayerController_Tick_IsAllowedToPerformAction, STATGROUP_PlayerController);

DECLARE_CYCLE_STAT(TEXT("STAT_ABasePlayerController_UpdateRadialTargetSelection"), STAT_ABasePlayerController_UpdateRadialTargetSelection, STATGROUP_PlayerController);
DECLARE_CYCLE_STAT(TEXT("STAT_ABasePlayerController_mAutoTargetUpdate"), STAT_ABasePlayerController_mAutoTargetUpdate, STATGROUP_PlayerController);



TAutoConsoleVariable<float> CVarMovementCheckDeadzone(
	TEXT("Dungeons.Player.MovementCheckDeadzone"),
	0.7f,
	TEXT("Deadzone to check for analogue stick movement post a movement check trigger\n"),
	ECVF_Cheat);

TAutoConsoleVariable<int> CVarLocalPlayersFollowTheLeader(
	TEXT("Dungeons.Player.LocalPlayersFollowTheLeader"),
	0,
	TEXT("Makes Local Players follow the primary player by pathing to it evey second or so\n"),
	ECVF_Cheat);

TAutoConsoleVariable<int32> CVarDebugActiveActionKeybinds(
	TEXT("Dungeons.Keybinds.DebugActionKeybinds"),
	0,
	TEXT("Enables on-screen messages displaying active action keybindingss for the current controller type.\n"),
	ECVF_Cheat);

extern TAutoConsoleVariable<int32> CVarDebugDrawNetCullDistance;
extern TAutoConsoleVariable<int32> CVarPlayerRelativeDodgeRoll;
const float ABasePlayerController::TRY_TO_DODGE_SECONDS = 0.2f;

namespace {
	void EnableCollisionTrace(APawn* pawn, const FName& componentTag, ECustomTraceChannels channel) {
		const auto& comps = pawn->GetComponentsByTag(UBoxComponent::StaticClass(), componentTag);
		check(comps.Num() == 1 && "Expected exactly one plane component");
		Cast<UShapeComponent>(comps[0])->SetCollisionResponseToChannel(static_cast<ECollisionChannel>(channel), ECR_Block);
	}
}

enum
{
		EBasePlayerControllerSetupStage_Uninitialised = 0
	,	EBasePlayerControllerSetupStage_InitCharacter
	,	EBasePlayerControllerSetupStage_PollBlueprintInitialisation
	,	EBasePlayerControllerSetupStage_Finalize
	,	EBasePlayerControllerSetupStage_Complete
};

ABasePlayerController::ABasePlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PathFollowingComponent = CreateDefaultSubobject<UPlayerPathFollowingComponent>(TEXT("PathFollowingComponent"));
	PickupStorableComponent = CreateDefaultSubobject<UPickupStorableComponent>(TEXT("PickupStorableComponent"));
	ReconnectComponent = CreateDefaultSubobject<UReconnectComponent>(TEXT("ReconnectComponent"));
	TargetTicker = CreateDefaultSubobject<UTargetingTickStageComponent>(TEXT("TargetTicker"));

	desiredNetSpeed = GetDefault<UPlayer>()->ConfiguredInternetSpeed;
	PrevRangeState = ERangeState::Idle;
	PrimaryActorTick.bCanEverTick = true;
#if !UE_BUILD_SHIPPING
	EnableDebug = true;
#else
	EnableDebug = false;
#endif
	mInitialSetupStage = EBasePlayerControllerSetupStage_Uninitialised;
}

void ABasePlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);
	if (auto world = GetWorld()) {
		world->GetTimerManager().ClearTimer(mDelayedLaunchTimerHandle);
		world->GetTimerManager().ClearTimer(PlayerIdleTimerHandle);
	}
}

void ABasePlayerController::OnActorChannelOpen(class FInBunch& InBunch, class UNetConnection* Connection)
{
	Super::OnActorChannelOpen(InBunch, Connection);
	ServerSetNetConnectionSpeed(GetDefault<UPlayer>()->ConfiguredInternetSpeed);
}

// Line 181 should look exactly like this:
void ABasePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	check(InputComponent);

	InputComponent->BindAxis("MoveForward", this, &ABasePlayerController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ABasePlayerController::MoveRight);

	InputComponent->BindAxis("RangeAttackAxisSecondary", this, &ABasePlayerController::OnRangeAttackAxisSecondary);
	InputComponent->BindAction<FOnRangeAttackPrimary>("AlternativeAttackGamepad", IE_Pressed, this, &ABasePlayerController::OnRangeAttackPrimary, true);
	InputComponent->BindAction<FOnRangeAttackPrimary>("AlternativeAttackGamepad", IE_Released, this, &ABasePlayerController::OnRangeAttackPrimary, false);
	InputComponent->BindAction("MainAttackGamepad", IE_Pressed, this, &ABasePlayerController::OnMeleeAttackButtonPressed);
	InputComponent->BindAction("MainAttackGamepad", IE_Released, this, &ABasePlayerController::OnMeleeAttackButtonReleased);
	InputComponent->BindAction("ToggleRangedTarget", IE_Pressed, this, &ABasePlayerController::OnRangedTargetingModePressed);
	InputComponent->BindAction("ToggleRangedTarget", IE_Released, this, &ABasePlayerController::OnRangedTargetingModeReleased);
	InputComponent->BindAction("RootPlayerGamepad", IE_Pressed, this, &ABasePlayerController::OnRootPlayerGamepadPressed);
	InputComponent->BindAction("RootPlayerGamepad", IE_Released, this, &ABasePlayerController::OnRootPlayerGamepadReleased);

	InputComponent->BindAction("Use Potion Slot", IE_Pressed, this, &ABasePlayerController::OnWantsToActivateSlot<ESlotType::HealthPotion, 0, true>);
	InputComponent->BindAction("Use Potion Slot", IE_Released, this, &ABasePlayerController::OnWantsToActivateSlot<ESlotType::HealthPotion, 0, false>);

	InputComponent->BindAction("Use Item Slot 1", IE_Pressed, this, &ABasePlayerController::OnWantsToActivateSlot<ESlotType::ActivePermanent, 0, true>);
	InputComponent->BindAction("Use Item Slot 1", IE_Released, this, &ABasePlayerController::OnWantsToActivateSlot<ESlotType::ActivePermanent, 0, false>);

	InputComponent->BindAction("Use Item Slot 2", IE_Pressed, this, &ABasePlayerController::OnWantsToActivateSlot<ESlotType::ActivePermanent, 1, true>);
	InputComponent->BindAction("Use Item Slot 2", IE_Released, this, &ABasePlayerController::OnWantsToActivateSlot<ESlotType::ActivePermanent, 1, false>);

	InputComponent->BindAction("Use Item Slot 3", IE_Pressed, this, &ABasePlayerController::OnWantsToActivateSlot<ESlotType::ActivePermanent, 2, true>);
	InputComponent->BindAction("Use Item Slot 3", IE_Released, this, &ABasePlayerController::OnWantsToActivateSlot<ESlotType::ActivePermanent, 2, false>);

}



void ABasePlayerController::OnGamepadActiveChanged(bool GamepadActive)
{
	APlayerControllerBase::OnGamepadActiveChanged(GamepadActive);

	if (GamepadActive)
	{
		bShowMouseCursor = false;
		bEnableMouseOverEvents = false;
		FSlateApplication::Get().OnCursorSet();

		StopMovement();
	}
	else
	{
		bShowMouseCursor = true;
		bEnableMouseOverEvents = true;

		if (mAutoTarget)
		{
			mAutoTarget->Clear(*this);
		}
	}
}

void ABasePlayerController::BeginPlay() {
	Super::BeginPlay();
	UpdateInitialSetup();

	RangedAttackComponent = GetRangedAttackComponent();
}

bool ABasePlayerController::IsInputCapturedByUI() const {
	return bInputCapturedByUI;
}

bool ABasePlayerController::IsTeleportListOpen() const {
	return bTeleportUIListOpen;
}

bool ABasePlayerController::IsInputCapturedByTeleport() const {
	return bTeleportUIListOpen && GetGamepadActive();
}

bool ABasePlayerController::IsTeleporting() const
{
	if (GetControlledPlayerCharacter()) {
		return GetControlledPlayerCharacter()->GetTeleportState() != ETeleportState::Idle;
	}

	return false;
}

TArray<APlayerCharacter*> ABasePlayerController::GetTeleportCandidates() const
{
	TArray<APlayerCharacter*> candidates;
	if (GetControlledPlayerCharacter()) {
		candidates = GetControlledPlayerCharacter()->GetTeleportCandidates();
	}

	return candidates;
}

TArray<APlayerCharacter*> ABasePlayerController::GetTeleportDependents() const
{
	TArray<APlayerCharacter*> dependents;
	if (GetControlledPlayerCharacter()) {
		dependents = GetControlledPlayerCharacter()->GetTeleportDependents();
	}

	return dependents;
}

void ABasePlayerController::SetInputCapturedByUI(bool Captured, bool CaptureLocalPlayers) {
	bInputCapturedByUI = Captured;
	if (Captured) {
		CancelCurrentInputActions();
		mTargetController.ResetTarget();
		mTargetController.ResetTargetCandidate();
	}

	if (CaptureLocalPlayers) {
		UICaptureLocalPlayers(Captured);
	}
}

void ABasePlayerController::SetTeleportListOpen(bool listOpen) {
	bTeleportUIListOpen = listOpen;

	if (IsPrimaryPlayer()) {
		for (auto dependent : GetTeleportDependents()) {
			if (dependent->GetPlayerController()) {
				dependent->GetPlayerController()->SetTeleportListOpen(listOpen);
			}
		}
	}
}

int32 ABasePlayerController::GetLocalPlayerIndex() const
{
	if (ULocalPlayer* localPlayer = GetLocalPlayer())
	{
		return GetGameInstance()->GetLocalPlayers().Find(localPlayer);
	}

	return -1;
}

void ABasePlayerController::OnPlayerDeath() {
	auto playerCharacter = GetControlledPlayerCharacter();
	check(playerCharacter && "Must have controller character on death!");

	playerCharacter->GetCharacterMovement()->StopMovementImmediately();

	if (!IsLocalController()) return;

	CancelCurrentInputActions();
	mTargetController.Update();
	mAutoTarget->Clear(*this);

	ResetAllStates();

	playerIsImmovable = true;
}

void ABasePlayerController::OnPlayerRespawned() {
	StopMovement();
	playerIsImmovable = false;
}

void ABasePlayerController::OnPawnTeleported() {
	if (AHighlightController* highlightController = AHighlightController::CheckHighlightControllerExists(GetWorld())) {
		highlightController->DisableHighlightsForOneFrame();
	}
	ResetAllStates();
}

void ABasePlayerController::ClientWasKicked_Implementation(const FText& KickReason) {
	UE_LOG(LogMultiplayer, Log, TEXT("Client have been kicked from the server, reason='%s'"), *(KickReason.ToString()));

	ReconnectComponent->ClearReconnect();

	GetWorld()->GetGameInstance<UDungeonsGameInstance>()->OnClientKicked();
}

void ABasePlayerController::ResetAllStates() {
	if (!IsLocalController()) return;

	auto playerCharacter = GetControlledPlayerCharacter();
	if (playerCharacter == nullptr) return;

	playerCharacter->SetAttackState(EAttackState::None);
	GetControlledPlayerCharacter()->GetCharacterMovement()->StopMovementImmediately();

	mMouseInputStateMachine.Reset(*this, mTargetController);
}

FVector ABasePlayerController::ProjectInputAxesFromPlayer(const FVector& Axes, float Distance, bool ProjectFromPlayerPos)
{
	auto delta = FVector(Axes.Y, Axes.X, 0.0f);

	if (!mLocalPlayerInHardBoundary)
	{
		delta.Normalize();
	}
	delta *= Distance;

	float cameraYaw = PlayerCameraManager->GetCameraRotation().Yaw;
	delta = delta.RotateAngleAxis(cameraYaw, FVector::UpVector);

	if (!ProjectFromPlayerPos)
	{
		return delta;
	}

	return GetControlledPlayerCharacter()->GetActorLocation() + delta;
}

FVector ABasePlayerController::ProjectCursorDirectionFromPlayer()
{
	if (auto character = GetControlledPlayerCharacter()) {
		FHitResult hitResult;
		if (GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType((ECollisionChannel)ECustomTraceChannels::PlayerPlane), false, hitResult)) {
			const auto source = character->GetActorLocation();
			const auto orientation = (hitResult.Location - source).GetSafeNormal2D();
			return orientation;
		}
		return character->GetActorRotation().Vector();
	}
	return FVector::ZeroVector;
}

bool ABasePlayerController::GetIsMoving() {
	auto player = GetControlledPlayerCharacter();
	return player->GetVelocity().Size() > 0.0f;
}

void ABasePlayerController::SetPlayerIsImmovable(bool immovable) {
	playerIsImmovable = immovable;
}

void ABasePlayerController::CenterMouseCursor() {
	GetMousePosition(mSavedMouseX, mSavedMouseY);

	const ULocalPlayer* LocalPlayer = GetLocalPlayer();
	if (LocalPlayer && LocalPlayer->ViewportClient) {
		FViewport* Viewport = LocalPlayer->ViewportClient->Viewport;
		if (Viewport) {
			FVector2D ViewportSize;
			LocalPlayer->ViewportClient->GetViewportSize(ViewportSize);
			const int32 X = static_cast<int32>(ViewportSize.X * 0.5f);
			const int32 Y = static_cast<int32>(ViewportSize.Y * 0.5f);

			auto manager = Cast<UDungeonsGameInstance>(GetWorld()->GetGameInstance())->GetControllerTypeManager();
			manager->SetPrevMousePos(X, Y);
			Viewport->SetMouse(X, Y);
		}
	}
}

void ABasePlayerController::ResetMouseCursorToLastPosition() {
	auto manager = Cast<UDungeonsGameInstance>(GetWorld()->GetGameInstance())->GetControllerTypeManager();
	manager->SetPrevMousePos(mSavedMouseX, mSavedMouseY);
	SetMouseLocation(mSavedMouseX, mSavedMouseY);
}

int32 ABasePlayerController::GetPlayerId() const {
	if (PlayerState) {
		return PlayerState->PlayerId;
	}
	return -1;
}

UWalletComponent* ABasePlayerController::GetWalletComponent() const {
	if (auto pawn = GetPawn()) {
		return pawn->FindComponentByClass<UWalletComponent>();
	}

	return nullptr;
}

UItemStashComponent* ABasePlayerController::GetItemStashComponent() const {
	if (auto pawn = GetPawn()) {
		return pawn->FindComponentByClass<UItemStashComponent>();
	}

	return nullptr;
}

UPathFollowingComponent* ABasePlayerController::GetPathFollowingComponent() const
{
	return PathFollowingComponent;
}

void ABasePlayerController::SpawnPlayerCameraManager() {
	Super::SpawnPlayerCameraManager();

	if (PlayerCameraManager)
		PlayerCameraManager->bUseClientSideCameraUpdates = false;
}


void ABasePlayerController::PostSeamlessTravel() {
	Super::PostSeamlessTravel();
	if (auto gameState = Cast<ADungeonsGameState>(GetWorld()->GetGameState()))
	{
		if (IsLocalController()) {
			if (auto game = actorquery::getGame(GetWorld())) {
				gameState->OnPostSeamlessTravel.Broadcast(game->levelName().c_str());
			}
		}
	}
	OnPostSeamlessTravel();
}

void ABasePlayerController::PreClientTravel(const FString & PendingURL, ETravelType TravelType, bool bIsSeamlessTravel) {
	Super::PreClientTravel(PendingURL, TravelType, bIsSeamlessTravel);

	auto gi = Cast<UDungeonsGameInstance>(GetWorld()->GetGameInstance());
	gi->GetFriendsInterface()->SetPresenceStatus(gi->Configuration.GetLevelDisplayName());

	if (bIsSeamlessTravel) {
		if (IsLocalController()) {
			if (auto gameState = Cast<ADungeonsGameState>(GetWorld()->GetGameState())) {
				gameState->OnPreSeamlessTravel.Broadcast();
			}
			if (auto gameInstance = Cast<UDungeonsGameInstance>(GetWorld()->GetGameInstance())) {
				gameInstance->BeginLoadingScreen(PendingURL, bIsSeamlessTravel);
			}
		}
		OnPreSeamlessTravel();
	}
}
void ABasePlayerController::GetSeamlessTravelActorList(bool bToEntry, TArray<AActor*>& ActorList)  {
	Super::GetSeamlessTravelActorList(bToEntry, ActorList);
	ActorList.Add(GetGameInstance<UDungeonsGameInstance>()->GetAudioMusicManager());

}

EPathFollowingRequestResult::Type ABasePlayerController::MoveToLocation(const FVector& Dest, float AcceptanceRadius, bool bStopOnOverlap, bool bUsePathfinding, bool bProjectDestinationToNavigation, bool bCanStrafe, TSubclassOf<UNavigationQueryFilter> FilterClass, bool bAllowPartialPaths) {

	// abort active movement to keep only one request running
	AbortPathFollowing();

	FAIMoveRequest MoveReq(Dest);
	MoveReq.SetUsePathfinding(bUsePathfinding);
	MoveReq.SetAllowPartialPath(bAllowPartialPaths);
	MoveReq.SetProjectGoalLocation(bProjectDestinationToNavigation);
	MoveReq.SetNavigationFilter(*FilterClass ? FilterClass : DefaultNavigationFilterClass);
	MoveReq.SetAcceptanceRadius(AcceptanceRadius);
	MoveReq.SetReachTestIncludesAgentRadius(bStopOnOverlap);
	MoveReq.SetCanStrafe(bCanStrafe);

	FNavPathSharedPtr outPath;

	auto resultData =  MoveTo(MoveReq, &outPath);

	if (resultData.Code != EPathFollowingRequestResult::RequestSuccessful)
		return resultData;

	return resultData;
}

EPathFollowingRequestResult::Type ABasePlayerController::MoveToActor(AActor* Goal, float AcceptanceRadius, bool bStopOnOverlap, bool bUsePathfinding, bool bCanStrafe, TSubclassOf<UNavigationQueryFilter> FilterClass, bool bAllowPartialPaths)
{
	if (!Goal->IsValidLowLevel() || Goal->IsPendingKillOrUnreachable()) return EPathFollowingRequestResult::Failed;

	// abort active movement to keep only one request running
	if (PathFollowingComponent && PathFollowingComponent->GetStatus() != EPathFollowingStatus::Idle)
	{
		PathFollowingComponent->AbortMove(*this, FPathFollowingResultFlags::ForcedScript | FPathFollowingResultFlags::NewRequest
			, FAIRequestID::CurrentRequest, EPathFollowingVelocityMode::Keep);
	}

	FAIMoveRequest MoveReq(Goal);
	MoveReq.SetUsePathfinding(bUsePathfinding);
	MoveReq.SetAllowPartialPath(bAllowPartialPaths);
	MoveReq.SetNavigationFilter(*FilterClass ? FilterClass : DefaultNavigationFilterClass);
	MoveReq.SetAcceptanceRadius(AcceptanceRadius);
	MoveReq.SetReachTestIncludesAgentRadius(bStopOnOverlap);
	MoveReq.SetCanStrafe(bCanStrafe);

	EPathFollowingRequestResult::Type result = MoveTo(MoveReq);

	if (result == EPathFollowingRequestResult::Failed) {
		FVector closestPoint;
		Goal->ActorGetDistanceToCollision(GetPawn()->GetActorLocation(), (ECollisionChannel)ECustomTraceChannels::IgnorePlayer, closestPoint);
		FAIMoveRequest LocationMoveReq(closestPoint);
		LocationMoveReq.SetUsePathfinding(bUsePathfinding);
		LocationMoveReq.SetAllowPartialPath(bAllowPartialPaths);
		LocationMoveReq.SetNavigationFilter(*FilterClass ? FilterClass : DefaultNavigationFilterClass);
		LocationMoveReq.SetAcceptanceRadius(AcceptanceRadius);
		LocationMoveReq.SetReachTestIncludesAgentRadius(bStopOnOverlap);
		LocationMoveReq.SetCanStrafe(bCanStrafe);
		result = MoveTo(LocationMoveReq);
	}

	return result;
}

void ABasePlayerController::AbortPathFollowing()
{
	if (PathFollowingComponent && PathFollowingComponent->GetStatus() != EPathFollowingStatus::Idle)
	{
		PathFollowingComponent->AbortMove(*this, FPathFollowingResultFlags::ForcedScript | FPathFollowingResultFlags::NewRequest
			, FAIRequestID::CurrentRequest, EPathFollowingVelocityMode::Reset);
	}
}

void ABasePlayerController::DodgeInDirection(FVector direction) {
	if (auto character = GetControlledPlayerCharacter()) {
		mTryToDodgeForSeconds = TRY_TO_DODGE_SECONDS;
		mDoDodgeDirection = direction;
	}
	OnAnyPlayerActionPerformed();
}

void ABasePlayerController::Reset()
{
	Super::Reset();

	if (PathFollowingComponent)
	{
		PathFollowingComponent->AbortMove(*this, FPathFollowingResultFlags::OwnerFinished | FPathFollowingResultFlags::ForcedScript);
	}
}

void ABasePlayerController::OnPossess(APawn* InPawn)
{
	auto role = Role;
	// don't even try possessing pending-kill pawns
	if (InPawn != nullptr && InPawn->IsPendingKill())
	{
		return;
	}

	GetGameInstance<UDungeonsGameInstance>()->GetLoadingScreenInitializer()->OnPlayerControllerPossessedPawn(this);

	Super::OnPossess(InPawn);

	if (GetPawn() == nullptr || InPawn == nullptr)
	{
		return;
	}

	if (PathFollowingComponent)
	{
		PathFollowingComponent->Initialize();
	}

}

void ABasePlayerController::OnUnPossess() {
	if (auto pc = Cast<APlayerCharacter>(GetPawn())) {
		if (!GetWorld()->IsInSeamlessTravel() && online::isOnlineSession()) {
			reconnect::disconnect(pc);
		}
	}

	Super::OnUnPossess();

	SaveCharacterData_OnlyIfValidSaveDataNum();

	if (PathFollowingComponent)
	{
		PathFollowingComponent->Cleanup();
	}
}

void ABasePlayerController::SetPawn(APawn* InPawn) {
	bool isNewPawn = GetPawn() != InPawn;
	Super::SetPawn(InPawn);

	if (GetWorld()->GetNetMode() != NM_DedicatedServer && isNewPawn) {

		OnPawnPossessedOnce(InPawn);
	}
}

void ABasePlayerController::SetPlayer(UPlayer* InPlayer) {
	Super::SetPlayer(InPlayer);

	UDungeonsGameInstance* gameInstance = Cast<UDungeonsGameInstance>(GetGameInstance());
	if (!gameInstance) {
		UE_LOG(LogDungeons, Log, TEXT("Game instance missing"));
		return;
	}

	UDungeonsFriendsInterface* friendsInterface = gameInstance->GetFriendsInterface();
	if (!friendsInterface) {
		UE_LOG(LogDungeons, Log, TEXT("FriendsInterface missing"));
		return;
	}
	friendsInterface->SetPresenceStatus(gameInstance->Configuration.GetLevelDisplayName());
	friendsInterface->UpdateFriendsList();
}

void ABasePlayerController::OnRep_Pawn() {
	Super::OnRep_Pawn();
	bool isNewPawn = GetPawn() != LastPawn;
	LastPawn = GetPawn();
	if (isNewPawn) {
		OnPawnPossessedOnce(GetPawn());
	}
}
 

void ABasePlayerController::OnPawnPossessedOnce(APawn* InPawn) {
	if (const auto playerCharacter = Cast<APlayerCharacter>(InPawn)) {
		playerCharacter->OnPawnPossessed();
		if (auto chatComponent = FindComponentByClass<UChatComponent>()) {
			chatComponent->OnPawnPossessed();
		}
	}
			
	ReceiveOnPawnPossessed(InPawn);
	
	if (InPawn != nullptr && IsLocalController()) {
		EnableCollisionTrace(InPawn, FName("PlayerPlane"), ECustomTraceChannels::PlayerPlane);
		EnableCollisionTrace(InPawn, FName("ArrowPlane"), ECustomTraceChannels::ArrowPlane);

		if (const auto playerCharacter = Cast<APlayerCharacter>(InPawn)) {
			playerCharacter->OnLocalPawnPossessed();
			UpdateInitialSetup();
		}
	}
}

void ABasePlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	UpdateInitialSetup();
}

bool ABasePlayerController::IsCustomMovementTypeActive(ECustomMovementType movementType)
{
	if (const auto character = GetControlledPlayerCharacter())
	{
		const auto movementComponent = character->GetCharacterMovement();
		return (movementComponent->MovementMode == EMovementMode::MOVE_Custom && movementComponent->CustomMovementMode == static_cast<uint8>(movementType));
	}
	
	return false;
}

class AGameBP* ABasePlayerController::GetCachedGameBP()
{
	if (!CachedGameBP.IsValid())
	{
		CachedGameBP = actorquery::getFirstActor<AGameBP>(GetWorld());
	}
	
	return CachedGameBP.Get();
}

void ABasePlayerController::MoveTowards(const FVector& point)
{	
	auto character = GetCharacter();
	FVector direction = point - character->GetActorLocation();
	direction.Normalize();
	
	character->AddMovementInput(direction);
	SetControlRotation(direction.ToOrientationRotator());
}

FPathFollowingRequestResult ABasePlayerController::MoveTo(const FAIMoveRequest& MoveRequest, FNavPathSharedPtr* OutPath)
{
	// both MoveToActor and MoveToLocation can be called from blueprints/script and should keep only single movement request at the same time.
	// this function is entry point of all movement mechanics - do NOT abort in here, since movement may be handled by AITasks, which support stacking 

	//SCOPE_CYCLE_COUNTER(STAT_MoveTo);
	//UE_VLOG(this, LogAINavigation, Log, TEXT("MoveTo: %s"), *MoveRequest.ToString());

	FPathFollowingRequestResult ResultData;
	ResultData.Code = EPathFollowingRequestResult::Failed;

	if (MoveRequest.IsValid() == false)
	{
		//UE_VLOG(this, LogAINavigation, Error, TEXT("MoveTo request failed due MoveRequest not being valid. Most probably desireg Goal Actor not longer exists"), *MoveRequest.ToString());
		return ResultData;
	}

	if (PathFollowingComponent == nullptr)
	{
		//UE_VLOG(this, LogAINavigation, Error, TEXT("MoveTo request failed due missing PathFollowingComponent"));
		return ResultData;
	}

	ensure(MoveRequest.GetNavigationFilter() || !DefaultNavigationFilterClass);

	//re-enable tick for move to
	PathFollowingComponent->SetComponentTickEnabled(true);

	bool bCanRequestMove = true;
	bool bAlreadyAtGoal = false;

	if (!MoveRequest.IsMoveToActorRequest())
	{
		if (MoveRequest.GetGoalLocation().ContainsNaN() || FAISystem::IsValidLocation(MoveRequest.GetGoalLocation()) == false)
		{
			//UE_VLOG(this, LogAINavigation, Error, TEXT("AAIController::MoveTo: Destination is not valid! Goal(%s)"), TEXT_AI_LOCATION(MoveRequest.GetGoalLocation()));
			bCanRequestMove = false;
		}

		// fail if projection to navigation is required but it failed
		if (bCanRequestMove && MoveRequest.IsProjectingGoal())
		{
			UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
			const FNavAgentProperties& AgentProps = GetNavAgentPropertiesRef();
			FNavLocation ProjectedLocation;

			if (NavSys && !NavSys->ProjectPointToNavigation(MoveRequest.GetGoalLocation(), ProjectedLocation, INVALID_NAVEXTENT, &AgentProps))
			{
				bCanRequestMove = false;
			}

			MoveRequest.UpdateGoalLocation(ProjectedLocation.Location);
		}

		bAlreadyAtGoal = bCanRequestMove && PathFollowingComponent->HasReached(MoveRequest);
	}
	else
	{
		bAlreadyAtGoal = bCanRequestMove && PathFollowingComponent->HasReached(MoveRequest);
	}

	if (bAlreadyAtGoal)
	{
		ResultData.MoveId = PathFollowingComponent->RequestMoveWithImmediateFinish(EPathFollowingResult::Success);
		ResultData.Code = EPathFollowingRequestResult::AlreadyAtGoal;
	}
	else if (bCanRequestMove)
	{
		FPathFindingQuery PFQuery;

		const bool bValidQuery = BuildPathfindingQuery(MoveRequest, PFQuery);
		if (bValidQuery)
		{
			FNavPathSharedPtr Path;
			FindPathForMoveRequest(MoveRequest, PFQuery, Path);

			const FAIRequestID RequestID = Path.IsValid() ? RequestMove(MoveRequest, Path) : FAIRequestID::InvalidRequest;
			if (RequestID.IsValid())
			{
				ResultData.MoveId = RequestID;
				ResultData.Code = EPathFollowingRequestResult::RequestSuccessful;

				if (OutPath)
				{
					*OutPath = Path;
				}
			}
		}
	}

	if (ResultData.Code == EPathFollowingRequestResult::Failed)
	{
		ResultData.MoveId = PathFollowingComponent->RequestMoveWithImmediateFinish(EPathFollowingResult::Invalid);
	}

	OnPlayerMovement.Broadcast(this);

	return ResultData;
}

bool ABasePlayerController::BuildPathfindingQuery(const FAIMoveRequest& MoveRequest, FPathFindingQuery& Query) const
{
	bool bResult = false;

	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());


	const ANavigationData* NavData = (NavSys == nullptr) ? nullptr :
		MoveRequest.IsUsingPathfinding() ? NavSys->GetNavDataForProps(GetNavAgentPropertiesRef()) :
		NavSys->GetAbstractNavData();

	if (NavData)
	{
		FVector GoalLocation = MoveRequest.GetGoalLocation();
		if (MoveRequest.IsMoveToActorRequest())
		{
			const INavAgentInterface* NavGoal = Cast<const INavAgentInterface>(MoveRequest.GetGoalActor());
			if (NavGoal)
			{
				const FVector Offset = NavGoal->GetMoveGoalOffset(this);
				GoalLocation = FQuatRotationTranslationMatrix(MoveRequest.GetGoalActor()->GetActorQuat(), NavGoal->GetNavAgentLocation()).TransformPosition(Offset);
			}
			else
			{
				GoalLocation = MoveRequest.GetGoalActor()->GetActorLocation();
			}
		}

		FSharedConstNavQueryFilter NavFilter = UNavigationQueryFilter::GetQueryFilter(*NavData, this, MoveRequest.GetNavigationFilter());
		Query = FPathFindingQuery(*this, *NavData, GetNavAgentLocation(), GoalLocation, NavFilter);
		Query.SetAllowPartialPaths(MoveRequest.IsUsingPartialPaths());

		if (PathFollowingComponent)
		{
			PathFollowingComponent->OnPathfindingQuery(Query);
		}

		bResult = true;
	}

	return bResult;
}

FAIRequestID ABasePlayerController::RequestMove(const FAIMoveRequest& MoveRequest, FNavPathSharedPtr Path)
{
	uint32 RequestID = FAIRequestID::InvalidRequest;
	if (PathFollowingComponent)
	{
		RequestID = PathFollowingComponent->RequestMove(MoveRequest, Path);
	}

	return RequestID;
}


void ABasePlayerController::FindPathForMoveRequest(const FAIMoveRequest& MoveRequest, FPathFindingQuery& Query, FNavPathSharedPtr& OutPath) const
{
	SCOPE_CYCLE_COUNTER(STAT_AI_Overall);

	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	if (NavSys)
	{
		FPathFindingResult PathResult = NavSys->FindPathSync(Query, EPathFindingMode::Hierarchical);
		if (PathResult.Result != ENavigationQueryResult::Error)
		{
			if (PathResult.IsSuccessful() && PathResult.Path.IsValid())
			{
				if (MoveRequest.IsMoveToActorRequest())
				{
					PathResult.Path->SetGoalActorObservation(*MoveRequest.GetGoalActor(), 100.0f);
				}

				PathResult.Path->EnableRecalculationOnInvalidation(true);
				OutPath = PathResult.Path;
			}
		}
	}
}

APlayerCharacter* ABasePlayerController::GetControlledPlayerCharacter() const
{
	auto* PlayerPawn = GetPawn();
	return PlayerPawn ? Cast<APlayerCharacter>(PlayerPawn) : nullptr;
}

void ABasePlayerController::RotatePlayerTowardsCursor() {
	FHitResult hitResult;
	if (GetHitResultUnderCursor((ECollisionChannel)ECustomTraceChannels::PlayerPlane, false, hitResult)) {
		GetControlledPlayerCharacter()->RotatePlayerTowardsLocation(hitResult.Location);
	}
}

UPickupStorableComponent* ABasePlayerController::GetPickupStorableComponent() const 
{
	return PickupStorableComponent;
}

// D11.KS/DJB
bool ABasePlayerController::IsLocalCoopMovingIntoHardBoundary(FVector2D& movementValue)
{
	//CurrentScreenStatus defaults to InSafeZone, and doesn't update if less than 2 players, so SinglePlayer should not be affected.
	if (CurrentCharacterScreenStatus >= ELocalPlayerCameraStatus::InHardBoundary)
	{
		FVector2D screenPosition;

		const auto gi = GetGameInstance<UDungeonsGameInstance>();
		const APlayerController* initialController = gi->GetUserManager()->GetInitialPlayerController();


		if (UGameplayStatics::ProjectWorldToScreen(initialController, GetControlledPlayerCharacter()->GetActorLocation(), screenPosition))
		{
			int viewportX, viewportY;
			initialController->GetViewportSize(viewportX, viewportY);

			const auto hardBoundaryWidth = viewportY * gi->GetLocalCoopHardBoundaryWidthPercentage();
			auto movementComponent = Cast<UPlayerCharacterMovementComponent>(GetControlledPlayerCharacter()->GetMovementComponent());


			bool isMovingIntoHardBoundary = false;
			const bool isFlying = movementComponent->IsMovingWithElytra();

			const bool flyingUp = isFlying && movementComponent->Velocity.X > 0 && movementComponent->Velocity.Y > 0;
			const bool flyingDown = isFlying && movementComponent->Velocity.X < 0 && movementComponent->Velocity.Y < 0;
			const bool flyingRight = isFlying && movementComponent->Velocity.X > 0 && movementComponent->Velocity.Y < 0;
			const bool flyingLeft = isFlying && movementComponent->Velocity.X < 0 && movementComponent->Velocity.Y > 0;

			if ((screenPosition.X < hardBoundaryWidth && (movementValue.X < 0 || flyingRight)) || (screenPosition.X > viewportX - hardBoundaryWidth && (movementValue.X > 0 || flyingLeft)))
			{
				movementValue.X = 0;
				if (isFlying)
				{
					movementComponent->Velocity.X = 0;
					movementComponent->Velocity.Y = 0;
				}
				isMovingIntoHardBoundary = true;
			}

			if ((screenPosition.Y < hardBoundaryWidth && (-movementValue.Y < 0 || flyingUp)) || (screenPosition.Y > viewportY - hardBoundaryWidth && (-movementValue.Y > 0 || flyingDown)))
			{
				movementValue.Y = 0;
				if (isFlying)
				{
					movementComponent->Velocity.X = 0;
					movementComponent->Velocity.Y = 0;
				}
				isMovingIntoHardBoundary = true;
			}

			return isMovingIntoHardBoundary;
		}
	}
	return false;
}

bool ABasePlayerController::CanLocalCoopRoll(const FVector& rollVector)
{
	//CurrentScreenStatus defaults to InSafeZone, and doesn't update if less than 2 players, so SinglePlayer should not be affected.
	if (CurrentCharacterScreenStatus >= ELocalPlayerCameraStatus::InDangerZone)
	{
		FVector2D screenPosition;

		const auto gi = GetGameInstance<UDungeonsGameInstance>();
		const APlayerController* initialController = gi->GetUserManager()->GetInitialPlayerController();

		if (UGameplayStatics::ProjectWorldToScreen(initialController, GetControlledPlayerCharacter()->GetActorLocation(), screenPosition))
		{
			int viewportX, viewportY;
			initialController->GetViewportSize(viewportX, viewportY);

			const auto dangerZoneWidth = viewportY * (gi->GetLocalCoopDangerZoneWidthPercentage());
			
			if (screenPosition.X < dangerZoneWidth && rollVector.X < 0) return false;
			if (screenPosition.Y < dangerZoneWidth && rollVector.Y < 0) return false;
			if (screenPosition.X > viewportX - dangerZoneWidth && rollVector.X > 0) return false;
			if (screenPosition.Y > viewportY - dangerZoneWidth && rollVector.Y > 0) return false;
		}
	}
	return true;
}

class UObjectDistanceNotifier* ABasePlayerController::InitializeAndGetDistanceNotifier() {
	if (!PlayerDistanceNotifier) {
		if (APlayerCharacter* character = Cast<APlayerCharacter>(GetCharacter())) {
			PlayerDistanceNotifier = NewObject<UObjectDistanceNotifier>(this, FName("PlayerDistanceNotifier"));
			PlayerDistanceNotifier->Initialize(character, 4000.0f, 10.f);
		}
	}
	return PlayerDistanceNotifier;
}

FrameTimeTracker& ABasePlayerController::GetFrameTimeTracker() {
	return mFrameTimeTracker;
}

void ABasePlayerController::OnRangeAttackPrimary(bool isPressed)
{
	if (!IsInputTypeAllowed(EPlayerInputType::Attack, true))
		return;

	const ERangeState state = isPressed ? ERangeState::Attack : ERangeState::Idle;

	if (state != PrevRangeState)
	{
		OnRangeStateChange(PrevRangeState, state);
	}
}

void ABasePlayerController::OnRangeAttackAxisSecondary(float value)
{
	if (!IsInputTypeAllowed(EPlayerInputType::Attack, true))
	{
		RangeStateRoot = 0.0f;
		return;
	}

	RangeStateRoot = value;
}

void ABasePlayerController::OnRangeStateChange(ERangeState prev, ERangeState next)
{
	// D11.DB - If OnRangeAttack is bound then it takes priority over range attacks.
	//			This is required for TNTBox throwing.
	if (prev == ERangeState::Idle && OnAlternativeRangeAttack.IsBound() && 
		RangedAttackComponent && RangedAttackComponent->GetHeldThrowables().Num() > 0)
	{
		OnAlternativeRangeAttack.Execute();
		PrevRangeState = ERangeState::Attack;
		return;
	}

	switch (next)
	{
		case ERangeState::Idle: {
			OnRootPlayerGamepadReleased();
			OnRangedAttackButtonGamepadReleased();
		} break;

		case ERangeState::Root: {
			if (prev == ERangeState::Attack) {
				OnRangedAttackButtonGamepadReleased();
			}
			else if (prev == ERangeState::Idle) {
				// #D11.CM - Don't root the player if we have no ammo, but allow the attack to play out (and fail) in full.
				if (auto rangeAttackComponent = GetRangedAttackComponent()) {
					if (rangeAttackComponent->HasAmmo()) {
						OnRootPlayerGamepadPressed();
					}
				}
			}
		} break;

		case ERangeState::Attack: {
			OnRangedAttackButtonGamepadPressed();
		} break;
	}

	PrevRangeState = next;
}

void ABasePlayerController::OnCancelTeleport(bool forceCancel)
{
	if (GetControlledPlayerCharacter()->GetTeleportState() != ETeleportState::Charging)
	{
		return;
	}	

	if (GetControlledPlayerCharacter())
	{
		GetControlledPlayerCharacter()->TryInterruptTeleport(forceCancel);
	}


	// #D11.CM - Push to our Dependents
	if (IsPrimaryPlayer())
	{
		for (auto dependent : GetTeleportDependents())
		{
			dependent->GetPlayerController()->OnCancelTeleport(forceCancel);
		}
	}
}

UAutoAimRangedAttackComponent* ABasePlayerController::GetRangedAttackComponent() {
	if (auto pawn = GetPawn())
		return pawn->FindComponentByClass<UAutoAimRangedAttackComponent>();
	return nullptr;
}

UCharacterSerializeComponent * ABasePlayerController::GetCharacterSerializeComponent() const {
	if (APlayerCharacter* player = Cast<APlayerCharacter>(GetPawn())) {
		return player->GetCharacterSerializeComponent();
	}
	
	return nullptr;
}


UMeleeAttackComponent* ABasePlayerController::GetMeleeAttackComponent() const {	
	if (auto pawn = GetPawn())
		return pawn->FindComponentByClass<UMeleeAttackComponent>();
	return nullptr;
}


void ABasePlayerController::ServerSetNetConnectionSpeed_Implementation(int32 NewNetSpeed)
{
	desiredNetSpeed = NewNetSpeed;
	SetNetSpeed(NewNetSpeed);
}

bool ABasePlayerController::ServerSetNetConnectionSpeed_Validate(int32 NewNetSpeed) {
	return true;
}

void ABasePlayerController::ServerRequestMoveEveryoneToLobby_Implementation() {
	if (AMissionProgressHandler* progressHandler = actorquery::getFirstActor<AMissionProgressHandler>(GetWorld())) {
		progressHandler->MoveEveryoneToLobbyOnce();
	}
}

bool ABasePlayerController::ServerRequestMoveEveryoneToLobby_Validate() {
	return true;
}

void ABasePlayerController::ServerInteract_Implementation(UReplicatedInteractableComponent* interactable, AActor* instigator) {
	if (interactable) {
		interactable->SetLastInstigator(Cast<ACharacter>(instigator));
		interactable->Interact(instigator);
	}
}

bool ABasePlayerController::ServerInteract_Validate(UReplicatedInteractableComponent* interactable, AActor* instigator) {
	return true;
}


void ABasePlayerController::ServerSpendEye_Implementation(AStrongholdFrame* strongholdFrame, const TArray<EEyeOfEnderType>& eyesSpent, AActor* spender) {
	if (strongholdFrame) {
		if(HasAuthority()) {
			strongholdFrame->TrySpendEyes(eyesSpent, spender);
		}
	}
}

bool ABasePlayerController::ServerSpendEye_Validate(AStrongholdFrame* strongholdFrame, const TArray<EEyeOfEnderType>& eyesSpent, AActor* instigator) {
	return true;
}

void ABasePlayerController::ClientFailSpendEye_Implementation(class AStrongholdFrame* strongholdFrame) {
	if (strongholdFrame) {
		strongholdFrame->SpendEyesFailure();
	}
}

bool ABasePlayerController::ClientFailSpendEye_Validate(class AStrongholdFrame* strongholdFrame) {
	return true;
}

void ABasePlayerController::Server_DeferredConsoleCommand_Implementation(const FString& command)
{
	GEngine->DeferredCommands.Add(command);
}

bool ABasePlayerController::Server_DeferredConsoleCommand_Validate(const FString& command)
{
	return actorquery::getGame(GetWorld())->ClientConsoleCommandsAllowed();
}

void ABasePlayerController::OnDebugState() {
	bDebugState = !bDebugState;
}

bool ABasePlayerController::HasValidLocalPlayer()
{
	return IsLocalController() && Cast<ULocalPlayer>(Player) != nullptr;
}

bool ABasePlayerController::GetDebugState() {
	return bDebugState;
}

void ABasePlayerController::ShowDebugInfo(bool showDebug)
{
	showDebugInfo = showDebug;
}

void ABasePlayerController::UpdateCursorTargetSelection(void)
{
	FVector2D mousePosition;

	if (!GetMousePosition(mousePosition.X, mousePosition.Y)) {
		return;
	}

	auto hud = GetHUD();
	if (hud != nullptr && hud->GetHitBoxAtCoordinates(mousePosition, true)) {
		return;
	}

	mTargetController.Update();
	mTargetController.UpdateCursorTargetSelection(GetWorld(), this, mousePosition, 200000.0f);
}

void ABasePlayerController::UpdateGamepadTargetSelection(float DeltaTime)
{

	{
		SCOPE_CYCLE_COUNTER(STAT_ABasePlayerController_UpdateRadialTargetSelection);
		mTargetController.UpdateRadialTargetSelection(GetControlledPlayerCharacter(), DeltaTime);
	}
	if( mAutoTarget )
	{
		SCOPE_CYCLE_COUNTER(STAT_ABasePlayerController_mAutoTargetUpdate);
		mAutoTarget->Update( *this);
	}	
}

//D11.KS - Attempt at setting up, host, client and local players, all in one place.
bool ABasePlayerController::UpdateInitialSetup()
{
	if (mInitialSetupStage == EBasePlayerControllerSetupStage_Complete)
	{
		return true;
	}

	UWorld* world = GetWorld();
	APlayerCharacter* character = Cast<APlayerCharacter>(GetCharacter());
	UPlayer* player = GetNetOwningPlayer();

	//We can definitely setup now.
	if (world && world->GetGameState() && character && player && HasValidLocalPlayer())
	{

		switch (mInitialSetupStage)
		{
		case EBasePlayerControllerSetupStage_Uninitialised:
		{
			//Start Stage
			++mInitialSetupStage;
			break;
		}
		case EBasePlayerControllerSetupStage_InitCharacter:
		{
			// Set the target controller
			mTargetController = TargetController();

			character->OnPlayerDeath.AddUObject(this, &ABasePlayerController::OnPlayerDeath);
			character->OnPlayerDown.AddUObject(this, &ABasePlayerController::OnPlayerDeath);
			character->OnPlayerRevive.AddUObject(this, &ABasePlayerController::OnPlayerRespawned);

			if (auto movement = character->GetPlayerCharacterMovementComponent()) {
				movement->OnMovingChange.AddDynamic(this, &ABasePlayerController::OnPlayerMovementChanged);
			}

			playerIsImmovable = false;

			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			mAutoTarget = world->SpawnActor<AAutoTarget>(AAutoTarget::StaticClass(), FTransform::Identity, SpawnParams);

			character->InitialSetup(this);

			++mInitialSetupStage;

			break;
		}

		case EBasePlayerControllerSetupStage_PollBlueprintInitialisation:
		{
			UpdateInitialBlueprintSetup();

			if (InitialBlueprintSetupComplete())
			{
				++mInitialSetupStage;
			}

			break;
		}

		case EBasePlayerControllerSetupStage_Finalize:
		{
			if (IsLocalController() && IsPrimaryPlayer()) {
				//Performance analytics generate far too much data. We have learned from Bedrock that we get a good deal of value from running the 
				//performance event on a small subset of players. This event will only be enabled for ~5 percent of sessions.
				if (FMath::RandRange(0, 99) < 5) {
					world->GetTimerManager().SetTimer(PerformanceTimerHandle, this, &ABasePlayerController::OnFirePerformanceAnalytics, 1.0f, true);
				}
			}

			if (IsOwnedByInitialLocalPlayer()) {

				FActorSpawnParameters SpawnParams;
				SpawnParams.Owner = this;

				if (auto highlightController = world->SpawnActor<AHighlightController>(AHighlightController::StaticClass(), FTransform::Identity, SpawnParams)) {
					highlightController->Claim(this, TargetTicker);
					highlightController->Enable(true);
				}
			}

			// D11.SSN
			if (auto component = GetControlledPlayerCharacter()->FindComponentByClass<UHealthComponent>()) {
				component->OnHealthFractionChanged.AddUObject(this, &ABasePlayerController::OnHealthFractionChanged, const_cast<const UHealthComponent*>(component));
			}

			if (auto pathComponent = GetPathFollowingComponent()) {
				pathComponent->OnRequestFinished.AddUObject(this, &ABasePlayerController::OnPlayerMoveFinished);
			}

			++mInitialSetupStage;
			break;
		}

		case EBasePlayerControllerSetupStage_Complete:
		{
			return true;
		}

		default:break;
		}

	}

	return false;

}


void ABasePlayerController::UpdateSecondaryTargets() {	
	auto closeMobsWithDistance = actorquery::getNearbyInstanceTrackedActorsDistanceSquared<AMobCharacter>(GetPawn(), 3000, actorquery::is::mobWithOnScreenHealthbar());
	closeMobsWithDistance.Sort([](TTuple<AMobCharacter*, float> mob1, TTuple<AMobCharacter*, float> mob2) -> bool { return mob1.Value < mob2.Value; });
	TArray<AMobCharacter*> closestMobs = algo::map_tarray(closeMobsWithDistance, RETLAMBDA(it.Key));
	
	if (!AreMobArraysTheSame(closestMobs, LastSecondaryTargets))
	{
		CurrentSecondaryTarget = closestMobs.Num() > 0 ? closestMobs[0] : nullptr;
		OnSecondaryHighlightTargetChanged(closestMobs, CurrentSecondaryTarget);
		LastSecondaryTargets = closestMobs;
	}
}

bool ABasePlayerController::AreMobArraysTheSame(TArray<AMobCharacter *>& array1, TArray<AMobCharacter *>& array2)
{
	if (array1.Num() != array2.Num())
	{
		return false;
	}

	for (int i = 0; i < array1.Num(); ++i)
	{
		if (array1[i] != array2[i])
		{
			return false;
		}
	}

	return true;
}

void ABasePlayerController::UpdateVelocityAccumulator(float deltaTime) {
	const float LowVelocityThresholdSquared = 20;
	const float velocity = GetPawn()->GetVelocity().SizeSquared2D();
	
	if (velocity >= LowVelocityThresholdSquared) {
		lowVelocityCounter = 0;
	} else {
		lowVelocityCounter++;
	}

	if (MouseCursorState == EMouseCursorStates::Move && lowVelocityCounter >= 10 && velocity > 0.f) {
		StopMovement();
	}
}

void ABasePlayerController::UICaptureLocalPlayers(bool Captured)
{
	if (GetWorld()->GetGameInstance() && GetWorld()->GetGameInstance()->GetNumLocalPlayers() > 1) {
		for (auto* localPlayer : GetWorld()->GetGameInstance()->GetLocalPlayers()) {

			if (auto* localPlayerController = localPlayer->GetPlayerController(GetWorld())) {
				if (auto* localBasePlayerController = Cast<ABasePlayerController>(localPlayerController)) {
					localBasePlayerController->SetInputCapturedByUI(Captured, false);
				}
			}

		}
	}
}

void ABasePlayerController::OnFirePerformanceAnalytics() {
	if (auto gameInstance = Cast<UDungeonsGameInstance>(GetGameInstance())) {
		if (const auto* playerCharacter = GetControlledPlayerCharacter()) {
			if (const auto* game = actorquery::getGame(GetWorld())) {
				analytics::Analytics::GetInstance().FirePerformanceMetrics(game, *playerCharacter, mPerfTelemetry);
			}
			mPerfTelemetry.Clear();
		}
	}
}

bool ABasePlayerController::GetAnyActionThisHeartbeatAndReset()
{
	auto anyAction = bPlayerActionThisHeartBeat;
	bPlayerActionThisHeartBeat = false;
	return anyAction;
}

AActor* ABasePlayerController::GetCurrentTarget() const {
	if(GetGamepadActive()) {
		ensureMsgf(mAutoTarget, TEXT("Auto target is null, is this called on server when not supposed to?"));
		return mAutoTarget ? mAutoTarget->GetTarget().Get() : nullptr;
	} else {
		return mTargetController.GetAttackTarget().Get();
	}
	
}

void ABasePlayerController::OnHealthFractionChanged(float newFraction, float oldFraction, const UHealthComponent* component) {
	if(newFraction < oldFraction) {
		OnAnyPlayerDamaged(component->GetCurrentHealth() * (oldFraction - newFraction));
	}
}

void ABasePlayerController::ClientDropItems_Implementation(const FItemDropSource& dropSource, const FNetworkedItemDropData& dropData, AActor* source, bool replicateItems, bool elongatedDrops, float dropDelay, float dropDelayCountSpeedUpFactor, EItemRarityChanceCategory rarityChanceCategory) {
	UItemDropComponent::GenerateDropsForPlayer(GetWorld(), Cast<APlayerCharacter>(GetPawn()), source, dropSource, dropData, replicateItems, elongatedDrops, dropDelay, dropDelayCountSpeedUpFactor, rarityChanceCategory);
}
bool ABasePlayerController::ClientDropItems_Validate(const FItemDropSource& dropSource, const FNetworkedItemDropData& dropData, AActor* source, bool replicateItems, bool elongatedDrops, float dropDelay, float dropDelayCountSpeedUpFactor, EItemRarityChanceCategory rarityChanceCategory) {
	return true;
}

void ABasePlayerController::Tick(float DeltaTime) {

	if (!UpdateInitialSetup())
	{
		// dont proceed until we have finished initialising
		return;
	}

	Super::Tick(DeltaTime);

	SCOPE_CYCLE_COUNTER(STAT_ABasePlayerController_Tick);

	APlayerCharacter* playerCharacter = nullptr;

	{
		SCOPE_CYCLE_COUNTER(STAT_ABasePlayerController_Tick_GetControlledPlayerCharacter);

		playerCharacter = GetControlledPlayerCharacter();
		if (playerCharacter == nullptr) return;
		auto playerState = Cast<ABasePlayerState>(playerCharacter->GetDungeonsBasePlayerState());
		if (playerState == nullptr) return;

		if (!IsLocalController()) return;

		if (showDebugInfo)
		{
			FVector ActorLocation = playerCharacter->GetActorLocation();

#if !UE_BUILD_SHIPPING
			FVector drawPos(0.0f);
			FColor drawColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f).ToFColor(true);
			float drawDuration = 0.0f;
			bool drawShadow = true;
			DrawDebugString(GetWorld(), playerCharacter->GetActorLocation(), *FString::Printf(TEXT("Location: [%f, %f, %f]"), ActorLocation.X, ActorLocation.Y, ActorLocation.Z), NULL, drawColor, 0.0f);
#endif //!UE_BUILD_SHIPPING
		}

		if (PlayerDistanceNotifier) {
			TArray<AActor*> actors;
			for (auto actor : InstanceTracker<APlayerCharacter>::GetList(GetWorld()).FilterByPredicate([playerCharacter](APlayerCharacter* player) { return playerCharacter != player; })) {
				if (!actor->IsLocallyControlled()) {
					actors.Emplace(actor);
				}
			}
			PlayerDistanceNotifier->Update(DeltaTime, actors);
		}
	}

	mFrameTimeTracker.Update(DeltaTime);
	if (PerformanceTimerHandle.IsValid()) {
		mPerfTelemetry.Update(FPlatformTime::ToMilliseconds(GGameThreadTime), FPlatformTime::ToMilliseconds(GRenderThreadTime), FPlatformTime::ToMilliseconds(RHIGetGPUFrameCycles()));
	}

	if (mTargetCameraFOV > -1.0f) {
		SCOPE_CYCLE_COUNTER(STAT_ABasePlayerController_Tick_SetFOV);

		float FOVdiff = mTargetCameraFOV - mCurrentCameraFOV;
		mCurrentCameraFOV += DeltaTime * FOVdiff;
		PlayerCameraManager->SetFOV(mCurrentCameraFOV);
	}

	// D11.DJB
	if (UDungeonsLocalPlayer* dLocalPlayer = Cast<UDungeonsLocalPlayer>(GetLocalPlayer())) {
		dLocalPlayer->RefreshDeadzones(this);
	}

	// D11.DJB
	if (!GetWorld()->IsInSeamlessTravel() && GetGamepadActive() && IsInputTypeAllowed(EPlayerInputType::Movement, true))
	{
		auto axis = FVector2D::ZeroVector;
		bool actionPerformed{ false };

		GetInputAnalogStickStateRadialDeadzone(EControllerAnalogStick::CAS_LeftStick, axis.X, axis.Y);

		// only check if player is not aiming
		if (GetAutoTargetSystem()->GetMode() != AAutoTarget::Mode::Shooting)
		{
			mLocalPlayerInHardBoundary = IsLocalCoopMovingIntoHardBoundary(axis);
		}


		switch (mCurrentControllerMovementCheck)
		{
		case Move_Normal:
		{
			if (!FMath::IsNearlyZero(axis.X))
			{
				actionPerformed = true;
				mLastMoveX = axis.X;
				mMouseInputStateMachine.OnMoveAxis(axis.X, 0.0f, *this, mTargetController);
			}
			if (!FMath::IsNearlyZero(axis.Y))
			{
				actionPerformed = true;
				mLastMoveY = axis.Y;
				mMouseInputStateMachine.OnMoveAxis(0.0f, axis.Y, *this, mTargetController);
			}
			if (actionPerformed)
			{
				OnAnyPlayerActionPerformed();
				OnPlayerMovement.Broadcast(this);
			}

			break;
		}

		case Move_Zero_Required:
		{
			if (FMath::IsNearlyZero(axis.X) && FMath::IsNearlyZero(axis.Y))
			{
				mCurrentControllerMovementCheck = Move_Deadzone_Check_Required;
			}

			break;
		}
		case Move_Deadzone_Check_Required:
		{
			float inputAxisMagnitudeSq = axis.SizeSquared();
			const float fDeadzoneCheckVal = CVarMovementCheckDeadzone.GetValueOnGameThread();
			if (inputAxisMagnitudeSq >= FMath::Square(fDeadzoneCheckVal))
			{
				mCurrentControllerMovementCheck = Move_Normal;
			}
			break;
		}
		default:break;
		};
		
		// D11.DB - Right stick rolling.
		if (RightStickDodgingEnabled) {
			auto rollVec = FVector::ZeroVector;
			GetInputAnalogStickRawState(EControllerAnalogStick::CAS_RightStick, rollVec.X, rollVec.Y);

			float rollThreshold{ 0.2f };
			if (PlayerInput)
			{
				FInputAxisProperties properties;
				PlayerInput->GetAxisProperties(EKeys::Gamepad_RightX, properties);
				rollThreshold = properties.DeadZone;
			}

			if (RightStickHasReset && CanLocalCoopRoll(rollVec)) {
				if (rollVec.SizeSquared() > rollThreshold * rollThreshold) {
					float yaw = 0.0f;

					if (CVarPlayerRelativeDodgeRoll.GetValueOnGameThread() == 1) {
						yaw = GetDesiredRotation().Yaw;
					}
					else {
						yaw = PlayerCameraManager->GetCameraRotation().Yaw;
					}

					rollVec = rollVec.RotateAngleAxis(yaw + 90.0f, FVector::UpVector);

					mLastRollX = rollVec.X;
					mLastRollY = rollVec.Y;
					DodgeInDirection(rollVec);
					OnAnyPlayerActionPerformed();
				}
			}
			// D11.SSN - prevent rolling immediately when coming out of menus
			else {
				if (rollVec.SizeSquared() < rollThreshold * rollThreshold) {
					RightStickHasReset = true;
				}
			}
		}
	}

	{
		SCOPE_CYCLE_COUNTER(STAT_ABasePlayerController_Tick_IsAllowedToPerformAction);

		bool shouldUpdateOnScreenMobs = IsOwnedByInitialLocalPlayer() && mAutoTarget;
		bool isAllowedToUpdateOnscreenMobs = TargetTicker->ShouldTick(ETargetingTickStage::HighlightingUpdate);
			
		if(isAllowedToUpdateOnscreenMobs && shouldUpdateOnScreenMobs) {
			mAutoTarget->UpdateOnScreenMobs();
		}
		
		if (playerCharacter->IsAllowedToPerformAction())
		{
			bool isAllowedToUpdateTargeting = TargetTicker->ShouldTick(ETargetingTickStage::UpdateGamepadTargetSelection);
			bool shouldUpdateTargeting = IsTargetHighlightingAllowed();

			if(isAllowedToUpdateTargeting) {
				if(shouldUpdateTargeting) {
					SCOPE_CYCLE_COUNTER(STAT_ABasePlayerController_Tick_UpdateTargetSelection);
					if (GetGamepadActive())
					{
						UpdateGamepadTargetSelection(DeltaTime);
					}
					else
					{
						UpdateCursorTargetSelection();
					}					
				}
				{
					SCOPE_CYCLE_COUNTER(STAT_ABasePlayerController_Tick_UpdateSecondaryTargets);
					UpdateSecondaryTargets();
				}
			}
			
			{
				SCOPE_CYCLE_COUNTER(STAT_ABasePlayerController_Tick_mMouseInputStateMachine);

				mMouseInputStateMachine.Tick(DeltaTime, *this, mTargetController);
			}

			if (GetGamepadActive() && mAutoTarget) {
				SCOPE_CYCLE_COUNTER(STAT_ABasePlayerController_Tick_UpdateHighlight);

				mAutoTarget->UpdateHighlight(*this);
			}
		}
	}

	{
		SCOPE_CYCLE_COUNTER(STAT_ABasePlayerController_Tick_UpdateMouseCursorState);

		UpdateMouseCursorState();
	}

	if (CVarDebugDrawNetCullDistance.GetValueOnGameThread() == 1 && IsLocalController()) {
		DungeonsDebugOnline::markNetRelevantActors(GetWorld(), this);
	}

	{
		SCOPE_CYCLE_COUNTER(STAT_ABasePlayerController_Tick_UpdateVelocityAccumulator);

		UpdateVelocityAccumulator(DeltaTime);
	}

#if !UE_BUILD_SHIPPING
	if (CVarDebugActiveActionKeybinds.GetValueOnGameThread() != 0) {
		if (auto controllerTypeManager = GetGameInstance<UDungeonsGameInstance>()->GetControllerTypeManager()) {
			UKeybindHelper::DebugActiveKeybindings(this, controllerTypeManager->GetControllerType(GetLocalPlayer()->GetControllerId()));
		}
	}
#endif // !UE_BUILD_SHIPPING

	TickWantsToDodge(DeltaTime);
	
	mLastMoveX = 0.0f;
	mLastMoveY = 0.0f;
	mLastRollX = 0.0f;
	mLastRollY = 0.0f;
	mLocalPlayerInHardBoundary = false;


	//D11.SC Consecutive local players can follow the primary player, to make local player testing easier 
	if (CVarLocalPlayersFollowTheLeader.GetValueOnGameThread() > 0)
	{
		if (ABasePlayerController* firstPlayerController = Cast<ABasePlayerController>(GetWorld()->GetFirstPlayerController()))
		{
			if (firstPlayerController != this)
			{
				if (FollowTheLeaderCountDown <= 0.0f)
				{					
					if (actorquery::is::alive(GetPawn()))
					{
						MoveToActor(firstPlayerController->GetPawn(), 100.0f);
					}
					FollowTheLeaderCountDown = 1.0f;
				}
				else
				{
					FollowTheLeaderCountDown -= DeltaTime;
				}
			}
		}
	}

}

void ABasePlayerController::TickWantsToDodge(float DeltaSeconds) {
	if (mTryToDodgeForSeconds > 0.0f) {
		if(auto player = GetControlledPlayerCharacter()) {
			if (player->CanDodge()) {
				player->Dodge(mDoDodgeDirection);
				ClearDodge();
			}
			mTryToDodgeForSeconds -= DeltaSeconds;
		}
	}
}

void ABasePlayerController::ClearDodge()
{
	mTryToDodgeForSeconds = 0;
}

bool ABasePlayerController::IsDisplayNameAssigned() const {
	if (PlayerState) {
		if (auto playerState = Cast<ABasePlayerState>(PlayerState)) {
			return playerState->IsDisplayNameAssigned();
		}
	}
	return false;
}

FString ABasePlayerController::GetPlayerDisplayName() const {
	if (auto playerState = Cast<ABasePlayerState>(PlayerState))	
		return playerState->GetPlayerDisplayName();
	return FString();
}

FString ABasePlayerController::GetPlayerPrimaryDisplayName() const {
	if (auto playerState = Cast<ABasePlayerState>(PlayerState))
		return playerState->GetPlayerPrimaryDisplayName();
	return FString();
}
FString ABasePlayerController::GetPlayerSecondaryDisplayName() const {
	if (auto playerState = Cast<ABasePlayerState>(PlayerState))
		return playerState->GetPlayerSecondaryDisplayName();
	return FString();
}

UItemSlot* ABasePlayerController::GetItemSlotOfType(ESlotType type, int index) const {
	if (auto player = GetControlledPlayerCharacter()) {
		return player->GetItemSlotOfType(type, index);
	}
	return nullptr;
}

void ABasePlayerController::SetWantsToActivateSlot(ESlotType type, int index, bool wantsToActivate) {

	if (IsChatWheelVisible())
	{
		return;
	}

	if (UItemSlot* slot = GetItemSlotOfType(type, index)) {
		if(slot->SetWantsToActivate(wantsToActivate))
		{
			OnAnyPlayerActionPerformed();
		}
	}
}

void ABasePlayerController::OnDodgeButton(bool pressed){
	if (pressed) {
		auto rollVec = ProjectCursorDirectionFromPlayer();
		if (CanLocalCoopRoll(rollVec))
		{
			DodgeInDirection(rollVec);
			OnAnyPlayerActionPerformed();
		}
	}
}

void ABasePlayerController::OnDodgeForwardButton(bool pressed){
	if (pressed) {
		auto rollVec = GetControlledPlayerCharacter()->GetActorForwardVector();
		if (CanLocalCoopRoll(-rollVec))
		{
			DodgeInDirection(rollVec);
			OnAnyPlayerActionPerformed();
		}
	}
}

void ABasePlayerController::OnMoveButton(bool pressed) {
	mMouseInputStateMachine.OnMoveButton(pressed, *this, mTargetController);
	if (pressed) {
		OnAnyPlayerActionPerformed();
	}
}

void ABasePlayerController::OnRangedAttackButton(bool pressed) {
	mMouseInputStateMachine.OnRangedButton(pressed, *this, mTargetController);
	if (pressed) {
		OnAnyPlayerActionPerformed();
	}
}

void ABasePlayerController::OnRangedAttackButtonGamepadPressed() {
	if (!IsInputTypeAllowed(EPlayerInputType::Attack, true))
		return;

	mMouseInputStateMachine.OnGamepadRangedButton(true, *this, mTargetController);
	mAutoTarget->SetShooting(true);
	OnAnyPlayerActionPerformed();
}

void ABasePlayerController::OnRangedAttackButtonGamepadReleased() {
	if (!IsInputTypeAllowed(EPlayerInputType::Attack, true))
		return;

	mMouseInputStateMachine.OnGamepadRangedButton(false, *this, mTargetController);
	mAutoTarget->SetShooting(false);
}

void ABasePlayerController::OnMeleeAttackButton(bool pressed, bool gamepad /*= false*/)
{
	mMouseInputStateMachine.OnMeleeButton(pressed, gamepad, *this, mTargetController);
	if (pressed) 
	{
		OnAnyPlayerActionPerformed();
	}
}

void ABasePlayerController::OnRangedTargetingModeToggle(bool pressed, bool gamepad /*= false*/)
{
	if (pressed) {
		GetAutoTargetSystem()->ToggleRangedTarget();
	}
}

void ABasePlayerController::OnRangedTargetingModePressed()
{
	if (!IsInputTypeAllowed(EPlayerInputType::Attack, true))
		return;

	OnRangedTargetingModeToggle(true, true);
}

void ABasePlayerController::OnRangedTargetingModeReleased()
{
	if (!IsInputTypeAllowed(EPlayerInputType::Attack, true))
		return;

	OnRangedTargetingModeToggle(false, true);
}

void ABasePlayerController::OnRootPlayerGamepadPressed()
{
	if (!IsInputTypeAllowed(EPlayerInputType::Movement, true))
		return;

	mMouseInputStateMachine.OnGamepadRootButton(true, *this, mTargetController);
	OnAnyPlayerActionPerformed();
}

void ABasePlayerController::OnRootPlayerGamepadReleased()
{
	if (!IsInputTypeAllowed(EPlayerInputType::Movement, true))
		return;

	mMouseInputStateMachine.OnGamepadRootButton(false, *this, mTargetController);
}

void ABasePlayerController::OnMeleeAttackButtonPressed()
{
	if (!IsInputTypeAllowed(EPlayerInputType::Attack, true))
		return;

	if (IsChatWheelVisible())
		return;

	OnMeleeAttackButton(true, true);
}

void ABasePlayerController::OnMeleeAttackButtonReleased()
{
	if (!IsInputTypeAllowed(EPlayerInputType::Attack, true))
		return;

	OnMeleeAttackButton(false, true);
}

void ABasePlayerController::OnRootPlayer(bool pressed) {
	mMouseInputStateMachine.OnRootButton(pressed, *this, mTargetController);
	if (pressed) {
		OnAnyPlayerActionPerformed();
	}
}

void ABasePlayerController::OnHotbarItemUsed() {
	OnAnyPlayerActionPerformed();
}

void ABasePlayerController::OnAnyPlayerActionPerformed()
{
	RefreshPlayerIdle(true);
	mCurrentControllerMovementCheck = Move_Normal;
	OnAnyPlayerAction.Broadcast(this);
	ReceiveOnAnyPlayerAction.Broadcast();
	bPlayerActionThisHeartBeat = true;
}

void ABasePlayerController::OnPlayerMoveFinished(FAIRequestID RequestID, const FPathFollowingResult& Result) {
	RefreshPlayerIdle();
}

void ABasePlayerController::OnPlayerMovementChanged(bool isMoving) {
	RefreshPlayerIdle();
}

void ABasePlayerController::RefreshPlayerIdle(bool forceRefresh /*= false*/) {
	// Cache our idle
	auto lastIdle = CurrentIdleState;

	// Make sure our timer is cleared out
	GetWorld()->GetTimerManager().ClearTimer(PlayerIdleTimerHandle);

	if (auto character = GetControlledPlayerCharacter()) {
		bool idle = !character->IsMoving();
		idle &= character->GetAttackState() == EAttackState::None;
		idle &= character->InWorldAndAlive();

		if (auto state = GetWorld()->GetGameState<ADungeonsGameState>()) {
			idle &= !state->IsCinematicPlaying();
		}

		if (idle) {
			// If we're inactive, we can start a countdown to idle
			GetWorld()->GetTimerManager().SetTimer(PlayerIdleTimerHandle, this, &ABasePlayerController::OnPlayerIdleTimer, mPlayerIdleTime);
			CurrentIdleState = EPlayerIdleState::Inactive;
		}
		else {
			// If we're active, set our state as such
			CurrentIdleState = EPlayerIdleState::Active;
		}

		// Broadcast if there's been a change to either Active or Inactive
		if (forceRefresh || lastIdle != CurrentIdleState) {
			ServerNotifyIdleChange(CurrentIdleState);
		}
	}
}

void ABasePlayerController::OnPlayerIdleTimer() {
	if (auto character = GetControlledPlayerCharacter()) {
		if (CurrentIdleState == EPlayerIdleState::Inactive) {
			bool idle = !character->IsMoving();
			idle &= character->GetAttackState() == EAttackState::None;

			if (idle) {
				CurrentIdleState = EPlayerIdleState::Idle;
				ServerNotifyIdleChange(CurrentIdleState);
				return;
			}
		}

		RefreshPlayerIdle();
	}
}

bool ABasePlayerController::IsRevivePrioritised() const {
	return GetGamepadActive() && GetControlledPlayerCharacter()->IsRevivingPlayer() && (!GetCurrentTarget() || !GetMeleeAttackComponent()->IsWithinAttackRange(GetCurrentTarget()));
}

void ABasePlayerController::OnDebugPointer(bool pressed) {
}


void ABasePlayerController::ServerNotifyIdleChange_Implementation(EPlayerIdleState idleState) {
	DungeonsGearUtilLibrary::OnPlayerIdleChange(this, idleState);
}

bool ABasePlayerController::ServerNotifyIdleChange_Validate(EPlayerIdleState idleState) {
	return true;
}

void ABasePlayerController::PlayerLoadedInLevel()
{
	SetNetSpeed(desiredNetSpeed);
}

bool ABasePlayerController::IsInitialSetupDone()
{
	return (mInitialSetupStage == EBasePlayerControllerSetupStage_Complete);
}

bool ABasePlayerController::ClickyClicked(UInteractableComponent* clicky) {
	using namespace input;
	OnAnyPlayerActionPerformed();
	//The ui has swallowed the actual click here, and we have to force increment to make the interaction still register.
	mMouseInputStateMachine.ForceIncrementMouseVersion();
	mTargetController.ForceSetTarget(clicky->GetOwner());
	mMouseInputStateMachine.ChangeState(GetId<MouseButtonClickyState>(), *this, mTargetController);
	return true;
}

void ABasePlayerController::NotifyLoadedWorld(FName WorldPackageName, bool bFinalDest) {
	Super::NotifyLoadedWorld(WorldPackageName, bFinalDest);
}

void ABasePlayerController::UpdateMouseCursorState() {
	EMouseCursorStates NewMouseCursorState = mMouseInputStateMachine.GetCursorState(*this, mTargetController);
	if (NewMouseCursorState != EMouseCursorStates::UNSET) {
		MouseCursorState = NewMouseCursorState;
	} else {
		MouseCursorState = EMouseCursorStates::Move;
	}

	if (GetGamepadActive()) {
		MouseCursorState = EMouseCursorStates::Invisible;
	}

	const auto DesiredMouseCursor = GetMouseCursor();
	if (CurrentMouseCursor != DesiredMouseCursor)
	{
		//Ugliest hack in the world to FORCE cursor update.
		//Unreal doesn't seem to update cursor unless mouse is moved or a mouse button is pressed.
		CurrentMouseCursor = DesiredMouseCursor;

		auto& App = FSlateApplication::Get();
		FVector2D CursorPos = App.GetCursorPos();
		App.SetCursorPos(CursorPos + FVector2D(0, 1));
		App.OnMouseMove();
		App.SetCursorPos(CursorPos);
		App.OnMouseMove();
	}	
}

EMouseCursor::Type ABasePlayerController::GetMouseCursor() const
{
	switch(MouseCursorState) {
		case EMouseCursorStates::Move:
			return EMouseCursor::Default;
		case EMouseCursorStates::MoveHeld:
			return EMouseCursor::TextEditBeam;
		case EMouseCursorStates::Attack:
			return EMouseCursor::CardinalCross;
		case EMouseCursorStates::AttackHeld:
			return EMouseCursor::Crosshairs;
		case EMouseCursorStates::Interact:
			return EMouseCursor::Hand;
		case EMouseCursorStates::InteractHeld:
			return EMouseCursor::EyeDropper;
		case EMouseCursorStates::Drag:
			return EMouseCursor::GrabHand;
		case EMouseCursorStates::DragHeld:
			return EMouseCursor::GrabHandClosed;
		case EMouseCursorStates::Invisible:
			return EMouseCursor::None;
		default:
			return EMouseCursor::Default;
	}
}

void ABasePlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void ABasePlayerController::AdjustCameraFovBasedOnCharacterSpeed() {
	//Update fov based on current movement speed multiplier
	auto abilitySystem = GetControlledPlayerCharacter()->GetAbilitySystemComponent();
	float speedBoostAlpha = (abilitySystem->GetNumericAttribute(UMovementAttributeSet::SpeedMultiplierAttribute()) - 1.f) / 0.4f;
	mCurrentCameraFOV = PlayerCameraManager->GetFOVAngle();
	mTargetCameraFOV = FMath::Lerp(55.f, 60.f, FMath::Clamp(speedBoostAlpha, 0.f, 1.f));
}

void ABasePlayerController::ServerRequestAwards_Implementation() {
	auto gameMode = Cast<ADungeonsGameMode>(GetWorld()->GetAuthGameMode());
// 	gameMode->UpdateRewards();
}

bool ABasePlayerController::ServerRequestAwards_Validate() {
	return true;
}


bool ABasePlayerController::IsTargetHighlightingAllowed() const {
	return !bInputCapturedByUI && !IsInputCapturedByTeleport();
}

void ABasePlayerController::CancelCurrentInputActions() {	
	RefreshPlayerIdle(true);
	mMouseInputStateMachine.Reset(*this, mTargetController);
	mTargetController.ResetTarget();
}

bool ABasePlayerController::IsInputTypeAllowed(EPlayerInputType InputType, bool gamepad) const {
	if (gamepad && !GetGamepadActive())
	{
		return false;
	}
	if (!gamepad && GetGamepadActive())
	{
		return false;
	}

	if (!bIsUIInputAllowed)
	{
		return false;
	}

	
	if (GetGameInstance<UDungeonsGameInstance>()->IsLoadingScreenActive()) {
		return false;
	}

	switch (InputType) {
	case EPlayerInputType::Teleport:
	{	
		if (bInputCapturedByUI) {
			return false;
		}

		if (auto character = GetControlledPlayerCharacter()) {
			return (bTeleportUIListOpen || GetTeleportCandidates().Num() > GetTeleportDependents().Num()) && !character->IsFrozenSolid() && character->IsAllowedToPerformAction() && GetLocalPlayer()->IsPrimaryPlayer();
		}

		return false;
	}
	case EPlayerInputType::Movement:
		if (auto character = GetControlledPlayerCharacter())
		{
			return !bInputCapturedByUI && character->IsAllowedToPerformAction();
		}
		return false;
	case EPlayerInputType::Attack:
		if (auto character = GetControlledPlayerCharacter())
		{
			return !bInputCapturedByUI && !IsInputCapturedByTeleport() && character->IsAllowedToPerformAction() && !IsRevivePrioritised();
		}
		return false;
	case EPlayerInputType::Hotbar:
		if (auto character = GetControlledPlayerCharacter()) {
			const auto characterMovement = character->GetPlayerCharacterMovementComponent();
			return !bInputCapturedByUI && !IsInputCapturedByTeleport() && !character->IsFrozenSolid() && character->IsAllowedToPerformAction() && !characterMovement->IsMovingWithElytra();
		}
		return false;
	case EPlayerInputType::Map:
	case EPlayerInputType::Inventory:
		if (GetControlledPlayerCharacter())
		{
			return true;
		}
		return false;
	case EPlayerInputType::Social:
		return !bInputCapturedByUI;
	case EPlayerInputType::TextChat:
		return UGameSettingsFunctionLibrary::GetChatEnabled(this);
	case EPlayerInputType::UI:
		return true;
	case EPlayerInputType::Debug:
		return !bInputCapturedByUI && EnableDebug;
	default:
		return true;
	}
}

void ABasePlayerController::ToggleTargetingState(bool enter) {
	if (enter) {
		mMouseInputStateMachine.ChangeState(input::GetId<input::MouseTargetingState>(), *this, mTargetController);
	}
	else {
		mMouseInputStateMachine.ChangeState(input::GetId<input::MouseButtonEntryState>(), *this, mTargetController);
	}
}

void ABasePlayerController::ToggleStateChangeAllowed(bool allow) {
	mMouseInputStateMachine.AllowStateChange(allow);
}

bool ABasePlayerController::InitialBlueprintSetupComplete_Implementation()
{
	return true;
}

FVector ABasePlayerController::GetAudioListenerLocation() {
	FVector listenerLocation;
	FVector listenerFrontDirection;
	FVector listenerRightDirection;
	GetAudioListenerPosition(listenerLocation, listenerFrontDirection, listenerRightDirection);
	return listenerLocation;
}

void ABasePlayerController::ResetMouseStates() {
	mMouseInputStateMachine.Reset(*this, mTargetController);
}

// --- PASTE PART 2 DIRECTLY BELOW HERE ---

void ABasePlayerController::MoveForward(float Value)
{
	if (Value != 0.0f && !playerIsImmovable)
	{
		if (PlayerCameraManager != nullptr)
		{
			const FRotator Rotation = PlayerCameraManager->GetCameraRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);
			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

			if (APawn* ControlledPawn = GetPawn())
			{
				ControlledPawn->AddMovementInput(Direction, Value);

				if (APlayerCharacter* PlayerChar = Cast<APlayerCharacter>(ControlledPawn))
				{
					FVector FutureLocation = PlayerChar->GetActorLocation() + (Direction * Value * 100.0f);
					PlayerChar->RotatePlayerTowardsLocation(FutureLocation);
				}

				OnAnyPlayerActionPerformed();
				OnPlayerMovement.Broadcast(this);
			}
		}
	}
}

void ABasePlayerController::MoveRight(float Value)
{
	if (Value != 0.0f && !playerIsImmovable)
	{
		if (PlayerCameraManager != nullptr)
		{
			const FRotator Rotation = PlayerCameraManager->GetCameraRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);
			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

			if (APawn* ControlledPawn = GetPawn())
			{
				ControlledPawn->AddMovementInput(Direction, Value);

				if (APlayerCharacter* PlayerChar = Cast<APlayerCharacter>(ControlledPawn))
				{
					FVector FutureLocation = PlayerChar->GetActorLocation() + (Direction * Value * 100.0f);
					PlayerChar->RotatePlayerTowardsLocation(FutureLocation);
				}

				OnAnyPlayerActionPerformed();
				OnPlayerMovement.Broadcast(this);
			}
		}
	}
}