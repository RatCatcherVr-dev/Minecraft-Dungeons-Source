// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "Dungeons.h"
#include <Engine.h>
#include "game/mission/MissionModifier.h"
#include "game/util/DungeonsTravelUtil.h"
#include "game/mission/theme/data/LoadingScreenStyle.h"
#include "ScreenFader.h"
#include "LoadingScreenInitializer.h"
#include "DungeonsGameInstance.h"
#include "online/sessions/CreateDungeonsSessionCallbackProxy.h"
#include "Assets/DungeonsAssetManager.h"
#include "SDungeonsLoadingScreenWidget.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "game/util/ActorQuery.h"
#include "DungeonsUserManagement.h"

static TAutoConsoleVariable<int> CVarShowLoadScreenTime(TEXT("Dungeons.ShowLoadScreenTime"), 0, TEXT("Shows the time spent within a loadscreen"), ECVF_Default);


double gLoadScreenTime = 0.0;

ELoadingScreenType GetScreenTypeFromLoadType(EMapLoadType loadType) {
	switch (loadType) {
		case EMapLoadType::StartIngameSession:
		case EMapLoadType::TravelIngameServer:
		case EMapLoadType::TravelIngameClient:
		case EMapLoadType::JoinIngameSession:
			return ELoadingScreenType::Level;
		case EMapLoadType::OpenMenu:
			return ELoadingScreenType::Menu;
		case EMapLoadType::StartLobbySession:
		case EMapLoadType::TravelLobbyServer:
		case EMapLoadType::TravelLobbyClient:
		case EMapLoadType::JoinLobbySession:
		case EMapLoadType::OpenLobby:
			return ELoadingScreenType::Lobby;
	}

	return ELoadingScreenType::Level;
}

void ULoadingScreenInitializer::Setup(TSharedPtr<game::loadingscreen::LoadingScreenBuilder> builder) {
	Builder = builder;
	FadeState = EFadeState::In;

	States = {
		{ ELoadingScreenStateID::Creation, MakeShareable(new CreationState()) },
		{ ELoadingScreenStateID::Teardown, MakeShareable(new TearDownState()) }
	};
}

void ULoadingScreenInitializer::InitializeLoadingScreen(UWorld* world, FadeTime fadeTime, MapLoadData mapLoadData) {

	if (LoadingState == ELoadingState::Loading)
	{
		PreviousLoadingScreenWidget = LoadingScreenWidget;
		LoadingScreenWidget = nullptr;

		if (CurrentAudioComponent)
		{
			CurrentAudioComponent->Stop();
			CurrentAudioComponent = nullptr;
		}
		world->GetGameInstance<UDungeonsGameInstance>()->GetDungeonsAssetManager()->OnLoadingComplete();

		LoadingScreenTornDown.Broadcast();
		LoadingState = ELoadingState::Idle;
		CurrentState.Reset();
	}


	const auto localPlayerControllers = Cast<UDungeonsGameInstance>(world->GetGameInstance())->GetUserManager()->GetAllLocalPlayerControllers();
	for (auto lpc : localPlayerControllers) {
		lpc->DisableInput(lpc);
		lpc->ClientStopForceFeedback(NULL, NAME_None);
	}

	ResetAllStates();

	auto newState = States[ELoadingScreenStateID::Creation];

	if (!Fader.IsValid()) {
		Fader = world->SpawnActor<AScreenFader>();
	}

	CurrentState = newState;

	CurrentMapLoadData = std::move(mapLoadData);

	ELoadingScreenType screenType = GetScreenTypeFromLoadType(mapLoadData.MapLoadType);

	const auto levelName = mapLoadData.LevelSettings.getLevelName();

	world->GetGameInstance<UDungeonsGameInstance>()->GetDungeonsAssetManager()->OnLoadingStarted();
	double val = FPlatformTime::Seconds();
	FPlatformMisc::LowLevelOutputDebugStringf(TEXT("##### ULoadingScreenInitializer::InitializeLoadingScreen  LoadingScreenStarted  LOAD_TIME (%f s) #####\n"), val);
	LoadingScreenStarted.Broadcast(fadeTime.Out);
	if (AAudioMusicManager* AudioManager = world->GetGameInstance<UDungeonsGameInstance>()->GetAudioMusicManager()) {
		CurrentAudioComponent = AudioManager->PlayLoadingScreenSound();
	}

	auto ThemeLibrary = world->GetGameInstance<UDungeonsGameInstance>()->GetMissionThemeLibrary();
	auto& missionDef = missions::get(levelName);
	const FMissionThemeRow* themeRow = ThemeLibrary->findRow(missionDef.theme());	
	FLoadingScreenStyle style = themeRow ? themeRow->LoadingScreen : FLoadingScreenStyle();
	LoadingScreenWidget = Builder->SetupLoadingScreenWidget(missionDef, mapLoadData.LevelSettings, screenType, fadeTime.Out, style);

	CurrentState->Start({ LoadingScreenWidget.ToSharedRef(), Fader.Get(), fadeTime, 
		FOnStateCompleted::CreateUObject(this, &ULoadingScreenInitializer::OnInitialiseStateCompleted, world), FOnLoadMap::CreateUObject(this, &ULoadingScreenInitializer::LoadMap, world) });

	UDungeonsGameInstance* OurGameInstance = world->GetGameInstance<UDungeonsGameInstance>();
	OurGameInstance->UpdateUsingMultiplayerFeatures(false);

	LoadingState = ELoadingState::Loading;
}

void ULoadingScreenInitializer::TearDownLoadingScreen(UWorld* world, FadeTime fadeTime) {
	auto newState = States[ELoadingScreenStateID::Teardown];

	if (!LoadingScreenWidget.IsValid() || CurrentState.IsValid() && (CurrentState == newState)) {
		return;
	}


	if (PreviousLoadingScreenWidget)
	{
		GEngine->GameViewport->RemoveViewportWidgetContent(PreviousLoadingScreenWidget.ToSharedRef());
		PreviousLoadingScreenWidget->SetVisibility(EVisibility::Collapsed);
		PreviousLoadingScreenWidget = nullptr;
	}

	check(LoadingState == ELoadingState::Loading);
	LoadingState = ELoadingState::TearingDown;

	const auto localPlayerControllers = Cast<UDungeonsGameInstance>(world->GetGameInstance())->GetUserManager()->GetAllLocalPlayerControllers();
	for (auto lpc : localPlayerControllers) {
		lpc->EnableInput(lpc);
	}

	if (!Fader.IsValid()) {
		Fader = world->SpawnActor<AScreenFader>();
	}

	if (CurrentAudioComponent) {
		CurrentAudioComponent->Stop();
		CurrentAudioComponent = nullptr;
	}
	world->GetGameInstance<UDungeonsGameInstance>()->GetDungeonsAssetManager()->OnLoadingComplete();

	CurrentState = newState;

	LoadingScreenTornDown.Broadcast();

	CurrentState->Start({ LoadingScreenWidget.ToSharedRef(), Fader.Get(), fadeTime, 
		FOnStateCompleted::CreateUObject(this, &ULoadingScreenInitializer::OnTearDownStateCompleted, world) });
}

void ULoadingScreenInitializer::OnPlayerControllerPossessedPawn(ABasePlayerController* pc) {
	if (IsActive()) {
		pc->DisableInput(pc);
	}
}

void ULoadingScreenInitializer::ResetAllStates()
{
	for (const std::pair<ELoadingScreenStateID, TSharedPtr<LoadingScreenState>>& pair : States)
	{
		TSharedPtr<LoadingScreenState> state = pair.second;
		state.Reset();
	}

	// Destroy widget
	if (LoadingScreenWidget)
	{
		GEngine->GameViewport->RemoveViewportWidgetContent(LoadingScreenWidget.ToSharedRef());
		LoadingScreenWidget->SetVisibility(EVisibility::Collapsed);
		LoadingScreenWidget = nullptr;
	}
}

bool ULoadingScreenInitializer::IsActive() const {
	return LoadingState != ELoadingState::Idle;
}

bool ULoadingScreenInitializer::IsCurrentlyShown() const
{
	if (LoadingScreenWidget)
	{
		return LoadingScreenWidget->GetVisibility() == EVisibility::Visible;
	}
	return false;
}

void ULoadingScreenInitializer::OnInitialiseStateCompleted(UWorld* world) 
{
	CurrentState.Reset();
}

void ULoadingScreenInitializer::OnTearDownStateCompleted(UWorld* world)
{
	CurrentState.Reset();

	if (LoadingScreenWidget)
	{
		GEngine->GameViewport->RemoveViewportWidgetContent(LoadingScreenWidget.ToSharedRef());
		LoadingScreenWidget->SetVisibility(EVisibility::Collapsed);
		LoadingScreenWidget = nullptr;
	}

	check(LoadingState == ELoadingState::TearingDown);
	LoadingState = ELoadingState::Idle;

	UDungeonsGameInstance* OurGameInstance = world->GetGameInstance<UDungeonsGameInstance>();
	OurGameInstance->CheckKickToTitleScreenReasons();
}


void ULoadingScreenInitializer::LoadMap(UWorld* world) {
	CreateSessionProxy = nullptr;
	switch (CurrentMapLoadData.MapLoadType) {
		case(EMapLoadType::TravelIngameServer): {
			travelutil::ServerTravelToGameMap(world, CurrentMapLoadData.LevelSettings);
			break;
		}
		case(EMapLoadType::TravelLobbyServer): {
			travelutil::ServerTravelToLobby(world, CurrentMapLoadData.LevelSettings);
			break;
		}
		case(EMapLoadType::StartLobbySession): {
			CreateSessionProxy = UCreateDungeonsSessionCallbackProxy::CreateLobbySession(world, CurrentMapLoadData.IsPrivateGame);
			break;
		}
		case(EMapLoadType::StartIngameSession): {
			CreateSessionProxy = UCreateDungeonsSessionCallbackProxy::CreateSession(world, CurrentMapLoadData.LevelSettings, CurrentMapLoadData.IsPrivateGame);
			break;
		}
		case(EMapLoadType::OpenMenu): {
			travelutil::OpenMenu(world);
			break;
		}
		case(EMapLoadType::OpenIngame): {
			travelutil::OpenLevel(world, CurrentMapLoadData.LevelSettings); //D11.PS - Open based on level settings
			break;
		}
		case(EMapLoadType::OpenLobby): {
			travelutil::OpenLevel(world, levelsettingsutil::generateLobbySettings(world));
			break;
		}

		case(EMapLoadType::JoinLobbySession):
		case(EMapLoadType::JoinIngameSession):
		{
			break;
		}
	}

	if (CurrentMapLoadData.LoadingScreenReadyCallback) {
		CurrentMapLoadData.LoadingScreenReadyCallback();
	}
	
	if (CreateSessionProxy) {
		CreateSessionProxy->CreateSessionSucceeded.AddUObject(this, &ULoadingScreenInitializer::OnCreateSessionSucceeded, world);
		CreateSessionProxy->CreateSessionFailed.AddUObject(this, &ULoadingScreenInitializer::OnCreateSessionFailed, world);
		CreateSessionProxy->Activate();
	}
}



void ULoadingScreenInitializer::OnCreateSessionFailed(ESessionFailureReason, UWorld* world) {
	TearDownLoadingScreen(world, { UDungeonsGameInstance::fDefaultFadeOutTime, UDungeonsGameInstance::fDefaultFadeInTime });

	auto gameInstance = world->GetGameInstance<UDungeonsGameInstance>();
	gameInstance->PushGlobalErrorMessage(EGlobalMessageTypes::FailedToCreate);
}

void ULoadingScreenInitializer::OnCreateSessionSucceeded(const FLevelSettings& levelSettings, UWorld* world) {
	travelutil::OpenLevel(world, levelSettings);
}

void CreationState::Start(StateInput input) {

	if (CVarShowLoadScreenTime.GetValueOnGameThread())
	{
		//start load screen timer
		gLoadScreenTime = FPlatformTime::Seconds();
		FPlatformMisc::LowLevelOutputDebugStringf(TEXT("\n\n################### LOAD_TIME START: (%f s) ##################\n\n"), gLoadScreenTime);
	}

	// Fade out scene
	input.Fader->StartFade(false, input.Fadetime.Out, FOnFadeComplete::CreateRaw(this, &CreationState::OnFadeComplete, input));

	// Construct loading screen widget to block mouse input, but wait with making it actually visible
	GEngine->GameViewport->AddViewportWidgetContent(
		SNew(SWeakWidget)
		.PossiblyNullContent(input.LoadingScreenWidget),
		1000
	);
	input.LoadingScreenWidget->SetVisibility(EVisibility::Visible);
	input.LoadingScreenWidget->SetRenderOpacity(0.0f);

	FadeState = EFadeState::Out;
}

void CreationState::OnFadeComplete(StateInput input) {
	if (FadeState == EFadeState::Out) {
		input.LoadingScreenWidget->SetRenderOpacity(1.0f);
		
		UGameplayStatics::SetEnableWorldRendering(input.Fader, false);

		if (CVarShowLoadScreenTime.GetValueOnGameThread())
		{
			double var = FPlatformTime::Seconds();
			FPlatformMisc::LowLevelOutputDebugStringf(TEXT("\n\n################### LOAD_TIME CreationState::OnFadeComplete EFadeState::Out (%f s) %f ##################\n\n"), var, var - gLoadScreenTime);
		}

		input.Fader->StartFade(true, input.Fadetime.In, FOnFadeComplete::CreateRaw(this, &CreationState::OnFadeComplete, input));
		FadeState = EFadeState::In;
	}
	else if (FadeState == EFadeState::In) {
		
		UDungeonsGameInstance::SetFastLoadEnabled(true);

		if (CVarShowLoadScreenTime.GetValueOnGameThread())
		{
			double var = FPlatformTime::Seconds();
			FPlatformMisc::LowLevelOutputDebugStringf(TEXT("\n\n################### LOAD_TIME CreationState::OnFadeComplete EFadeState::In (%f s) %f ##################\n\n"), var, var - gLoadScreenTime);
		}

		// Start real loading
		input.OnLoadMap.Execute();
		// Notify that state is over
		input.OnStateCompleted.Execute();
	}
}

void TearDownState::Start(StateInput input) {
	// Fade out loading screen
	UDungeonsGameInstance::SetFastLoadEnabled(false);
	input.Fader->StartFade(false, input.Fadetime.Out, FOnFadeComplete::CreateRaw(this, &TearDownState::OnFadeComplete, input));

	if (CVarShowLoadScreenTime.GetValueOnGameThread())
	{
		double var = FPlatformTime::Seconds();
		FPlatformMisc::LowLevelOutputDebugStringf(TEXT("\n\n################### LOAD_TIME  TearDownState::Start (%f s) %f  (timer %f )##################\n\n"), var, var - gLoadScreenTime, input.Fadetime.Out);
	}

	FadeState = EFadeState::Out;
}

void TearDownState::OnFadeComplete(StateInput input) {
	if (FadeState == EFadeState::Out) {
			
		// Destroy widget
		GEngine->GameViewport->RemoveViewportWidgetContent(input.LoadingScreenWidget);
		input.LoadingScreenWidget->SetVisibility(EVisibility::Collapsed);

		UGameplayStatics::SetEnableWorldRendering(input.Fader, true);

		if (CVarShowLoadScreenTime.GetValueOnGameThread())
		{
			double var = FPlatformTime::Seconds();
			FPlatformMisc::LowLevelOutputDebugStringf(TEXT("\n\n################### LOAD_TIME  TearDownState::OnFadeComplete EFadeState::Out (%f s) %f (timer %f )##################\n\n"), var, var - gLoadScreenTime, input.Fadetime.In);
		}

		// Fade in scene
		input.Fader->StartFade(true, input.Fadetime.In, FOnFadeComplete::CreateRaw(this, &TearDownState::OnFadeComplete, input));
		FadeState = EFadeState::In;
	}
	else if (FadeState == EFadeState::In) {
		// Notify that state is over
		input.OnStateCompleted.Execute();

		if (CVarShowLoadScreenTime.GetValueOnGameThread())
		{
			if (CVarShowLoadScreenTime.GetValueOnGameThread())
			{
				double var = FPlatformTime::Seconds();
				FPlatformMisc::LowLevelOutputDebugStringf(TEXT("\n\n################### LOAD_TIME  TearDownState::OnFadeComplete EFadeState::In (%f s) %f (timer %f )##################\n\n"), var, var - gLoadScreenTime, input.Fadetime.In);
			}

			double EndLoadScreenTime = (FPlatformTime::Seconds() - gLoadScreenTime);
			FPlatformMisc::LowLevelOutputDebugStringf(TEXT("\n\n################### LOAD SCREEN TIME : (%f s) ##################\n\n"), EndLoadScreenTime);
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("LOAD SCREEN TIME : (%f s)"), EndLoadScreenTime));
		}

	}
}


StateInput::StateInput(TSharedRef<SWidget> loadingScreenWidget, AScreenFader* fader, FadeTime fadeTime, FOnStateCompleted onStateCompleted, FOnLoadMap onLoadMap) :
	LoadingScreenWidget(loadingScreenWidget),
	Fader(fader),
	Fadetime(fadeTime),	
	OnStateCompleted(onStateCompleted),
	OnLoadMap(onLoadMap) {
}


StateInput::~StateInput()
{
	
}
