// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LoadingScreenBuilder.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "gamemodes/LobbyActor.h"
#include "online/sessions/DungeonsSessionCommon.h"
#include "DungeonsDefsMinimal.h"
#include "LoadingScreenInitializer.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoadingScreenStart, float, fadeTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLoadingScreenTearDown);

DECLARE_DELEGATE(FOnLoadMap)
DECLARE_DELEGATE(FOnStateCompleted)

class AScreenFader;
enum class ELoadingScreenType : uint8;
enum class EMapLoadType : uint8;
class UCreateDungeonsSessionCallbackProxy;
class APlayerControllerBase;

enum class ELoadingScreenStateID {
	Default,
	Creation,
	Teardown
};

enum class EFadeState {
	In,
	Out
};

struct FadeTime {
	float Out, In;

	FadeTime(float out, float in) : Out(out), In(in) {}
};

struct StateInput {
	TSharedRef<SWidget> LoadingScreenWidget;
	AScreenFader* Fader;
	FadeTime Fadetime;
	FOnStateCompleted OnStateCompleted;
	FOnLoadMap OnLoadMap;

	StateInput(TSharedRef<SWidget>, AScreenFader*, FadeTime, FOnStateCompleted, FOnLoadMap = FOnLoadMap());
	~StateInput();
};

UENUM(BlueprintType)
enum class ELoadingState : uint8 {
	Idle,
	Loading,
	TearingDown
};
ENUM_NAME(ELoadingState);

class LoadingScreenState {
public:
	LoadingScreenState() {} //D11.PS
	virtual ~LoadingScreenState() {} //D11.PS
	virtual void Start(StateInput) {}
	virtual void OnFadeComplete(StateInput) {}

	EFadeState GetFadeState() const { return FadeState; }
protected:
	EFadeState FadeState;
};

class CreationState : public LoadingScreenState {
public:
	void Start(StateInput) override;
	void OnFadeComplete(StateInput) override;
};

class TearDownState : public LoadingScreenState {
public:
	void Start(StateInput);
	void OnFadeComplete(StateInput) override;
};

struct MapLoadData {
	MapLoadData() {}
	MapLoadData(FLevelSettings levelSettings, EMapLoadType mapLoadType, std::function<void()> loadingScreenReadyCallback, bool isPrivateGame) :
		LevelSettings(std::move(levelSettings)), MapLoadType(mapLoadType), LoadingScreenReadyCallback(std::move(loadingScreenReadyCallback)), IsPrivateGame(isPrivateGame) {}

	bool isSessionLoad() const { return MapLoadType == EMapLoadType::JoinIngameSession || MapLoadType == EMapLoadType::StartIngameSession || MapLoadType == EMapLoadType::TravelIngameServer || MapLoadType == EMapLoadType::TravelIngameClient; }
	bool isCreatingOrJoiningSession() const { return MapLoadType == EMapLoadType::JoinIngameSession || MapLoadType == EMapLoadType::JoinLobbySession || MapLoadType == EMapLoadType::StartIngameSession || MapLoadType == EMapLoadType::StartLobbySession; }

	FLevelSettings LevelSettings;
	EMapLoadType MapLoadType;
	std::function<void()> LoadingScreenReadyCallback;
	bool IsPrivateGame = false;
};

UCLASS()
class DUNGEONS_API ULoadingScreenInitializer : public UObject {

	GENERATED_BODY()

public:
	void Setup(TSharedPtr<game::loadingscreen::LoadingScreenBuilder>);

	void InitializeLoadingScreen(UWorld*, FadeTime, MapLoadData);
	void TearDownLoadingScreen(UWorld*, FadeTime);

	void OnPlayerControllerPossessedPawn(ABasePlayerController* pc);

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
	FOnLoadingScreenStart LoadingScreenStarted;

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
	FOnLoadingScreenTearDown LoadingScreenTornDown;

	void ResetAllStates();

	bool IsActive() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool IsCurrentlyShown() const;

private:
	void OnInitialiseStateCompleted(UWorld* world);
	void OnTearDownStateCompleted(UWorld* world);
	void LoadMap(UWorld* world);
	void OnCreateSessionFailed(ESessionFailureReason, UWorld*);
	void OnCreateSessionSucceeded(const FLevelSettings&, UWorld*);

	TWeakObjectPtr<AScreenFader> Fader;

	EFadeState FadeState;

	std::map<ELoadingScreenStateID, TSharedPtr<LoadingScreenState>> States;

	TSharedPtr<SWidget> LoadingScreenWidget;
	TSharedPtr<SWidget> PreviousLoadingScreenWidget;

	TSharedPtr<game::loadingscreen::LoadingScreenBuilder> Builder;

	MapLoadData CurrentMapLoadData;

	UPROPERTY()
	UCreateDungeonsSessionCallbackProxy* CreateSessionProxy;

	TSharedPtr<LoadingScreenState> CurrentState;

	UPROPERTY()
	UAudioComponent* CurrentAudioComponent;

	UPROPERTY()
	ELoadingState LoadingState = ELoadingState::Idle;
};
