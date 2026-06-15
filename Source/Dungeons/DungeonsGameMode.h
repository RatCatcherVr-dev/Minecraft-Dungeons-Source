#pragma once

#include "Dungeons/online/OnlineCommon.h"
#include "game/component/AwardsGeneratorComponent.h"
#include "game/difficulty/Difficulty.h"
#include "game/actor/MapStatActor.h"
#include "game/LevelSettings.h"
#include "util/DungeonsPlayerStatTracker.h"
#include "util/DungeonsGameStatTracker.h"
#include <GameFramework/GameModeBase.h>
#include <GameplayEffectTypes.h>
#include <WeakObjectPtrTemplates.h>
#include "game/actor/character/player/BasePlayerState.h"
#include "DungeonsGameMode.generated.h"

class APlayerCharacter;
class APlayerController;
class ABasePlayerController;
class UDungeonsGameInstance;
class AMobCharacter;
class UPlayerExperienceComponent;
class USecretComponent;
struct FLevelSettings;

namespace game { class Game; }
namespace analytics { class Analytics; }

DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnActorDeath, AActor*, AActor*, AActor*);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnActorUsedItem, const AActor*, const AItemInstance*)

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerCharacterAdded, APlayerCharacter*);
DECLARE_MULTICAST_DELEGATE(FOnPlayerCharacterRemoved);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerControllerLogin, ABasePlayerController*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerControllerLogout, ABasePlayerController*);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnInitDifficulty, const FLevelSettings&);

DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnPlayerKilledMob, const AMobCharacter* /*killed mob*/, AActor* /*by Actor*/, AActor* /*by What*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnXPChanged, int32);

UCLASS(minimalapi)
class ADungeonsGameMode : public AGameModeBase {
	GENERATED_BODY()
public:
	ADungeonsGameMode();

	void StartPlay() override;

	void SetupActorStatsTracking();

	void OnLevelAddedToWorld(ULevel* InLevel, UWorld* InWorld);

	void EndPlay(EEndPlayReason::Type) override;

	void StartToLeaveMap() override;
	void InitGameState() override;	
	FString InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal = TEXT("")) override;

	void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void PostLogin(APlayerController* NewPlayer) override;

	void Logout(AController* Exiting) override;
	void ResetLevel() override;
	void RemovePlayer() const;

	UFUNCTION()
	void OnJoinedPlayerNameChanged(ABasePlayerState* playerState);

	void GamePlayerCountChanged();	
	void ApplyDifficultyEffects(AMobCharacter& mob);
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void ApplyDifficultyEffects(AMobCharacter* mob);
	void ApplyPlayerDifficultyEffects(ABaseCharacter& character);
	FOnExternalGameplayModifierDependencyChange OnDifficultyParametersChanged;

	FOnActorDeath ActorDeath;
	FOnActorUsedItem ActorUsedItem;
	FOnPlayerKilledMob OnPlayerKilledMob;
		
	FOnPlayerCharacterAdded OnPlayerCharacterAdded;	
	FOnPlayerCharacterRemoved OnPlayerCharacterRemoved;	

	FOnPlayerControllerLogin OnPlayerControllerLogin;
	FOnPlayerControllerLogout OnPlayerControllerLogout;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool KickPlayer(APlayerController* playerController, const FText& KickReason);

	void LevelGenerated(const FLevelSettings&);
	void OnActorDeath(AActor* actor, AActor* byWhom, AActor* byWhat);

	void OnSecretSpawned(USecretComponent&);

	UFUNCTION()
	void OnOpenLootChest(const FVector& location, AActor* opener, AActor* chest);

	void OnActorUsedItem(const AActor* actor, const AItemInstance* item);

	void OnActorHeal(AActor* actor, float amount);

	void OnActorDamage(AActor* actor, float damage, AActor* source, AActor* sourceWeapon);

	void OnActorSpawnedProjectile(const AActor* actor, const ABaseProjectile* projectile);

	void OnActorSpawnedProjectileHit(const AActor* owner, const ABaseProjectile* projectile);

	void OnActorAttackFinished(const AActor* actor, unsigned targetsHit, float damageDone);

	void OnActorSteppedOnNewBlock(const AActor* actor, EMaterialTypeEnum blockMaterial);

	FMissionFinishedSummary GenerateMissionFinishedSummary();

	static constexpr float RespawnTimeSecondsSingleplayer = 5.f;
	static constexpr float RespawnTimeSecondsMultiplayer = 7.f;

	const TMap<int, DungeonsPlayerStatTracker>& GetPlayerTrackers() const;
	DungeonsGameStatTracker& GetGameStatTracker() { return GameStatTracker; }

	FOnInitDifficulty OnInitDifficulty;

	game::Game* GetGame() const;

	void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	void InitDifficulty(const FLevelSettings&);

	void TrackEmeraldSpawned();

	static UPlayerExperienceComponent* GetXPComponentFor(const TWeakObjectPtr<APlayerController>& controller);

protected:
	TSubclassOf<AGameSession> GetGameSessionClass() const override;

	APawn* SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot) override;
	void Tick(float DeltaSeconds) override;
	class ADungeonsGameState* gameState;

private:
	void UpdatePlayerRespawnTimes(float respawnTimeSeconds) const;

	void OnPlayerDeath(const game::Game* game, APlayerCharacter* player, AActor* byWhom, AActor* byWhat);

	void OnMobDeath(const game::Game* game, const AMobCharacter* mob, AActor* byWhom, AActor* byWhat);	
	void GrantXPOnMobKill(const game::Game* game, const AMobCharacter* mob) const;
	int CalculateMobKillXPValue(const game::Game* game, const AMobCharacter* mob) const;

	void TrackNonPooledActorSpawnedStats(AActor*);
	template <class T>
	void TrackNonPooledActorSpawnedStats(const TArray<T*>& actors) {
		for (auto* actor : actors) {
			TrackNonPooledActorSpawnedStats(actor);
		}
	}
	void TrackExistingActorStats();

	void _updatePendingPlayers();
	TArray<TWeakObjectPtr<APlayerCharacter>> mPendingPlayers;	
	double LastSessionUpdateSec = 0;

	TMap<int, DungeonsPlayerStatTracker> PlayerStatTrackers;
	DungeonsGameStatTracker GameStatTracker;

	DungeonsPlayerStatTracker& getEnsuredPlayerStatTracker(const APlayerCharacter*);

	DungeonsPlayerStatTracker DummyPlayerStatTracker;

	UPROPERTY()
	TWeakObjectPtr<AGameBP> GameBP;
	TWeakObjectPtr<UDungeonsGameInstance> GameInstance;

	UFUNCTION()
	AGameBP* GetOrFindGame();

	UFUNCTION()
	void OnSecretFound(AActor* SecretFinder);

	AMapStatActor& GetEnsuredMapStatTracker();
	
	TWeakObjectPtr<AMapStatActor> MapStatActor;

	bool ForceOnlineUpdate = true;
};

