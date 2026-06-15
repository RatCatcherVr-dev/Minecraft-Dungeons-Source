#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "game/difficulty/Difficulty.h"
#include "game/LevelSettings.h"
#include "RespawnAsTeamUsingLivesComponent.generated.h"

UENUM(BlueprintType)
enum class ETeamDeathState : uint8 {	
	WaitingForMissionStart,
	AnyPlayerAlive,
	LifeDeductedAndReviving,
	GameOver,
};


class ABasePlayerState;
class ADungeonsGameState;
class APlayerCharacter;
struct FLevelSettings;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONS_API URespawnAsTeamUsingLivesComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	URespawnAsTeamUsingLivesComponent();

	void InitializeComponent() override;
	void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	static bool HasPlayerForcedDownState(UWorld* world, APlayerCharacter* player);
	static bool HasAnyPlayerForcedDownState(UWorld* world);
private:
	void OnPlayerCharacterAdded(APlayerCharacter* player);
	void OnPlayerAliveStateChanged(APlayerCharacter* player);

	void SetTeamDeathState(ETeamDeathState state);

	//Tracks the team 'play/die/revive/consume-life/respawn' state machine.
	void RefreshTeamDeathState();

	static bool AreAllPlayersDead(ADungeonsGameState* gameState);

	void InitDifficultyLives(const FLevelSettings&);

	UFUNCTION()
	void OnAffectorsChanged();

	ADungeonsGameState* GetGameState() const;	

	ETeamDeathState teamDeathState = ETeamDeathState::WaitingForMissionStart;

	FLevelSettings LevelSettings;
};
