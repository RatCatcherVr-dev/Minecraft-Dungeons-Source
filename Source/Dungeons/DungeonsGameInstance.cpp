#include "Dungeons.h"
#include "DungeonsGameInstance.h"
#include "DungeonsGameState.h"
#include "Engine.h"
#include "Assets/PreloadConfig.h"
#include "game/GameTypes.h"
#include "game/GameProgress.h"
#include "game/util/DungeonsTravelUtil.h"
#include "game/util/ActorQuery.h"
#include "lovika/LovikaLevelActor.h"
#include "online/friends/DungeonsFriendsCommon.h"
#include "online/sessions/JoinDungeonsSessionCallbackProxy.h"
#include "online/sessions/SessionSettings.h"
#include "platform/DungeonsVersion.h"
#include "platform/DungeonsVersionContext.h"
#include <UserWidget.h>
#include <UObjectGlobals.h>
#include <GenericPlatformCrashContext.h>
#include "RenderTargetPool.h"
#include "Runtime/MoviePlayer/Public/MoviePlayer.h"
#include "online/sessions/OnlineUtil.h"
#include "DungeonsUserManagement.h"
#include "DungeonsControllerTypeManager.h"
#include "HAL/FileManager.h"
#include "game/Enchantments/Enchantment.h"
#include "game/mission/trial/TrialUtil.h"
#include "ui/hints/HintManager.h"
#include "util/ConfigFileUtil.h"
#include "GameSettingsFunctionLibrary.h"
#include "game/Conversion.h"
#include "SaveSpinnerInterface.h"
#include "ChartCreation.h"
#include "OnlineAchievementsInterface.h"
#include "OnlineSubsystem.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/effects/PermanentInvisibilityEffect.h"
#include "PlayfabServices/Public/PlayfabServices.h"
#include "TextToSpeechBPLibrary.h"
#include "game/level/chests/ChestSelector.h"
#include "game/ArmorProperties/ArmorPropertyTypeDefs.h"
#include "locale/LocalizationUtils.h"
#include "LoadingScreen/LoadingScreenInitializer.h"
#include "online/crossplay/Identity.h"
#include "online/crossplay/ExternalUI.h"
#include "online/crossplay/CrossplayOSS.h"
#include "DungeonsLoginFlowWin.h"
#include "DungeonsLoginFlowCrossPlay.h"
#include "DungeonsLoginFlowNull.h"
#include "DungeonsLoginFlowSwitch.h"
#include "DungeonsLoginFlowPS4.h"
#include "game/actor/character/player/PlayerControllerBase.h"
#include "game/component/CharacterSerializeComponent.h"
#include "GameSettingsFunctionLibrary.h"

//D11.PS Had to remove the PLATFORM_WINDOWS because it was getting defined to 1 via some plug in.
#if (!UE_BUILD_SHIPPING) && (!PLATFORM_PS4 && !PLATFORM_SWITCH) && (PLATFORM_XBOXONE || PLATFORM_WINDOWS) 
#include "UnrealToolsFrameworkPCH.h"
#include "game/util/DungeonsGameHooks.h"
#include "game/util/DungeonsGameHookEvents.h"
#endif

#if PLATFORM_SWITCH
#include <nn/oe.h>
#include "CoreDelegates.h"
#endif


#include "game/actor/character/player/BasePlayerController.h"
#include "Assets/DungeonsAssetManager.h"
#include <OnlineSessionInterface.h>
#include "game/util/UnlockKeyUtils.h"
#include "platform/GameVersion.h"
#include "game/GameBP.h"
#include "save/DungeonsConsoleSave.h"
#include "game/actor/character/player/DungeonsLocalPlayer.h"
#include "online/entitlements/EntitlementsRepositoryFactory.h"
#include <Themida/Anticheat.hpp>
#include "Assets/ItemAssetFinder.h"
#include "online/sessions/FindSessionHandler.h"
#include "save/cloud/DungeonsCloudLoadSave.h"
#include "online/seasons/LiveOps.h"
#include "game/dlc/DLCDefs.h"

#ifdef EPIC_STORE_BUILD
#include "EpicServices.h"
#endif

#include "IDungeonsAPIClient.h"
#include "platform/ClientInfoBuilder.h"
#include "LogMacros.h"
#include "PlayfabServices.h"
#include "PlayfabPlayer.h"

static TAutoConsoleVariable<float> CVarSwitchHandheldScreenPercentage(TEXT("Dungeons.Switch.HandheldScreenPercentage"), 100.0f, TEXT("Sets the screen percentage for handheld mode"), ECVF_Default);

static TAutoConsoleVariable<float> CVarSwitchConsoleScreenPercentage(TEXT("Dungeons.Switch.ConsoleScreenPercentage"), 83.33f, TEXT("Sets the screen percentage for console mode"), ECVF_Default);

static TAutoConsoleVariable<int> CVarSwitchFastLoad(TEXT("Dungeons.Switch.FastLoad"), 1, TEXT("Enable or disable fast load for level transitions"), ECVF_Default);

static TAutoConsoleVariable<float> CVarControllerDisconnectGracePeriod(TEXT("Dungeons.ControllerDisconnectGracePeriod"), 15.0f, TEXT("The timeout between disconnecting the controller and the player being removed from the world."));

static TAutoConsoleVariable<int> CVarEnableUICaching(TEXT("Dungeons.EnableUICache"), 0, TEXT("Enable or disable the caching of UI UMGs to reduce transition overhead & stalls"), ECVF_Default);

const uint32 UDungeonsGameInstance::DEFAULT_MAX_FPS = 144;

const float UDungeonsGameInstance::REFRESH_ENTITLEMENTS_DELAY_SECONDS = 4.f * 60.f;

constexpr int ANTICHEAT_VALID_PROTECTION_CHECK_VALUE = 0x53656a54;
constexpr int ANTICHEAT_VALID_USER_CHECK_VALUE = 0xa8f0846d;
constexpr int ANTICHEAT_VALID_ENFORCED_CHECK_VALUE = 0xf850826;
constexpr int ANTICHEAT_VALID_DEBUGGER_CHECK_VALUE = 0xe7f0342d;

namespace game { namespace integrity {
	float getRandomEnforceFailedCheckDelay() {
		return FMath::FRandRange(30, 600);
	}
}}

void FDungeonsConfiguration::Reset() {
	LevelSettings.Reset();
}

void FDungeonsConfiguration::PrepareTravel(const FLevelSettings& levelSettings) {
	UE_LOG(LogDungeons, Log, TEXT("FDungeonsConfiguration::PrepareTravel to unreal map name '%s' and level name '%s'"), *levelSettings.getUnrealMapName(), *levelSettings.getLevelDisplayName().ToString());
	this->LevelSettings = levelSettings;
}

const TOptional<FLevelSettings>& FDungeonsConfiguration::GetLevelSettings() const {
	return LevelSettings;
}

FCriticalSection UDungeonsGameInstance::authLock;

FString FDungeonsConfiguration::GetLevelDisplayName() const {
	if (LevelSettings.IsSet()) {
		return LevelSettings->getLevelDisplayName().ToString();
	}
	return "";
}

ELevelNames FDungeonsConfiguration::GetLevelName() const {
	if (LevelSettings.IsSet()) {
		return LevelSettings->getLevelName();
	}
	return ELevelNames::Invalid;
}

ANTICHEAT_NO_OPTIMIZATION_BEGIN
UDungeonsGameInstance::UDungeonsGameInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {
    ANTICHEAT_VIRT_PROTECT_STRINGS_BEGIN;

	//D11.PS - moved all of these into here so that it doesn't setup the delegates multiple times
	if (!IsTemplate()) {
#if defined ENGINE_WITH_CRASHREPORT_CB
		dungeonsversion::AddDungeonsVersionToCrashReport();
#endif

		//D11.PS - improves frame rate, but dont want to check it in on PC until our QA have given it a good test
#if PLATFORM_PS4 || PLATFORM_SWITCH || PLATFORM_XBOXONE
		GSlateLayoutCaching = true;
		GSlateFastWidgetPath = true;
#endif
		//D11.PS free up 4.5% of the GPU. Might be a better place for this?
#if PLATFORM_XBOXONE
		FXboxOneMisc::TakeKinectGPUReserve(true);
#endif
		OnLoginCompleteDelegate = FOnLoginCompleteDelegate::CreateUObject(this, &UDungeonsGameInstance::OnLoginLocalPlayerComplete);
		OnLoginStatusChangedDelegate = FOnLoginStatusChangedDelegate::CreateUObject(this, &UDungeonsGameInstance::OnLoginStatusChanged);
#if defined(SUBSYSTEM_HAS_TELEMETRY_CALLBACKS)
		OnWriteConnectionTypeTelemetryDelegate = FOnWriteConnectionTypeTelemetryDelegate::CreateUObject(this, &UDungeonsGameInstance::OnWriteConnectiontypeTelemetry);
#endif
#if !defined(HAS_RAW_INPUT_LISTENER)
		OnInputKeyPressedDelgate = FOnInputKeyPressedDelegate::CreateUObject(this, &UDungeonsGameInstance::OnInputKeyPressed);
		OnInputKeyReleasedDelgate = FOnInputKeyReleasedDelegate::CreateUObject(this, &UDungeonsGameInstance::OnInputKeyReleased);
#endif

		if (PLATFORM_PS4) {
			OnJoinSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(this, &UDungeonsGameInstance::OnJoinSessionCompletePS4);
		}

		FriendsInterface = CreateDefaultSubobject<UDungeonsFriendsInterface>(TEXT("DungeonsFriendsInterface"));
		FindSessionHandler = CreateDefaultSubobject<UFindSessionHandler>(TEXT("FindSessionHandler"));
		LSBuilder = MakeShareable<game::loadingscreen::LoadingScreenBuilder>(new game::loadingscreen::LoadingScreenBuilder());
		UE_LOG(LogDungeons, Log, TEXT("Dungeons Game Instance Initialized"));

		UserManager = CreateDefaultSubobject<UDungeonsUserManager>(TEXT("DungeonsUserManager"));

		if (UserManager) {
			UserManager->OnUserAddedDelegate.AddDynamic(this, &UDungeonsGameInstance::OnUserAdded);
			UserManager->OnAddedLocalPlayer.AddUObject(this, &UDungeonsGameInstance::UpdateControllerLocalPlayerDisplayIndex);
			UserManager->OnRemovedLocalPlayer.AddUObject(this, &UDungeonsGameInstance::UpdateControllerLocalPlayerDisplayIndex);
		}




#if PLATFORM_WINDOWS

		if (online::usingNullSubsystem(GetWorld()))
		{
			LoginFlow = CreateDefaultSubobject<UDungeonsLoginFlowNull>(TEXT("LoginFlow"));
		}
		else
		{
			LoginFlow = CreateDefaultSubobject<UDungeonsLoginFlowWin>(TEXT("LoginFlow"));
		}

#elif PLATFORM_SWITCH
		LoginFlow = CreateDefaultSubobject<UDungeonsLoginFlowSwitch>(TEXT("LoginFlow"));
#elif PLATFORM_PS4
		LoginFlow = CreateDefaultSubobject<UDungeonsLoginFlowPS4>(TEXT("LoginFlow"));
#else
		LoginFlow = CreateDefaultSubobject<UDungeonsLoginFlow>(TEXT("LoginFlow"));
#endif



#if PLATFORM_SWITCH
		mCurrentOperationMode = nn::oe::GetOperationMode() == nn::oe::OperationMode_Console ? SwitchOperationMode::Console : SwitchOperationMode::Handheld;
		FCoreDelegates::OnSwitchOperationModeChanged.AddUObject(this, &UDungeonsGameInstance::SwitchOperationModeChanged);
#endif

#if !WITH_EDITOR && !defined(HAS_RAW_INPUT_LISTENER)
		//D11.PS - if the widget does not have focus we do not get controller input from it, this ensures we do.
		FSlateApplication::Get().SetUnhandledKeyDownEventHandler(FOnKeyEvent::CreateUObject(this, &UDungeonsGameInstance::OnUnhandledKeyDown));
#endif
		//D11.PS Register controller type manager
		ControllerTypeManager = CreateDefaultSubobject<UControllerTypeManager>(TEXT("DungeonsControllerTypeManager"));
		ControllerTypeManager->OnControllerConnectionChangedDelegate.AddDynamic(this, &UDungeonsGameInstance::HandleControllerConnectionChange);

		KeyboardManager = CreateDefaultSubobject<UVirtualKeyboardManager>(TEXT("DungeonsKeyboardManager"));

		AssetManager = CreateDefaultSubobject<UDungeonsAssetManager>(TEXT("DungeonsAssetManager"));
		InventoryAssetManager = CreateDefaultSubobject<UDungeonsInventoryAssetManager>(TEXT("DungeonsInventoryAssetManager"));

		HintManager = CreateDefaultSubobject<UHintManager>(TEXT("HintManager"));

		ConnectionChecker = CreateDefaultSubobject<UConnectionChecker>(TEXT("ConnectionChecker"));


		FCoreDelegates::ApplicationWillEnterBackgroundDelegate.AddUObject(this, &UDungeonsGameInstance::OnApplicationEnterBackground);
		FCoreDelegates::ApplicationHasEnteredForegroundDelegate.AddUObject(this, &UDungeonsGameInstance::OnApplicationEnterForeground);		

		// D11.DB
		const ConstructorHelpers::FObjectFinder<UCurveFloat> Curve(TEXT("CurveFloat'/Game/UI/Menu/Gamma/GammaCurve.GammaCurve'"));
		if (Curve.Object)
		{
			GammaCurve = Curve.Object;
		}

	}

#if PLATFORM_SWITCH
	CVarSwitchHandheldScreenPercentage.AsVariable()->SetOnChangedCallback(FConsoleVariableDelegate::CreateUObject(this, &UDungeonsGameInstance::OnChangeHandheldScreenPercentage));
	CVarSwitchConsoleScreenPercentage.AsVariable()->SetOnChangedCallback(FConsoleVariableDelegate::CreateUObject(this, &UDungeonsGameInstance::OnChangeConsoleScreenPercentage));
#endif

	FViewport::ViewportResizedEvent.AddUObject(this, &UDungeonsGameInstance::WindowDidResize);

    ANTICHEAT_VIRT_PROTECT_STRINGS_END;

	InventoryDataholder = CreateDefaultSubobject<UInventoryItemDataHolder>(TEXT("InventoryItemDataHolder"));
}
ANTICHEAT_NO_OPTIMIZATION_END

SwitchOperationMode UDungeonsGameInstance::GetSwitchOperationMode() {
#if PLATFORM_SWITCH
	return nn::oe::GetOperationMode() == nn::oe::OperationMode_Console ? SwitchOperationMode::Console : SwitchOperationMode::Handheld;
#endif
	return SwitchOperationMode::Console;
}

bool UDungeonsGameInstance::IsHost()
{
	FNamedOnlineSession* Session = online::getSessionInterface()->GetNamedSession(DungeonsGameSessionName);
	return Session ? Session->bHosting : false;
}
void UDungeonsGameInstance::SwitchOperationModeChanged() {
#if PLATFORM_SWITCH
	auto newOperationMode = nn::oe::GetOperationMode() == nn::oe::OperationMode_Console ? SwitchOperationMode::Console : SwitchOperationMode::Handheld;
	check(newOperationMode != mCurrentOperationMode);
	mCurrentOperationMode = newOperationMode;

	if (mCurrentOperationMode == SwitchOperationMode::Console)
	{
		SetScreenPercentage(mSwitchConsoleScreenPercentage);
	}
	if (mCurrentOperationMode == SwitchOperationMode::Handheld)
	{
		SetScreenPercentage(mSwitchHandheldScreenPercentage);
	}
	OnSwitchOperationModeChanged.Broadcast();
#endif
}

void UDungeonsGameInstance::LoadGlobalSaveData(int32 localUserNum) {
	bool bSaveAlreadyExists = mSaveStates.Contains(localUserNum);
	UGlobalStateData* SaveState = bSaveAlreadyExists ? mSaveStates[localUserNum] : NewObject<UGlobalStateData>(this);

	if (!bSaveAlreadyExists)
	{
		mSaveStates.Emplace(localUserNum, SaveState);
	}

	if (mSaveState == nullptr)
	{
		mSaveState = SaveState;
	}

#if WITH_EDITOR
	if (const auto pieIndex = GetPIEClientIndex()) {
		mSaveState->SetPIESaveFolder("PIE_" + std::to_string(pieIndex));
	}
#endif

	SaveState->OnGlobalSaveDataLoaded.AddDynamic(this, &UDungeonsGameInstance::OnGlobalSaveDataLoaded);
	SaveState->Load(localUserNum);
}

void UDungeonsGameInstance::OnGlobalSaveDataLoaded(UGlobalStateData* saveData, int32 systemId)
{
	saveData->OnGlobalSaveDataLoaded.RemoveDynamic(this, &UDungeonsGameInstance::OnGlobalSaveDataLoaded);

	//This needs to be done on the main thread so use the task graph to push it to the main thread
	FFunctionGraphTask::CreateAndDispatchWhenReady([this, systemId, saveData]()
	{
		InitialiseUserSettingsFromSaveData(Cast<APlayerControllerBase>(GetFirstLocalPlayerController())); // D11.DJB
		OnSaveStateLoaded.Broadcast();
		OnGlobalSaveDataLoadFinished.Broadcast(systemId);
		UnlockCachedAchievements(saveData, systemId);
		LoadCachedEntitlements(saveData);

		if (mSaveState->GetAllProfiles().Num() > 0) {
			LookForInvites();
		}
	}, TStatId(), nullptr, ENamedThreads::GameThread);
}

void UDungeonsGameInstance::UnlockCachedAchievements(UGlobalStateData* saveData, int32 systemId)
{
#if ACHIEVEMENTS_ENABLED && !WITH_EDITOR
	auto onlineIdentity = online::getCrossplayOss()->GetIdentityIF();
	auto onlineAchievements = online::getCrossplayOss()->GetAchievementsIF();

	ULocalPlayer* const* localPlayer = GetLocalPlayers().FindByPredicate([systemId](ULocalPlayer* LocalPlayer) {
		return Cast<UDungeonsLocalPlayer>(LocalPlayer)->GetSystemUserId() == systemId; 
 	});

	if (localPlayer && (*localPlayer) && onlineIdentity.IsValid()) {
		if (TSharedPtr<const FUniqueNetId> netID = (*localPlayer)->GetPreferredUniqueNetId().GetUniqueNetId()) {
			// check if the player is a guest by comparing their NetID against the first player's
			if (!(netID == GetFirstGamePlayer()->GetPreferredUniqueNetId().GetUniqueNetId() && (*localPlayer) != GetFirstGamePlayer())) {
#if PLATFORM_XBOXONE
				// extra check for xbox guests
				if (onlineIdentity->GetSponsorUniquePlayerId(systemId)) {
					return;
				}
#endif
				// D11.SSN - check for cached achievements to unlock
				WriteAchievements(netID, saveData->GetTrackedStats().cachedAchievements);

				//check and write out any achievements you have already got.
#ifdef STEAM_BUILD
				WriteAchievements(netID, saveData->GetTrackedStats().achievements);
#endif
			}
		}
	}
#endif
}

void UDungeonsGameInstance::WriteAchievements(TSharedPtr<const FUniqueNetId> netID, std::bitset<128> &achievementList)
{
	auto onlineAchievements = online::getCrossplayOss()->GetAchievementsIF();
	if (onlineAchievements.IsValid())
	{
		// D11.SSN - check for cached achievements to unlock
		for (std::size_t i = static_cast<std::size_t>(EAchievement::REVIVE_20); i < static_cast<std::size_t>(EAchievement::ENUM_END); i++) 
		{
			if (achievementList.test(i))
			{
				FOnlineAchievementsWritePtr writeObject = MakeShareable(new FOnlineAchievementsWrite());
				writeObject->SetFloatStat(FName(*GetEnumValueToStringStripped(EAchievement(i))), 100);
				FOnlineAchievementsWriteRef writeObjectRef = writeObject.ToSharedRef();
				onlineAchievements->WriteAchievements(*netID.Get(), writeObjectRef);
			}
		}
	}
}

void UDungeonsGameInstance::LoadAllLocalControllerSaveData() const {
	for (auto* actor : TActorRange<APlayerControllerBase>(GetWorld(), APlayerControllerBase::StaticClass())) {
		if (actor->IsLocalController()) {
			actor->LoadSaveData();
		}
	}
}

void UDungeonsGameInstance::InitTelemetry(int LocalPlayerIndex, APlayerController *PlayerController) {
#if defined(SUBSYSTEM_HAS_AUTH_TOKEN_FUNCTIONS)
	if (GetUserManager()->GetInitialPlayerController() == PlayerController) {
		auto identityInterface = online::getCrossplayOss()->GetIdentityIF();
		identityInterface->AddAuthToken();
		PlayfabServices::SetAuthenticationTokenFunction([identityInterface](FString& issuerId)
		{
			FScopeLock Lock(&authLock);
			return identityInterface->GetAuthToken("playfab_key", issuerId);
		});
		PlayfabServices::AddPlayerToCache(FString::FromInt(PlayerController->GetUniqueID()));
		analytics::Analytics::GetInstance().FireEventPlayerLogin(FString::FromInt(PlayerController->GetUniqueID()), PlayerController->GetLocalPlayer()->GetNickname(), LocalPlayerIndex);
		GetWorld()->GetTimerManager().SetTimer(HeartBeatTimerHandle, this, &UDungeonsGameInstance::OnFireHeartBeatAnalytics, 60.0f, true);


		static bool FirstLoginEvent = true;
		if (FirstLoginEvent) {
			analytics::Analytics::GetInstance().FireAppLaunch();
			analytics::Analytics::GetInstance().FireReportHardware();
			FirstLoginEvent = false;
		}
	}
	bIgnoreNetworkStatusChanged = false;
#endif
}

void UDungeonsGameInstance::LoadSaveData(int LocalPlayerIndex, APlayerController *PlayerController) {
	LoadGlobalSaveData(LocalPlayerIndex);
	GetFriendsInterface()->UpdateFriendsList();
	InitTelemetry(LocalPlayerIndex, PlayerController);
}


void UDungeonsGameInstance::OnFireHeartBeatAnalytics() {
	if (GetWorld()) {
		TArray<FString> localUserIds;
		TArray<FString> networkUserIds;
		TArray<FString> networkPlatforms;
		bool anyAction = false;
		bool firstLocalFound = false;
		for (auto playerIter = GetWorld()->GetPlayerControllerIterator(); playerIter; ++playerIter) {
			if (playerIter->Get()->GetNetConnection() != nullptr) {
				networkUserIds.Add(playerIter->Get()->GetNetConnection()->PlayerId.ToString());
				networkPlatforms.Add(playerIter->Get()->GetNetConnection()->GetPlayerOnlinePlatformName().ToString());
			} else if (playerIter->Get()->GetLocalPlayer() != nullptr) {
				if (!firstLocalFound && playerIter->Get()->IsLocalController()) {
					auto firstBasePlayerController = Cast<ABasePlayerController>(playerIter->Get());
					if (firstBasePlayerController) {
						anyAction = firstBasePlayerController->GetAnyActionThisHeartbeatAndReset();
						firstLocalFound = true;
					}
				}
				localUserIds.Add(playerIter->Get()->GetLocalPlayer()->GetPreferredUniqueNetId().ToString());
			}
		}
		analytics::Analytics::GetInstance().FireHeartBeat(anyAction, localUserIds, networkUserIds, networkPlatforms);
	}
}

AAudioMusicManager* UDungeonsGameInstance::GetAudioMusicManager() {
	if (!AudioMusicManager) {
		AudioMusicManager = actorquery::getFirstActor<AAudioMusicManager>(GetWorld());
	}

	if (!AudioMusicManager) {
		// create new manager:
		if (!AudioMusicManagerBP->IsValidLowLevel())
		{
			UE_LOG(LogDungeons, Error, TEXT("Missing AudioMusicManagerBP template on DungeonsGameInstance, this breaks game Music!"));
			return nullptr;
		}

		FActorSpawnParameters SpawnParameters;
		SpawnParameters.Owner = GetWorld()->GetFirstPlayerController();

		AudioMusicManager = GetWorld()->SpawnActor<AAudioMusicManager>(AudioMusicManagerBP, SpawnParameters);
	}

	check(AudioMusicManager);
	return AudioMusicManager;
}

AAudioSFXManager* UDungeonsGameInstance::GetAudioSFXManager() {
	if (!AudioSFXManager) {
		AudioSFXManager = actorquery::getFirstActor<AAudioSFXManager>(GetWorld());
	}

	if (!AudioSFXManager) {
		if (!AudioSFXManagerBP->IsValidLowLevel())
		{
			UE_LOG(LogDungeons, Error, TEXT("Missing AudioSFXManagerBP template on DungeonsGameInstance, this breaks SFX Audio!"));
			return nullptr;
		}
		AudioSFXManager = GetWorld()->SpawnActor<AAudioSFXManager>(AudioSFXManagerBP);
	}

	check(AudioSFXManager);
	return AudioSFXManager;
}

USoundMixManager* UDungeonsGameInstance::GetSoundMixManager() {
	if (!SoundMixManager) {
		// create new manager:
		SoundMixManager = NewObject<USoundMixManager>(this, USoundMixManager::StaticClass());

	}

	check(SoundMixManager);
	return SoundMixManager;
}

ULoadingScreenInitializer* UDungeonsGameInstance::GetLoadingScreenInitializer() {
	return LSInitializer;
}

UHintManager* UDungeonsGameInstance::GetHintManager() {
	return HintManager;
}

// D11.KS
void UDungeonsGameInstance::UpdateLocalCoopCamera(const float startZoomRadius, const float endZoomRadius, const float zoomMaxmimum, float& outZoomAmount, FVector& outPosition) {
	TArray<FVector2D> screenPositions;
	TArray<FVector> worldPositions;

	auto* initialController = LocalPlayers[0]->GetPlayerController(GetWorld());

	for (auto* localPlayer : LocalPlayers) {
		if (auto* character = localPlayer->GetPlayerController(GetWorld())->GetCharacter()) {
			if (Cast<APlayerCharacter>(character)->GetWorldState() != ECharacterWorldState::InWorld) {
				continue;
			}

			auto localPlayerLocation = character->GetActorLocation();

			worldPositions.Push(localPlayerLocation);

			FVector2D transformScreenLocation;
			UGameplayStatics::ProjectWorldToScreen(initialController, localPlayerLocation, transformScreenLocation);
			screenPositions.Push(transformScreenLocation);
		}
	}

	if (worldPositions.Num() == 0 && screenPositions.Num() == 0) {
		outPosition = CoopCameraLookAtCache;
		return;
	}

	FBox worldPositionsBox(worldPositions);

	FBox2D screenPositionsBox(screenPositions);

	FVector deprojectedScreenCenter, deprojectedScreenDirection;
	UGameplayStatics::DeprojectScreenToWorld(initialController, screenPositionsBox.GetCenter(), deprojectedScreenCenter, deprojectedScreenDirection);

	FHitResult hitResult;

	auto largestDistance = worldPositionsBox.GetSize().X > worldPositionsBox.GetSize().Y ? worldPositionsBox.GetSize().X : worldPositionsBox.GetSize().Y;
	outZoomAmount = Math::clamp((largestDistance - startZoomRadius) / (endZoomRadius - startZoomRadius), 0.0f, 1.0f) * zoomMaxmimum;

	//Fall back on the world space center if our raycast fails.
	outPosition = GetWorld()->LineTraceSingleByChannel(hitResult, deprojectedScreenCenter, deprojectedScreenCenter + deprojectedScreenDirection * 10000, static_cast<ECollisionChannel>(ECustomTraceChannels::PlayerPlane)) ? hitResult.ImpactPoint : worldPositionsBox.GetCenter();

	// D11.DB
	CoopCameraLookAtCache = outPosition;
}

//D11.KS
void UDungeonsGameInstance::UpdateLocalCoopScreenStates() {
	const auto* initialController = LocalPlayers[0]->GetPlayerController(GetWorld());
	FVector2D screenPosition;

	int viewportX, viewportY;
	initialController->GetViewportSize(viewportX, viewportY);

	const auto hardBoundaryWidth = viewportY * GetLocalCoopHardBoundaryWidthPercentage();

	const FBox2D hardBoundaryRect(FVector2D(0, 0),
		FVector2D(viewportX, viewportY));

	const FBox2D dangerZoneRect(FVector2D(hardBoundaryWidth, hardBoundaryWidth ),
		FVector2D(viewportX - hardBoundaryWidth, viewportY - hardBoundaryWidth));

	const FBox2D safeZoneRect(FVector2D(hardBoundaryWidth * 2, hardBoundaryWidth * 2),
		FVector2D(viewportX - hardBoundaryWidth * 2, viewportY - hardBoundaryWidth * 2));

	bool bInCinematic = GetWorld()->GetGameState<ADungeonsGameState>()->IsCinematicPlaying();
	for (const ULocalPlayer* lp : LocalPlayers) {
		auto* playerController = Cast<ABasePlayerController>(lp->GetPlayerController(GetWorld()));
		if (const auto* character = playerController->GetCharacter()) {
			// Default to offscreen
			ELocalPlayerCameraStatus newScreenStatus = ELocalPlayerCameraStatus::OffScreen;

			// Try and find our correct screen status
			if (bInCinematic) {
				newScreenStatus = ELocalPlayerCameraStatus::InCinematic;
			}
			else if (UGameplayStatics::ProjectWorldToScreen(initialController, character->GetActorLocation(), screenPosition)) {
				if (safeZoneRect.IsInside(screenPosition)) {
					newScreenStatus = ELocalPlayerCameraStatus::InSafeZone;
				}
				else if (dangerZoneRect.IsInside(screenPosition)) {
					newScreenStatus = ELocalPlayerCameraStatus::InDangerZone;
				}
				else if (hardBoundaryRect.IsInside(screenPosition)) {
					newScreenStatus = ELocalPlayerCameraStatus::InHardBoundary;
				}
			}

			// Set current controller screen status.
			playerController->CurrentCharacterScreenStatus = newScreenStatus;
		}
	}
}

// #D11.CM
void UDungeonsGameInstance::HandlePlayerPopping(float deltaTime) {
	if (mWaitingToPop.Num() > 0) {
		TArray<TWeakObjectPtr<APlayerCharacter>> validWaitingToPop;
		for (auto& waitingToPop : mWaitingToPop) {
			if (waitingToPop.IsValid() && !waitingToPop.IsStale()) {
				validWaitingToPop.Add(waitingToPop);
			}
		}
		mWaitingToPop = validWaitingToPop;

		// Pop if we can
		if (mPopTimer >= mPopTimerThreshold) {
			if (PopPlayers()) {
				return;
			}

			// We couldn't pop, add to our timer.
			mPopTimer = Math::max(0, mPopTimer - 0.5);
		}
	}

	// Loop our local players and check if they can pop
	bool aPlayerIsOffscreen = false;
	bool pausePopTimer = false;

	for (auto player : GetLocalPlayers()) {
		if (auto* playerController = Cast<ABasePlayerController>(player->GetPlayerController(GetWorld()))) {
			if (auto* playerCharacter = playerController->GetControlledPlayerCharacter()) {

				if (playerCharacter->CanPop()) {

					bool offScreen = playerController->CurrentCharacterScreenStatus == ELocalPlayerCameraStatus::OffScreen;
					bool popBlocked = playerCharacter->GetAliveState() == EAliveState::Alive && playerCharacter->HasBeenInCombat() && !offScreen;

					// Check if there is a player offscreen.
					if (!aPlayerIsOffscreen) {
						aPlayerIsOffscreen = offScreen;
					}

					// If we're eligible for popping, but can't, we need to try to pause the pop timer
					if (!pausePopTimer) {
						pausePopTimer = popBlocked;
					}

					//  We're not pop blocked
					if (!popBlocked) {

						// Start our warning effect if we're close to popping
						if (mPopTimer / mPopTimerThreshold > 0.8 && GetPoppingLeader() != playerCharacter) {
							playerCharacter->StartPoppingWarning();
						}

						// Add us in to the pop list
						mWaitingToPop.AddUnique(playerCharacter);
						continue;
					}
				}

				// We aren't in danger of popping, remove us from the list if we're in there
				if (mWaitingToPop.Contains(playerCharacter)) {
					playerCharacter->StopPoppingWarning();
					mWaitingToPop.RemoveSwap(playerCharacter);
				}
			}
		}
	}

	if (aPlayerIsOffscreen) {
		// If even one player is off screen, accelerate the popping process, regardless of if others are in combat.
		mPopTimer = Math::min(mPopTimer + (deltaTime * 10), mPopTimerThreshold);
	}
	else if (!pausePopTimer && mWaitingToPop.Num() >= mPlayerCountPopThreshold) {
		// Count our pop timer up normally
		mPopTimer = Math::min(mPopTimer + deltaTime, mPopTimerThreshold);
	}
	else if (mPopTimer > 0) {
		// No pop increase, so decrease our timer gradually
		mPopTimer = Math::max(mPopTimer - deltaTime, 0);
	}

}

void UDungeonsGameInstance::ResetPoppingData() {
	mWaitingToPop.Empty(4);
	mPopTimer = 0;
}

void UDungeonsGameInstance::PopLocalPlayersToCheckpoint() {
	for (auto player : GetLocalPlayers()) {
		if (auto* playerController = Cast<ABasePlayerController>(player->GetPlayerController(GetWorld()))) {
			if (auto* playerCharacter = playerController->GetControlledPlayerCharacter()) {
				playerCharacter->RestartAtCheckpoint();
			}
		}
	}

	CheckpointRestartCooldown = true;
	GetTimerManager().SetTimer(CheckpointRestartCooldownTimer, [&] {
		CheckpointRestartCooldown = false;
		OnCheckpointRespawnCooldown.Broadcast();
	}, CheckpointRestartCooldownTime, false);
}

int UDungeonsGameInstance::GetTimeUntilTeleportCooldown() {
	int timeLeft = 0;
	if (GetTimerManager().IsTimerActive(CheckpointRestartCooldownTimer)) {
		timeLeft = GetTimerManager().GetTimerRemaining(CheckpointRestartCooldownTimer);
	}

	return Math::ceil(timeLeft);
}

bool UDungeonsGameInstance::CanTeleportToCheckpoint() {
	bool canTeleport = !CheckpointRestartCooldown && !IsLocalCoop();
	if (auto controller = GetPrimaryPlayerController()) {
		if(auto baseController = Cast<ABasePlayerController>(controller)) {
			if (auto playerCharacter = baseController->GetControlledPlayerCharacter()) {
				canTeleport &= playerCharacter->GetAliveState() == EAliveState::Alive;
				canTeleport &= playerCharacter->GetWorldState() == ECharacterWorldState::InWorld;
			}
		}
	}
	
	return canTeleport;
}

// #D11.CM - It is possible for this function to return a nullptr
APlayerCharacter* UDungeonsGameInstance::GetNextObjectiveBasedLeader(const FVector target, APlayerCharacter* excludeCharacter) {
	// #D11.CM - This is an "As The Crow Flies" straight line.
	// There could be a benefit for path finding to the target to get a better distance reading in more complex environments.
	const auto isViable = [](AActor* player) {
		return Cast<ULocalPlayer>(player->GetNetOwningPlayer()) != nullptr && Cast<APlayerCharacter>(player)->InWorldAndAlive();
	};

	float dist = 0;
	const int excludeIndex = excludeCharacter ? excludeCharacter->GetUniqueID() : INDEX_NONE;
	return Cast<APlayerCharacter>(actorquery::getClosestToLocation<APlayerCharacter>(target, 100000, dist, GetWorld(), isViable, excludeIndex, true));
}

// Get our first objective, if there is no objectives then get the next door.
bool UDungeonsGameInstance::GetNextProgressTarget(FVector& outPosition) const {
	if (auto* gameBP = Cast<ABasePlayerController>(GetFirstLocalPlayerController())->GetCachedGameBP()) {
		if (!gameBP->bUsesObjectives) {
			return false;
		}

		APlayerCharacter* furthestPlayer = gameBP->GetGame()->progress().currentlyLocalFurthest()->player;
		if (gameBP->GetObjectiveLocations().bUseLocations && gameBP->GetObjectiveLocations().Locations.Num() > 0 && gameBP->IsPlayerInRangeOfObjective(furthestPlayer)) {
			outPosition = gameBP->GetObjectiveLocations().Locations[0];
			return true;
		}

		const TOptional<game::PlayerTile> furthestTile = gameBP->GetGame()->progress().currentlyLocalFurthest();
		if (furthestTile && furthestTile.IsSet()) {
			return gameBP->GetNextDoor(furthestTile->player, outPosition);
		}
	}

	return false;
}


static void TestMessageBoxOkPressed()
{
	UE_LOG(LogPlayerManagement, Log, TEXT("TestMessageBoxOkPressed()"));
}
static void TestMessageBoxYesPressed()
{
	UE_LOG(LogPlayerManagement, Log, TEXT("TestMessageBoxYesPressed()"));
}
static void TestMessageBoxNoPressed()
{
	UE_LOG(LogPlayerManagement, Log, TEXT("TestMessageBoxNoPressed()"));
}

void UDungeonsGameInstance::MessageBoxButtonPressed(EMessageType messagetype, int button) const
{
	switch (messagetype)
	{
	case OneButton:
		TestMessageBoxOkPressed();

	case TwoButton:
		button == 1 ? TestMessageBoxYesPressed() : TestMessageBoxNoPressed() ;

	}
}

APlayerCharacter* UDungeonsGameInstance::GetPoppingLeader() {
	const auto* primaryController = Cast<ABasePlayerController>(GetFirstLocalPlayerController(GetWorld()));
	if (!primaryController || !primaryController->GetControlledPlayerCharacter()) {
		return nullptr;
	}

	APlayerCharacter* leader = nullptr;
	FVector targetPos(0);

	// Find a leader
	if (auto* gameBP = Cast<ABasePlayerController>(GetFirstLocalPlayerController())->GetCachedGameBP()) {
		if (gameBP->bUsesObjectives) {
			if (GetNextProgressTarget(targetPos)) {
				leader = GetNextObjectiveBasedLeader(targetPos);
			}
		}
		else if (primaryController->GetControlledPlayerCharacter()->InWorldAndAlive()) {
			leader = primaryController->GetControlledPlayerCharacter();
		}
	}

	// Cache last leader
	if (leader)
	{
		lastLeader = leader;
	}

	return leader;
}

APlayerCharacter* UDungeonsGameInstance::GetLastLeader()
{
	//Best way of updating the last leader if possible.
	GetPoppingLeader();
	return lastLeader.IsValid() ? lastLeader.Get() : nullptr;
}


bool UDungeonsGameInstance::PopPlayers() {
	// Get our leader
	auto* leader = GetPoppingLeader();
	if (!leader) {
		return false;
	}

	// Remove the leader from the popping list.
	mWaitingToPop.Remove(leader);

	// Get the first to pop, we only want to pop 1 player per tick.
	for (TWeakObjectPtr<APlayerCharacter> playerToPop : mWaitingToPop) {	
		// Calculate the angle on the popping radius that we should occupy.
		float poppingAngle = GetNumLocalPlayers() == 1 ? 0 : mWaitingToPop.Num() * ((2 * PI) / (GetNumLocalPlayers() - 1));
		if (!playerToPop->PopToPlayer(leader, poppingAngle)) {
			return false;
		}

		playerToPop->StopPoppingWarning();
		mWaitingToPop.RemoveSwap(playerToPop);
		return true;
	}

	return false;

}

void UDungeonsGameInstance::UpdateControllerLocalPlayerDisplayIndex() {
	for (auto* controller : TActorRange<APlayerControllerBase>(GetWorld(), APlayerControllerBase::StaticClass())) {
		controller->UpdateLocalPlayerDisplayIndex();
	}
}

APlayerCharacter* UDungeonsGameInstance::GetAlivePlayerCharacter(ULocalPlayer* local) {
	if (auto* playerController = Cast<ABasePlayerController>(local->GetPlayerController(GetWorld()))) {
		if (auto* playerCharacter = playerController->GetControlledPlayerCharacter()) {
			if (playerCharacter->GetAliveState() == EAliveState::Alive) {
				return playerCharacter;
			}
		}
	}

	return nullptr;
}

void UDungeonsGameInstance::OnUserAdded(int LocalPlayerIndex, APlayerController* PlayerController) {
	if (IsSaveStateValid(LocalPlayerIndex)) {
		if (auto* playerControllerBase = Cast<APlayerControllerBase>(PlayerController)) {
			check(playerControllerBase->IsLocalController() && "Platform controllers are assumed be local in this code path");
			playerControllerBase->LoadSaveData();
		}
	}
}

bool UDungeonsGameInstance::HandleLocalPlayerLeave(int32 incomingID) {
	if (auto* pWorld = GetWorld()) {
		//D11.KS - Don't disconnect the main player.
		auto initialControllerId = UserManager->GetInitialUser();
		if (initialControllerId == -1 || initialControllerId == incomingID) return false;

		return LocalPlayerLeave(incomingID);
	}

	return false;
}



void UDungeonsGameInstance::AddRecentlyLeftController(int32 incomingID, const float fGracePeriod)
{
	if (ULocalPlayer* localPlayer = FindLocalPlayerFromControllerId(incomingID))
	{
		if (GetLoginFlow()->GetLoggingInPlayerController() == localPlayer->GetPlayerController(GetWorld()))
		{
			return;
		}
		//D11.SC Add permanent invisibility effect
		if (auto playerController = Cast<ABasePlayerController>(localPlayer->GetPlayerController(GetWorld())))
		{
			mRecentlyLeftControllers.Add(FRecentlLeftControllerData(incomingID, fGracePeriod));

			if (auto* pPlayerCharacter = playerController->GetControlledPlayerCharacter())
			{
				if (auto abilitySystemComponent = pPlayerCharacter->GetAbilitySystemComponent())
				{
					auto spec = effects::CreateGameplayEffectSpec<UPermanentInvisibilityEffect>(abilitySystemComponent);
					abilitySystemComponent->ApplyGameplayEffectSpecToSelf(spec);
				}
			}
		}
		else if(auto menuPlayerController = Cast<ABaseMenuPlayerController>(localPlayer->GetPlayerController(GetWorld())))
		{
			OnLocalPlayerChangedUser.Broadcast(incomingID);
		}
	}
}

bool UDungeonsGameInstance::LocalPlayerLeave(int32 incomingID)
{

	if (ULocalPlayer* localPlayer = FindLocalPlayerFromControllerId(incomingID))
	{
		
		OnLocalPlayerLeave.Broadcast(localPlayer);
		localPlayer->SetCachedUniqueNetId(nullptr);

		int UserId = Cast<UDungeonsLocalPlayer>(localPlayer)->GetSystemUserId();
		if (UserId != UserManager->GetInitialUserSystemId())
		{
			// D11.PC   Remove the save data associated with this LocalPlayer
			mSaveStates.Remove(UserId);
		}

		if (auto playerController = Cast<ABasePlayerController>(localPlayer->GetPlayerController(GetWorld())))
		{
			playerController->ResetControllerLightColor();

			//push and remove invisibility effect to give it a pop
			if (auto* pPlayerCharacter = playerController->GetControlledPlayerCharacter())
			{
				if (auto abilitySystemComponent = pPlayerCharacter->GetAbilitySystemComponent())
				{
					auto spec = effects::CreateGameplayEffectSpec<UPermanentInvisibilityEffect>(abilitySystemComponent);
					abilitySystemComponent->ApplyGameplayEffectSpecToSelf(spec);

					FGameplayEffectQuery q;
					q.EffectDefinition = UPermanentInvisibilityEffect::StaticClass();
					abilitySystemComponent->RemoveActiveEffects(q);
				}
			}
		}

		if ((GetNumLocalPlayers() - 1) < 2)
		{
			GetFirstLocalPlayerController(GetWorld())->ResetControllerLightColor();
		}

		if (UserManager->RemovePlayer(incomingID))
		{
			//D11.KS - Refresh all display names.
			for (auto lp : GetWorld()->GetGameInstance()->GetLocalPlayers())
			{
				if (auto pc = lp->GetPlayerController(GetWorld()))
				{
					if (auto lpPlayerState = Cast<ABasePlayerState>(pc->PlayerState))
					{
						lpPlayerState->RefreshDisplayName();
						lpPlayerState->RefreshSecondaryUniqueId();
					}
				}
			}

			return true;
		}
	}

	return false;
}

void UDungeonsGameInstance::HandleControllerConnectionChange(bool bIsConnection, int32 GameUserIndex)
{
	if (!bIsConnection)
	{
		if (GameUserIndex != GetUserManager()->GetInitialUser())
		{
			const float fGracePeriod = CVarControllerDisconnectGracePeriod.GetValueOnAnyThread();
			AddRecentlyLeftController(GameUserIndex, fGracePeriod);
		}
	}
	else
	{
		RemoveRecentlyLeftController(GameUserIndex);
	}
}

void UDungeonsGameInstance::RemoveRecentlyLeftController(int32 incomingID)
{
	const int32 FoundIndex = mRecentlyLeftControllers.IndexOfByPredicate([incomingID](const FRecentlLeftControllerData& c) { return incomingID == c.mControllerID; });
	if (FoundIndex != INDEX_NONE)
	{
		//D11.SC Remove permanent invisibility effect
		if (ULocalPlayer* localPlayer = FindLocalPlayerFromControllerId(mRecentlyLeftControllers[FoundIndex].mControllerID))
		{
			if (auto playerController = Cast<ABasePlayerController>(localPlayer->GetPlayerController(GetWorld())))
			{
				if (auto* pPlayerCharacter = playerController->GetControlledPlayerCharacter())
				{
					if (auto abilitySystemComponent = pPlayerCharacter->GetAbilitySystemComponent())
					{
						FGameplayEffectQuery q;
						q.EffectDefinition = UPermanentInvisibilityEffect::StaticClass();
						abilitySystemComponent->RemoveActiveEffects(q);
					}
				}
			}
		}

		mRecentlyLeftControllers.RemoveAtSwap(FoundIndex);
	}
}

APlayerController* UDungeonsGameInstance::HandleLocalPlayerRequest(int32 incomingID, FKey incomingKey, StatusEnum& Branches) {
	Branches = StatusEnum::Fail;

	if (GetWorld()) {
		//D11.KS - Don't disconnect the main player.
		const auto initialControllerId = UserManager->GetInitialUser();

		if (initialControllerId == -1 || initialControllerId == incomingID) {
			return nullptr;
		}

		APlayerController* PlayerController = GetPlayerControllerFromControllerID(incomingID);

		//Switch Single Joycon thumbstick is considered as RightThumbstick
		bool needsToUseRightThumbstick = false;
#if PLATFORM_SWITCH
		needsToUseRightThumbstick = incomingKey == EKeys::Gamepad_RightThumbstick ? true : false;
#endif
		if ((incomingKey == EKeys::Gamepad_LeftThumbstick || needsToUseRightThumbstick) && LocalPlayers.Num() < PlatformMaxPlayers && (!PlayerController))
		{
			PlayerController = UserManager->CreatePlayer(incomingID, true);

			Branches = PlayerController ? StatusEnum::Success : StatusEnum::Fail;

			if(PlayerController)
				!PlayerController->IsA(ABaseMenuPlayerController::StaticClass()) ? PlayerController->GetPlayerState<ABasePlayerState>()->bLocallyDroppingIn = true : false;

			return PlayerController;
		}
	}

	return nullptr;
}

int UDungeonsGameInstance::GetMaxLocalPlayers()
{
	return UGameVersion::GetPlatformEnum() == EPlatformType::E_PLATFORM_SWITCH ? 2 : 4;
}

APlayerCharacter* UDungeonsGameInstance::GetFirstPlayerCharacter()
{
	auto playerController = Cast<ABasePlayerController>(GetFirstLocalPlayerController());
	if (playerController)
		return playerController->GetControlledPlayerCharacter();
	return nullptr;
}

APlayerController* UDungeonsGameInstance::GetPlayerControllerFromIndex(int32 incomingID)
{
	return incomingID < LocalPlayers.Num() ? GetLocalPlayers()[incomingID]->GetPlayerController(GetWorld()) : nullptr;
}

APlayerController* UDungeonsGameInstance::GetPlayerControllerFromControllerID(int32 ControllerID)
{

	if (ULocalPlayer* const* PlayerFound = GetLocalPlayers().FindByPredicate([ControllerID](ULocalPlayer* LocalPlayer) { return LocalPlayer->GetControllerId() == ControllerID; }))
	{
		return (*PlayerFound)->GetPlayerController(GetWorld());
	}
	return nullptr;
}

void UDungeonsGameInstance::OnFakeSessionInviteReceived(FBlueprintGameSession& GameSession) {
	SessionInviteReceived(GameSession);
}

void UDungeonsGameInstance::OnSessionInviteReceived(const FUniqueNetId& UserId, const FUniqueNetId& FromId, const FString& AppId, const FOnlineSessionSearchResult& InviteResult) {

	const auto IsOfflinePlay = [this] {return !online::isOnlineSession() && !IsInMenu(); };
	if (IsOfflinePlay()) {
		return;
	}

	FBlueprintSessionResult wrapper;
	wrapper.OnlineResult = InviteResult;
	SessionSettings ss(InviteResult);

	FBlueprintGameSession GameSession;
	GameSession.SearchResult = InviteResult;
	GameSession.HostDisplayName = InviteResult.Session.OwningUserName;
	GameSession.HostUniqueNetId = InviteResult.Session.OwningUserId;
	GameSession.IsPublic = !ss.IsPrivate();
	GameSession.ReceivedStartTime = GetWorld()->GetTimeSeconds();
	GameSession.LevelSettings = ss.GetLevelSettings();
	GameSession.InviteSenderName = sessionSettings::GetInviteSenderName(ss.GetPlayerNames(), FromId.ToString());

	// Broadcast to UI
	SessionInviteReceived(GameSession);
}

void UDungeonsGameInstance::HandleInviteAcceptedError() {
	if (IsInMenu()) {
		EndLoadingScreen(GetWorld(), "", UDungeonsGameInstance::fDefaultFadeOutTime, UDungeonsGameInstance::fDefaultFadeInTime);
	}
	else {
		BeginLoadingScreenWithTravel(FLevelSettings(), EMapLoadType::OpenMenu);
	}
	PushGlobalErrorMessage(EGlobalMessageTypes::InviteTimedOut);

	UE_LOG(LogOnline, Warning, TEXT("Something happened while accepting the invite"));
}

void UDungeonsGameInstance::OnSessionInviteAccepted(const bool bSuccess, const int32 ControllerId, TSharedPtr<const FUniqueNetId> PlayerUniqueNetId, const FOnlineSessionSearchResult& sessionResult) {
	UE_LOG(LogOnline, Log, TEXT("OnSessionInviteAccepted"));

	if (!bSuccess) {
		HandleInviteAcceptedError();
		return;
	}

	PendingInvite.bSuccessfullyAccepted = bSuccess;
	PendingInvite.ControllerId = ControllerId;
	PendingInvite.UserId = PlayerUniqueNetId;
	PendingInvite.InviteResult = sessionResult;
	PendingInvite.bPrivilegesCheckedAndAllowed = false;

	if (!online::shouldShowSession(sessionResult, GetWorld()))
	{
		return;
	}

	// D11.PC If it was an user than the initial one we don't want to process the invite
	// and want to inform the user of that 
	if (GetUserManager()->GetInitialUser() != -1 && ControllerId != GetUserManager()->GetInitialUser())
	{
		OnNonInitialUserAcceptedInvite();
		return;
	}
	
	// D11.PC If we're on a local multiplayer, inform the user that has to choose between join the session
	// and kick all the other local players or continue the local play
	if (LocalPlayers.Num() > 1)
	{
		OnInviteAcceptedOnLocalPlay();
		return;
	}

	// D11.AH Due to a bug on the PS5 where you can join your own hosted session in the system UI,we want to ignore
	// if the local player is the same user as the invite user.
	auto initialUser = GetUserManager()->GetInitialUser();
	auto currentUserId = online::getCrossplayOss()->GetIdentityIF()->GetUniquePlayerId(initialUser);
	if ( currentUserId.IsValid() && currentUserId == sessionResult.Session.OwningUserId)
	{
		return;
	}

	// update the connection status, in case we are offline
	CheckConnectionStatus();

	OnSessionInviteAcceptedBP(bSuccess, ControllerId);
	ProcessAcceptedInvite();
	return;
}

void UDungeonsGameInstance::ProcessAcceptedInvite()
{
	KickAllLocalPlayers();

	if (!PendingInvite.bSuccessfullyAccepted)
	{
		HandleInviteAcceptedError();
		return;
	}

	if (!PendingInvite.UserId.IsValid())
	{
		UE_LOG(LogOnline, Warning, TEXT("Invite accept returned no user."));
		return;
	}

	LookForInvites();
}

void UDungeonsGameInstance::DeclineInvite()
{
	PendingInvite.UserId.Reset();
	PendingInvite = FDungeonsPendingInvite();
}

void UDungeonsGameInstance::AcceptInvite(const FOnlineSessionSearchResult& sessionResult)
{
	AcceptedInvite = sessionResult.GetSessionIdStr();
	JoinSessionProxy = UJoinDungeonsSessionCallbackProxy::JoinDungeonsSession(GetWorld(), GetWorld()->GetFirstPlayerController(), sessionResult);
	JoinSessionProxy->Activate();
	PendingInvite.UserId.Reset();
	PendingInvite = FDungeonsPendingInvite();
}

void UDungeonsGameInstance::AcceptInvite(const FBlueprintGameSession& gameSession)
{
	// Set the pending invite
	PendingInvite.ControllerId = GetUserManager()->GetInitialUser();
	PendingInvite.UserId = online::getCrossplayOss()->GetIdentityIF()->GetUniquePlayerIdForSessionSubsystem(PendingInvite.ControllerId);
	PendingInvite.InviteResult = gameSession.SearchResult;
	PendingInvite.bPrivilegesCheckedAndAllowed = false;
	AcceptedInvite = gameSession.SearchResult.GetSessionIdStr();

	LookForInvites();
}

void UDungeonsGameInstance::OnSessionFailure(const FUniqueNetId& id, ESessionFailure::Type reason) {
	if (id.ToString() == "JoinPermissionError") {
		PushGlobalErrorMessage(EGlobalMessageTypes::JoinPermissionError);
		return;
	}

#if PLATFORM_XBOXONE
	if (ESessionFailure::ServiceConnectionLost == reason && "XboxRTAConnectionLost" == id.ToString()) {
		bool inSession = online::getCurrentSession() != nullptr;
		/* XR-074 Loss of Connectivity to Xbox and Partner Services https://docs.microsoft.com/en-us/gaming/xbox-live/policies/xr/live-pc-xr074 */
		if (!inSession && IsInMenu()) {
			PushGlobalErrorMessage(EGlobalMessageTypes::NoInternetConnection);
		}
		return;
	}
#endif

	if (!online::isOnlineSession())
	{
		return;
	}

	auto pWorld = GetWorld();
	ABasePlayerController** ActorPtr = nullptr;
	TArray<ABasePlayerController*> Actors;

	if ( pWorld != nullptr )
	{
		Actors = actorquery::getActors<ABasePlayerController>(pWorld);
		ActorPtr = Actors.FindByPredicate([&id](const ABasePlayerController* Player)
		{
			if (Player && Player->IsValidLowLevel() && Player->PlayerState && Player->PlayerState->UniqueId.IsValid())
			{
				return id == *Player->PlayerState->UniqueId;
			}
			return false;
		});
	}

	ABasePlayerController* PlayerController = ActorPtr ? *ActorPtr : nullptr;
	if (PlayerController && !PlayerController->IsLocalController())
	{
		//The session failure was triggered on a client netid (multiplayer) - kick/drop connection
		if (Cast<UNetConnection>(PlayerController->Player) != nullptr)
		{
			if (PlayerController->GetPawn() != nullptr)
			{
				PlayerController->GetPawn()->Destroy();
			}
			PlayerController->Destroy();
		}
	}
	else
	{
		const TMap<FString, EGlobalMessageTypes> errorMap = {
			{"NotConnectedToOnlineService", EGlobalMessageTypes::NotConnectedToOnlineService},
			{"DisconnectedFromHost", EGlobalMessageTypes::DisconnectedFromHost},
			{"JoinPermissionError", EGlobalMessageTypes::JoinPermissionError}
		};

		if (errorMap.Contains(id.ToString()) //Currently, this is how we distinguish the local player from a remote
#ifdef SUBSYSTEM_HAS_GAME_SUSPENDED_SESSION_FAILURE
			&& (reason != ESessionFailure::GameSuspended)
#endif
			)
		{
			if (IsInMenu()) {
				EndLoadingScreen(GetWorld(), "", UDungeonsGameInstance::fDefaultFadeOutTime, UDungeonsGameInstance::fDefaultFadeInTime);
			}
			else {
				BeginLoadingScreenWithTravel(FLevelSettings(), EMapLoadType::OpenMenu);
			}
			PushGlobalErrorMessage(errorMap[id.ToString()]);
		}
	}
}

void UDungeonsGameInstance::CatchNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString) {
	UE_LOG(LogOnline, Log, TEXT("Network Failure: %s"), *ErrorString);

	if (bPlayerWasKicked || ErrorString == "ClientKicked") {
		bPlayerWasKicked = false;
		AcceptedInvite = "";
		PushGlobalErrorMessage(EGlobalMessageTypes::ClientKicked);
	}
	else {
		OnNetworkError(FailureType, NetDriver->GetNetMode() != NM_Client);
	}
}

void UDungeonsGameInstance::HandleNetworkConnectionStatusChanged(const FString& ServiceName, EOnlineServerConnectionStatus::Type LastConnectionStatus, EOnlineServerConnectionStatus::Type ConnectionStatus)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, FString::Printf(TEXT("NetworConnectionStatusChanged  %d  to  %d"), (int)LastConnectionStatus, (int)ConnectionStatus));
	}

	// D11.PC Check if we're in a session
	bool inSession = online::getCurrentSession() != nullptr;

	// Check if we disconnected and are on an online game. If so, go to main menu
	if (!bIgnoreNetworkStatusChanged && inSession && ConnectionStatus != EOnlineServerConnectionStatus::Normal && ConnectionStatus != EOnlineServerConnectionStatus::Connected)
	{
		if (IsInMenu())
		{
			EndLoadingScreen(GetWorld(), "", UDungeonsGameInstance::fDefaultFadeOutTime, UDungeonsGameInstance::fDefaultFadeInTime);
		}
		else
		{
			BeginLoadingScreenWithTravel(FLevelSettings(), EMapLoadType::OpenMenu);
			PushGlobalErrorMessage(EGlobalMessageTypes::NoInternetConnection);
		}
	}
	CurrentConnectionStatus = ConnectionStatus;
}

bool UDungeonsGameInstance::CheckConnectionStatus()
{
#if PLATFORM_WINDOWS || PLATFORM_XBOXONE
	CurrentConnectionStatus = ConnectionChecker->CheckConnection();
#endif

	return CurrentConnectionStatus == EOnlineServerConnectionStatus::Connected;
}

bool UDungeonsGameInstance::IsConnected()
{
	return CurrentConnectionStatus == EOnlineServerConnectionStatus::Connected;
}

bool UDungeonsGameInstance::CanGoOnline()
{
//D11.IG - disabling the feature to promote offline games to online until Cross-Play is fully implemented and stable
#if defined(GO_ONLINE_ENABLED)
	return !IsInMenu() && !online::isOnlineSession() && CheckConnectionStatus() && !IsLocalCoop() && nullptr == CreateSessionProxy;
#else
	return false;
#endif
}

void UDungeonsGameInstance::GoOnline()
{
	if (!CanGoOnline())
		return;
	if (auto world = GetWorld())
	{
		if (const auto sessionInterface = online::getSessionInterface())
		{
			auto levelSettings = Configuration.GetLevelSettings();			
			CreateSessionProxy = UCreateDungeonsSessionCallbackProxy::CreateSession(
				world, 
				levelSettings.IsSet() ? levelSettings.GetValue() : FLevelSettings(),
				false
			);
			CreateSessionProxy->CreateSessionSucceeded.AddLambda([this](const FLevelSettings&) {
				OnCreateSessionCompleted.Broadcast();
				CreateSessionProxy = nullptr;
			});
			CreateSessionProxy->CreateSessionFailed.AddLambda([this](ESessionFailureReason reason) {
				OnCreateSessionFailed.Broadcast();
				CreateSessionProxy = nullptr;
			});
			CreateSessionProxy->Activate();
		}
	}
}

void UDungeonsGameInstance::JoinSessionFailed(ESessionFailureReason Reason, ESessionJoinFailureReason ExtendedReason)
{
	OnJoinSessionFailed.Broadcast(ExtendedReason);
}

UDungeonsFriendsInterface* UDungeonsGameInstance::GetFriendsInterface() {
	return FriendsInterface;
}

UFindSessionHandler* UDungeonsGameInstance::GetFindSessionHandler() const {
	return FindSessionHandler;
}

void UDungeonsGameInstance::OnPreLoadMap(const FString& mapName) {
	BeginLoadingScreen(mapName, false);
}

void UDungeonsGameInstance::OnPostLoadMap(UWorld* world) {
	if(world){
		const auto map = world->GetMapName();
		if (map.EndsWith("Menu")) {
			LSInitializer->TearDownLoadingScreen(GetWorld(), { UDungeonsGameInstance::fDefaultFadeOutTime, UDungeonsGameInstance::fDefaultFadeInTime });
			Configuration.Reset();
		}
		online::getLiveOps(this)->RequestDataUpdateAsync();
	}
}

void UDungeonsGameInstance::BeginLoadingScreen(const FString& mapName, bool isSeamlessTravel) {
	if (!bFirstLoad) {
		return;
	}

	// D11.SSN
	// only perform on Windows, all other platforms done through engine
#if PLATFORM_WINDOWS
	FLoadingScreenAttributes loadingScreen;
	loadingScreen.PlaybackType = EMoviePlaybackType::MT_LoadingLoop;
	loadingScreen.bAutoCompleteWhenLoadingCompletes = configfile::SkipSplash();
	loadingScreen.bMoviesAreSkippable = configfile::SkipSplash();
#if UE_BUILD_SHIPPING
	loadingScreen.bMoviesAreSkippable = false;
	loadingScreen.bAutoCompleteWhenLoadingCompletes = false;
#endif
	// length of the movies excluding loader_splash, don't force in dev environment
	if (!loadingScreen.bMoviesAreSkippable) {
		loadingScreen.MinimumLoadingScreenDisplayTime = configfile::SplashTime();
	}

	TArray<FString> StartupMovies;
	GConfig->GetArray(TEXT("Splash"), TEXT("StartupMovies"), StartupMovies, GGameIni);

	// double check that the movies exist
	// we don't know the extension so compare against any file in the directory with the same name for now
	TArray<FString> ExistingMovieFiles;
	IFileManager::Get().FindFiles(ExistingMovieFiles, *(FPaths::ProjectContentDir() + TEXT("Movies")));

	bool bHasValidMovie = false;
	for (const FString& Movie : StartupMovies) {
		bool bFound = ExistingMovieFiles.ContainsByPredicate([&Movie](const FString& ExistingMovie) {
			return ExistingMovie.Contains(Movie);
		});

		if (bFound) {
			bHasValidMovie = true;
			loadingScreen.MoviePaths.Add(Movie);
		}
	}

	if (bHasValidMovie) {
		// These movies are all considered safe to play in very early startup sequences
		loadingScreen.bAllowInEarlyStartup = true;
		// now setup the actual loading screen
		GetMoviePlayer()->SetupLoadingScreen(loadingScreen);
	}
#endif

#if PLATFORM_XBOXONE
	//D11.SC Seems a bit odd, but XBox finding if files exist outside of pak files is slow, this triggers off an internal caching of found mp4 files to prevent stalls during game time
	TArray<FString> ExistingMovieFiles;
	IFileManager::Get().FindFilesRecursive(ExistingMovieFiles, *(FPaths::ProjectContentDir() + TEXT("Movies")), TEXT("*.mp4"), true, false);
#endif

	bFirstLoad = false;
}

void UDungeonsGameInstance::BeginLoadingScreenWithTravel(const FLevelSettings& levelSettings, EMapLoadType mapLoadType, float fadeOutTime, float fadeinTime, APlayerController* playerController, const FString& connectionStr) {
	BeginLoadingScreenWithTravel({ levelSettings, mapLoadType, {}, false }, fadeOutTime, fadeinTime);
}

ANTICHEAT_NO_OPTIMIZATION_BEGIN
void UDungeonsGameInstance::BeginLoadingScreenWithTravel(const MapLoadData& mapLoadData, float fadeOutTime, float fadeinTime) {
	ANTICHEAT_OBFUSCATE_BEGIN
		auto gameBP = actorquery::getFirstActor<AGameBP>(GetWorld());
		if (gameBP != nullptr) {
			auto PC = Cast<ABasePlayerController>(GetFirstLocalPlayerController(GetWorld()));
			if (PC && PC->IsValidLowLevel())
			{
				auto& frameTimeTracker = PC->GetFrameTimeTracker();
				analytics::Analytics::GetInstance().FireLevelPerformanceSummary(gameBP->GetGame(), Configuration.GetLevelSettings().Get(FLevelSettings()), frameTimeTracker.GetFrameTimeCounts(), frameTimeTracker.GetFrameDeltaCounts());
				frameTimeTracker.Reset();
			}
		}
	FPlatformMisc::LowLevelOutputDebugStringf(TEXT("##### UDungeonsGameInstance::BeginLoadingScreenWithTravel #####\n"));

#if PLATFORM_XBOXONE
	if (auto xCloud = XCloud::Get())
	{
		if (xCloud->IsStreaming())
		{
			xCloud->DisableTouchControls();
		}
	}
#endif

	if (fadeOutTime < 0.0f)
		fadeOutTime = fDefaultFadeOutTime;
	if (fadeinTime < 0.0f)
		fadeinTime = fDefaultFadeInTime;

	LSInitializer->InitializeLoadingScreen(GetWorld(), { fadeOutTime, fadeinTime }, mapLoadData);

	// #D11.CM - Clear our popping list on travel
	ResetPoppingData();

	ANTICHEAT_CHECK_PROTECTION_CODE_INTEGRITY(EnforcedCodeIntegrityCheck, ANTICHEAT_VALID_ENFORCED_CHECK_VALUE);

	if (EnforcedCodeIntegrityCheck != ANTICHEAT_VALID_ENFORCED_CHECK_VALUE) {
		FTimerHandle enforceFailedIntegrityCheckTimerHandle;
		const auto enforceFailedIntegrityCheckTimerDelegate = FTimerDelegate::CreateLambda([=] {
			Shutdown();
		});
		
		GetTimerManager().SetTimer(enforceFailedIntegrityCheckTimerHandle, enforceFailedIntegrityCheckTimerDelegate, game::integrity::getRandomEnforceFailedCheckDelay(), false);
	}

	EnforcedCodeIntegrityCheck = 0;

	GetTimerManager().ClearTimer(PollSessionTimerHandle);
	GetTimerManager().ClearTimer(PollSessionFinishHandle);

	ANTICHEAT_OBFUSCATE_END
}
ANTICHEAT_NO_OPTIMIZATION_END

void UDungeonsGameInstance::EndLoadingScreen(UWorld* world, FName worldName, float fadeOutTime, float fadeInTime) {
	LSInitializer->TearDownLoadingScreen(GetWorld(), { fadeOutTime, fadeInTime });
}

UMinecraftAPI* UDungeonsGameInstance::GetMinecraftAPI() const {
	return MinecraftAPI;
}

int32 UDungeonsGameInstance::GetLocalPlayerUserCount() const {
	return GetNumLocalPlayers();
}

bool UDungeonsGameInstance::IsLocalCoop() const {
	return GetNumLocalPlayers() > 1;
}

UCosmeticsLibrary* UDungeonsGameInstance::GetCosmeticsLibrary() const {
	return CosmeticsLibrary;
}

UMissionThemeLibrary* UDungeonsGameInstance::GetMissionThemeLibrary() const {
	return MissionThemeLibrary;
}

UMobIconLibrary* UDungeonsGameInstance::GetMobIconLibrary() const {
	return MobIconLibrary;
}

USkinsLibrary* UDungeonsGameInstance::GetSkinsLibrary() const {
	return SkinsLibrary;
}

UEndlessStruggleLibrary* UDungeonsGameInstance::GetEndlessStruggleLibrary() const {
	return EndlessStruggleLibrary;
}

UEntitlementsRepository* UDungeonsGameInstance::GetEntitlementsRepository() const {
	return EntitlementsRepository;
}

ANTICHEAT_NO_OPTIMIZATION_BEGIN
void UDungeonsGameInstance::Init() {
	ANTICHEAT_VIRT_PROTECT_STRINGS_BEGIN;
	Super::Init();

	ANTICHEAT_CHECK_PROTECTION_CODE_INTEGRITY(ProtectionCodeIntegrityCheck, ANTICHEAT_VALID_PROTECTION_CHECK_VALUE);	

	ANTICHEAT_CHECK_USER_CODE_INTEGRITY(UserCodeIntegrityCheck, ANTICHEAT_VALID_USER_CHECK_VALUE);

	ANTICHEAT_CHECK_DEBUGGER(DebuggerIntegrityCheck, ANTICHEAT_VALID_DEBUGGER_CHECK_VALUE);

	if (UserCodeIntegrityCheck != ANTICHEAT_VALID_USER_CHECK_VALUE || DebuggerIntegrityCheck != ANTICHEAT_VALID_DEBUGGER_CHECK_VALUE) {
		Shutdown();
	}

	UserCodeIntegrityCheck = 0;
	DebuggerIntegrityCheck = 0;

#ifdef EPIC_STORE_BUILD
	InitEpicSDK();
#endif

	// D11.DJB - Load default system loc until we revisit hot-reloading loc on consoles.
#if PLATFORM_XBOXONE || PLATFORM_PS4 || PLATFORM_SWITCH
	ULocalizationUtils::ChangeLocalizationToSystemDefault();
#endif

#if PLATFORM_XBOXONE
	// Kinect off 
	FXboxOneMisc::TakeKinectGPUReserve(true);
#endif

	AssignOnlineEvents();

	IDungeonsAPIClient::Get().Init(ClientInfoBuilder::AggregateClientInfo(), GetTimerManager());

	//D11.PS - setup the login/logout delegates
	auto identity = online::getCrossplayOss()->GetIdentityIF();
	auto session = online::getSessionInterface();

	if (identity)
	{
		for (int i = 0; i < MAX_LOCAL_PLAYERS; ++i)
		{
			identity->AddOnLoginStatusChangedDelegate_Handle(i, OnLoginStatusChangedDelegate);
		}
		identity->AddOnControllerPairingChangedDelegate_Handle(OnControllerPairingChangedDelegate);
	}

#if defined(SUBSYSTEM_HAS_TELEMETRY_CALLBACKS)
	if (session)
	{
		session->AddOnWriteConnectionTypeTelemetryDelegate_Handle(OnWriteConnectionTypeTelemetryDelegate);
	}
#endif

	FNetworkVersion::GetLocalNetworkVersionOverride.BindLambda([]()
	{
		int32 concatenatedVersion = -1;
		TOptional<online::GameVersion> gameVersion = online::getVersion(FNetworkVersion::GetProjectVersion());
		if (gameVersion.IsSet())
		{
			concatenatedVersion = FCString::Atoi(*gameVersion.Get(online::GameVersion(-1, -1)).Get());
		}
		return concatenatedVersion;
	});

	mAutoDetect = std::make_unique<AutoDetect>();

	LSInitializer = NewObject<ULoadingScreenInitializer>();
	LSInitializer->Setup(LSBuilder);
	
	EntitlementsRepository = online::entitlements::createRepository();

	BlockingMessages = NewObject<UBlockingMessageBoard>(this);
	EntitlementsRepository->OnEntitlementsRequestFailed.AddUObject(BlockingMessages, &UBlockingMessageBoard::PushMessage, EBlockingMessageType::EntitlementsRequestFailed);
	EntitlementsRepository->OnEntitlementsProvided.AddUObject(this, &UDungeonsGameInstance::OnEntitlementsProvidedByRepository);

	BlockingMessages->OnRequestLogout.AddUObject(this, &UDungeonsGameInstance::Logout);

	MinecraftAPI = NewObject<UMinecraftAPI>();
	MinecraftAPI->Init();
#if PLATFORM_WINDOWS
	MinecraftAPI->OnAuthenticationFailed.AddUObject(BlockingMessages, &UBlockingMessageBoard::PushMessage, EBlockingMessageType::SourceAuthenticationFailed);
#endif
	BlockingMessages->OnRequestRetryAuthentication.AddUObject(MinecraftAPI, &UMinecraftAPI::RetryAuthenticate);

	// annotate log file:
	const auto gameVersion = FString(GAME_VERSION);
	UE_LOG(LogDungeons, Log, TEXT("Dungeons Version, build # %s"), *gameVersion);

	LogDetectors = std::make_unique<logissues::Detectors>();
	LogDetectors->addAll(logissues::detectors::createDefault());
	registerDetectors(*LogDetectors);

	FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UDungeonsGameInstance::OnPreLoadMap);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UDungeonsGameInstance::OnPostLoadMap);
	GetEngine()->OnNetworkFailure().AddUObject(this, &UDungeonsGameInstance::CatchNetworkFailure);
	online::getCrossplayOss()->AddOnConnectionStatusChangedDelegate_Handle(FOnConnectionStatusChangedDelegate::CreateUObject(this, &UDungeonsGameInstance::HandleNetworkConnectionStatusChanged));

	FString OnlineServiceName = online::getCrossplayOss()->GetOnlineServiceName().ToString();
	if (GEngine)
	{
		FString OnlineServiceNameString = FString::Printf(TEXT("OnlineServiceName: %s"), *OnlineServiceName);
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, OnlineServiceNameString);
	}

	SetInitConnectionType();
	ConnectionChecker->Init();	
	CheckConnectionStatus();

#ifdef ENABLE_GAMESTEST_RPC
	const auto hookRegisterResult = Microsoft::Internal::GamesTest::UnrealToolsFramework::FUnrealToolsFramework::RegisterHookService();
	if (FAILED(hookRegisterResult)) {
		UE_LOG(LogUToolsFramework, Error, TEXT("Unable to initialize hook service. Error: %X"), hookRegisterResult);
	}
#endif

	IDungeonsModule::Get().InitAssetFinders(true);
	
	if (PLATFORM_XBOXONE && session)
	{
		session->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegate);
	}

	// #D11.CM - Reserve space to stop allocations.
	mWaitingToPop.Reserve(4);

	UControllerTypeManager::PreloadControllerTypeAssets();

	ItemType::PreloadAllItemClassTypes();

	game::tile::chest::PreloadChestTypes();

	UEnchantment::PreloadIconTextures();
	
	UInventoryItemSlot::PreloadIconInventoryTextures();

	game::armorproperties::type::PreloadArmorPropertyTypes();

	UArmorProperty::PreloadIconTextures();

	InventoryDataholder->Initialize();
	
#if CLOUDSAVE_ENABLED
	FDungeonsCloudLoadSave::Init();
#endif

#if CONSOLE_SAVE_SYSTEM
	DungeonsConsoleSave::Init();
#endif

	UGameSettingsFunctionLibrary::GetOnLightFunctionQualitySet().AddUObject(this, &UDungeonsGameInstance::OnLightFunctionQualityLevelSet);

    ANTICHEAT_VIRT_PROTECT_STRINGS_END;
}
ANTICHEAT_NO_OPTIMIZATION_END

#ifdef EPIC_STORE_BUILD
void UDungeonsGameInstance::InitEpicSDK()
{
	using namespace minecraft::epicstore;
	/* Load the Module, Initialize the Epic Online Services SDK and initiate the login sequence */
	FEpicServicesModule &EosModule = FEpicServicesModule::Get();
	EosModule.SetEpicGameServicesSignInFailedCallback(
		FOnEpicGameServicesSignInFailed::CreateUObject(this, &UDungeonsGameInstance::OnEpicGameServicesSignInFailed)
	);
	/* Sign into the Epic Game Services after the global save data is loaded for the first player */
	OnGlobalSaveDataLoadFinished.AddDynamic(this, &UDungeonsGameInstance::OnReadyToSignIntoEGS);
}

/* Retries the Epic Account Services Sign In after a timeout */
void UDungeonsGameInstance::OnEpicGameServicesSignInFailed()
{
	using namespace minecraft::epicstore;
	FEpicServicesModule &EosModule = FEpicServicesModule::Get();
	GetTimerManager().SetTimer(EpicSignInRetryTimer, 
		FTimerDelegate::CreateRaw(&EosModule, &FEpicServicesModule::SignIntoEpicAccountServices), 
		10.0f, false
	);
}
#endif

ANTICHEAT_NO_OPTIMIZATION_BEGIN
void UDungeonsGameInstance::LoadCachedEntitlements(UGlobalStateData* saveData) const {
	ANTICHEAT_VIRT_BEGIN
	if (online::IsUsingCachedEntitlements()) {
		GetEntitlementsRepository()->SetCachedEntitlements(saveData->mRecordedData.mCachedEntitlements);
	}
	ANTICHEAT_VIRT_END
}
ANTICHEAT_NO_OPTIMIZATION_END

ANTICHEAT_NO_OPTIMIZATION_BEGIN
void UDungeonsGameInstance::SaveCachedEntitlements(const TArray<FEntitlement>& entitlements) const {
	ANTICHEAT_VIRT_BEGIN
	if (online::IsUsingCachedEntitlements()) {
		if (auto player = Cast<UDungeonsLocalPlayer>(GetLocalPlayerByIndex(0))) {
			int localUserNum = player->GetSystemUserId();
			if (auto* save = EditGlobalSaveState(localUserNum)) {
				save->mRecordedData.mCachedEntitlements = entitlements;
				save->Save(localUserNum);
			}
		}
	}
	ANTICHEAT_VIRT_END
}
ANTICHEAT_NO_OPTIMIZATION_END

void UDungeonsGameInstance::OnReadyToSignIntoEGS(int32 playerIndex)
{
#ifdef EPIC_STORE_BUILD
	OnGlobalSaveDataLoadFinished.RemoveDynamic(this, &UDungeonsGameInstance::OnReadyToSignIntoEGS);
	using namespace minecraft::epicstore;
	FEpicServicesModule &EosModule = FEpicServicesModule::Get();
	EosModule.SignIntoEpicAccountServices();
#endif
}

void UDungeonsGameInstance::SetInitConnectionType()
{

	switch (FPlatformMisc::GetNetworkConnectionType())
	{
	case ENetworkConnectionType::Unknown:
	case ENetworkConnectionType::None:
		CurrentConnectionStatus = EOnlineServerConnectionStatus::NotConnected;
		break;
	case ENetworkConnectionType::AirplaneMode:
	case ENetworkConnectionType::Cell:
	case ENetworkConnectionType::WiFi:
	case ENetworkConnectionType::WiMAX:
	case ENetworkConnectionType::Bluetooth:
	case ENetworkConnectionType::Ethernet:
		CurrentConnectionStatus = EOnlineServerConnectionStatus::Connected;
		break;
	default:
		CurrentConnectionStatus = EOnlineServerConnectionStatus::Normal;
		break;
	}
}

void UDungeonsGameInstance::SetupInputKeyPressedDelegate() {
	//D11.PS - Set the OnInputKeyPressedDelegate
	if (auto* DungeonsViewport = Cast<UDungeonsGameViewportClient>(GetGameViewportClient())) {
		OnInputKeyPressedDelgateHandle = DungeonsViewport->AddOnInputKeyPressedDelegate_Handle(OnInputKeyPressedDelgate);
	}
}

void UDungeonsGameInstance::SetupInputKeyReleasedDelegate()
{
	//D11.SSN - Set the OnInputKeyReleasedDelegate
	UDungeonsGameViewportClient *DungeonsViewport = static_cast<UDungeonsGameViewportClient*>(GetGameViewportClient());
	if (DungeonsViewport)
	{
		OnInputKeyReleasedDelgateHandle = DungeonsViewport->AddOnInputKeyReleasedDelegate_Handle(OnInputKeyReleasedDelgate);
	}
}

bool UDungeonsGameInstance::IsConsoleCurrentlySaving() const
{
	#if CONSOLE_SAVE_SYSTEM
		return DungeonsConsoleSave::Instance()->GetNumAsyncTasksInflight() > 0;
	#endif

	return false;
}

bool UDungeonsGameInstance::IsAnyPlayerAliveAndInWorld() {
	for (auto* controller : UserManager->GetAllLocalPlayerControllers()) 
	{
		if (auto* character = Cast<ABasePlayerController>(controller)->GetControlledPlayerCharacter())
		{
			if (character->InWorldAndAlive()) {
				return true;
			}
		}
	}

	return false;
}

bool UDungeonsGameInstance::IsAnyPlayerInWorld() {
	for (auto* controller : UserManager->GetAllLocalPlayerControllers()) 
	{
		if (auto* character = Cast<ABasePlayerController>(controller)->GetControlledPlayerCharacter())
		{
			if (character->GetWorldState() == ECharacterWorldState::InWorld) {
				return true;
			}
		}
	}

	return false;
}

void UDungeonsGameInstance::SetDebugPromptMessage(const FText& DebugMessage) {
	OnDebugPromptMessage(DebugMessage);
}

int UDungeonsGameInstance::GetPIEClientIndex() const {
	return mPIEClientIndex;
}

void UDungeonsGameInstance::OnStart() {
	UE_LOG(LogTemp, Log, TEXT("GameInstance::OnStart"));
	Super::OnStart();

	CosmeticsLibrary = NewObject<UCosmeticsLibrary>();

	MissionThemeLibrary = NewObject<UMissionThemeLibrary>();

	MobIconLibrary = NewObject<UMobIconLibrary>();

	SkinsLibrary = NewObject<USkinsLibrary>();

	EndlessStruggleLibrary = NewObject<UEndlessStruggleLibrary>();

#ifdef ENABLE_GAMESTEST_RPC
	HRESULT UtfStartResult = Microsoft::Internal::GamesTest::UnrealToolsFramework::FUnrealToolsFramework::Start();
	if (FAILED(UtfStartResult))
	{
		UE_LOG(LogUToolsFramework, Error, TEXT("Unable to start some services. Error: %X"), UtfStartResult);
	}
	RegisterGameHooks();
	RegisterGameEvents();
#endif

#if !defined(HAS_RAW_INPUT_LISTENER)
	SetupInputKeyPressedDelegate();
	SetupInputKeyReleasedDelegate();
#endif
	if (auto* DungeonsViewport = Cast<UDungeonsGameViewportClient>(GetGameViewportClient())) {
		DungeonsViewport->OnPlayerAdded().AddUObject(this, &UDungeonsGameInstance::LocalPlayerAddedToViewport);
		DungeonsViewport->OnPlayerRemoved().AddUObject(this, &UDungeonsGameInstance::LocalPlayerRemovedFromViewport);
	}

	}

void UDungeonsGameInstance::OnAchievementUnlocked(FString achievementName, FString achievementURL, FString achievementTitle, FString achievementDesc) {
	FText achievementText = FText::FromString("Achievement Unlocked: " + achievementTitle + "-" + achievementDesc);
	UTextToSpeechBPLibrary::PlayTextToSpeech(achievementText, true);
	OnAchievementUnlockedWithImageURL.Broadcast(achievementName, achievementURL);
}

int UDungeonsGameInstance::GetSessionNumPlayers()
{
	auto sessions = online::getSessionInterface();
	if (!sessions) {
		return 0;
	}

	FOnlineUserPresenceStatus status;
	FNamedOnlineSession* session = sessions->GetNamedSession(DungeonsGameSessionName);
	if (session) 
	{
		return session->RegisteredPlayers.Num();
	}

	return 0;
}

ANTICHEAT_NO_OPTIMIZATION_BEGIN

void UDungeonsGameInstance::FailedIntegrityCheck() {
	Shutdown();
}



void UDungeonsGameInstance::Shutdown() {
	
	UGameSettingsFunctionLibrary::GetOnLightFunctionQualitySet().RemoveAll(this);

	Super::Shutdown();
	ANTICHEAT_VIRT_BEGIN;
	if (ProtectionCodeIntegrityCheck != ANTICHEAT_VALID_PROTECTION_CHECK_VALUE) {
		analytics::Analytics::GetInstance().FireIntegrityCheckFailed();
	} else {
		analytics::Analytics::GetInstance().FireIntegrityCheckDecoy();
	}

	ProtectionCodeIntegrityCheck = 0;
	
	analytics::Analytics::GetInstance().FireReportShutdown();

	for (const TPair<int32, UGlobalStateData*>& pair : mSaveStates)
	{
		SaveGlobalState(pair.Key);

	}
	online::removeLiveOps(this);

#if CLOUDSAVE_ENABLED
	FDungeonsCloudLoadSave::Shutdown();
#endif

#if CONSOLE_SAVE_SYSTEM
	DungeonsConsoleSave::Shutdown();
#endif
	ANTICHEAT_VIRT_END;
}
ANTICHEAT_NO_OPTIMIZATION_END

#if WITH_EDITOR

FGameInstancePIEResult UDungeonsGameInstance::InitializeForPlayInEditor(int32 PIEInstanceIndex, const FGameInstancePIEParameters& Params) {
	UE_LOG(LogDungeons, Log, TEXT("Setting client index for 'PLAY IN EDITOR' client : %d"), PIEInstanceIndex);
	mPIEClientIndex = PIEInstanceIndex;

	return Super::InitializeForPlayInEditor(PIEInstanceIndex, Params);
}

void UDungeonsGameInstance::LoadPlayerControllerGlobalSaveDataInEditor() {
	LoadGlobalSaveData(0);
	GetLoginFlow()->Login(0, GetPlayerControllerFromIndex(0));
	OnSaveStateLoaded.Broadcast();
}

#endif

UGlobalStateData* UDungeonsGameInstance::EditGlobalSaveState(int32 LocalUserNum) const {
	check(!IsDedicatedServerInstance());
	return mSaveStates.Find(LocalUserNum) ? mSaveStates[LocalUserNum] : nullptr;
}

const UGlobalStateData* UDungeonsGameInstance::ReadGlobalSaveState(int32 LocalUserNum) const {
	check(!IsDedicatedServerInstance());
	return mSaveStates.Find(LocalUserNum) ? mSaveStates[LocalUserNum] : nullptr;
}

bool UDungeonsGameInstance::IsSaveStateValid(int32 LocalUserNum) const {
	return mSaveStates.Find(LocalUserNum) ? mSaveStates[LocalUserNum] != nullptr : false;
}

bool UDungeonsGameInstance::IsInitialBoot() const
{
	int LocalUserNum = Cast<UDungeonsLocalPlayer>(GetLocalPlayerByIndex(0))->GetSystemUserId();
	return !ReadGlobalSaveState(LocalUserNum)->IsInitialBootFlowComplete();
}

void UDungeonsGameInstance::SetInitialBootFlowComplete()
{
	int LocalUserNum = Cast<UDungeonsLocalPlayer>(GetLocalPlayerByIndex(0))->GetSystemUserId();
	auto* save = EditGlobalSaveState(LocalUserNum);
	save->SetInitialBootFlowComplete();
	save->Save(LocalUserNum);
}

bool UDungeonsGameInstance::IsXblActiveInSaveFile(int playerIx) const
{
	int LocalUserNum = Cast<UDungeonsLocalPlayer>(GetLocalPlayerByIndex(playerIx))->GetSystemUserId();
	return ReadGlobalSaveState(LocalUserNum)->mRecordedData.mXblActive;
}

bool UDungeonsGameInstance::IsUserAlreadyInGame(TSharedPtr<const FUniqueNetId> UniqueId)
{
#if PLATFORM_SWITCH || PLATFORM_PS4
	return false;
#endif
	//D11.KS - PC local players can be invalid upon joining.
	if (!UniqueId.IsValid())
	{
		return false;
	}

	for (auto localPlayer : GetLocalPlayers())
	{
		if (localPlayer && localPlayer->GetCachedUniqueNetId() == UniqueId)
		{
			return true;
		}
	}
	return false;
}


static const TableCharacterLevels& DefaultCharacterLevels() {
	static const auto levels = defaultIncreasingCharacterLevels(500, 600, 1000000);
	return levels;
}

FCharacterLevel UDungeonsGameInstance::createCharacterLevel(int32 xp) {
	return DefaultCharacterLevels().createCharacterLevel(xp);
}

FCharacterLevel UDungeonsGameInstance::createCharacterLevelForLevel(int level) {
	return DefaultCharacterLevels().createCharacterLevelForLevel(level);
}

void UDungeonsGameInstance::SetMissionFinishedSummary(const FMissionFinishedSummary& summary) {
	mMissionFinishedSummary = summary;
}

const TOptional<FMissionFinishedSummary>& UDungeonsGameInstance::GetMissionFinishedSummary() const {
	return mMissionFinishedSummary;
}

void UDungeonsGameInstance::ClearMissionFinishedSummary() {
	mMissionFinishedSummary.Reset();
}

void UDungeonsGameInstance::SetLevelSettingsLastStarted(const FLevelSettings& levelSettings) {
	LevelSettingsLastStarted = levelSettings;
	TrialIdLastStarted = GetCurrentTrialId(levelSettings.getLevelName());
}

const FLevelSettings& UDungeonsGameInstance::GetLevelSettingsLastStarted() const {
	return LevelSettingsLastStarted;
}

bool UDungeonsGameInstance::QueryUnlockKeys(const FString& expression) const {
	return unlockkey::unlocks(LevelSettingsLastStarted.getProgressionKeys(), expression);
}

void UDungeonsGameInstance::SetCurrentTrials(const TArray<FTrialDef>& trialDefs) {
	TrialDefs = trialDefs;
	trial::prepareMissionDefs(trialDefs);
}

TOptional<FString> UDungeonsGameInstance::GetTrialType(const FString& id)
{
	auto Idx = algo::index_of_if(TrialDefs, [&id](const FTrialDef& def) {
		return def.id == id;
	});

	if (Idx.IsSet()) {
		return TrialDefs[Idx.GetValue()].type;
	}
	return {};
}

TOptional<FString> UDungeonsGameInstance::GetCurrentTrialId(ELevelNames level) const {
	if (auto* trial = TrialDefs.FindByPredicate(RETLAMBDA(it.level == level))) {
		return trial->id;
	}
	return {};
}

TOptional<FString> UDungeonsGameInstance::GetTrialIdLastStarted() const {
	return UMissionDefs::IsTrial(LevelSettingsLastStarted.getLevelName()) ? TrialIdLastStarted : TOptional<FString>{};
}

ReconnectStates& UDungeonsGameInstance::GetReconnectStates() {
	return ReconnectStates;
}

const ReconnectStates& UDungeonsGameInstance::GetReconnectStates() const {
	return ReconnectStates;
}

void UDungeonsGameInstance::StartOnlinePrivilegeTask(const IOnlineIdentity::FOnGetUserPrivilegeCompleteDelegate& Delegate, EUserPrivileges::Type Privilege, TSharedPtr< const FUniqueNetId > UserId)
{
	if (CurrentConnectionStatus != EOnlineServerConnectionStatus::Connected && CurrentConnectionStatus != EOnlineServerConnectionStatus::Normal)
	{
		Delegate.ExecuteIfBound(FUniqueNetIdString(), Privilege, static_cast<int>(IOnlineIdentity::EPrivilegeResults::NetworkConnectionUnavailable));
		return;
	}
	auto Identity = online::getCrossplayOss()->GetIdentityIF();

	if (Identity->GetLoginStatus(GetUserManager()->GetInitialUser()) != ELoginStatus::LoggedIn)
	{
		Delegate.ExecuteIfBound(FUniqueNetIdString(), Privilege, static_cast<int>(IOnlineIdentity::EPrivilegeResults::UserNotLoggedIn));
		return;
	}

	OnPrivilegeTaskStarted.Broadcast();
	
	if (Identity.IsValid() && UserId.IsValid())
	{
		Identity->GetUserPrivilege(*UserId, Privilege, Delegate);
	}
	else
	{
		// Can only get away with faking the UniqueNetId here because the delegates don't use it
		Delegate.ExecuteIfBound(FUniqueNetIdString(), Privilege, static_cast<int32>(IOnlineIdentity::EPrivilegeResults::NoFailures));
	}


}

void UDungeonsGameInstance::OnUserCanPlayInvite(const FUniqueNetId& UserId, EUserPrivileges::Type Privilege, uint32 PrivilegeResults)
{
	OnPrivilegeTaskFinished.Broadcast();
	if (PrivilegeResults == static_cast<uint32>(IOnlineIdentity::EPrivilegeResults::NoFailures))
	{
		PendingInvite.bPrivilegesCheckedAndAllowed = true;
		LookForInvites();
	}
	else
	{
		OnPrivilegeError(UserId, PrivilegeResults);
		DeclineInvite();
	}
}

void UDungeonsGameInstance::OnUserCanPlayTogether(const FUniqueNetId& UserId, EUserPrivileges::Type Privilege, uint32 PrivilegeResults)
{
	OnPrivilegeTaskFinished.Broadcast();
	if (PrivilegeResults == static_cast<uint32>(IOnlineIdentity::EPrivilegeResults::NoFailures))
	{
		// TODO: Do something after we checked that the user can use the PlayTogether feature
	}
	else
	{
		OnPrivilegeError(UserId, PrivilegeResults);
	}
}

void UDungeonsGameInstance::OnPrivilegeError(const FUniqueNetId& UserId, uint32 PrivilegeError)
{
	OnPrivilegeTaskFinished.Broadcast();

	if (PrivilegeError != 0)
	{

		// If we want to do something special depending on the error.

		if ((PrivilegeError & static_cast<uint32>(IOnlineIdentity::EPrivilegeResults::UserNotLoggedIn)) != 0)
		{

#if PLATFORM_SWITCH || PLATFORM_PS4
			 ABaseMenuPlayerController* MenuPlayerController = static_cast< ABaseMenuPlayerController*>(GetUserManager()->GetInitialPlayerController());

			if (MenuPlayerController)
			{
				if (OnUserNotLoggedIn.IsBound())
				{
					OnUserNotLoggedIn.Broadcast();
				}
			}
			return;
#endif
		}

		if ((PrivilegeError & static_cast<uint32>(IOnlineIdentity::EPrivilegeResults::AccountTypeFailure)) != 0)
		{
			// Show the account upgrade UI
			auto ExternalUI = online::getExternalUIInterface();
			if (ExternalUI.IsValid())
			{
				ExternalUI->ShowAccountUpgradeUI(UserId);
				FInputModeGameOnly InputMode;
				GetUserManager()->GetInitialPlayerController()->SetInputMode(InputMode);
			}
		}

		//D11.GM  - cast is needed as BP does not accept uint32
		OnPrivilegeTaskFailed.Broadcast(static_cast<int32>(PrivilegeError));
	}
}



void UDungeonsGameInstance::StartOnlinePrivilegeCheck()
{
	int InitialUserId = GetUserManager()->GetInitialUser();
	StartOnlinePrivilegeTask(IOnlineIdentity::FOnGetUserPrivilegeCompleteDelegate::CreateUObject(this, &UDungeonsGameInstance::OnOnlinePrivilegesCheckTaskFinished), EUserPrivileges::CanPlayOnline, online::getCrossplayOss()->GetIdentityIF()->GetUniquePlayerId(InitialUserId));
}

void UDungeonsGameInstance::OnOnlinePrivilegesCheckTaskFinished(const FUniqueNetId& UserId, EUserPrivileges::Type Privilege, uint32 PrivilegeResults)
{
	OnPrivilegeTaskFinished.Broadcast();
	if (PrivilegeResults == (uint32)IOnlineIdentity::EPrivilegeResults::NoFailures)
	{
		OnHasOnlinePrivileges();
	}
	else
	{
		OnPrivilegeError(UserId, PrivilegeResults);
	}
}

void UDungeonsGameInstance::OnHasOnlinePrivileges()
{
	OnPrivilegeTaskSuccessful.Broadcast();
}

//D11.PS
void UDungeonsGameInstance::OnInputKeyPressed(int ControllerId, FKey Key) {
	OnAnyKeyPressedControllerIdDelegate.Broadcast(ControllerId, Key, false);
}

//D11.PS
void UDungeonsGameInstance::OnInputKeyPressed(int ControllerId, FKey Key, bool IsRepeat) {
	OnAnyKeyPressedControllerIdDelegate.Broadcast(ControllerId, Key, IsRepeat);
}


//D11.SSN
void UDungeonsGameInstance::OnInputKeyReleased(int ControllerId, FKey Key)
{
	OnAnyKeyReleasedControllerIdDelegate.Broadcast(ControllerId, Key, false);
}

//D11.PS
FReply UDungeonsGameInstance::OnUnhandledKeyDown(const FKeyEvent& InKeyEvent) {
	OnAnyKeyPressedControllerIdDelegate.Broadcast(InKeyEvent.GetUserIndex(), InKeyEvent.GetKey(), InKeyEvent.IsRepeat());
	return FReply::Unhandled();
}

void UDungeonsGameInstance::CheckMultiplayerFeatures()
{
	const FLevelSettings& levelSettings = GetLevelSettingsLastStarted();
	bool bUsingMultiplayerFeatures = online::getCurrentSession() && !UMissionDefs::IsTutorial(levelSettings.getLevelName()) && !levelSettings.isLobby();
	UpdateUsingMultiplayerFeatures(bUsingMultiplayerFeatures);
}

void UDungeonsGameInstance::SetPlayerWasKicked(bool bPlayerKicked)
{
	bPlayerWasKicked = bPlayerKicked;
}

bool UDungeonsGameInstance::GetPlayerWasKicked()
{
	return bPlayerWasKicked;
}

void UDungeonsGameInstance::Logout(int32 LocalUserNum) {
	auto IdentityInterface = online::getCrossplayOss()->GetIdentityIF();
	if (IdentityInterface) {
		online::removeLiveOps(this, LocalUserNum);
		IdentityInterface->Logout(LocalUserNum);
	}
}

UUserWidget* UDungeonsGameInstance::GetCachedUIWidget(TSubclassOf< UUserWidget > WidgetClass)
{
	if (CVarEnableUICaching.GetValueOnGameThread())
	{
		if (auto FoundWidget = mUIInterfaceWidgetCache.Find(WidgetClass))
		{
			return *FoundWidget;
		}
	}

	return nullptr;
}

void UDungeonsGameInstance::SetCachedUIWidget(UUserWidget* pWidget)
{
	if (CVarEnableUICaching.GetValueOnGameThread())
	{
		if (pWidget)
		{
			mUIInterfaceWidgetCache.Add(pWidget->GetClass(), pWidget);
		}
	}
}

void UDungeonsGameInstance::ClearCachedUIWidgets()
{
	mUIInterfaceWidgetCache.Empty();
}

void UDungeonsGameInstance::OnLoginStatusChanged(int32 LocalUserNum, ELoginStatus::Type OldStatus, ELoginStatus::Type NewStatus, const FUniqueNetId& NewId) {

	UE_LOG(LogDungeons, Log, TEXT("OnLoginStatusChanged LocalUserNumber %d, OldStatus %d, NewStatus %d"), LocalUserNum, OldStatus, NewStatus);

	auto identity = online::getCrossplayOss()->GetIdentityIF();
	//D11.PS Kick the user back to the title screen
	if ((NewStatus == ELoginStatus::NotLoggedIn || NewStatus == ELoginStatus::UsingLocalProfile)
		&& OldStatus == ELoginStatus::LoggedIn 
		&& identity)
	{
		if (ULocalPlayer *Player = GetUserManager()->GetInitialLocalPlayer())
		{
			const auto& cached = Player->GetCachedUniqueNetId();
			if (cached.IsValid()
				&& NewId.IsValid()
				/* transform the cached Net Id to the type passed as an argument */
				&& Cast<UDungeonsLocalPlayer>(Player)->GetSystemUserId() == LocalUserNum)
			{
				if (NewStatus == ELoginStatus::NotLoggedIn)
				{
					Player->SetCachedUniqueNetId(nullptr);
					TitleScreenKickback(EBootMode::UserSignedOut);
				}
				// D11.PC This is mostly only for PS4. This means that the console/game has a user but is not logged into the PSN
				else if (NewStatus == ELoginStatus::UsingLocalProfile)
				{
					// D11.PC Check if we're in a session
					bool inSession = online::getCurrentSession() != nullptr;

					// Check if we disconnected and are on an online game. If so, go to main menu
					if (inSession)
					{
						if (!IsInMenu())
						{
							BeginLoadingScreenWithTravel(FLevelSettings(), EMapLoadType::OpenMenu);
							PushGlobalErrorMessage(EGlobalMessageTypes::NotConnectedToOnlineService);
						}
					}
				}
			}
			//D11.KS - We should handle local coop logouts here.
			else if(GetUserManager()->GetInitialUserSystemId() != LocalUserNum && NewStatus == ELoginStatus::NotLoggedIn)
			{
				if (ULocalPlayer* const* PlayerFound = GetLocalPlayers().FindByPredicate([&](ULocalPlayer* LocalPlayer) { return Cast<UDungeonsLocalPlayer>(LocalPlayer)->GetSystemUserId() == LocalUserNum; }))
				{
					// Don't kick from a profile switch if someone is logging in, the profile signing in is the one switching.
					if (GetLoginFlow()->IsPlayerLoggingIn()) return;
					OnLocalPlayerChangedUser.Broadcast((*PlayerFound)->GetControllerId());
				}
			}
		}
	}
}

/**
 * This is a quick and ugly hack to prevent a player from joining a crossplay session through out-of-game invites/join when crossplay is disabled
 * This is a corner case on PS4 builds
 */
void UDungeonsGameInstance::OnJoinSessionCompletePS4(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (Result == EOnJoinSessionCompleteResult::UnknownError && SessionName == "PS4CrossplayDisabled" && IsInMenu())
	{
		PushGlobalErrorMessage(EGlobalMessageTypes::PS4CrossplayDisabled);
	}
	else if (Result == EOnJoinSessionCompleteResult::UnknownError && SessionName == "PS4CrossplayEnabled" && IsInMenu())
	{
		PushGlobalErrorMessage(EGlobalMessageTypes::PS4CrossplayEnabled);
	}
}

void UDungeonsGameInstance::OnWriteConnectiontypeTelemetry(bool IsUsingRelay)
{
	analytics::Analytics::GetInstance().FireReportConnectionType(IsUsingRelay);
}

void UDungeonsGameInstance::OnControllerPairingChanged(int32 LocalUserNum, const FUniqueNetId& PreviousUser, const FUniqueNetId& NewUser)
{
	UE_LOG(LogDungeons, Log, TEXT("OnControllerPairingChanged LocalUserNum %d,  PreviousUser %s, NewStatus %s"), LocalUserNum, *PreviousUser.ToString(), *NewUser.ToString());

	auto identity = online::getCrossplayOss()->GetIdentityIF();
	//Either a controller disconnect or logout.
	if (!NewUser.IsValid() || !identity) return;

	//Only do something if the initial user is set
	if (ULocalPlayer *Player = GetUserManager()->GetInitialLocalPlayer())
	{
		int initialUser = GetUserManager()->GetInitialUser();
		//Deal with the initial user
		if (Player->GetCachedUniqueNetId().IsValid() 
			&& *identity->GetUniqueNetIdFromName(*Player->GetCachedUniqueNetId(), PreviousUser.GetType()) == PreviousUser 
			&& LocalUserNum == initialUser)
		{
			//Switch the controller Id
			//Player->SetControllerId(LocalUserNum);
			if (UWorld *World = GetWorld())
			{
				// Try to get platform identity first
				FString PreviousUserName = identity->GetPlayerNickname(PreviousUser);
				FString NewUserName = identity->GetPlayerNickname(NewUser);

				//D11.PS - Throw the user switched delegate and show the prompt
				OnSwitchedUserDelegate.Broadcast(LocalUserNum, PreviousUserName, NewUserName);
			}
		}
		else
		{
			//D11.PS TODO - Deal with other logged in users
			// D11.PC - If the user was already in game, then we need to kick him out (Have to check XR to see if is supposed to change the player info instead)
			if (ULocalPlayer* const* PlayerFound = GetLocalPlayers().FindByPredicate(
				[&](ULocalPlayer* LocalPlayer)
				{
					return LocalPlayer->GetCachedUniqueNetId().IsValid()
						&& *identity->GetUniqueNetIdFromName(*LocalPlayer->GetCachedUniqueNetId(), PreviousUser.GetType()) == PreviousUser
						&& Cast<UDungeonsLocalPlayer>(LocalPlayer)->GetControllerId() == LocalUserNum;
				}
			))
			{
				// Don't kick from a profile switch if someone is logging in, the profile signing in is the one switching.
				if (GetLoginFlow()->IsPlayerLoggingIn()) return;

				// SEND THE CONTROLLER ID
				OnLocalPlayerChangedUser.Broadcast((*PlayerFound)->GetControllerId());
			}
		}
	}
}


#if PLATFORM_SWITCH
void UDungeonsGameInstance::LogSessionInterval(FString s)
{
//	UE_LOG(LogDungeons, Log, TEXT("***** UDungeonsGameInstance::SessionInterval %s") , *s);
}
#endif

void UDungeonsGameInstance::OnApplicationEnterForeground()
{
	ControllerTypeManager->EnteredForeground();
#if !PLATFORM_SWITCH
	if (auto userManager = GetUserManager())
	{
		//D11.PS - No callbacks for controller pairing get called during a suspended state, so we must now check the state of the signed in users
		if (ULocalPlayer *Player = userManager->GetInitialLocalPlayer())
		{
			//D11.GM - If the user has save data then they were signed in before suspending the game 
			if (ReadGlobalSaveState(Player->GetControllerId()))
			{
				if (!Player->GetCachedUniqueNetId().IsValid())
				{
					TitleScreenKickback(EBootMode::UserSignedOut);
					return;
				}
			}
		}
	}
#endif

#if PLATFORM_XBOXONE
	if (online::getCurrentSession() && online::isOnlineSession())
	{
		if (IsInMenu()) {
			EndLoadingScreen(GetWorld(), "", UDungeonsGameInstance::fDefaultFadeOutTime, UDungeonsGameInstance::fDefaultFadeInTime);
		}
		else {
			BeginLoadingScreenWithTravel(FLevelSettings(), EMapLoadType::OpenMenu);
		}
		PushGlobalErrorMessage(EGlobalMessageTypes::NotConnectedToOnlineService);
	}
#endif

#if PLATFORM_PS4
	auto* netDriver = GWorld ? GWorld->GetNetDriver() : nullptr;
	if (netDriver) {
		netDriver->Shutdown();
	}
	if (online::getCrossplayOss()) {
		auto SessionInterface = online::getCrossplayOss()->GetSessionIF();
#ifdef SUBSYSTEM_HAS_GAME_SUSPENDED_SESSION_FAILURE
		auto SessionFailure = ESessionFailure::GameSuspended;
#else
		auto SessionFailure = ESessionFailure::ServiceConnectionLost;
#endif
		SessionInterface->TriggerOnSessionFailureDelegates(FUniqueNetIdString("NotConnectedToOnlineService"), SessionFailure);
	}
#endif

#if PLATFORM_SWITCH
	//D11.SC Temp catch for broken session on returning from sleep on Switch
	
	{
		//force a recheck on the session, in case it was lost due to background
		if (auto world = GetWorld())
		{
			const float PollSessionInterval = 0.1f;
			const float PollSessionTimeout = 3.0f;
			PollSessionTimerCurrentTime = 0.0f;
			TWeakObjectPtr<UWorld> WeakWorld(world);
			//poll for session failue
			LogSessionInterval("Start");
			GetTimerManager().SetTimer(PollSessionTimerHandle, [&, WeakWorld] {

				if (const auto sessionInterface = online::getSessionInterface())
				{
					auto world = WeakWorld.Get();
					bool bKillTimer = true;
					if (world != nullptr)
					{
						LogSessionInterval("UpdateSession");
						if (const auto sessionInterface = online::getSessionInterface())
						{
							if (auto* session = online::getCurrentSession())
							{
								sessionInterface->UpdateSession(DungeonsGameSessionName, session->SessionSettings);
							}
						}
						PollSessionTimerCurrentTime += PollSessionInterval;
						bKillTimer = PollSessionTimerCurrentTime >= PollSessionTimeout;
						if (bKillTimer)
							LogSessionInterval("Timer expired");
					}
					else
						LogSessionInterval("World has gone null");
					if (bKillTimer)
					{
						LogSessionInterval("Finished");
						GetTimerManager().ClearTimer(PollSessionTimerHandle);
					}
				}
			}, PollSessionInterval, true);
		}
		else
			LogSessionInterval("Don't have a world");
	}

#endif


}

void UDungeonsGameInstance::TitleScreenKickback(EBootMode Mode)
{

#if CLOUDSAVE_ENABLED
	FDungeonsCloudLoadSave::Instance()->CloudTitleScreenKickback();
#endif


	bIgnoreNetworkStatusChanged = true;
	BootMode = Mode;
	UserManager->InitialUserSignedOut();
	BeginLoadingScreenWithTravel(FLevelSettings(), EMapLoadType::OpenMenu);
	ClearSaveStates();

	for(auto localController : UserManager->GetAllLocalPlayerControllers())
	{
		if (auto csc = localController->GetCharacterSerializeComponent())
		{
			csc->InvalidateSaveData();
		}
	}

	PlayfabServices::ClearAuthenticationTokenFunction();
	GetTimerManager().ClearTimer(PollSessionTimerHandle);
	GetTimerManager().ClearTimer(PollSessionFinishHandle);
	GetTimerManager().ClearTimer(HeartBeatTimerHandle);
}

void UDungeonsGameInstance::CleanUpGameRenderTargets()
{
	//fire off a command to clean up any extra RTs created
	ENQUEUE_RENDER_COMMAND(FreePool_GameBPInit_Start)(
		[](FRHICommandListImmediate & RHICmdList)
	{
		GRenderTargetPool.FreeUnusedResources();
	}
	);
}

void UDungeonsGameInstance::CheckKickToTitleScreenReasons()
{
	if (KickBackToTitleScreenReason != EBootMode::Unset)
	{
		TitleScreenKickback(KickBackToTitleScreenReason);
		KickBackToTitleScreenReason = EBootMode::Unset;
	}
}

const FString& UDungeonsGameInstance::GetAcceptedInvite() const
{
	return AcceptedInvite;
}

void UDungeonsGameInstance::ClearSaveStates()
{
	mSaveState = nullptr;
	mSaveStates.Empty();
}
//D11.RR
void UDungeonsGameInstance::OnLoginLocalPlayerComplete(int localUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
{
	online::getIdentityInterface()->ClearLocalPlayerLoginDelegate(localUserNum, OnLoginCompleteDelegateHandle);
	TSharedPtr<const FUniqueNetId> UniqueID = online::getIdentityInterface()->GetUniquePlayerId(localUserNum);
	if (!UniqueID)
		return;
	//set the cached id to the player
	APlayerController* playerController = GetPlayerControllerFromControllerID(localUserNum);
	ULocalPlayer* localPlayer = playerController->GetLocalPlayer();
	localPlayer->SetCachedUniqueNetId(UniqueID);
	if (UDungeonsLocalPlayer* DungeonsLocalPlayer = Cast<UDungeonsLocalPlayer>(localPlayer)) {
		DungeonsLocalPlayer->SetUserSystemId(localUserNum);
	}
	if (playerController->PlayerState != nullptr) {
		playerController->PlayerState->SetUniqueId(UniqueID);
	}
}
//D11.RR - The same as the function in MissionProgressComponent but this one can be used in the main menu. 
ANTICHEAT_NO_OPTIMIZATION_BEGIN
bool UDungeonsGameInstance::HasDLCEntitlements(EDLCName dlcName) const
{
	ANTICHEAT_OBFUSCATE_BEGIN
	if (const auto* dlcDef = dlc::getChecked(dlcName))
	{	
		return dlcDef->IsUnlockedByEntitlements(GetWorld()) && dlcDef->IsReleased();
	}

	return false;

	ANTICHEAT_OBFUSCATE_END
}
ANTICHEAT_NO_OPTIMIZATION_END

#if PLATFORM_SWITCH

void UDungeonsGameInstance::OnChangeHandheldScreenPercentage(IConsoleVariable* Var)
{
	float percentage = Var->GetFloat();
	FPlatformMisc::LowLevelOutputDebugStringf(TEXT("Switch Handheld Screen Percentage: %f"), percentage);
	mSwitchHandheldScreenPercentage = percentage;
	if (mCurrentOperationMode == SwitchOperationMode::Handheld)
	{
		SetScreenPercentage(mSwitchHandheldScreenPercentage);
	}
}

void UDungeonsGameInstance::OnChangeConsoleScreenPercentage(IConsoleVariable* Var)
{
	float percentage = Var->GetFloat();
	FPlatformMisc::LowLevelOutputDebugStringf(TEXT("Switch Console Screen Percentage: %f"), percentage);
	mSwitchConsoleScreenPercentage = percentage;
	if (mCurrentOperationMode == SwitchOperationMode::Console)
	{
		SetScreenPercentage(mSwitchConsoleScreenPercentage);
	}
}

#endif

void UDungeonsGameInstance::SetFastLoadEnabled(bool bEnabled)
{
	if (CVarSwitchFastLoad.GetValueOnGameThread())
	{
		//Platform Dependant CPU/Loading Boosters in here

#if PLATFORM_SWITCH
		nn::oe::SetCpuBoostMode(bEnabled ? nn::oe::CpuBoostMode_FastLoad : nn::oe::CpuBoostMode_Normal);
#endif

	}
}

void UDungeonsGameInstance::NotifyUserOfLowPerformanceDetected(const FText& NotifyMessageBody)
{
	OnNotifyLowPerformanceDetected.Broadcast(NotifyMessageBody);
}

void UDungeonsGameInstance::LocalPlayerAddedToViewport(int playerIndex) {
	UE_LOG(LogDungeons, Log, TEXT("Local Player added to viewport %d"), playerIndex);
	OnLocalPlayerAddedToViewport.Broadcast();
}

void UDungeonsGameInstance::LocalPlayerRemovedFromViewport(int playerIndex) {
	UE_LOG(LogDungeons, Log, TEXT("Local Player removed from viewport %d"), playerIndex);
	OnLocalPlayerRemovedFromViewport.Broadcast();
}

void UDungeonsGameInstance::OnApplicationEnterBackground() {
	// D11.DH
	// Hook for quit from nexus on Xbox 
	// Fix for session still running when host leaves and for clients staying in the game session after quitting
	// ideally this would transfer control but it's easier this way for now
	// Mojang.JE
	// The behavior is different on switch, this function will get called when pressing home screen
	// and we don't want to teardown the NetDriver as Dungeons keeps running as normal in home screen.
#if PLATFORM_XBOXONE
	if (online::getCrossplayOss())
	{
		auto SessionInterface = online::getCrossplayOss()->GetSessionIF();
#ifdef SUBSYSTEM_HAS_GAME_SUSPENDED_SESSION_FAILURE
		auto SessionFailure = ESessionFailure::GameSuspended;
#else
		auto SessionFailure = ESessionFailure::ServiceConnectionLost;
#endif
		SessionInterface->TriggerOnSessionFailureDelegates(FUniqueNetIdString("NotConnectedToOnlineService"), SessionFailure);
	}

	auto* netDriver = GWorld ? GWorld->GetNetDriver() : nullptr;
	if (netDriver) {
		netDriver->Shutdown();
	}
#endif
}

void UDungeonsGameInstance::AssignOnlineEvents() {
	OnSessionInviteReceivedDelegate = FOnSessionInviteReceivedDelegate::CreateUObject(this, &UDungeonsGameInstance::OnSessionInviteReceived);
	OnSessionInviteAcceptedDelegate = FOnSessionUserInviteAcceptedDelegate::CreateUObject(this, &UDungeonsGameInstance::OnSessionInviteAccepted);
	OnSessionFailureDelegate = FOnSessionFailureDelegate::CreateUObject(this, &UDungeonsGameInstance::OnSessionFailure);
	OnControllerPairingChangedDelegate = FOnControllerPairingChangedDelegate::CreateUObject(this, &UDungeonsGameInstance::OnControllerPairingChanged);

	if (auto sessions = online::getSessionInterface()) {
		UE_LOG(LogOnline, Log, TEXT("OnStart clearing & binding invite received and invite accepted"));
		ClearOnlineEvents();
		OnSessionInviteReceivedDelegateHandle = sessions->AddOnSessionInviteReceivedDelegate_Handle(OnSessionInviteReceivedDelegate);
		OnSessionInviteAcceptedDelegateHandle = sessions->AddOnSessionUserInviteAcceptedDelegate_Handle(OnSessionInviteAcceptedDelegate);
		OnSessionFailureDelegateHandle = sessions->AddOnSessionFailureDelegate_Handle(OnSessionFailureDelegate);
	}
	else {
		UE_LOG(LogOnline, Log, TEXT("OnStart Could not find valid sessions"));
	}
}

void UDungeonsGameInstance::ClearOnlineEvents() {
	if (auto sessions = online::getSessionInterface())
	{
		sessions->ClearOnSessionInviteReceivedDelegate_Handle(OnSessionInviteReceivedDelegateHandle);
		sessions->ClearOnSessionUserInviteAcceptedDelegate_Handle(OnSessionInviteAcceptedDelegateHandle);
		sessions->ClearOnSessionFailureDelegate_Handle(OnSessionFailureDelegateHandle);
	}
}

void UDungeonsGameInstance::OnEntitlementsProvidedByRepository(const TArray<FEntitlement>& entitlements) {
	ANTICHEAT_OBFUSCATE_BEGIN
	OnEntitlementsReceived.Broadcast();

	if (!HasBaseGameEntitlement()) {
		UE_LOG(LogDungeons, Warning, TEXT("User lacks base game entitlement."));
		GetBlockingMessages()->PushMessage(EBlockingMessageType::MissingBaseGameEntitlement);
	}
	else {
		if (!GetTimerManager().IsTimerActive(RefreshEntitlementsTimerHandle)) {
			GetTimerManager().SetTimer(RefreshEntitlementsTimerHandle, this, &UDungeonsGameInstance::RefreshEntitlements, REFRESH_ENTITLEMENTS_DELAY_SECONDS, true);
		}
		SaveCachedEntitlements(GetEntitlementsRepository()->GetCachedEntitlements());
	}
	ANTICHEAT_OBFUSCATE_END
}

void UDungeonsGameInstance::RefreshEntitlements() const {
	GetEntitlementsRepository()->RequestEntitlements();
}

void UDungeonsGameInstance::KickAllLocalPlayers()
{
	for (ULocalPlayer* OurLocalPlayer : LocalPlayers)
	{
		HandleLocalPlayerLeave(OurLocalPlayer->GetControllerId());
	}
}

void UDungeonsGameInstance::SetScreenPercentage(float percentage) {
	FPlatformMisc::LowLevelOutputDebugStringf(TEXT("Setting Screen percentage to %f%%"), percentage);
	GEngine->Exec(GetWorld(), *FString::Printf(TEXT("r.ScreenPercentage %f"), percentage));
}

void UDungeonsGameInstance::OnLightFunctionQualityLevelSet(int qualityLevel, APlayerController * playerController)
{
	OnLightFunctionQualitySet.Broadcast(qualityLevel, playerController);
}

void UDungeonsGameInstance::SaveGlobalState(int32 LocalUserNum) {
	// we should also do this for the controller logging out 
	if (IsSaveStateValid(LocalUserNum)) {

		//D11.KS - Noticed it failed to get a valid initial user id.
		if (LocalUserNum > -1) {
			ReadGlobalSaveState(LocalUserNum)->Save(LocalUserNum);
		}
	}
}


template<class Ptr>
static bool ArePtrContentsTheSame(Ptr A, Ptr B)
{
	return A.IsValid() && B.IsValid() && *A == *B;
}

void UDungeonsGameInstance::LookForInvites()
{
	TSharedPtr<const FUniqueNetId> PrimaryUniqueNetId;

	if (PendingInvite.UserId)
	{
		PrimaryUniqueNetId = online::getCrossplayOss()->GetIdentityIF()->GetUniquePlayerId(UserManager->GetInitialUser(), PendingInvite.UserId->GetType());
	}

	if (PendingInvite.UserId.IsValid() && mSaveState != nullptr && mSaveState->GetAllProfiles().Num() > 0 && ArePtrContentsTheSame(PrimaryUniqueNetId, PendingInvite.UserId) )
	{
		
		if (!PendingInvite.bPrivilegesCheckedAndAllowed)
			StartOnlinePrivilegeTask(IOnlineIdentity::FOnGetUserPrivilegeCompleteDelegate::CreateUObject(this, &UDungeonsGameInstance::OnUserCanPlayInvite), EUserPrivileges::CanPlayOnline, PendingInvite.UserId);
		else if (LocalPlayers.Num() > 1)
		{
			// D11.PC If we're on a local multiplayer, inform the user that has to choose between join the session
			// and kick all the other local players or continue the local play
			PendingInvite.bSuccessfullyAccepted = true;
			OnInviteAcceptedOnLocalPlay();		
		}
		else
			AcceptInvite(PendingInvite.InviteResult);
	}
}

bool UDungeonsGameInstance::IsLoadingScreenActive() const {
	return LSInitializer->IsActive();
}

bool UDungeonsGameInstance::IsLoadingScreenShown() const
{
	return LSInitializer->IsCurrentlyShown();
}

ANTICHEAT_NO_OPTIMIZATION_BEGIN
bool UDungeonsGameInstance::HasBaseGameEntitlement() const {
	ANTICHEAT_VIRT_PROTECT_STRINGS_BEGIN
	#if UE_BUILD_SHIPPING && PLATFORM_WINDOWS
	return UEntitlementsValidator::HasEntitlement(GetEntitlementsRepository()->GetEntitlements(), "game_dungeons");
	#endif
	return true;
	ANTICHEAT_VIRT_PROTECT_STRINGS_END
}
ANTICHEAT_NO_OPTIMIZATION_END

void UDungeonsGameInstance::UpdateUsingMultiplayerFeatures(bool bIsUsingMultiplayerFeatures)
{
	for (const auto* LocalPlayer : LocalPlayers)
	{
		FUniqueNetIdRepl PlayerId = LocalPlayer->GetPreferredUniqueNetId();
		if (PlayerId.IsValid())
		{
			 online::getCrossplayOss()->SetUsingMultiplayerFeatures(*PlayerId, bIsUsingMultiplayerFeatures);
		}
	}
}

UBlockingMessageBoard* UDungeonsGameInstance::GetBlockingMessages() const {
	return BlockingMessages;
}

void UDungeonsGameInstance::ForceLeaveRecentlyLeftControllers()
{
	for (auto& entry : mRecentlyLeftControllers)
	{
		OnLocalPlayerChangedUser.Broadcast(entry.mControllerID);
	}
	mRecentlyLeftControllers.Empty();
}

void UDungeonsGameInstance::UpdateRecentlyLeftControllers(float fTimeDelta)
{
	//D11.SC remove players after disconnect grace period
	for (int i = 0; i < mRecentlyLeftControllers.Num(); ++i)
	{
		FRecentlLeftControllerData& entry = mRecentlyLeftControllers[i];

		APlayerController* playerController = GetPlayerControllerFromIndex(entry.mControllerID);
		const bool shouldLeaveImmediately = playerController && playerController->PlayerState && Cast<ABasePlayerState>(playerController->PlayerState)->bLocallyDroppingIn;

		entry.mGracePeriod -= fTimeDelta;		
		if (entry.mGracePeriod < 0.0f || shouldLeaveImmediately)
		{
			// SEND THE CONTROLLER ID
			OnLocalPlayerChangedUser.Broadcast(entry.mControllerID);

			mRecentlyLeftControllers.RemoveAtSwap(i);
			--i;
		}
	}
}

bool UDungeonsGameInstance::ShouldDisableOnlineGame()
{
	if (!GConfig) {
		return false;
	}

	FString SectionName = "/Script/Dungeons.OnlineGame";

	bool DisableOnlineGame;
	GConfig->GetBool(*SectionName, TEXT("DisableOnlineGame"), DisableOnlineGame, GGameIni);

	return DisableOnlineGame;
}

bool UDungeonsGameInstance::IsInMenu() const {
	return GetWorld() != nullptr && GetWorld()->GetFirstPlayerController<ABaseMenuPlayerController>() != nullptr;
}

bool UDungeonsGameInstance::IsControllerIdInGracePeriod(int32 controllerID) const
{
	return mRecentlyLeftControllers.ContainsByPredicate([controllerID](FRecentlLeftControllerData c) { return c.mControllerID == controllerID; });
}

void UDungeonsGameInstance::WindowDidResize(FViewport* viewport, uint32 value) {
	if (OnViewportResizedDynamicDelegate.IsBound()) {
		OnViewportResizedDynamicDelegate.Broadcast(viewport->GetSizeXY());
	}
}

void UDungeonsGameInstance::InitialiseUserSettingsFromSaveData(APlayerControllerBase* PlayerControllerBase)
{
	UGameSettingsFunctionLibrary::InitialiseUserSettings(PlayerControllerBase);
}

void UDungeonsGameInstance::OnControllerPairingChangedCommand(int32 LocalUserNum)
{
	OnLocalPlayerChangedUser.Broadcast(LocalUserNum);
}

void UDungeonsGameInstance::KickBackPlayerToMenu(EBootMode Reason)
{
	TitleScreenKickback(Reason);
}

UPoppingEffect::UPoppingEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Popping.Popped"), 0, 1);
}

void UDungeonsGameInstance::OnInGameJoin()
{
	StartOnlinePrivilegeTask(IOnlineIdentity::FOnGetUserPrivilegeCompleteDelegate::CreateUObject(this, &UDungeonsGameInstance::OnInGameJoinPrivilegesFinished), EUserPrivileges::CanPlayOnline, GetUserManager()->GetInitialLocalPlayer()->GetCachedUniqueNetId().GetUniqueNetId());
}

void UDungeonsGameInstance::OnInGameJoinPrivilegesFinished(const FUniqueNetId& UserId, EUserPrivileges::Type Privilege, uint32 PrivilegeResults)
{
	OnPrivilegeTaskFinished.Broadcast();
	if (PrivilegeResults == static_cast<uint32>(IOnlineIdentity::EPrivilegeResults::NoFailures))
	{
		OnPrivilegeTaskSuccessful.Broadcast();
	}
	else
	{
		OnPrivilegeError(UserId, PrivilegeResults);
	}
}

TArray<APlayerCharacter*> UDungeonsGameInstance::GetAllPlayerCharacters()
{
	return InstanceTracker<APlayerCharacter>::GetList(GetWorld());
}

void UDungeonsGameInstance::LevelStarted()
{
	if (OnLevelStart.IsBound())
	{
		OnLevelStart.Broadcast();
	}
#if PLATFORM_XBOXONE
	if (auto xCloud = XCloud::Get())
	{
		if (xCloud->IsStreaming())
		{
			xCloud->EnableTouchControls();
		}
	}
#endif

}

ETitle UDungeonsGameInstance::GetPlayerTitle(int32 LocalUserNum)
{
	return EditGlobalSaveState(LocalUserNum)->GetTitle();
}

void UDungeonsGameInstance::SetPlayerTitle(int32 LocalUserNum, ETitle title)
{
	EditGlobalSaveState(LocalUserNum)->ChangeTitle(title);
}
